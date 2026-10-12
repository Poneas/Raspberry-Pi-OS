#ifndef SCHEDULER_H
#define SCHEDULER_H

typedef void (*task_entry_t)(void);

void scheduler_init(void);
int task_create(task_entry_t entry);

void scheduler_start(void);

unsigned long scheduler_on_timer(unsigned long frame_sp);

int scheduler_task_count(void);
int scheduler_current_task(void);

#endif
