#ifndef IRQ_H
#define IRQ_H

#include "kaos_int.h"

#define IRQ_VEC_DIVISION_ERROR                 0
#define IRQ_VEC_DEBUG                          1
#define IRQ_VEC_NMI                            2
#define IRQ_VEC_BREAKPOINT                     3
#define IRQ_VEC_OVERFLOW                       4
#define IRQ_VEC_BOUND_RANGE_EXCEEDED           5
#define IRQ_VEC_INVALID_OPCODE                 6
#define IRQ_VEC_DEVICE_NOT_AVAILABLE           7
#define IRQ_VEC_DOUBLE_FAULT                   8
#define IRQ_VEC_COPROCESSOR_SEGMENT_OVERRUN    9
#define IRQ_VEC_INVALID_TSS                    10
#define IRQ_VEC_SEGMENT_NOT_PRESENT            11
#define IRQ_VEC_STACK_SEGMENT_FAULT            12
#define IRQ_VEC_GENERAL_PROTECTION_FAULT       13
#define IRQ_VEC_PAGE_FAULT                     14
#define IRQ_VEC_X87_FP_EXCEPTION               16
#define IRQ_VEC_ALIGNMENT_CHECK                17
#define IRQ_VEC_MACHINE_CHECK                  18
#define IRQ_VEC_SIMD_FP_EXCEPTION              19
#define IRQ_VEC_VIRTUALIZATION_EXCEPTION       20
#define IRQ_VEC_CONTROL_PROTECTION_EXCEPTION   21
#define IRQ_VEC_HYPERVISOR_INJECTION_EXCEPTION 28
#define IRQ_VEC_VMM_COMMUNICATOIN_EXCEPTION    29
#define IRQ_VEC_SECURITY_EXCEPTION             30

void irq_init();

void irq_register_handler(int num, __attribute__((interrupt)) void (*irq)(void *irq_frame), uint8_t type);
void irq_enable();

#endif /* IRQ_H */
