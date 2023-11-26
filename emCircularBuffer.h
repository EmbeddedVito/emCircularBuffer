/*
 * @file emCircularBuffer.h
 * @author: Mannone Vito
 *
 * @brief This module implements a FIFO circular buffer that is data type independent.
 *
 * The circular buffer is implemented so that it is not possible to add
 * new elements to the buffer if it is full and is not possible to take elements
 * from the buffer if it is empty.
 * To make this module thread-safe, it is necessary to define the locking mechanism
 * and functions in the emCircularPort.h file.
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

#ifndef EMCIRCULARBUFFER_H_
#define EMCIRCULARBUFFER_H_

#include <stddef.h>

/*
 * Necessary to define the porting functions
 */
#include "emCircularPort.h"

/*
 * User defines for configuration
 */
#define CB_DEBUG 0 // Set to 1 to call printf function for debugging log messages

/*
 * Definitions of module return values
 */
typedef enum
{
	CB_error = -1,
	CB_true,
	CB_false
} CBStatus_t;

/*
 * Definition of the circular buffer data type
 */
typedef struct CBuffer_t
{
	size_t tailInd;				// index of the tail element to be taken
	size_t headInd;				// index of the head element that can be used
	unsigned char *startBuffer; // pointer of the first address of the buffer used
								// for the circular buffer
	size_t elemSize;			// dimension of the elements of the buffer
	size_t maxElems;			// dimension of the buffer in terms of number of elements
	size_t NbElems;				// actual number of elements in the buffer
	CB_sem_t sem;				// semaphore to be used
} CBuffer_t;

/*
 * @brief This function initializes the circular buffer allocating the necessary memory for it.
 *
 * @param maxElems, number of elements of the buffer
 * @param elemSize, size of every element in terms of bytes
 * @param sem_name, name for the semafore initialisation. Can be NULL if
 * 		no locking mechanism is defined
 * @return CBuffer_t*, pointer to the circular buffer created. Returns
 * 		NULL if it was not possible to create the buffer
 */
CBuffer_t *emCircularInit(const size_t maxElems, const size_t elemSize, const char *sem_name);

/*
 * @brief This function deletes and frees all the memory dedicated to the
 * 		circular buffer.
 *
 * @param buffer, pointer to the circular buffer to be deleted
 * @return CBStatus_t, return value.
 */
CBStatus_t emCircularDelete(CBuffer_t *buffer);

/*
 * @brief This function is used to know if the buffer is empty.
 *
 * @param buffer, pointer to the circular buffer to be checked
 * @return CBStatus_t, return value. Returns CB_true if the buffer
 * 		is empty, CB_false otherwise
 */
CBStatus_t emCircularIsEmpty(const CBuffer_t *buffer);

/*
 * @brief This function is used to know if the buffer is full.
 *
 * @param buffer, pointer to the circular buffer to be checked
 * @return CBStatus_t, return value. Returns CB_true if the buffer
 * 		is full, CB_false otherwise
 */
CBStatus_t emCircularIsFull(const CBuffer_t *buffer);

/*
 * @brief This function is used to know how many empty elements are left
 * 		in the circular buffer. It represents the number of remaining
 * 		emCircularGetHead() call that can be made.
 *
 * @param buffer, pointer to the circular buffer to be checked
 * @return size_t, number of elements
 */
size_t emCircularGetRemainingSpace(const CBuffer_t *buffer);

/*
 * @brief This function is used to get the pointer to the next
 * 		free block of memory to be used.
 *
 * @param buffer, pointer to the circular buffer to be used
 * @return void*, pointer to the next free element of the buffer
 */
void *emCircularGetHead(CBuffer_t *buffer);

/*
 * @brief This function is used to get the pointer to the next
 * 		block of memory to be read.
 *
 * @param buffer, pointer to the circular buffer to be used
 * @return void*, pointer to the next element of the buffer
 * 		that has to be used/read
 */
void *emCircularGetTail(CBuffer_t *buffer);

#endif /* EMCIRCULARBUFFER_H_ */
