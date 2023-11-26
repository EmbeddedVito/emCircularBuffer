/*
 * @file emCircularBuffer.c
 * @author Mannone Vito
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "emCircularBuffer.h"
#include "emCircularPort.h"

/*
 * PRIVATE FUNCTIONS
 */
#if CB_DEBUG
#include <stdio.h>
#define CB_DEBUG_Print(...) printf(__VA_ARGS__)
#else
#define CB_DEBUG_Print(...)
#endif

/*
 * PUBLIC FUNCTIONS
 */

CBuffer_t *emCircularInit(const size_t maxElems, const size_t elemSize, const char *sem_name)
{
	if (maxElems < 2)
		return NULL;
	if (elemSize < 1)
		return NULL;
	CBuffer_t *retval = (CBuffer_t *)emCircularPortMalloc(sizeof(CBuffer_t));
	unsigned char *buffer = (unsigned char *)emCircularPortMalloc(maxElems * elemSize);
	retval->headInd = 0;
	retval->tailInd = 0;
	retval->startBuffer = buffer;
	retval->elemSize = elemSize;
	retval->maxElems = maxElems;
	retval->NbElems = 0;
	retval->sem = emCircularPort_InitBynSem(sem_name);
#if CIRCULAR_USE_LOCK_MECHANISM
	if (retval->sem == NULL)
	{
		retval = NULL;
		CB_DEBUG_Print("CB Error:\tCannot inizialize buffer semaphore!\r\n");
	}
#endif
#if CB_DEBUG
	if (retval != NULL)
	{
		CB_DEBUG_Print("CB:\tCircularBuffer initialised. CB pointer is %p.\r\n", retval);
	}
#endif
	return retval;
}

CBStatus_t emCircularDelete(CBuffer_t *buffer)
{
	CBStatus_t retval = CB_false;
	int sem_retval = emCircularPort_EnterCritical(buffer->sem);
	CB_sem_t temp_sem = buffer->sem;
	if (sem_retval != 0)
	{
		emCircularPort_ExitCritical(temp_sem);
		return CB_error;
	}
	if (buffer->startBuffer != NULL)
	{
		emCircularPortFree(buffer->startBuffer);
	}
	if (buffer == NULL)
	{
		retval = CB_true;
	}
	else
	{
		emCircularPortFree(buffer);
	}
	emCircularPort_ExitCritical(temp_sem);
	emCircularPort_BynSemDelete(temp_sem);
	return retval;
}

CBStatus_t emCircularIsEmpty(const CBuffer_t *buffer)
{
	CBStatus_t retval = CB_false;
	int sem_retval = emCircularPort_EnterCritical(buffer->sem);
	if (sem_retval != 0)
	{
		emCircularPort_ExitCritical(buffer->sem);
		return CB_error;
	}
	if (buffer == NULL)
	{
		retval = CB_error;
	}
	else
	{
		if (buffer->headInd == buffer->tailInd)
		{
			retval = CB_true;
		}
	}
	emCircularPort_ExitCritical(buffer->sem);
	return retval;
}

CBStatus_t emCircularIsFull(const CBuffer_t *buffer)
{
	CBStatus_t retval = CB_false;
	int sem_retval = emCircularPort_EnterCritical(buffer->sem);
	if (sem_retval != 0)
	{
		emCircularPort_ExitCritical(buffer->sem);
		return CB_error;
	}
	if (buffer == NULL)
	{
		retval = CB_error;
	}
	else
	{
		if (((buffer->headInd + 1) % buffer->maxElems) == buffer->tailInd)
		{
			retval = CB_true;
		}
	}
	emCircularPort_ExitCritical(buffer->sem);
	return retval;
}

size_t emCircularGetRemainingSpace(const CBuffer_t *buffer)
{
	if (buffer == NULL)
		return 0;
	int sem_retval = emCircularPort_EnterCritical(buffer->sem);
	if (sem_retval != 0)
	{
		emCircularPort_ExitCritical(buffer->sem);
		return 0;
	}
	size_t retval = 0;
	if (buffer->maxElems >= buffer->NbElems)
	{
		retval = buffer->maxElems - buffer->NbElems;
	}
	emCircularPort_ExitCritical(buffer->sem);
	return retval;
}

void *emCircularGetHead(CBuffer_t *buffer)
{
	if (buffer == NULL)
		return NULL;
	switch (emCircularIsFull(buffer))
	{
	case CB_true:
		CB_DEBUG_Print("CB:\tBuffer is full.\r\n");
		return NULL;
	case CB_error:
		CB_DEBUG_Print("CB:\tError checking whether CB is full.\r\n");
		return NULL;
	case CB_false:
		break;
	default:
		CB_DEBUG_Print("CB:\tError emCircularIsFull returned a a strange value.\r\n");
		return NULL;
	}
	int sem_retval = emCircularPort_EnterCritical(buffer->sem);
	if (sem_retval != 0)
	{
		emCircularPort_ExitCritical(buffer->sem);
		return NULL;
	}
	void *retval = (unsigned char *)buffer->startBuffer + (buffer->headInd * buffer->elemSize);

	CB_DEBUG_Print("CB:\tBuffer Head pointer is %p.\r\n", retval);

	buffer->headInd = (buffer->headInd + 1) % buffer->maxElems;
	buffer->NbElems++;

	if (buffer->NbElems > buffer->maxElems)
	{
		CB_DEBUG_Print("Error!! Number of elements greater than max number of elements!\r\n");
		retval = NULL;
	}
	emCircularPort_ExitCritical(buffer->sem);
	return retval;
}

void *emCircularGetTail(CBuffer_t *buffer)
{
	if (buffer == NULL)
		return NULL;
	switch (emCircularIsEmpty(buffer))
	{
	case CB_true:
		CB_DEBUG_Print("CB:\tBuffer is empty.\r\n");
		return NULL;
	case CB_error:
		CB_DEBUG_Print("CB:\tError checking whether CB is empty.\r\n");
		return NULL;
	case CB_false:
		break;
	default:
		CB_DEBUG_Print("CB:\tError emCircularIsFull returned a a strange value.\r\n");
		return NULL;
	}
	int sem_retval = emCircularPort_EnterCritical(buffer->sem);
	if (sem_retval != 0)
	{
		emCircularPort_ExitCritical(buffer->sem);
		return NULL;
	}
	void *retval = (unsigned char *)buffer->startBuffer + (buffer->tailInd * buffer->elemSize);

	CB_DEBUG_Print("CB:\tBuffer Tail pointer is %p.\r\n", retval);

	buffer->tailInd = (buffer->tailInd + 1) % buffer->maxElems;
	buffer->NbElems--;

	if (buffer->NbElems < 0)
	{
		CB_DEBUG_Print("Error!! No elements in the buffer!\r\n");
		retval = NULL;
	}
	emCircularPort_ExitCritical(buffer->sem);
	return retval;
}
