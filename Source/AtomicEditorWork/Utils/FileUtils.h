
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

    void OpenProjectFileDialog();
    String NewProjectFileDialog();
    void RevealInFinder(const String& fullpath);

private:

};


}
