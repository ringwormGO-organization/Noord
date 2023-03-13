#ifndef __CPU_ISR
#define __CPU_ISR

#include <stdint.h>

// Define the ISR's for CPU exceptions
extern "C" void isr_0();
extern "C" void isr_1();
extern "C" void isr_2();
extern "C" void isr_3();
extern "C" void isr_4();
extern "C" void isr_5();
extern "C" void isr_6();
extern "C" void isr_7();
extern "C" void isr_8();
extern "C" void isr_9();
extern "C" void isr_10();
extern "C" void isr_11();
extern "C" void isr_12();
extern "C" void isr_13();
extern "C" void isr_14();
extern "C" void isr_15();
extern "C" void isr_16();
extern "C" void isr_17();
extern "C" void isr_18();
extern "C" void isr_19();
extern "C" void isr_20();
extern "C" void isr_21();
extern "C" void isr_22();
extern "C" void isr_23();
extern "C" void isr_24();
extern "C" void isr_25();
extern "C" void isr_26();
extern "C" void isr_27();
extern "C" void isr_28();
extern "C" void isr_29();
extern "C" void isr_30();
extern "C" void isr_31();

// Function to install the ISR's to the IDT and
// load the IDT to the CPU
extern "C" void isr_install();

// Structure to push registers when saving for ISR
typedef struct
{
    uint64_t ds;
    uint64_t rdi, rsi, rbp, rsp, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, eflags, useresp, ss;
} registers;

// One handler for all ISR's
extern "C" void isr_handler(registers regs);

#endif
