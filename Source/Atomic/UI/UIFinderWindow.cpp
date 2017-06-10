//
// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>

#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include "UI.h"
#include "UIEvents.h"
#include "UIWindow.h"
#include "UIEditField.h"
#include "UISelectList.h"
#include "UIPromptWindow.h"
#include "UIFinderWindow.h"
#include "UISelectItem.h"
#include "UIMenuWindow.h"

using namespace tb;

namespace Atomic
{

/// finder window
UIFinderWindow::UIFinderWindow(Context* context, UIWidget* target, const String& id, bool createWidget) : 
    UIWindow(context, false),
    finderMode_(0),
    bookmarksDirty_(0)
{
    if (createWidget)
    {
        widget_ = new TBFinderWindow(target ? target->GetInternalWidget() : 0, TBIDC(id.CString()));
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UIFinderWindow::~UIFinderWindow()
{
    SaveBookmarks();
    
    if ( !newFolderPtr_.Expired())
        newFolderPtr_->UnsubscribeFromAllEvents();
    if ( !newBookmarkPtr_.Expired() )
       newBookmarkPtr_->UnsubscribeFromAllEvents();

}

void UIFinderWindow::FindFile(const String& title, const String& preset, int dimmer, int width, int height)
{
    if (!widget_)
        return;

    if ( ((TBFinderWindow*)widget_)->Show(title.CString(), preset.CString(), dimmer, width, height) )
    {
        CreateBookmarks();
        PresetCurrentPath(preset);
        UpdateUiPath();
        UpdateUiList();
        UpdateUiResult();
    }
}

void UIFinderWindow::FindPath(const String& title, const String& preset, int dimmer, int width, int height)
{
    if (!widget_)
        return;

    finderMode_ = 1;
    
    if ( ((TBFinderWindow*)widget_)->Show(title.CString(), preset.CString(), dimmer, width, height) )
    {
        UIWidget *reswid = GetResultWidget();
        if (reswid) reswid->SetVisibility( UI_WIDGET_VISIBILITY_INVISIBLE );
        CreateBookmarks();
        PresetCurrentPath(preset);
        UpdateUiPath();
        UpdateUiList();
        UpdateUiResult();
    }
}

bool UIFinderWindow::OnEvent(const tb::TBWidgetEvent &ev)
{

    if ( ev.type == EVENT_TYPE_CHANGED && ev.target && ((uint32)ev.target->GetID()) == UIFINDEREDITPATHID ) 
    {
        FileSystem* filesystem = GetSubsystem<FileSystem>();
        UIWidget *pathwidget = GetPathWidget();  // paste or type in currentpath widget
        if(pathwidget)
        {
            if ( filesystem->DirExists (pathwidget->GetText())) 
            {
                if ( pathwidget->GetText() != currentPath_ ) 
                {
                    resultPath_ = "";
                    SetCurrentPath (pathwidget->GetText());
                    UpdateUiList();
                    UpdateUiResult();
                }
            }
        }
        return true;
    }

   if ( ev.type == EVENT_TYPE_POINTER_UP && ev.target && ((uint32)ev.target->GetID()) == UIFINDERUPBUTTONID ) // go up
    {
        GoFolderUp();
        return true;
    }

    if ( ev.type == EVENT_TYPE_POINTER_UP && ev.target && ((uint32)ev.target->GetID()) == UIFINDERBOOKBUTTONID ) // add bookmark request
    {
        // this check is necessary because you can kill the bookmark window with the "X" and
        // this kills the UI but not the newBookmarkPtr to it, and it doesnt get cleaned up properly.
        if ( newBookmarkPtr_.NotNull() )
        {
            newBookmarkPtr_->UnsubscribeFromAllEvents();
            newBookmarkPtr_.Reset();
        }
        newBookmarkPtr_ = new UIPromptWindow(context_, this, "createbookmark",  true);
        SubscribeToEvent(newBookmarkPtr_, E_UIPROMPTCOMPLETE, ATOMIC_HANDLER(UIFinderWindow, HandleCreateBookmark ));

        String prospect = "";
        char delim = '/';
        Vector <String> tokens = currentPath_.Split(delim, false);
        prospect = tokens[ tokens.Size()-1 ]; // get the last folder name as preset
    
        newBookmarkPtr_->Show("Create New Bookmark", "Enter a name for the new bookmark", prospect );
        return true;
    }

    if ( ev.type == EVENT_TYPE_POINTER_UP && ev.target && ((uint32)ev.target->GetID()) == UIFINDERFOLDERBUTTONID ) // add folder request
    {
        if (newFolderPtr_.NotNull())
        {
            newFolderPtr_->UnsubscribeFromAllEvents();
            newFolderPtr_.Reset();
        }
        newFolderPtr_ = new UIPromptWindow(context_, this, "createfolder",  true);
        SubscribeToEvent(newFolderPtr_, E_UIPROMPTCOMPLETE, ATOMIC_HANDLER(UIFinderWindow, HandleCreateFolder));
        newFolderPtr_->Show("Create new folder", "Enter a name for the new folder", "" );
        return true;
    }

    if ( ev.type == EVENT_TYPE_CLICK && ev.target && ((uint32)ev.target->GetID()) == 5 ) // clicked in bookmarks
    {
        UISelectList *bklist = static_cast<UISelectList *>(GetBookmarksWidget());
        int selected = bklist->GetValue();
        if ( selected >= 0 )
        {
            resultPath_ = "";  // were going back, give up file.
            SetCurrentPath ( bookmarkPaths_[selected]);
            UpdateUiPath();
            UpdateUiList();
            UpdateUiResult();
            bklist->SetValue(-1); // clear the select visuals
        }
        return true;
    }
    
    if ( ev.type == EVENT_TYPE_CUSTOM && ev.target && ((uint32)ev.target->GetID()) == UIFINDERBOOKLISTID ) // bookmarks TB context menu result
    {
        UI* ui = GetSubsystem<UI>();
        if ( ev.special_key == tb::TB_KEY_DELETE ) // we wanna delete something
        {
            String myid;
            ui->GetTBIDString(ev.target?((uint32)ev.target->GetID()) : 0, myid);
            UISelectList *bklist = static_cast<UISelectList *>(GetBookmarksWidget());
            if (bklist)
            {
                int myindex = bklist->FindId ( (uint32)ev.ref_id );
                if ( myindex >= 0 )
                {
                    DeleteBookmark(myindex);
                }
            }
        }
        return true;
    }

    if ( ev.type == EVENT_TYPE_CLICK && ev.target && ((uint32)ev.target->GetID()) == UIFINDERFILELISTID ) // clicked dirfiles list
    {
        UISelectList *filelist = static_cast<UISelectList *>(GetFilesWidget());
        ComposePath( filelist->GetSelectedItemString() );
        return true;
    }

    if ( ev.type == EVENT_TYPE_CLICK && (ev.ref_id ==  UIFINDEROKBUTTONID|| ev.ref_id == UIFINDERCANCELBUTTONID ) ) // clicked ok or cancel button
    {
        UI* ui = GetSubsystem<UI>();
        VariantMap eventData;

        String title = "FinderWindow";
        TBStr tbtext;
        if(  widget_ && (TBWindow*)widget_->GetText(tbtext) )
            title = tbtext.CStr();

        eventData[UIFinderComplete::P_TITLE] = title;
        eventData[UIFinderComplete::P_SELECTED] = "";
        eventData[UIFinderComplete::P_REASON] = "CANCEL";

        if (ev.ref_id == UIFINDEROKBUTTONID) // ok button was pressed, otherwise it was cancel button
        {
            eventData[UIFinderComplete::P_REASON] = "OK";
            if ( finderMode_ == 0 ) // finding a file
            {   // get from widget, in case the user had been typing.
                UIWidget *ewidget = GetResultWidget(); 
                if( ewidget) eventData[UIFinderComplete::P_SELECTED] = ewidget->GetText();
            }
            else  // finding a folder
            {
                UIWidget *cwidget = GetPathWidget();
                if( cwidget) eventData[UIFinderComplete::P_SELECTED] = cwidget->GetText();
            }
        }

        ConvertEvent(this, ui->WrapWidget(ev.target), ev, eventData);
        SendEvent(E_UIFINDERCOMPLETE, eventData);

        if (eventData[WidgetEvent::P_HANDLED].GetBool())
           return true;
    }

    return UIWindow::OnEvent(ev);
}

void UIFinderWindow::HandleCreateBookmark(StringHash eventType, VariantMap& eventData)
{
    const String& Title = eventData["Title"].GetString();
    const String& Reason = eventData["Reason"].GetString();
    const String& Selected = eventData["Selected"].GetString();

    if( Reason == "OK" )
        CreateBookmark( Selected, currentPath_ );
    if (newBookmarkPtr_)
    {
        newBookmarkPtr_->UnsubscribeFromAllEvents();
        newBookmarkPtr_.Reset();
    }
}

void UIFinderWindow::HandleCreateFolder(StringHash eventType, VariantMap& eventData)
{
    const String& Title = eventData["Title"].GetString();
    const String& Reason = eventData["Reason"].GetString();
    const String& Selected = eventData["Selected"].GetString();

    if( Reason == "OK" )
        CreateFolder(Selected); 
    if (newFolderPtr_)
    {
         newFolderPtr_->UnsubscribeFromAllEvents();
         newFolderPtr_.Reset();
    }
}

UIWidget* UIFinderWindow::GetWindowWidget()
{
    if (!widget_)
        return 0;
    UI* ui = GetSubsystem<UI>();
    return ui->WrapWidget(widget_);
}

UIWidget* UIFinderWindow::GetPathWidget()
{
    if (!widget_)
        return 0;
    TBWidget* child = (TBWidget*) widget_->GetWidgetByIDAndType<TBEditField>(UIFINDEREDITPATHID);
    if (!child)
        return 0;
    UI* ui = GetSubsystem<UI>();
    return ui->WrapWidget(child);
}

UIWidget* UIFinderWindow::GetResultWidget()
{
    if (!widget_)
        return 0;
    TBWidget* child = (TBWidget*)widget_->GetWidgetByIDAndType<TBEditField>(UIFINDEREDITFILEID);
    if (!child)
        return 0;
    UI* ui = GetSubsystem<UI>();
    return ui->WrapWidget(child);
}

UIWidget* UIFinderWindow::GetBookmarksWidget()
{
    if (!widget_)
        return 0;
    TBWidget* child = (TBWidget*)widget_->GetWidgetByIDAndType<TBSelectList>(UIFINDERBOOKLISTID);
    if (!child)
        return 0;
    UI* ui = GetSubsystem<UI>();
    return ui->WrapWidget(child);
}

UIWidget* UIFinderWindow::GetFilesWidget()
{
    if (!widget_)
        return 0;
    TBWidget* child = (TBWidget*)widget_->GetWidgetByIDAndType<TBSelectList>(UIFINDERFILELISTID);
    if (!child)
        return 0;
    UI* ui = GetSubsystem<UI>();
    return ui->WrapWidget(child);
}

// where the finder starts
void UIFinderWindow::PresetCurrentPath( const String& preset ) 
{
    FileSystem* filesystem = GetSubsystem<FileSystem>();
    if ( !preset.Empty() && filesystem->DirExists (preset) )
        SetCurrentPath (preset);
    else
        SetCurrentPath ( filesystem->GetUserDocumentsDir() );
}

// set the current path value
void UIFinderWindow::SetCurrentPath( const String& string ) 
{ 
    currentPath_ = string;
}

//using the list, jam things together, we'll either get another path or a file.
void UIFinderWindow::ComposePath (const String& string )
{
    String prospect = currentPath_ + string;

    FileSystem* filesystem = GetSubsystem<FileSystem>();
 
    if ( !filesystem->FileExists ( prospect ) )  // its a dir, feel the joy
    {
        SetCurrentPath( prospect + "/" );  // add the trailing slash, OR ELSE
        UpdateUiPath();
        UpdateUiList();
        UpdateUiResult();
    }
    else  // its a file
    {
        resultPath_ = prospect;
        UpdateUiResult();
    }
}

// create the list of bookmarks ... can be different per platform
void UIFinderWindow::CreateBookmarks()
{
    FileSystem* filesystem = GetSubsystem<FileSystem>();
    String basepath = filesystem->GetUserDocumentsDir();
    UISelectList *bklist = static_cast<UISelectList *>(GetBookmarksWidget());
    
#if defined(ATOMIC_PLATFORM_LINUX) 
    if ( filesystem->DirExists ( basepath )) CreateBookmark ( "Home", basepath );
    if ( filesystem->DirExists ( basepath + "Documents")) CreateBookmark ( "Documents", basepath + "Documents/" );
    if ( filesystem->DirExists ( basepath + "Music")) CreateBookmark ( "Music", basepath + "Music/" );
    if ( filesystem->DirExists ( basepath + "Pictures" )) CreateBookmark ( "Pictures", basepath + "Pictures/" );
    if ( filesystem->DirExists ( basepath + "Videos" )) CreateBookmark ( "Videos", basepath + "Videos/" );
    if ( filesystem->DirExists ( basepath + "Downloads")) CreateBookmark ( "Downloads", basepath + "Downloads/" );
#elif defined(ATOMIC_PLATFORM_WINDOWS)
    if ( filesystem->DirExists ( basepath )) CreateBookmark ( "Home", basepath );
    if ( filesystem->DirExists ( basepath + "Desktop")) CreateBookmark ( "Desktop", basepath + "Desktop/" );
    if ( filesystem->DirExists ( basepath + "Documents")) CreateBookmark ( "Documents", basepath + "Documents/" );
    if ( filesystem->DirExists ( basepath + "Downloads")) CreateBookmark ( "Downloads", basepath + "Downloads/" );
    if ( filesystem->DirExists ( basepath + "Music")) CreateBookmark ( "Music", basepath + "Music/" );
    if ( filesystem->DirExists ( basepath + "Pictures" )) CreateBookmark ( "Pictures", basepath + "Pictures/" );
    if ( filesystem->DirExists ( basepath + "Videos" )) CreateBookmark ( "Videos", basepath + "Videos/" );
#elif defined(ATOMIC_PLATFORM_OSX)
    if ( filesystem->DirExists ( basepath )) CreateBookmark ( "Home", basepath );
    if ( filesystem->DirExists ( basepath + "Documents")) CreateBookmark ( "Documents", basepath + "Documents/" );
    if ( filesystem->DirExists ( basepath + "Downloads")) CreateBookmark ( "Downloads", basepath + "Downloads/" );
    if ( filesystem->DirExists ( basepath + "Public")) CreateBookmark ( "Public", basepath + "Public/" );
#else  // android, ios, web?
    if ( filesystem->DirExists ( basepath )) CreateBookmark ( "Home", basepath );
#endif
    CreateBookmark ( "-", basepath );  // create separator!
    LoadBookmarks();
    bklist->SetValue(-1); // fix the selection and scrolling

}

// go up tree 1 folder
void UIFinderWindow::GoFolderUp()
{
    String prospect = "";
    char delim = '/';
    Vector <String> tokens = currentPath_.Split(delim, false);
 
    if ( tokens.Size() == 0 ) // were at the top
        prospect = "/";
    else
    {
        int nn = 0;
        for ( nn=0; nn<tokens.Size()-1; nn++ )
        {
            prospect += delim;
            prospect += tokens[nn];
        }
        prospect += delim;
    }

    if ( prospect != currentPath_ ) 
    {
        resultPath_ = "";
        SetCurrentPath (prospect);
        UpdateUiPath();
        UpdateUiList();
        UpdateUiResult();
    }
}

// move current path to widget
void UIFinderWindow::UpdateUiPath ()
{
    UIWidget *pathwidget = GetPathWidget();
    if(pathwidget)
    {
        if ( pathwidget->GetText() != currentPath_ )
            pathwidget->SetText(currentPath_);
    }
}

// move result path to widget
void UIFinderWindow::UpdateUiResult ()
{
    UIWidget *resultwidget = GetResultWidget();
    if( resultwidget)
    {
        if ( resultwidget->GetText() != resultPath_ )
            resultwidget->SetText(resultPath_);
    }
}

#include "Container/Sort.h"

// a very local compare function
bool CompareStrs(const String& a, const String &b)  
{
    return a < b;
}

// move folder contents into list
void UIFinderWindow::UpdateUiList()
{
    FileSystem* filesystem = GetSubsystem<FileSystem>();
    UISelectList *filelist = static_cast<UISelectList *>(GetFilesWidget());
    UISelectItemSource *fileSource = new UISelectItemSource(context_);
    
    if ( filesystem->DirExists (currentPath_ ) )
    { 
        Vector <String> mydirs;
        int nn = 0;
        filesystem->ScanDir (mydirs,currentPath_, "*", SCAN_DIRS, false );
        Sort(mydirs.Begin(), mydirs.End(), CompareStrs); // sort them
        for ( nn=0; nn<mydirs.Size(); nn++ ) 
        {
            if ( mydirs[nn] == "." ) continue;  
            if ( mydirs[nn] == ".." ) continue;
            String idz = "DIR"+ String(nn);
            fileSource->AddItem( new UISelectItem(context_, mydirs[nn], idz, "FolderIcon" ));
        }
        Vector <String> myfiles;
        filesystem->ScanDir (myfiles, currentPath_, "*", SCAN_FILES, false );
        Sort(myfiles.Begin(), myfiles.End(), CompareStrs);
        for ( nn=0; nn<myfiles.Size(); nn++ ) 
        {
            String idz = "FIL"+ String(nn);
            fileSource->AddItem(new UISelectItem( context_, myfiles[nn], idz));
        }
    }
    filelist->SetValue(-1);
    filelist->SetSource(fileSource);
}

// utility to add a folder in current path
void UIFinderWindow::CreateFolder( const String& string )
{
    FileSystem* filesystem = GetSubsystem<FileSystem>();
    if ( filesystem->CreateDir( currentPath_ + string ) )
    {
        UpdateUiList();
    }
}

// utility to add a bookmark from the current path
void UIFinderWindow::CreateBookmark ( const String& bkname, const String&  bkpath ) 
{
    UISelectList *bklist = static_cast<UISelectList *>(GetBookmarksWidget());
    if (bklist ) 
    {
        int inspos = bklist->GetNumItems();
        if ( inspos < 0 ) inspos = 0;
        String idz = "BKM" + String(inspos);
        if ( bklist->AddItem ( inspos, bkname, idz ) )
        {
            bookmarks_.Push(bkname);
            bookmarkPaths_.Push (bkpath);
            bookmarksDirty_ = 1;
        }
    }
}

// removes a bookmark based upon its index in the bookmark list
void UIFinderWindow::DeleteBookmark ( int bkindex )
{
    UISelectList *bklist = static_cast<UISelectList *>(GetBookmarksWidget());
    if (bklist && bkindex >= 0) 
    {
        bklist->DeleteItem(bkindex);
        bookmarks_.Erase(bkindex,1);
        bookmarkPaths_.Erase(bkindex,1);
        resultPath_ = "";
        SetCurrentPath (bookmarkPaths_[0]);
        UpdateUiPath();
        UpdateUiList();
        UpdateUiResult();
        bklist->SetValue(-1);
        bookmarksDirty_ = 1;
    }
}

void UIFinderWindow::LoadBookmarks()
{
    FileSystem* filesystem = GetSubsystem<FileSystem>();
    String bkdata = "";
    String bkpath = "";
    
#if defined(ATOMIC_PLATFORM_ANDROID) || defined(ATOMIC_PLATFORM_IOS)
    bkpath = filesystem->GetUserDocumentsDir(); // somewhere writable on mobile
#else
    bkpath = filesystem->GetAppPreferencesDir("AtomicGameEngine", "Bookmarks"); // desktop systems
#endif
    
    bkpath += "/Bookmarks.txt";
    if ( filesystem->FileExists ( bkpath ) )
    {
        File *fp = new File (context_, bkpath, FILE_READ);
        if(fp->IsOpen())
        {
            fp->ReadText(bkdata);
            fp->Close();
        }
    }
    char delim = '\n';
    Vector <String> tokens = bkdata.Split(delim, false);
    int nn=0;
    for ( nn=0; nn< tokens.Size(); nn+=2)
    {
        CreateBookmark ( tokens[nn], tokens[nn+1] );
    }
    bookmarksDirty_ = 0;

}

void UIFinderWindow::SaveBookmarks()
{
    if ( bookmarksDirty_ > 0 )
    {
        FileSystem* filesystem = GetSubsystem<FileSystem>();
        String bkpath = "";

#if defined(ATOMIC_PLATFORM_ANDROID) || defined(ATOMIC_PLATFORM_IOS)
        bkpath = filesystem->GetUserDocumentsDir();
#else
        bkpath = filesystem->GetAppPreferencesDir("AtomicGameEngine", "Bookmarks");
#endif

        bkpath += "/Bookmarks.txt";
        String bkdata = "";
        int nn=0, sep=-1;
        for ( nn = 0; nn<bookmarks_.Size(); nn++)
        {
            if ( sep > 0 )
            {
                bkdata += bookmarks_[nn];
                bkdata += "\n";
                bkdata += bookmarkPaths_[nn];
                bkdata += "\n";
            }
            if ( bookmarks_[nn] == "-" ) 
                sep = nn;
        }
        File *fp = new File (context_, bkpath, FILE_WRITE);
        if(fp->IsOpen())
        {
            fp->Write ((const void *)bkdata.CString(), bkdata.Length() );
            fp->Close();
        }
    }
}

}
