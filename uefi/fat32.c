/**
 * 
 */

#include "fat32.h"

uint64_t Strlen(const int8_t *file)
{
    uint64_t length = 0;
    for (; *(file + length) != '\0'; ++length);
}

void opendir(const int8_t *file)
{
    int8_t *spare = file;
    // 後段の処理のため「/」終わりなら除去する
    if (*(spare + Strlen(spare) - 1) == '/') {
        *(spare + Strlen(spare) - 1) = '\0';
    }
}

void debugMBR()
{
    
}