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
#include "io.hpp"

#include "idt/idt.h"
#include "idt/isr.h"

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

    IDTDescEntry *int_PageFault = (IDTDescEntry *)(idtr.Offset + 0xE * sizeof(IDTDescEntry));
    int_PageFault->SetOffset((uint64_t)PageFault_Handler);
    int_PageFault->type_attr = IDT_TA_InterruptGate;
    int_PageFault->selector = 0x08;

    IDTDescEntry *int_DoubleFault = (IDTDescEntry *)(idtr.Offset + 0x8 * sizeof(IDTDescEntry));
    int_DoubleFault->SetOffset((uint64_t)DoubleFault_Handler);
    int_DoubleFault->type_attr = IDT_TA_InterruptGate;
    int_DoubleFault->selector = 0x08;

    IDTDescEntry *int_GPFault = (IDTDescEntry *)(idtr.Offset + 0xD * sizeof(IDTDescEntry));
    int_GPFault->SetOffset((uint64_t)GPFault_Handler);
    int_GPFault->type_attr = IDT_TA_InterruptGate;
    int_GPFault->selector = 0x08;

    IDTDescEntry *int_Keyboard = (IDTDescEntry *)(idtr.Offset + 0x21 * sizeof(IDTDescEntry));
    int_Keyboard->SetOffset((uint64_t)KeyboardInt_Handler);
    int_Keyboard->type_attr = IDT_TA_InterruptGate;
    int_Keyboard->selector = 0x08;

    asm("lidt %0"
        :
        : "m"(idtr));

    RemapPIC();

    outb(PIC1_DATA, 0b11111101);
    outb(PIC2_DATA, 0b11111111);

    asm("sti");
}

int ringOSX(Framebuffer framebuffer, PSF1_FONT *psf1_font, Memory memory)
{
    r = BasicRenderer(&framebuffer, psf1_font);
    GlobalRenderer = &r;

    //PrepareMemory(framebuffer, memory);

    //GDTDescriptor gdtDescriptor;
    //gdtDescriptor.Size = sizeof(GDT) - 1;
    //gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
    //LoadGDT(&gdtDescriptor);

    //PrepareInterrupts();

    isr_install();

    GlobalRenderer->Clear(Colors.black, true);
    GlobalRenderer->Print("ovo je test\n");

    GlobalRenderer->Print("I am in the new Page Map!\n");
    return 0;
}
