//
// Created by ghdawn on 16-1-10.
//

#ifndef ITRLIB_BITOPERATOR_H
#define ITRLIB_BITOPERATOR_H

#include "typedef.h"

U8 GetPartBit(U32 data, int begin, int len)
{
    data >>= begin;
    U8 result = 0;
    for (int i = 0; i < len; ++i)
    {
        result |= (data & 1) << i;
        data >>= 1;
    }
    return result;
}

void SetPartBit(U32 data, U8 value, int begin, int len)
{
    for (int i = 0; i < len; ++i)
    {
        data &= ~(1 << begin + i);
        data |= ((value >> i) & 1) << (begin + i);
    }
}

#endif //ITRLIB_BITOPERATOR_H
