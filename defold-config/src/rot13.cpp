
// from Rosetta: https://rosettacode.org/wiki/Rot-13#C

#include <limits.h>
static char rot13_table[UCHAR_MAX + 1];

static void init_rot13_table(void) {
    static const unsigned char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static const unsigned char lower[] = "abcdefghijklmnopqrstuvwxyz";

    for (int ch = '\0'; ch <= UCHAR_MAX; ch++) {
        rot13_table[ch] = ch;
    }
    for (const unsigned char *p = upper; p[13] != '\0'; p++) {
        rot13_table[p[0]] = p[13];
        rot13_table[p[13]] = p[0];
    }
    for (const unsigned char *p = lower; p[13] != '\0'; p++) {
        rot13_table[p[0]] = p[13];
        rot13_table[p[13]] = p[0];
    }
}

static int g_Rot13TableInitialized = 0;

void rot13(char* s)
{
    if (!g_Rot13TableInitialized)
        init_rot13_table();

    while (*s != 0) {
        *s = rot13_table[*s];
        ++s;
    }
}
