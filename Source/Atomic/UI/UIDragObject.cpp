
#include "UIDragObject.h"

namespace Atomic
{

UIDragObject::UIDragObject(Context* context, Object* object, const String &text, const String &icon) : Object(context),
    text_(text),
    icon_(icon),
    object_(object)
{

}

UIDragObject::~UIDragObject()
{

}


}
