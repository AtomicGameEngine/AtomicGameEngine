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

#pragma once

#include <TurboBadger/tb_atomic_widgets.h>

#include "UIWindow.h"

namespace Atomic
{

class UIPromptWindow;

class UIFinderWindow : public UIWindow
{
    ATOMIC_OBJECT(UIFinderWindow, UIWindow)

public:

    UIFinderWindow(Context* context, UIWidget* target, const String& id, bool createWidget = true);
    virtual ~UIFinderWindow();

    void FindFile(const String& title, const String& preset, int dimmer=0, int width=0, int height=0);
    void FindPath(const String& title, const String& preset, int dimmer=0, int width=0, int height=0);

protected:

    void HandleCreateBookmark(StringHash eventType, VariantMap& eventData); /// event handler for bookmark creation
    void HandleCreateFolder(StringHash eventType, VariantMap& eventData); /// event handler for folder creation
    virtual bool OnEvent(const tb::TBWidgetEvent &ev); /// general event handler
    UIWidget* GetWindowWidget(); /// get internal widgets
    UIWidget* GetPathWidget();
    UIWidget* GetResultWidget();
    UIWidget* GetBookmarksWidget();
    UIWidget* GetFilesWidget();
   
private:

    void PresetCurrentPath( const String& preset );  /// location where the finder starts
    void SetCurrentPath( const String& string );  /// set the current path value
    void ComposePath (const String& string );  /// using the list, jam things together, we'll either get another path or a file.
    void CreateBookmarks(); /// create the list of bookmarks, per platform
    void GoFolderUp(); /// go up one folder
    void UpdateUiPath ();  /// move current path to widget
    void UpdateUiResult (); /// move result path to widget
    void UpdateUiList(); /// move folder contents into list
    void CreateFolder( const String& string ); /// utility to add a folder in current path
    void CreateBookmark ( const String& bkname, const String&  bkpath ); /// utility to add a bookmark from the current path
    void DeleteBookmark ( int bkindex ); /// removes a bookmark based upon its index in the bookmark list
    void LoadBookmarks();  /// load user created bookmarks from file
    void SaveBookmarks();  /// save user created bookmarks to file

    int finderMode_;  /// finder mode, 0 = find file, 1 = find folder
    String currentPath_; /// current path for the finder
    String resultPath_;  /// result file for finder
    Vector <String> bookmarks_;  /// array of names of bookmarks
    Vector <String> bookmarkPaths_; /// array of bookmark paths
    WeakPtr<UIPromptWindow> newBookmarkPtr_;  /// pointer for bookmark prompt window
    WeakPtr<UIPromptWindow> newFolderPtr_; /// pointer for new folder prompt window
    int bookmarksDirty_;  /// flag to track if the bookmark array has changed

};



}
