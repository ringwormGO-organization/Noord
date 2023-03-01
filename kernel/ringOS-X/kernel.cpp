#include "kernel.h"
#include "../e9print.h"

int ringOSX(Framebuffer framebuffer)
{
    e9_printf("%d | %d", framebuffer.Width, framebuffer.Height);
    return 0;
}
