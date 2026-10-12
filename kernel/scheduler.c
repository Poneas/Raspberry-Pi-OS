#include "scheduler.h"
#include "memory.h"
#include "uart.h"

#define MAX_TASKS 4

typedef struct {
    unsigned long x19;
    unsigned long x20;
    unsigned long x21;
    unsigned long x22;
    unsigned long x23;
    unsigned long x24;
    unsigned long x25;
    unsigned long x26;
    unsigned long x27;
    unsigned long x28;
    unsigned long x29;
    unsigned long x30;
    unsigned long sp;
} context_t;

typedef struct {
    context_t context;

    task_entry_t entry;

    void *stack_page;

    int id;
    int active;
} task_t;

static task_t tasks[MAX_TASKS];

static int task_count = 0;
static int current_task = -1;

static context_t kernel_context;

extern void context_switch(
    context_t *old_context,
    context_t *new_context
);

static void task_bootstrap(void)
{
    if (current_task < 0 || current_task >= task_count) {
        uart_puts("Invalid task state.\r\n");

        while (1) {
            __asm__ volatile("wfe");
        }
    }

    task_entry_t entry = tasks[current_task].entry;

    entry();

    uart_puts("Task returned unexpectedly.\r\n");

    while (1) {
        scheduler_yield();
    }
}

void scheduler_init(void)
{
    task_count = 0;
    current_task = -1;

    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].active = 0;
        tasks[i].stack_page = 0;
        tasks[i].entry = 0;
    }
}

int task_create(task_entry_t entry)
{
    if (task_count >= MAX_TASKS)
        return -1;

    void *stack = page_alloc();

    if (stack == 0)
        return -1;

    task_t *task = &tasks[task_count];

    /*
     * Clear initial CPU context.
     */
    unsigned long *ctx =
        (unsigned long *)&task->context;

    for (unsigned long i = 0;
         i < sizeof(context_t) / sizeof(unsigned long);
         i++)
    {
        ctx[i] = 0;
    }

    task->entry = entry;
    task->stack_page = stack;
    task->id = task_count;
    task->active = 1;

    /*
     * ARM64 stack grows downward.
     */
    task->context.sp =
        (unsigned long)stack + PAGE_SIZE;

    /*
     * First "ret" into this task will jump here.
     */
    task->context.x30 =
        (unsigned long)task_bootstrap;

    task_count++;

    return task->id;
}

void scheduler_start(void)
{
    if (task_count == 0)
        return;

    current_task = 0;

    context_switch(
        &kernel_context,
        &tasks[0].context
    );
}

void scheduler_yield(void)
{
    if (task_count < 2)
        return;

    int previous = current_task;

    int next = current_task + 1;

    if (next >= task_count)
        next = 0;

    current_task = next;

    context_switch(
        &tasks[previous].context,
        &tasks[next].context
    );
}

int scheduler_task_count(void)
{
    return task_count;
}

int scheduler_current_task(void)
{
    return current_task;
}
