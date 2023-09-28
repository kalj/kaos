#ifndef IRQ_H
#define IRQ_H

#include "kaos_int.h"

#define IRQ_DIVISION_ERROR                 0
#define IRQ_DEBUG                          1
#define IRQ_NMI                            2
#define IRQ_BREAKPOINT                     3
#define IRQ_OVERFLOW                       4
#define IRQ_BOUND_RANGE_EXCEEDED           5
#define IRQ_INVALID_OPCODE                 6
#define IRQ_DEVICE_NOT_AVAILABLE           7
#define IRQ_DOUBLE_FAULT                   8
#define IRQ_COPROCESSOR_SEGMENT_OVERRUN    9
#define IRQ_INVALID_TSS                    10
#define IRQ_SEGMENT_NOT_PRESENT            11
#define IRQ_STACK_SEGMENT_FAULT            12
#define IRQ_GENERAL_PROTECTION_FAULT       13
#define IRQ_PAGE_FAULT                     14
#define IRQ_X87_FP_EXCEPTION               16
#define IRQ_ALIGNMENT_CHECK                17
#define IRQ_MACHINE_CHECK                  18
#define IRQ_SIMD_FP_EXCEPTION              19
#define IRQ_VIRTUALIZATION_EXCEPTION       20
#define IRQ_CONTROL_PROTECTION_EXCEPTION   21
#define IRQ_HYPERVISOR_INJECTION_EXCEPTION 28
#define IRQ_VMM_COMMUNICATOIN_EXCEPTION    29
#define IRQ_SECURITY_EXCEPTION             30

void irq_init();

void irq_register_handler(int num, void *irq, uint8_t type);
void irq_enable();

#endif /* IRQ_H */
