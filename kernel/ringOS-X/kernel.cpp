#include "kernel.h"
#include "../e9print.h"

#include "BasicRenderer.hpp"
#include "paging/PageFrameAllocator.h"
#include "paging/PageMapIndexer.h"
#include "paging/PageTableManager.h"
#include "paging/paging.h"

static PageFrameAllocator t = PageFrameAllocator();

#define USER_END 0x0000007fffffffff
#define TRANSLATED_PHYSICAL_MEMORY_BEGIN 0xffff800000000000llu
#define MMIO_BEGIN 0xffffffff00000000llu
#define KERNEL_TEMP_BEGIN 0xffffffff40000000llu
#define KERNEL_DATA_BEGIN 0xffffffff80000000llu
#define KERNEL_HEAP_BEGIN 0xffffffff80300000llu

static inline uint64_t earlyVirtualToPhysicalAddr(const void *vAddr)
{
    if ((0xfffff00000000000llu & (uint64_t)vAddr) == TRANSLATED_PHYSICAL_MEMORY_BEGIN)
        return ~TRANSLATED_PHYSICAL_MEMORY_BEGIN & (uint64_t)vAddr;
    else
        return ~KERNEL_DATA_BEGIN & (uint64_t)vAddr;
}

void PrepareMemory(Framebuffer framebuffer, void *freeMemStart, void *extraMemStart, uint64_t freeMemSize, void *kernelStart, uint64_t kernelSize, void *kernelStartV)
{
    GlobalAllocator = &t;
    GlobalAllocator->ReadEFIMemoryMap(freeMemStart, freeMemSize);

    uint64_t rFB = earlyVirtualToPhysicalAddr(framebuffer.BaseAddress);

    PageTable *PML4 = (PageTable *)GlobalAllocator->RequestPage();
    memset(PML4, 0, 0x1000);
    asm volatile("mov %%cr3, %0"
                 : "=r"(PML4));

    GlobalPageTableManager = PageTableManager(PML4);

    uint64_t fbBase = (uint64_t)framebuffer.BaseAddress;
    uint64_t fbSize = (uint64_t)framebuffer.BufferSize;

    for (uint64_t i = fbBase; i < fbBase + fbSize; i += 4096)
    {
        GlobalPageTableManager.MapMemory((void *)i, (void *)i, false);
    }

    int32_t testing[0x1000 / 4];
    testing[0] = 123;
}

BasicRenderer r = BasicRenderer(NULL, NULL);
int ringOSX(Framebuffer framebuffer, PSF1_FONT *psf1_font, void *freeMemStart, void *extraMemStart, uint64_t freeMemSize, void *kernelStart, uint64_t kernelSize, void *kernelStartV)
{
    r = BasicRenderer(&framebuffer, psf1_font);
    GlobalRenderer = &r;

    PrepareMemory(framebuffer, freeMemStart, extraMemStart, freeMemSize, kernelStart, kernelSize, kernelStartV);

    GlobalRenderer->Clear(Colors.black, true);
    GlobalRenderer->putStr("ovo je test\n", 60, 70);

    GlobalPageTableManager.MapMemory((void*)0x600000000, (void*)0x800000, false);

    uint64_t* test = (uint64_t*)0x600000000;
    *test = 26;

    GlobalRenderer->putStr("I am in the new Page Map!\n", 230, 240);
    return 0;
}
