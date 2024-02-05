#include "ctype.h"

bool
isalpha(char c)
{
    if (isupper(c) || islower(c)) {
        return true;
    }

    return false;
}

bool
isdigit(char c)
{
    if (c >= 48 && c <= 57) {
        return true;
    }

    return false;
}

bool
isalnum(char c)
{
    if (isalpha(c) || isdigit(c)) {
        return true;
    }

    return false;
}

bool
isupper(char c)
{
    if (c >= 65 && c <= 90) {
        return true;
    }

    return false;
}

bool
islower(char c)
{
    if (c >= 97 && c <= 122) {
        return true;
    }

    return false;
}
