#include "kernel.h"
#include "../e9print.h"

#include "BasicRenderer.hpp"

BasicRenderer *GlobalRenderer;
BasicRenderer r = BasicRenderer(NULL, NULL);
int ringOSX(Framebuffer framebuffer, PSF1_FONT* psf1_font)
{
    r = BasicRenderer(&framebuffer, psf1_font);
    GlobalRenderer = &r;

    GlobalRenderer->Clear(true);
    GlobalRenderer->putStr("ovo je test\n", 60, 70);

    return 0;
}
