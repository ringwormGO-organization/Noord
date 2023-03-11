#include "interrupts.hpp"

__attribute__((interrupt)) void PageFault(struct interrupt_frame* frame)
{
    GlobalRenderer->Print("Page Fault detected!");
    while(true);
    //asm ("hlt");
}
