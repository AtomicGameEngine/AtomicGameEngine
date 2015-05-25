
#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
{

class JSBModule;

class JSBEnum : public Object
{

    OBJECT(JSBEnum)

public:

    JSBEnum(Context* context, JSBModule* module, const String& name);
    virtual ~JSBEnum();

    const String& GetName() { return name_; }

    void Preprocess();

    void AddValue(const String& value) { values_.Push(value); }

private:

    String name_;

    SharedPtr<JSBModule> module_;

    Vector<String> values_;

};


}
