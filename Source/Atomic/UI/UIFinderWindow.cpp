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
    finderMode(0),
    currentPath(),
    resultPath(),
    bookmarks(),
    bookmarkPaths(),
    newBookmarkPtr(),
    newFolderPtr(),
    bookmarksDirty(0)
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
    
    if ( !newFolderPtr.Expired())
        newFolderPtr->UnsubscribeFromAllEvents();
    if ( !newBookmarkPtr.Expired() )
       newBookmarkPtr->UnsubscribeFromAllEvents();

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

    finderMode = 1;
    
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

    if ( ev.type == EVENT_TYPE_CHANGED && ev.target && ((uint32)ev.target->GetID()) == 1 ) 
    {
        FileSystem* filesystem = GetSubsystem<FileSystem>();
        UIWidget *pathwidget = GetPathWidget();  // paste or type in currentpath widget
        if(pathwidget)
        {
            if ( filesystem->DirExists (pathwidget->GetText())) 
            {
                if ( pathwidget->GetText() != currentPath ) 
                {
                    resultPath = "";
                    SetCurrentPath (pathwidget->GetText());
                    UpdateUiList();
                    UpdateUiResult();
                }
            }
        }
        return true;
    }

   if ( ev.type == EVENT_TYPE_POINTER_UP && ev.target && ((uint32)ev.target->GetID()) == 2 ) // go up
    {
        GoFolderUp();
        return true;
    }

    if ( ev.type == EVENT_TYPE_POINTER_UP && ev.target && ((uint32)ev.target->GetID()) == 3 ) // add bookmark request
    {
        // this check is necessary because you can kill the bookmark window with the "X" and
        // this kills the UI but not the newBookmarkPtr to it, and it doesnt get cleaned up properly.
        if ( newBookmarkPtr.NotNull() )
        {
            newBookmarkPtr->UnsubscribeFromAllEvents();
            newBookmarkPtr.Reset();
        }
        newBookmarkPtr = new UIPromptWindow(context_, this, "createbookmark",  true);
        SubscribeToEvent(newBookmarkPtr, E_UIPROMPTCOMPLETE, ATOMIC_HANDLER(UIFinderWindow, HandleCreateBookmark ));

        String prospect = "";
        char delim = '/';
        Vector <String> tokens = currentPath.Split(delim, false);
        prospect = tokens[ tokens.Size()-1 ]; // get the last folder name as preset
    
        newBookmarkPtr->Show("Create New Bookmark", "Enter a name for the new bookmark", prospect );
        return true;
    }

    if ( ev.type == EVENT_TYPE_POINTER_UP && ev.target && ((uint32)ev.target->GetID()) == 4 ) // add folder request
    {
        if (newFolderPtr.NotNull())
        {
            newFolderPtr->UnsubscribeFromAllEvents();
            newFolderPtr.Reset();
        }
        newFolderPtr = new UIPromptWindow(context_, this, "createfolder",  true);
        SubscribeToEvent(newFolderPtr, E_UIPROMPTCOMPLETE, ATOMIC_HANDLER(UIFinderWindow, HandleCreateFolder));
        newFolderPtr->Show("Create new folder", "Enter a name for the new folder", "" );
        return true;
    }

    if ( ev.type == EVENT_TYPE_CLICK && ev.target && ((uint32)ev.target->GetID()) == 5 ) // clicked in bookmarks
    {
        UISelectList *bklist = static_cast<UISelectList *>(GetBookmarksWidget());
        int selected = bklist->GetValue();
        if ( selected >= 0 )
        {
            resultPath = "";  // were going back, give up file.
            SetCurrentPath ( bookmarkPaths[selected]);
            UpdateUiPath();
            UpdateUiList();
            UpdateUiResult();
            bklist->SetValue(-1); // clear the select visuals
        }
        return true;
    }
    
    if ( ev.type == EVENT_TYPE_CUSTOM && ev.target && ((uint32)ev.target->GetID()) == 5 ) // bookmarks TB context menu result
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

    if ( ev.type == EVENT_TYPE_CLICK && ev.target && ((uint32)ev.target->GetID()) == 6 ) // clicked dirfiles list
    {
        UISelectList *filelist = static_cast<UISelectList *>(GetFilesWidget());
        ComposePath( filelist->GetSelectedItemString() );
        return true;
    }

    if ( ev.type == EVENT_TYPE_CLICK && (ev.ref_id == 8 || ev.ref_id == 9) ) // clicked 8 = ok button, 9 = cancel button
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

        if (ev.ref_id == 8) // ok button was pressed, otherwise it was cancel button
        {
            eventData[UIFinderComplete::P_REASON] = "OK";
            if ( finderMode == 0 ) // finding a file
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
    String Title = eventData["Title"].GetString();
    String Reason = eventData["Reason"].GetString();
    String Selected = eventData["Selected"].GetString();
    if( Reason == "OK" )
        CreateBookmark( Selected, currentPath );
    if (newBookmarkPtr)
    {
        newBookmarkPtr->UnsubscribeFromAllEvents();
        newBookmarkPtr.Reset();
    }
}

