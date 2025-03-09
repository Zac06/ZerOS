#include<i686/isr.h>
#include<i686/idt.h>
#include<i686/gdt.h>
#include<i686/io.h>
#include<std/stdio.h>
#include<stddef.h>

isr_handler g_isr_handlers[IDT_SIZE];
static const char* g_exceptions[]={
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Overflow",
    "Bound range exceeded",
    "Invalid opcode",
    "Device not available",
    "Double fault",
    "Coprocessor segment overrun",
    "Invalid TSS",
    "Segment not present",
    "Stack-segment fault",
    "General protection fault",
    "Page fault",
    "",
    "x87 FPU floating-point error",
    "Alignment check",
    "Machine check",
    "SIMD floating-point exception",
    "Virtualization exception",
    "Control protection exception",
    "",
    "",
    "",
    "",
    "",
    "",
    "Hypervisor injection exception",
    "VMM communication exception",
    "Security exception",
    ""
};

void i686_isr_init_gates();

void i686_isr_init(){
    i686_isr_init_gates();
    for (int i = 0; i < 256; i++)
        i686_idt_enablegate(i);

    i686_idt_disablegate(0x80);
}


void __attribute__((cdecl)) i686_isr_handler(registers* regs){
    //interrno<32: exception, interrno>=32: interrupt
    if(g_isr_handlers[regs->interrno] != NULL){
        g_isr_handlers[regs->interrno](regs);
    }else if(regs->interrno>=32){
        printf("Unhandled interrupt: %d\n", regs->interrno);
    }else{
        printf("Unhandled exception: [%d] %s\n", regs->interrno, g_exceptions[regs->interrno]);
        printf("Interrno: %x, errno: %x\n", regs->interrno, regs->errno);
        printf("CPU registers:\n");
        printf(" |--EAX: 0x%x\t\t ESP:    0x%x\n", regs->eax, regs->esp);
        printf(" |--EBX: 0x%x\t\t EBP:    0x%x\n", regs->ebx, regs->ebp);
        printf(" |--ECX: 0x%x\t\t EIP:    0x%x\n", regs->ecx, regs->eip);
        printf(" |--EDX: 0x%x\t\t EFLAGS: 0x%x\n", regs->edx, regs->eflags);
        printf(" |--ESI: 0x%x\t\t CS:     0x%x\n", regs->esi, regs->cs);
        printf(" |--EDI: 0x%x\t\t DS:     0x%x\n", regs->edi, regs->ds);
        printf("           \t\t |--SS:     0x%x\n", regs->ss);

        printf("========= KERNEL PANIC ========= \n");
        i686_halt();
    }
}

void i686_isr_register_handler(int interrno, isr_handler handler){
    g_isr_handlers[interrno]=handler;
    i686_idt_enablegate(interrno);
}