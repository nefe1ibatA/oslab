#include "printk.h"
#include "defs.h"

// Please do not modify

void test() {
    while (1) {
        // set frequency of output information
        for (int i = 0; i < 520000000; i++);
        printk("kernel is running!\n");
    }
}
