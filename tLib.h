#ifndef __TLIB_H
#define __TLIB_H

#include "stdint.h"

typedef struct
{
    uint32_t u32bitmap;
}tBitmap;

void tBitmapInit(tBitmap *bitmap);
void tBitmapSet(tBitmap *bitmap, uint32_t pos);
void tBitmapClear(tBitmap *bitmap, uint32_t pos);
uint32_t tBitmapGetFirstSet(tBitmap *bitmap);
uint32_t tBitmapPosCount(void);

#endif

