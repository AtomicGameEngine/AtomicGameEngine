
#pragma once

#include "../Core/Variant.h"

namespace Atomic
{
  struct EnumInfo
  {
      EnumInfo(const String& name = String::EMPTY, const Variant& v = Variant::EMPTY)
      {
          name_ = name;
          value_ = v;
      }

      String name_;
      Variant value_;
  };

}
