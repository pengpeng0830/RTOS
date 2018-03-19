#include "tLib.h"

void tNodeInit (tNode * node)
{
	node->nextNode = node;
	node->preNode = node;
}

#define	firstNode headNode.nextNode
#define	lastNode headNode.preNode

void tListInit (tList * list)
{
	list->firstNode = &(list->headNode);
	list->lastNode = &(list->headNode);
	list->nodeCount = 0;
}

uint32_t tListCount (tList * list)
{
	return list->nodeCount;
}

tNode * tListFirst (tList * list)
{
	tNode * node = (tNode *)0;
	
	if (list->nodeCount != 0)
	{
		node = list->firstNode;
	}
	return node;
}

tNode * tListLast (tList * list)
{
	tNode * node = (tNode *)0;
	
	if (list->nodeCount != 0)
	{
		node = list->lastNode;
	}
	return node;
}

tNode * tListPre (tList * list, tNode * node)
{
	if (node->preNode == node)
	{
		return (tNode *)0;
	}
	else
	{
		return node->preNode;
	}
}

