#pragma once

#include <common.h>

// TODO: create a custo string class
class Strings
{
public:

    static i32 FindFirstInstance(const char *string,  char character);
    static i32 FindLastInstance(const char* string, char character);
    static i32 FindPenultimateInstance(const char* string, char character);
    static void ReplaceInstance(char* string, char character, char newCharacter);

};
