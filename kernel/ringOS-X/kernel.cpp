#include "kernel.h"
#include "../e9print.h"

#include "BasicRenderer.hpp"
#include "paging/PageFrameAllocator.h"
#include "paging/PageMapIndexer.h"
#include "paging/PageTableManager.h"
#include "paging/paging.h"
#include "gdt/gdt.hpp"
#include "interrupts/idt.hpp"
#include "interrupts/interrupts.hpp"

static BasicRenderer r = BasicRenderer(NULL, NULL);
static PageFrameAllocator t = PageFrameAllocator();

void PrepareMemory(Framebuffer framebuffer, Memory memory)
{
    GlobalAllocator = &t;
    GlobalAllocator->ReadEFIMemoryMap(memory.freeMemStart, memory.freeMemSize);

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

IDTR idtr;
void PrepareInterrupts()
{
    idtr.Limit = 0x0FFF;
    idtr.Offset = (uint64_t)GlobalAllocator->RequestPage();

    IDTDescEntry* int_PageFault = (IDTDescEntry*)(idtr.Offset + 0xE * sizeof(IDTDescEntry));
    int_PageFault->SetOffset((uint64_t)PageFault);
    int_PageFault->type_attr = IDT_TA_InterruptGate;
    int_PageFault->selector = 0x08;

    asm ("lidt %0" : : "m" (idtr));
}

int ringOSX(Framebuffer framebuffer, PSF1_FONT *psf1_font, Memory memory)
{
    r = BasicRenderer(&framebuffer, psf1_font);
    GlobalRenderer = &r;

    PrepareMemory(framebuffer, memory);

    GDTDescriptor gdtDescriptor;
    gdtDescriptor.Size = sizeof(GDT) - 1;
    gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
    LoadGDT(&gdtDescriptor);

    PrepareInterrupts();

    GlobalRenderer->Clear(Colors.black, true);
    GlobalRenderer->Print("ovo je test\n");

    GlobalPageTableManager.MapMemory((void*)0x600000000, (void*)0x800000, false);

    uint64_t* test = (uint64_t*)0x600000000;
    *test = 26;

    asm("int $0x0e");

    GlobalRenderer->Print("I am in the new Page Map!\n");
    return 0;
}
