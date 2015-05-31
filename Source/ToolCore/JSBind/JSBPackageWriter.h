#pragma once

#include <Atomic/Container/Str.h>

using namespace Atomic;

namespace ToolCore
{

class JSBPackage;
class JSBClass;

class JSBPackageWriter
{

public:

    JSBPackageWriter(JSBPackage* package);

    void GenerateSource(String& sourceOut);

private:

    void WriteProtoTypeRecursive(String &source, JSBClass* klass,  Vector<JSBClass*>& written);
    void WriteProtoTypeSetup(String& source);

    JSBPackage* package_;

};

}
