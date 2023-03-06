#pragma once

#include <stdint.h>
#include "kernel.h"

struct Point
{
    unsigned int x;
    unsigned int y;
};

class BasicRenderer
{
    public:
        Point CursorPosition;
        Framebuffer* framebuffer;
        PSF1_FONT* psf1_font;

        BasicRenderer(Framebuffer* framebuffer, PSF1_FONT* psf1_font);

        unsigned int color;
        bool overwrite = false;

        void putChar(char chr, int64_t xoff, int64_t yoff, uint32_t fg, uint32_t bg);
        void putChar(char chr, int64_t xoff, int64_t yoff);
        void delChar(int64_t xoff, int64_t yoff, uint32_t col);
        void delChar(int64_t xoff, int64_t yoff);
        void putStr(const char* chrs, int64_t xoff, int64_t yoff);
        void putStr(const char *chrs, int64_t xoff, int64_t yoff, uint32_t col);

        void Clear(bool resetCursor);
};
