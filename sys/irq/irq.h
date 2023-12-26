#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

/**
 * @brief Intialize the IRQ subsystem
 *
 */
void
irq_init();

/**
 * @brief Interrupt handler prototype definition
 *
 */
typedef void (*IRQ_HANDLER)();

/**
 * @brief Register an interrupt handler
 *
 * @param interrupt_no  Interrupt number
 * @param handler       Pointer to interrupt handler
 * @return int          Status code
 */
int
register_irq(int interrupt_no, IRQ_HANDLER irq_handler);

/**
 * @brief Execute an irq handler
 *
 * @param interrupt_no  Interrupt number
 * @return int          Status code
 */
int
do_irq(int interrupt_no);

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