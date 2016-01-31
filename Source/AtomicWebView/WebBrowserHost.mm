#include <Cocoa/Cocoa.h>

namespace Atomic
{

void *GetNSWindowContentView(void *window)
{
    return ((NSWindow*)window).contentView;
}

}
