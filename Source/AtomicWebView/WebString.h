
#pragma once

#include <Atomic/Container/Str.h>

namespace Atomic
{

/// Convert a CEF3 string to a UTF8 String
bool ConvertCEFString(const CefString& cefString, String& output);

}

