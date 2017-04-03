/*
 * FIFO.c
 *
 *  Created on: 22 Mar 2017
 *      Author: 10692820
 */


#include "FIFO.h"

void FIFO_Init(TFIFO * const FIFO){
  FIFO->Start = 0;
  FIFO->End = 0;
  FIFO->NbBytes = 0;

}

bool FIFO_Put(TFIFO * const FIFO, const uint8_t data){
  if(FIFO->NbBytes == FIFO_SIZE){
      return false;
  }else{
      FIFO->Buffer[FIFO->Start] = data;
      FIFO->NbBytes++;
      FIFO->Start++;
      if(FIFO->Start == FIFO_SIZE){
	  FIFO->Start = 0;
      }
      return true;
  }
}

bool FIFO_Get(TFIFO * const FIFO, uint8_t * const dataPtr){
  if(FIFO->NbBytes == 0){
      return false;
  }else{
      *dataPtr = FIFO->Buffer[FIFO->End];
      FIFO->NbBytes--;
      FIFO->End++;
      if(FIFO->End == FIFO_SIZE){
	  FIFO->End = 0;
      }
      return true;
  }
}
