/*
 * @file emCircularPort.h
 * @author Mannone Vito
 *
 * @brief Porting header file for emCircularBuffer software module.
 * This header file must be used to define the implementation of all the
 * functions needed for this sopftware module.
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

#ifndef EMCIRCULARPORT_H_
#define EMCIRCULARPORT_H_

/*
 * Use this define to enable/disable the use of the software module
 * emAlloc to handle the dynamic allocation of memory, otherwise stdlib
 * will be used.
 * Set this define to 0 and add your own custom malloc/free implementation
 * if necessary in the place of malloc/free from stdlib.
 */
#define CIRCULAR_USE_EMALLOC 0

/*
 * Use this define to enable/disable the use of locking mechanisms
 */
#define CIRCULAR_USE_LOCK_MECHANISM 0

/*
 * Definition of necessary functions for memory management:
 * 		emCircularPortMalloc(bytes) [dynamic memory allocation]
 * 			@param bytes, number of bytes to allocate
 * 			@return void*, pointer to memory area allocated
 * 		emCircularPortFree(pointer) [dynamic free of memory block]
 * 			@param pointer, pointer to the memory block to be freed
 * 			@return void, not evaluated
 */
#if CIRCULAR_USE_EMALLOC
#include "emAlloc.h"
#define emCircularPortMalloc(bytes) emMalloc(bytes)
#define emCircularPortFree(pointer) emFree(pointer)
#else
#include <stdlib.h>
#define emCircularPortMalloc(bytes) malloc(bytes)
#define emCircularPortFree(pointer) free(pointer)
#endif

/*
 * Necessary definition for lock/unlock functions
 * Note: only binary semaphore must be used
 * 		emCircularPort_EnterCritical(ptrSem)[binary semaphore acquisition]
 * 			@param ptrSem, pointer to the semaphore to be used
 * 			@return 0, if there is no error in the semaphore acquisition
 * 		emCircularPort_ExitCritical(ptrSem) [semaphore release]
 * 			@param ptrSem, pointer to the semaphore to be used
 * 			@return void, not evaluated
 * 		emCircularPort_InitBynSem(strName) [initialization of the binary semaphore]
 * 			@param strName, semaphore name
 * 			@return void*, pointer to the semaphore created
 * 		emCircularPort_BynSemDelete(ptrSem) [semaphore deletion]
 * 			@param ptrSem, pointer to the semaphore to be deleted
 * 			@return void, not evaluated
 *
 * Note: here is used the library "cmsis_os2.h" to handle the system-calls, as example
 */
#if CIRCULAR_USE_LOCK_MECHANISM
#include "cmsis_os2.h"
#define emCircularPort_EnterCritical(ptrSem) (osSemaphoreAcquire(ptrSem, osWaitForever))
#define emCircularPort_ExitCritical(ptrSem) (osSemaphoreRelease(ptrSem))
#define emCircularPort_InitBynSem(strName) (osSemaphoreNew(1, 1, NULL))
#define emCircularPort_BynSemDelete(ptrSem) (osSemaphoreDelete(ptrSem))
typedef osSemaphoreId_t CB_sem_t;
#else
#define emCircularPort_EnterCritical(ptrSem) (0)
#define emCircularPort_ExitCritical(ptrSem) (void)ptrSem
#define emCircularPort_InitBynSem(strName) \
    (NULL);                                \
    (void)strName
#define emCircularPort_BynSemDelete(ptrSem) (void)ptrSem
typedef void *CB_sem_t;
#endif /* USE_LOCK_MECHANISM */

#endif /* EMCIRCULARPORT_H_ */
