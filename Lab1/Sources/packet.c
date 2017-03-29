/*
 * packet.c
 *
 *  Created on: 22 Mar 2017
 *      Author: 10692820
 */

#include <stdio.h>
#include "packet.h"
#include "UART.h"

bool checkChecksum(void);

void HandlePacket(void);

uint8_t NumBytes,
	Packet_Command,		/*!< Initialising the packet's command */
	Packet_Parameter1,	/*!< Initialising the packet's parameter1 */
	Packet_Parameter2,	/*!< Initialising the packet's parameter2 */
	Packet_Parameter3,	/*!< Initialising the packet's parameter3 */
	Packet_Checksum;	/*!< Initialising the packet's checksum */

bool Packet_Init(const uint32_t baudRate, const uint32_t moduleClk){
  NumBytes = 0;
  Packet_Command = 0;
  Packet_Parameter1 = 0;
  Packet_Parameter2 = 0;
  Packet_Parameter3 = 0;
  Packet_Checksum = 0;
  return UART_Init(baudRate,moduleClk);
}

bool Packet_Get(void){
  if(UART_InChar(&Packet_Checksum)){
      NumBytes++;
      if (NumBytes == 5){
	  if(checkChecksum()){
	      NumBytes = 0;
	      return true;
	  }
	  else{
	      Packet_Command = Packet_Parameter1;
	      Packet_Parameter1 = Packet_Parameter2;
	      Packet_Parameter2 = Packet_Parameter3;
	      Packet_Parameter3 = Packet_Checksum;
	      NumBytes--;
	  }
      }
  }
  return false;
}
//
//  return false;
//
//    switch(NumBytes)
//    {
//      case 0:
//        if(UART_InChar(&Packet_Command))
//  	NumBytes++;
//        break;
//      case 1:
//        if(UART_InChar(&Packet_Parameter1))
//        	NumBytes++;
//        break;
//      case 2:
//        if(UART_InChar(&Packet_Parameter2))
//  	NumBytes++;
//        break;
//      case 3:
//        if(UART_InChar(&Packet_Parameter3))
//  	NumBytes++;
//        break;
//      case 4:
//        if(UART_InChar(&Packet_Checksum))
//  	NumBytes++;
//        break;
//      case 5:
//
//        if(checkChecksum()){
//	  NumBytes = 0;
//	  Packet_Command = 0;
//	  Packet_Parameter1 = 0;
//	  Packet_Parameter2 = 0;
//	  Packet_Parameter3 = 0;
//	  Packet_Checksum = 0;
//  	  return true;
//        }
//        else{
//  	  Packet_Command = Packet_Parameter1;
//  	  Packet_Parameter1 = Packet_Parameter2;
//  	  Packet_Parameter2 = Packet_Parameter3;
//  	  Packet_Parameter3 = Packet_Checksum;
//  	  NumBytes--;
//        }
//        break;
//    }
//    return false;
//
//}



bool Packet_Put(const uint8_t command, const uint8_t parameter1, const uint8_t parameter2, const uint8_t parameter3){
  // Puts each byte into the FIFO and returns true if all is successful
  return(UART_OutChar(command) && UART_OutChar(parameter1) && UART_OutChar(parameter2) && UART_OutChar(parameter3) && UART_OutChar(calculateChecksum(Packet_Command,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3)));
}

bool checkChecksum(void){
  return (Packet_Checksum == (Packet_Command ^ Packet_Parameter1 ^ Packet_Parameter2 ^ Packet_Parameter3));
}

void HandlePacket(void){
//  if(Packet_Command == 0x04)
}

