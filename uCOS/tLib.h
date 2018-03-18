#ifndef __TLIB_H
#define __TLIB_H

#include "stdint.h"

typedef struct
{
    uint32_t u32bitmap;
}tBitmap;

typedef struct _tNode
{
	struct _tNode * preNode;
	struct _tNode * nextNode;
}tNode;

void tNodeInit (tNode * node);

typedef struct _tList
{
	tNode headNode;
	uint32_t nodeCount;
}tList;

void tBitmapInit(tBitmap *bitmap);
void tBitmapSet(tBitmap *bitmap, uint32_t pos);
void tBitmapClear(tBitmap *bitmap, uint32_t pos);
uint32_t tBitmapGetFirstSet(tBitmap *bitmap);
uint32_t tBitmapPosCount(void);

#endif

