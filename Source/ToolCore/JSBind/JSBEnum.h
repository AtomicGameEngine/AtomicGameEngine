
#pragma once

#include <Atomic/Core/Object.h>

#include "JSBModule.h"

using namespace Atomic;

namespace ToolCore
{

class JSBHeader;

class JSBEnum : public Object
{

    OBJECT(JSBEnum)

public:

    JSBEnum(Context* context, JSBModule* module, const String& name);    
    virtual ~JSBEnum();

    const String& GetName() { return name_; }
    JSBHeader* GetHeader() { return header_; }
    JSBModule* GetModule() { return module_; }
    JSBPackage* GetPackage() { return module_->GetPackage(); }

    Vector<String>& GetValues() { return values_; }

    void SetHeader(JSBHeader* header) { header_ = header; }

    void Preprocess();

    void AddValue(const String& value) { values_.Push(value); }

private:

    String name_;

    SharedPtr<JSBModule> module_;

    Vector<String> values_;

    JSBHeader* header_;

};


}
