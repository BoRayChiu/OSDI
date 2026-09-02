#include "time.h"

unsigned long get_timer_freq() {
    unsigned long timer_freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r" (timer_freq));

    return timer_freq;
}

unsigned long get_timer_cnt() {
    unsigned long timer_cnt;
    asm volatile("mrs %0, cntvct_el0" : "=r" (timer_cnt));

    return timer_cnt;
}

unsigned long get_timestamp() {
    unsigned long timer_freq = get_timer_freq();
    unsigned long timer_cnt = get_timer_cnt();

    return timer_cnt / timer_freq;
}