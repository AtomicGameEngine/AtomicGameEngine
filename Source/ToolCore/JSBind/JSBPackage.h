
#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
{

class JSBModule;
class JSBClass;
class JSBEnum;

class JSBPackage : public Object
{

    OBJECT(JSBPackage)

public:

    JSBPackage(Context* context);
    virtual ~JSBPackage();

    bool Load(const String& packageFolder);

    void PreprocessModules();

    JSBClass* GetClass(const String& name);

    // get a class by name across all loaded packages
    static JSBClass* GetClassAllPackages(const String& name);

    JSBEnum* GetEnum(const String& name);

    // get an enum by name across all loaded packages
    static JSBEnum* GetEnumAllPackages(const String& name);

    bool ContainsConstant(const String& constantName);

    static bool ContainsConstantAllPackages(const String& constantName);


private:

    // The name of the package
    String name_;

    // The name of the C++ namespace if any
    String namespace_;

    Vector<SharedPtr<JSBPackage> > dependencies_;

    Vector<SharedPtr<JSBModule> > modules_;

    static Vector<SharedPtr<JSBPackage> > allPackages_;

};


}
