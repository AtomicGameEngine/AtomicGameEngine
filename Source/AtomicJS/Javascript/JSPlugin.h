
#pragma once

#include <Atomic/Container/Str.h>

namespace Atomic
{
    class JSVM;

    void js_init_jsplugin(JSVM* vm);
    bool jsplugin_load(JSVM* vm, const String& pluginLibrary);

}