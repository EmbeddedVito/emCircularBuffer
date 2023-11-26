@author: Mannone Vito

@brief This module implements a FIFO circular buffer that is data type independent.

The circular buffer is implemented so that it is not possible to add new elements to the buffer if it is full and is not possible to take elements from the buffer if it is empty.
To make this module thread-safe, it is necessary to define the locking mechanism and functions in the emCircularPort.h file.



