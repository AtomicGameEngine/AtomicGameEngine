
#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace AtomicEditor
{

class MyJSClass : public Object
{
    OBJECT(MyJSClass);

public:
    /// Construct.
    MyJSClass(Context* context);
    virtual ~MyJSClass();

    const String& GetAha() { return aha_; }

private:

    String aha_;


};

}
