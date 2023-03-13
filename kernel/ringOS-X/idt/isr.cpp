#include "idt.h"
#include "isr.h"

#include "../BasicRenderer.hpp"

// Give string values for each exception
char *exception_messages[] = {
    "Division by Zero",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bat TSS",
    "Segment not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"};

// Install the ISR's to the IDT
void isr_install()
{
    set_idt_gate(0, (uint64_t)isr_0);
    set_idt_gate(1, (uint64_t)isr_1);
    set_idt_gate(2, (uint64_t)isr_2);
    set_idt_gate(3, (uint64_t)isr_3);
    set_idt_gate(4, (uint64_t)isr_4);
    set_idt_gate(5, (uint64_t)isr_5);
    set_idt_gate(6, (uint64_t)isr_6);
    set_idt_gate(7, (uint64_t)isr_7);
    set_idt_gate(8, (uint64_t)isr_8);
    set_idt_gate(9, (uint64_t)isr_9);
    set_idt_gate(10, (uint64_t)isr_10);
    set_idt_gate(11, (uint64_t)isr_11);
    set_idt_gate(12, (uint64_t)isr_12);
    set_idt_gate(13, (uint64_t)isr_13);
    set_idt_gate(14, (uint64_t)isr_14);
    set_idt_gate(15, (uint64_t)isr_15);
    set_idt_gate(16, (uint64_t)isr_16);
    set_idt_gate(17, (uint64_t)isr_17);
    set_idt_gate(18, (uint64_t)isr_18);
    set_idt_gate(19, (uint64_t)isr_19);
    set_idt_gate(20, (uint64_t)isr_20);
    set_idt_gate(21, (uint64_t)isr_21);
    set_idt_gate(22, (uint64_t)isr_22);
    set_idt_gate(23, (uint64_t)isr_23);
    set_idt_gate(24, (uint64_t)isr_24);
    set_idt_gate(25, (uint64_t)isr_25);
    set_idt_gate(26, (uint64_t)isr_26);
    set_idt_gate(27, (uint64_t)isr_27);
    set_idt_gate(28, (uint64_t)isr_28);
    set_idt_gate(29, (uint64_t)isr_29);
    set_idt_gate(30, (uint64_t)isr_30);
    set_idt_gate(31, (uint64_t)isr_31);

    // Load the IDT to the CPU
    set_idt();

    // Enable Interrupts
    __asm__ volatile("sti");
}

void isr_handler(registers regs)
{
    GlobalRenderer->Print("Interrupt\n");

    const char *message = exception_messages[regs.int_no];

    GlobalRenderer->Print(message);
    GlobalRenderer->Print("\n");

    asm ("hlt");
}