void UIFinderWindow::HandleCreateFolder(StringHash eventType, VariantMap& eventData)
{
    String Title = eventData["Title"].GetString();
    String Reason = eventData["Reason"].GetString();
    String Selected = eventData["Selected"].GetString();
    if( Reason == "OK" )
        CreateFolder( Selected); 
    if (newFolderPtr)
    {
         newFolderPtr->UnsubscribeFromAllEvents();
         newFolderPtr.Reset();
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
    TBWidget* child = (TBWidget*) widget_->GetWidgetByIDAndType<TBEditField>(1);
    if (!child)
        return 0;
    UI* ui = GetSubsystem<UI>();
    return ui->WrapWidget(child);
}

UIWidget* UIFinderWindow::GetResultWidget()
{
    if (!widget_)
        return 0;
    TBWidget* child = (TBWidget*)widget_->GetWidgetByIDAndType<TBEditField>(7);
    if (!child)
        return 0;
    UI* ui = GetSubsystem<UI>();
    return ui->WrapWidget(child);
}

UIWidget* UIFinderWindow::GetBookmarksWidget()
{
    if (!widget_)
        return 0;
    TBWidget* child = (TBWidget*)widget_->GetWidgetByIDAndType<TBSelectList>(5);
    if (!child)
        return 0;
    UI* ui = GetSubsystem<UI>();
    return ui->WrapWidget(child);
}

UIWidget* UIFinderWindow::GetFilesWidget()
{
    if (!widget_)
        return 0;
    TBWidget* child = (TBWidget*)widget_->GetWidgetByIDAndType<TBSelectList>(6);
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
    currentPath = string;
}

//using the list, jam things together, we'll either get another path or a file.
void UIFinderWindow::ComposePath (const String& string )
{
    String prospect = currentPath + string;

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
        resultPath = prospect;
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
    Vector <String> tokens = currentPath.Split(delim, false);
 
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

    if ( prospect != currentPath ) 
    {
        resultPath = "";
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
        if ( pathwidget->GetText() != currentPath )
            pathwidget->SetText(currentPath);
    }
}

// move result path to widget
void UIFinderWindow::UpdateUiResult ()
{
    UIWidget *resultwidget = GetResultWidget();
    if( resultwidget)
    {
        if ( resultwidget->GetText() != resultPath )
            resultwidget->SetText(resultPath);
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
    
    if ( filesystem->DirExists (currentPath ) )
    { 
        Vector <String> mydirs;
        int nn = 0;
        filesystem->ScanDir (mydirs,currentPath, "*", SCAN_DIRS, false );
        Sort(mydirs.Begin(), mydirs.End(), CompareStrs); // sort them
        for ( nn=0; nn<mydirs.Size(); nn++ ) 
        {
            if ( mydirs[nn] == "." ) continue;  
            if ( mydirs[nn] == ".." ) continue;
            String idz = "DIR"+ String(nn);
            fileSource->AddItem( new UISelectItem(context_, mydirs[nn], idz, "FolderIcon" ));
        }
        Vector <String> myfiles;
        filesystem->ScanDir (myfiles, currentPath, "*", SCAN_FILES, false );
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
    if ( filesystem->CreateDir( currentPath + string ) )
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
            bookmarks.Push(bkname);
            bookmarkPaths.Push (bkpath);
            bookmarksDirty = 1;
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
        bookmarks.Erase(bkindex,1);
        bookmarkPaths.Erase(bkindex,1);
        resultPath = "";
        SetCurrentPath (bookmarkPaths[0]);
        UpdateUiPath();
        UpdateUiList();
        UpdateUiResult();
        bklist->SetValue(-1);
        bookmarksDirty = 1;
    }
}

void UIFinderWindow::LoadBookmarks()
{
    FileSystem* filesystem = GetSubsystem<FileSystem>();
    String bkdata = "";
    String bkpath = "";
    
#if defined(ANDROID) || defined( __ANDROID__) || defined(IOS)
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
    bookmarksDirty = 0;

}

void UIFinderWindow::SaveBookmarks()
{
    if ( bookmarksDirty > 0 )
    {
        FileSystem* filesystem = GetSubsystem<FileSystem>();
        String bkpath = "";

#if defined(ANDROID) || defined( __ANDROID__) || defined(IOS)
        bkpath = filesystem->GetUserDocumentsDir();
#else
        bkpath = filesystem->GetAppPreferencesDir("AtomicGameEngine", "Bookmarks");
#endif

        bkpath += "/Bookmarks.txt";
        String bkdata = "";
        int nn=0, sep=-1;
        for ( nn = 0; nn<bookmarks.Size(); nn++)
        {
            if ( sep > 0 )
            {
                bkdata += bookmarks[nn];
                bkdata += "\n";
                bkdata += bookmarkPaths[nn];
                bkdata += "\n";
            }
            if ( bookmarks[nn] == "-" ) 
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
