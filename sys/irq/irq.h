#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

struct interrupt_frame {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
} __attribute__((packed));

/**
 * @brief Interrupt handler prototype definition
 * 
 */
typedef void (*ISR_HANDLER)();

/**
 * @brief Register an interrupt handler
 * 
 * @param interrupt_no  Interrupt number
 * @param handler       Pointer to interrupt handler
 * @return int 
 */
int
register_interrupt(int interrupt_no, ISR_HANDLER handler);

/**
 * @brief Enable Interrupts
 * 
 */
void
enable_interrupts();

/**
 * @brief Disable Interrupts
 * 
 */
void
disable_interrupts();

#endif