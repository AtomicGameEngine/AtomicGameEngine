
#include <include/cef_client.h>

#include "WebString.h"

namespace Atomic
{

bool ConvertCEFString(const CefString& cefString, String& output)
{
    output = "Failed to Convert CefString";

    cef_string_utf8_t utf8;
    memset(&utf8, 0, sizeof(utf8));

    if (!cef_string_utf16_to_utf8(cefString.c_str(), cefString.length(), &utf8))
    {
        cef_string_utf8_clear(&utf8);
        return false;
    }

    output = utf8.str;
    cef_string_utf8_clear(&utf8);

    return true;
}

}
