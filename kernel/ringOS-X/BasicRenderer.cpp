#include "BasicRenderer.hpp"

BasicRenderer::BasicRenderer(Framebuffer* framebuffer, PSF1_FONT* psf1_font)
{
    color = 0xffffffff;
    CursorPosition = {0, 0};
    this->framebuffer = framebuffer;
    this->psf1_font = psf1_font;
}

void BasicRenderer::putChar(char chr, int64_t xoff, int64_t yoff, uint32_t fg, uint32_t bg)
{
    delChar(xoff, yoff, bg);
    uint32_t tcol = color;
    bool toverwrite = overwrite;
    color = fg;
    overwrite = false;
    putChar(chr, xoff, yoff);
    overwrite = toverwrite;
    color = tcol;
}

void BasicRenderer::putChar(char chr, int64_t xoff, int64_t yoff)
{
    unsigned int *pixPtr = (unsigned int *)framebuffer->BaseAddress;
    char *fontPtr = ((char *)psf1_font->glyphBuffer) + (chr * psf1_font->psf1_Header->charsize);

    if (overwrite)
        BasicRenderer::delChar(xoff, yoff);

    for (int64_t y = yoff; y < yoff + 16; y++)
    {
        for (int64_t x = xoff; x < xoff + 8; x++)
        {
            if (x >= 0 && x < framebuffer->Width && y >= 0 && y < framebuffer->Height)
                if ((*fontPtr & (0b10000000 >> (x - xoff))) > 0)
                {
                    *(unsigned int *)(pixPtr + x + (y * framebuffer->PixelsPerScanLine)) = color;
                }
        }
        fontPtr++;
    }
}

void BasicRenderer::delChar(int64_t xoff, int64_t yoff, uint32_t col)
{
    unsigned int *pixPtr = (unsigned int *)framebuffer->BaseAddress;

    for (int64_t y = yoff; y < yoff + 16; y++)
        for (int64_t x = xoff; x < xoff + 8; x++)
            if (x >= 0 && x < framebuffer->Width && y >= 0 && y < framebuffer->Height)
                *(uint32_t*)(pixPtr + x + (y * framebuffer->PixelsPerScanLine)) = col;
}

void BasicRenderer::delChar(int64_t xoff, int64_t yoff)
{
    BasicRenderer::delChar(xoff, yoff, 0x00000000);
}

void BasicRenderer::putStr(const char *chrs, int64_t xoff, int64_t yoff)
{
    for (unsigned int x = 0; chrs[x] != 0; x++)
        putChar(chrs[x], xoff + (x * 8), yoff);
}

void BasicRenderer::putStr(const char *chrs, int64_t xoff, int64_t yoff, uint32_t col)
{
    uint32_t tcol = color;
    color = col;
    for (unsigned int x = 0; chrs[x] != 0; x++)
        putChar(chrs[x], xoff + (x * 8), yoff);
    color = tcol;
}

void BasicRenderer::Clear(uint32_t color, bool resetCursor)
{
    uint64_t fbBase = (uint64_t)framebuffer->BaseAddress;
    uint64_t pxlsPerScanline = framebuffer->PixelsPerScanLine;
    uint64_t fbHeight = framebuffer->Height;

    for (int64_t y = 0; y < framebuffer->Height; y++)
    {
        for (int64_t x = 0; x < framebuffer->Width; x++)
        {
            *((uint32_t*)(fbBase + 4 * (x + pxlsPerScanline * y))) = color;
        }
    }

    if (resetCursor)
    {
        CursorPosition = {0, 0};
    }
}
