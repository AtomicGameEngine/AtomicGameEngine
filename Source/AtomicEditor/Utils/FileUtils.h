
#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace AtomicEditor
{

class FileUtils : public Object
{
    OBJECT(FileUtils);

public:

    /// Construct.
    FileUtils(Context* context);
    /// Destruct.
    ~FileUtils();

    bool CreateDirs(const String& folder);

    String GetMobileProvisionPath();
    String GetAndroidSDKPath(const String& defaultPath);
    String OpenProjectFileDialog();
    String NewProjectFileDialog();
    String GetBuildPath(const String& defaultPath);
    void RevealInFinder(const String& fullpath);

private:

};


}
