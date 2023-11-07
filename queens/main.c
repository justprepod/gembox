#include <stdio.h>

typedef unsigned char byte;
typedef unsigned char bool;

#define true	1
#define false	0

enum Status { EMPTY, MARK, QUEEN };

/*
* Доска хранится в битовом массиве, на каждую клетку отводится 2 бита, в каждом байте содержатся данные о 4 клетках
* 
* Пример извлечения значения из третьего элемента 
* 01011000 - значение байта
* 00110000 - маска, соответствующая третьему элементу
* 00010000 - маскированное значение
* 00000001 - маскированное значение, сдвинутое вправо на четыре бита* 
*/

void serialize(byte a[16]) {
    byte i;

    for (i = 0; i < 64; i++) {
        byte s = (i << 1) % 8;
        byte value = (a[i / 4] & (0x03 << s)) >> s;
        printf("%c", value ? (value == QUEEN ? 'X' : '.') : ' ');
        if ((i % 8) == 7)
            printf("\n");
    }
}

void SET(byte* a, byte x, byte y, byte v) {
    byte b = ((y << 3) + x) >> 2;
    byte s = (x << 1) % 8;
    a[b] = (a[b] & ~(0x03 << s)) | (v << s);
}

#define GET(a, x, y) (a[(((y) << 3) + (x)) >> 2] & (0x03 << (((x) << 1) % 8))) >> (((x) << 1) % 8)

void put(byte* a, byte x, byte y) {
    /*
    *                     X
    *         .----------->
    *         | 7   0   1
    *         |   \ | /	
    *         | 6 -   - 2
    *         |   / | \
    *         | 5   4   3
    *       Y V
    */
    char dx[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
    char dy[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
    byte nx;
    byte ny;

    SET(a, x, y, QUEEN);

    for (byte d = 0; d < 8; d++)
        for (byte i = 1; i < 8; i++) {
            nx = x + i * dx[d];
            ny = y + i * dy[d];
            if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8)
                SET(a, nx, ny, MARK);
        }

}

bool turn(byte a[16], byte n) {
    byte A[16];

    if (n == 0) {
        serialize(a);
        printf("\n");
        return true;
    }

    for (byte x = 0; x < 8; x++) {
        for (byte y = 0; y < 8; y++) {
            if (GET(a, x, y) == EMPTY) {
                memcpy(A, a, sizeof(A));
                put(A, x, y);
                if (turn(A, n - 1))
                    return true;
            }
        }
    }
    return false;
}

int main(int argc, char** argv) {
    byte a[16];

    memset(a, 0, sizeof(a));

    turn(a, 8);

    return 0;
}
