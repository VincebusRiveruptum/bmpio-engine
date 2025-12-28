// THIS IS 16-BIT use Borland C for testing

#include <dos.h>
#include <stdio.h>

#define BDA_SEG   0x40
#define KBD_HEAD  0x1A
#define KBD_TAIL  0x1C
#define KBD_START 0x1E
#define KBD_END   0x3E

int main() {
    unsigned short head, tail, start, end;
    unsigned char ascii, scan;

    unsigned short far *head_ptr =
        (unsigned short far*)MK_FP(BDA_SEG, KBD_HEAD);
    unsigned short far *tail_ptr =
        (unsigned short far*)MK_FP(BDA_SEG, KBD_TAIL);

    while (1) {
        head = *head_ptr;
        tail = *tail_ptr;

        if (head != tail) {
            unsigned char far *buf =
                (unsigned char far*)MK_FP(BDA_SEG, head);

            ascii = buf[0];
            scan  = buf[1];

            start = *(unsigned short far*)MK_FP(BDA_SEG, KBD_START);
            end   = *(unsigned short far*)MK_FP(BDA_SEG, KBD_END);

            head += 2;
            if (head >= end)
                head = start;

            *head_ptr = head;
            break;
        }
    }

    printf("ASCII: %02X  SCAN: %02X\n", ascii, scan);
    return 0;
}