#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/JSONFile.h>

#include "ProjectSettings.h"

namespace ToolCore
{

    ProjectSettings::ProjectSettings(Context* context) : Object(context),
        desktopSettings_(new DesktopProjectSettings()),
        androidSettings_(new AndroidProjectSettings())
    {
        SetDefault();
    }

    ProjectSettings::~ProjectSettings()
    {

    }

    void ProjectSettings::SetDefault()
    {
        name_ = "AtomicProject";
        platforms_.Clear();
        platforms_.Push("desktop");
    }

    bool ProjectSettings::GetSupportsDesktop() const
    {
        return platforms_.Contains("desktop");
    }

    bool ProjectSettings::GetSupportsAndroid() const
    {
        return platforms_.Contains("android");
    }

    bool ProjectSettings::GetSupportsIOS() const
    {
        return platforms_.Contains("ios");
    }

    bool ProjectSettings::GetSupportsWeb() const
    {
        return platforms_.Contains("web");
    }

    bool ProjectSettings::GetSupportsLinux() const
    {
        return platforms_.Contains("linux");
    }

    bool ProjectSettings::GetSupportsPlatform(const String& platform) const
    {
        return platforms_.Contains(platform);
    }

    void ProjectSettings::AddSupportedPlatform(const String& platform)
    {
        if (!ValidPlatform(platform))
        {
            ATOMIC_LOGERRORF("ProjectPlatformSettings::AddSupportedPlatform - Attempting to add invalid platform: %s", platform.CString());
            return;
        }

        if (platforms_.Contains(platform))
            return;

        platforms_.Push(platform);
    }

    bool ProjectSettings::ValidPlatform(const String& platform) const
    {
        if (platform == "desktop")
            return true;
        if (platform == "android")
            return true;
        if (platform == "ios")
            return true;
        if (platform == "linux")
            return true;
        if (platform == "web")
            return true;

        return false;

    }

    bool ProjectSettings::Load(const String& path)
    {
        FileSystem* fileSystem = GetSubsystem<FileSystem>();

        if (!fileSystem->FileExists(path))
        {
            ATOMIC_LOGERRORF("No platform settings specified, using default: %s", path.CString());
            SetDefault();
            return true;
        }

        SharedPtr<File> file(new File(context_, path));
        if (!file->IsOpen())
        {
            ATOMIC_LOGERRORF("Unable to open platform settings: %s", path.CString());
            return false;
        }

        SharedPtr<JSONFile> jsonFile(new JSONFile(context_));
        bool result = jsonFile->Load(*file);
        file->Close();

        if (!result)
        {
            ATOMIC_LOGERRORF("Unable to load platform settings: %s", path.CString());
            return false;
        }

        JSONValue root = jsonFile->GetRoot();
        if (!root.IsObject())
        {
            ATOMIC_LOGERRORF("No root object in platform settings: %s", path.CString());
            return false;
        }

        JSONArray platforms = root["platforms"].GetArray();

        platforms_.Clear();

        if (!platforms.Size())
        {
            ATOMIC_LOGERRORF("No platforms array defined in platform settings: %s, using default", path.CString());
            SetDefault();
        }
        else
        {
            for (unsigned i = 0; i < platforms.Size(); i++)
            {
                const String& platform = platforms[i].GetString();

                if (!ValidPlatform(platform))
                {
                    ATOMIC_LOGERRORF("Unknown platform %s in platform settings: %s, skipping", platform.CString(), path.CString());
                    continue;
                }

                platforms_.Push(platform);
            }
        }

        if (!platforms_.Size())
        {
            ATOMIC_LOGERRORF("No valid platforms defined in platform settings: %s, using default", path.CString());
            SetDefault();
        }

        name_ = root["name"].GetString();

        if (!name_.Length())
            name_ = "AtomicProject";

        desktopSettings_->Read(root);
        androidSettings_->Read(root);

        return true;
    }

    void ProjectSettings::Save(const String& path)
    {
        SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

        JSONValue& root = jsonFile->GetRoot();

        SharedPtr<File> file(new File(context_, path, FILE_WRITE));

        desktopSettings_->Write(root);
        androidSettings_->Write(root);

        jsonFile->Save(*file, String("   "));

        file->Close();

    }

}
