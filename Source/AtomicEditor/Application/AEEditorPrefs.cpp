//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

#include <Atomic/Core/Context.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Graphics/Graphics.h>

#include "AEEditorPrefs.h"

namespace AtomicEditor
{

    AEEditorPrefs::AEEditorPrefs(Context* context) :
        Object(context)
    {

    }

    AEEditorPrefs::~AEEditorPrefs()
    {

    }

    bool AEEditorPrefs::CreateDefaultPreferences(String& path, JSONValue& prefs)
    {
        // Note there is some duplication here with the editor's
        // TypeScript preference code, this is due to the preferences for
        // the editor window needing to be available at window creation time
        // It could be better to split this all out to a native, scriptable
        // preferences object

        ATOMIC_LOGINFOF("Creating default Atomic Editor preferences: %s", path.CString());

        SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

        JSONValue& root = jsonFile->GetRoot();

        root.Clear();
        root["recentProjects"] = JSONArray();

        JSONValue editorWindow;
        GetDefaultWindowPreferences(editorWindow, true);

        JSONValue playerWindow;
        GetDefaultWindowPreferences(playerWindow, false);

        root["editorWindow"] = editorWindow;
        root["playerWindow"] = playerWindow;

        prefs = root;

        SavePreferences(prefs);

        return true;
    }

    bool AEEditorPrefs::ReadPreferences(VariantMap& engineParameters)
    {
        FileSystem* fileSystem = GetSubsystem<FileSystem>();
        String path = GetPreferencesPath();

        JSONValue prefs;

        LoadPreferences(prefs);

        if (!prefs.IsObject() || !prefs["editorWindow"].IsObject())
        {
            if (!CreateDefaultPreferences(path, prefs))
                return false;
        }

        JSONValue& editorWindow = prefs["editorWindow"];

        engineParameters["WindowPositionX"] = editorWindow["x"].GetUInt();
        engineParameters["WindowPositionY"] = editorWindow["y"].GetUInt();
        engineParameters["WindowWidth"] = editorWindow["width"].GetUInt();
        engineParameters["WindowHeight"] = editorWindow["height"].GetUInt();
        engineParameters["WindowMaximized"] = editorWindow["maximized"].GetBool();

        return true;
    }

    void AEEditorPrefs::ValidateWindow()
    {
        Graphics* graphics = GetSubsystem<Graphics>();
        IntVector2 windowPosition = graphics->GetWindowPosition();
        int monitors = graphics->GetNumMonitors();
        IntVector2 maxResolution;

        for (int i = 0; i < monitors; i++)
        {
            IntVector2 monitorResolution = graphics->GetMonitorResolution(i);
            maxResolution += monitorResolution;
        }

        if (windowPosition.x_ >= maxResolution.x_ || windowPosition.y_ >= maxResolution.y_ || (windowPosition.x_ + graphics->GetWidth()) < 0 || (windowPosition.y_ + graphics->GetHeight()) < 0)
        {
            JSONValue prefs;

            if (!LoadPreferences(prefs))
                return;

            bool editor = context_->GetEditorContext();

            JSONValue window;
            GetDefaultWindowPreferences(window, editor);

            prefs[editor ? "editorWindow" : "playerWindow"] = window;


            // TODO: add highDPI support
            bool highDPI = false;

            //Setting the mode to 0 width/height will use engine defaults for window size and layout
            graphics->SetMode(0, 0, graphics->GetFullscreen(), graphics->GetBorderless(), graphics->GetResizable(), highDPI, graphics->GetVSync(), graphics->GetTripleBuffer(), graphics->GetMultiSample());

            SavePreferences(prefs);
        }
    }

    void AEEditorPrefs::GetDefaultWindowPreferences(JSONValue& windowPrefs, bool maximized)
    {
        windowPrefs["x"] = 0;
        windowPrefs["y"] = 0;
        windowPrefs["width"] = 0;
        windowPrefs["height"] = 0;
        windowPrefs["monitor"] = 0;
        windowPrefs["maximized"] = maximized;
    }

    String AEEditorPrefs::GetPreferencesPath()
    {
        FileSystem* fileSystem = GetSubsystem<FileSystem>();
        String path = fileSystem->GetAppPreferencesDir("AtomicEditor", "Preferences");
        path += "prefs.json";
        return path;
    }

    bool AEEditorPrefs::LoadPreferences(JSONValue& prefs)
    {
        FileSystem* fileSystem = GetSubsystem<FileSystem>();
        String path = GetPreferencesPath();

        if (!fileSystem->FileExists(path))
        {
            if (!CreateDefaultPreferences(path, prefs))
                return false;
        }
        else
        {
            SharedPtr<File> file(new File(context_, path, FILE_READ));
            SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

            if (!jsonFile->BeginLoad(*file))
            {
                file->Close();
                if (!CreateDefaultPreferences(path, prefs))
                    return false;
            }
            else
            {
                prefs = jsonFile->GetRoot();
            }

            file->Close();
        }

        return true;
    }

    bool AEEditorPrefs::SavePreferences(JSONValue& prefs)
    {
        FileSystem* fileSystem = GetSubsystem<FileSystem>();
        String path = GetPreferencesPath();

        SharedPtr<File> file(new File(context_, path, FILE_WRITE));
        SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

        jsonFile->GetRoot() = prefs;

        if (!file->IsOpen())
        {
            ATOMIC_LOGERRORF("Unable to open Atomic Editor preferences for writing: %s", path.CString());
            return false;
        }

        jsonFile->Save(*file, "   ");
        file->Close();

        return true;
    }

}
