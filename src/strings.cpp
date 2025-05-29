#include "strings.h"
#include <string.h>

i32 Strings::FindFirstInstance(const char* string, char character)
{
    i32 len = strlen(string);
    for (i32 i = 0; i < len; ++i)
    {
        if (string[i] == character)
        {
            return i;
        }
    }
    return -1;
}

i32 Strings::FindLastInstance(const char* string, char character)
{
    i32 len = strlen(string);
    i32 lastIndexFound = -1;
    for (i32 i = 0; i < len; ++i)
    {
        if (string[i] == character)
        {
            lastIndexFound = i;
        }
    }
    return lastIndexFound;
}

i32 Strings::FindPenultimateInstance(const char* string, char character)
{
    i32 len = strlen(string);
    i32 penultimateIndexFound = -1;
    i32 lastIndexFound = -1;
    for (i32 i = 0; i < len; ++i)
    {
        if (string[i] == character)
        {
            penultimateIndexFound = lastIndexFound;
            lastIndexFound = i;
        }
    }
    return penultimateIndexFound;
}

void Strings::ReplaceInstance(char* string, char character, char newCharacter)
{
    i32 len = strlen(string);
    for (i32 i = 0; i < len; ++i)
    {
        if (string[i] == character)
        {
            string[i] = newCharacter;
        }
    }
}

