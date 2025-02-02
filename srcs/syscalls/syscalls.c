#include "syscalls.h"
#include "../utils/stdint.h"
#include "../display/display.h"

typedef int (*syscall_handler_3_t)(uint32_t arg1, uint32_t arg2, uint32_t arg3);
typedef int (*syscall_handler_2_t)(uint32_t arg1, uint32_t arg2);
typedef int (*syscall_handler_1_t)(uint32_t arg1);
typedef int (*syscall_handler_0_t)();

typedef union
{
    syscall_handler_3_t handler_3;
    syscall_handler_2_t handler_2;
    syscall_handler_1_t handler_1;
    syscall_handler_0_t handler_0;
} syscall_handler_t;

typedef struct
{
    syscall_handler_t handler;
    uint8_t num_args;
} syscall_entry_t;

int sys_exit(int status)
{
    printf("Syscall: exit(%d)\n", status);
    return 0;
}

int sys_write(int fd, const char* buf, size_t count)
{
    if (!buf || count == 0)
    {
        printf("Invalid buffer or count\n");
        return -1;
    }
    puts(buf);
    return count;
}

int sys_read(int fd, char* buf, size_t count)
{
    printf("Syscall: read(%d, %p, %d)\n", fd, buf, count);
    return 0;
}

int sys_open(const char* path, int flags)
{
    printf("Syscall: open(%s, %d)\n", path, flags);
    return 0;
}

int sys_close(int fd)
{
    printf("Syscall: close(%d)\n", fd);
    return 0;
}

int sys_get_pid()
{
    printf("Syscall: getpid()\n");
    return 0;
}

syscall_entry_t syscall_table[MAX_SYSCALLS] = {
    { .handler.handler_1 = (syscall_handler_1_t)sys_exit,       .num_args = 1 },
    { .handler.handler_3 = (syscall_handler_3_t)sys_write,      .num_args = 3 },
    { .handler.handler_3 = (syscall_handler_3_t)sys_read,       .num_args = 3 },
    { .handler.handler_2 = (syscall_handler_2_t)sys_open,       .num_args = 2 },
    { .handler.handler_1 = (syscall_handler_1_t)sys_close,      .num_args = 1 },
    { .handler.handler_0 = (syscall_handler_0_t)sys_get_pid,    .num_args = 0 } 
};

int syscall_handler(registers reg, uint32_t intr_no, uint32_t err_code, error_state stack)
{
    uint32_t syscall_number = reg.eax;
    uint32_t arg1 = reg.ebx;
    uint32_t arg2 = reg.ecx;
    uint32_t arg3 = reg.edx;

    if (syscall_number >= MAX_SYSCALLS)
    {
        printf("Unknown syscall: %d\n", syscall_number);
        return -1;
    }

    syscall_entry_t entry = syscall_table[syscall_number];

    int ret_value;
    switch (entry.num_args)
    {
        case 0:
            ret_value = entry.handler.handler_0();
            break;
        case 1:
            ret_value = entry.handler.handler_1(arg1);
            break;
        case 2:
            ret_value = entry.handler.handler_2(arg1, arg2);
            break;
        case 3:
            ret_value = entry.handler.handler_3(arg1, arg2, arg3);
            break;
        default:
            printf("Invalid number of arguments for syscall %d\n", syscall_number);
            ret_value = -1;
            break;
    }

    ret_value = ret_value < 0 ? -1 : ret_value;
    return ret_value;
}
