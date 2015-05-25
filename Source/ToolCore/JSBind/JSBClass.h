
#pragma once

#include <Atomic/Core/Object.h>

#include "JSBHeader.h"

using namespace Atomic;

namespace ToolCore
{

class JSBModule;
class JSBFunction;

class JSBClass : public Object
{

    OBJECT(JSBClass)

public:

    JSBClass(Context* context, JSBModule* module, const String& name, const String& nativeName);
    virtual ~JSBClass();

    const String& GetName() { return name_; }
    const String& GetNativeName() { return nativeName_; }

    void SetHeader(JSBHeader* header) { header_ = header; }

    void AddBaseClass(JSBClass* baseClass) { baseClasses_.Push(baseClass); }
    void SetAbstract() { isAbstract_ = true; }

    void AddFunction(JSBFunction* function);

private:

    bool isAbstract_;

    String name_;
    String nativeName_;

    SharedPtr<JSBHeader> header_;
    SharedPtr<JSBModule> module_;


    PODVector<JSBFunction*> functions;
    PODVector<JSBClass*> baseClasses_;

};


}
