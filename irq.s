.altmacro

.macro gen_macro_noparam_irqh isrb, isre
.global _irq_entry_\isrb
    _irq_entry_\isrb:
    cli
    pushq $0x0
    pushq $\isrb
    jmp isr_common_handler
    .if     \isre-\isrb
        gen_macro_noparam_irqh %isrb+1,\isre
    .endif
.endm

.macro gen_macro_param_irqh isrb, isre
.global _irq_entry_\isrb
    _irq_entry_\isrb:
    cli
    pushq $\isrb
    jmp isr_common_handler
    .if     \isre-\isrb
        gen_macro_param_irqh %isrb+1,\isre
    .endif
.endm

// exception handlers
gen_macro_noparam_irqh 0, 7
gen_macro_param_irqh 8, 8
gen_macro_noparam_irqh 9, 9
gen_macro_param_irqh 10, 14
gen_macro_noparam_irqh 15, 31
gen_macro_noparam_irqh 255, 255

.extern isr_handler
isr_common_handler:    
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %rbx
    pushq %rsp
    pushq %rbp
    pushq %rsi
    pushq %rdi

    movq %cr3, %r12    

    movq %ds, %rax
    pushq %rax

    movq $0x10, %rax
    movq %rax, %ds
    movq %rax, %es
    movq %rax, %fs
    movq %rax, %gs

    call isr_handler
    lfence
    cld

    popq %rax
    movq %rax, %ds
    movq %rax, %es
    movq %rax, %fs
    movq %rax, %gs
        
    movq %r12, %cr3   

    popq %rdi
    popq %rsi
    popq %rbp
    popq %rsp
    popq %rbx
    popq %rdx
    popq %rcx
    popq %rax
    // two pushq
    addq 0x0f, %rsp 
    sti
    iretq
    