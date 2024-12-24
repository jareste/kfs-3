#ifndef IDT_H
#define IDT_H

#include "../utils/stdint.h"


struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));


#define IDT_SIZE 256
extern struct idt_entry idt[IDT_SIZE];
extern struct idt_ptr idtp;

extern void idt_load();

#endif
