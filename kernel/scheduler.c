#include "scheduler.h"
#include "memory.h"
#include "uart.h"

#define MAX_TASKS 4

#define IRQ_FRAME_SIZE 272UL
#define IRQ_ELR_OFFSET 256UL
#define IRQ_SPSR_OFFSET 264UL

#define TIME_SLICE_TICKS 5

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
 unsigned long irq_sp;

    int id;
    int active;
} task_t;

static task_t tasks[MAX_TASKS];

static int task_count = 0;
static int current_task = -1;

static context_t kernel_context;
static unsigned int slice_ticks = 0;
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

    uart_puts("Task returned.\r\n");

    while (1) {
        __asm__ volatile("wfe");
    }
}


void scheduler_init(void)
{
    task_count = 0;
    current_task = -1;
    slice_ticks = 0;
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].active = 0;
        tasks[i].stack_page = 0;
        tasks[i].entry = 0;
        tasks[i].irq_sp = 0;
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

    unsigned long stack_top =
        (unsigned long)stack + PAGE_SIZE;
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

     unsigned long frame_sp =
        stack_top - IRQ_FRAME_SIZE;

    unsigned char *frame =
        (unsigned char *)frame_sp;

    for (unsigned long i = 0;
         i < IRQ_FRAME_SIZE;
         i++)
    {
        frame[i] = 0;
    }

    /*
     * ELR_EL1 -> where execution begins.
     */
    *(unsigned long *)(frame_sp + IRQ_ELR_OFFSET) =
        (unsigned long)task_bootstrap;

    /*
     * SPSR = EL1h, interrupts enabled.
     */
    *(unsigned long *)(frame_sp + IRQ_SPSR_OFFSET) =
        0x5;

    task->irq_sp = frame_sp;

    task_count++;

    return task->id;
}

void scheduler_start(void)
{
    if (task_count == 0)
        return;

    current_task = 0;
    slice_ticks = 0;

    context_switch(
        &kernel_context,
        &tasks[0].context
    );
}

unsigned long scheduler_on_timer(unsigned long frame_sp)
{
    /*
     * Shell is still running or scheduler
     * hasn't started.
     */
    if (current_task < 0 || task_count < 2)
        return frame_sp;

    tasks[current_task].irq_sp = frame_sp;

    slice_ticks++;

    /*
     * Keep current task until its time slice expires.
     */
    if (slice_ticks < TIME_SLICE_TICKS)
        return frame_sp;

    slice_ticks = 0;

    int next = current_task + 1;

    if (next >= task_count)
        next = 0;

    current_task = next;

    return tasks[next].irq_sp;
}

int scheduler_task_count(void)
{
    return task_count;
}

int scheduler_current_task(void)
{
    return current_task;
}
