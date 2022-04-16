#include "print.h"
#include "sbi.h"

void puts(char *s) {
    // unimplemented
    for (int i = 0; s[i]; i++) {
        uint64 _output = s[i];
        sbi_ecall(0x1, 0x0, _output, 0, 0, 0, 0, 0);
    }   // traverse
}

void puti(int x) {
    // unimplemented
    unsigned long long cyc; // from the highest bit
    int _x = x;
    int length = 0; // length of str
    cyc = 1;
    while(cyc <= x){    // to get the highest bit
        cyc *= 10;
        length++;
    }
    cyc /= 10;

    while(cyc){ // from the highest bit
        int current = _x / cyc;
        _x = _x % cyc;
        cyc /= 10;
        sbi_ecall(0x1, 0x0, (current + 0x30), 0, 0, 0, 0, 0);
    }
}
