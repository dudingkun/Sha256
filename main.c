/*
 * main.c
 */

#include <stdio.h>
#include "sha256.h"

int main(int argc, char *argv[])
{
    uint8_t aucHash[32] = {0};

    Sha256("abc", 3, aucHash);

    printf("Hash-Sha256: \t");
    for (uint8_t i = 0; i < 32; i++)
    {
        printf("%x", aucHash[i]);
    }
    printf("\n");

    return 0;
}