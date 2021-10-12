#include "print.h"
#include "sbi.h"

void puts(char *s) {
    // unimplemented
    for (int i = 0; s[i]; i++) {
        uint64 _output = s[i];
        sbi_ecall(0x1, 0x0, _output, 0, 0, 0, 0, 0);
    }
}

void puti(int x) {
    // unimplemented
    unsigned long long cyc;
    int _x = x;
    int length = 0;
    cyc = 1;
    while(cyc <= x){
        cyc *= 10;
        length++;
    }
    cyc /= 10;

    while(cyc){
        int current = _x / cyc;
        _x = _x % cyc;
        cyc /= 10;
        sbi_ecall(0x1, 0x0, (current + 0x30), 0, 0, 0, 0, 0);

    }
}
