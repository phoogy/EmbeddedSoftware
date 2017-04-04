/*! @file FIFO.c
 *  @brief Contains the functions for accessing a byte-wide FIFO.
 *  @author Phong 10692820 & Emily 12016681
 *  @date 22 Mar 2017
 */

#include "FIFO.h"

/* Initialise and set variables to zero */
void FIFO_Init(TFIFO * const FIFO){
  FIFO->Start = 0;
  FIFO->End = 0;
  FIFO->NbBytes = 0;
}

/* Puts data into FIFO buffer */
bool FIFO_Put(TFIFO * const FIFO, const uint8_t data){
  // Returns false if array is full
  if(FIFO->NbBytes == FIFO_SIZE)
    return false;
  // Puts data into buffer and increments counter
  else{
    FIFO->Buffer[FIFO->Start] = data;
    FIFO->NbBytes++;
    FIFO->Start++;
    // Resets counter when data saved is the last one on the array
    if(FIFO->Start == FIFO_SIZE)
      FIFO->Start = 0;
    return true;
  }
}

/* Gets data from FIFO buffer */
bool FIFO_Get(TFIFO * const FIFO, uint8_t * const dataPtr){
  // Returns false if array is empty
  if(FIFO->NbBytes == 0)
    return false;
  // Retrieves data from buffer and decrements counter
  else{
    *dataPtr = FIFO->Buffer[FIFO->End];
    FIFO->NbBytes--;
    FIFO->End++;
    // Resets counter when data retrieved is the last one on the array
    if(FIFO->End == FIFO_SIZE)
	FIFO->End = 0;
    return true;
  }
}
