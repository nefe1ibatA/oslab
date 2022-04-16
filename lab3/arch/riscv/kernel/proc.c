#include "proc.h"
#include "rand.h"
#include "defs.h"
#include "mm.h"

extern void __dummy();
extern void __switch_to(struct task_struct *prev,
						struct task_struct *next);

struct task_struct *idle;
struct task_struct *current;
struct task_struct *task[NR_TASKS];

void task_init() 
{
	/**
	 *	Set IDLE
	 */
	// 1. call kalloc
	
	idle = (struct task_struct*)kalloc();
	idle->thread.sp = (uint64)idle + PGSIZE;

	// 2. set state -> TASK_RUNNING

	idle->state = TASK_RUNNING;

	// 3. set counter / priority -> 0

	idle->counter = 0;
	idle->priority = 0;

	// 4. set pid -> 0

	idle->pid = 0;

	// 5. current / task[0] point to idle

	current = idle;
	task[0] = idle;

	/**
	 * 	Set task[1] ~ task[NR_TASKS - 1]
	 */
	for (int i = 1; i < NR_TASKS; i++) {
		// 1. init task[i]
		
		task[i] = (struct task_struct*)kalloc();
		
		// 2. state -> TASK_RUNNING; counter -> 0; priority -> rand(); pid -> index
		
		task[i]->state = TASK_RUNNING;
		task[i]->counter = 0;
		task[i]->priority = rand();
		task[i]->pid = i;

		// 3. set thread_struct `ra` & `sp`
		// 		ra -> &(__dummy); sp -> high addr of the page
		task[i]->thread.ra = (uint64)__dummy;
		task[i]->thread.sp = (uint64)task[i] + PGSIZE;
	}

	printk("...proc_init done!\n");
}

void do_timer()
{
	if (current == idle) {
		schedule();
	}
	else {
		current->counter--;
		if (current->counter <= 0) {
			schedule();
		}
	}
}

// SJF
#ifdef SJF
void schedule()
{ 	
	int next_index = 0;
	int count = 0;
	for (int i = NR_TASKS - 1; i > 0; i--) {
		if (task[i]->state == TASK_RUNNING && task[i]->counter != 0) {
			if (count == 0) {
				count = task[i]->counter;
				next_index = i;
			}
			else if (count != 0) {
				if (task[i]->counter < count) {
					count = task[i]->counter;
					next_index = i;
				}
			}
		}
	}
	if (count == 0) {
		for (int i = 1; i < NR_TASKS; i++) {
			task[i]->counter = rand();
			printk("SET [PID = %d COUNTER = %d]\n", i, task[i]->counter);
		}
		schedule();
	}
	else {
		switch_to(task[next_index]);
	}
}
#endif

#ifdef PRIORITY
void schedule()
{
	int next_index = 0;
	while (1) {
		int c = 0;
		int i = NR_TASKS;
		struct task_struct **p;
		p = &task[NR_TASKS];
		while (--i) {
			if (!*--p) {
				continue;
			}
			if ((*p)->state == TASK_RUNNING && (*p)->priority > c && (*p)->counter) {
				c = (*p)->priority;
				next_index = i;
			}
		}
		if (c) break;
		for (p = &task[NR_TASKS - 1]; p > &task[0]; --p) {
			if (*p) {
				(*p)->counter = ((*p)->counter >> 1) + (*p)->priority;
				printk("SET [PID = %d PRIORITY = %d COUNTER = %d]\n", p - &task[0], (*p)->priority, (*p)->counter);
			}
		}
	}
	switch_to(task[next_index]);
}
#endif

void switch_to(struct task_struct *next)
{
	if (current != next) {
		struct task_struct *cur = current;
		current = next;
		printk("switch to [PID = %d COUNTER = %d]\n", next->pid, next->counter);
		__switch_to(cur, next);
	}
}

void dummy()
{
	uint64 MOD = 1000000007;
	uint64 auto_inc_local_var = 0;
	int last_counter = -1;
	if (current == idle) {
		printk("idle process is running!\n");
	}
	while (1) {
		if (last_counter == -1 || current->counter != last_counter) {
			last_counter = current->counter;
			auto_inc_local_var = (auto_inc_local_var + 1) % MOD;
			printk("[PID = %d] is running. auto_inc_local_var = %d\n",
				current->pid, auto_inc_local_var);
		}
	}
}