#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <interrupts/IDT.h>


void div0_handler(struct InterruptFrame* frame);
void debug_exception(struct InterruptFrame* frame);
void overflow_exception(struct InterruptFrame* frame);
void bre_exception(struct InterruptFrame* frame);
void invalid_opcode_exception(struct InterruptFrame* frame);
void dev_not_avail_exception(struct InterruptFrame* frame);
void double_fault(struct InterruptFrame* frame);
void invalid_tss_exception(struct InterruptFrame* frame);
void snp_exception(struct InterruptFrame* frame);
void stackseg_fault(struct InterruptFrame* frame);
void gpf(struct InterruptFrame* frame);
void page_fault(struct InterruptFrame* frame);


#endif
