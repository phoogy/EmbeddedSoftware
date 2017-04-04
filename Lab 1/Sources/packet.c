/*! @file packet.c
 *  @brief Contains the functions for handling 5-byte packets
 *  @author Phong 10692820 & Emily 12016681
 *  @date 22 Mar 2017
 */

#include "packet.h"
#include "UART.h"

#define CMD_TOWER_STARTUP 0x04	//Tower Startup command in hex
#define CMD_TOWER_VERSION 0x09	//Tower Version command in hex
#define CMD_TOWER_NUMBER 0x0B	//Tower Number command in hex

/*
 * @brief Determines the packet's instructions and calls the relevant function.
 * @return void.
 */
void HandlePacket(void);

/*
 * @brief Calculates the checksum of the first four bytes in the packet and compares it to Packet_Checksum.
 * @return bool TRUE if the calculation equals the Packet_Checksum.
 */
bool Check_Checksum(void);

/*
 * @brief Splits the tower number into Most Significant Bit and Least Significant Bit.
 * @param towerNumber The tower number to be sent and displayed in decimal.
 * @return bool TRUE after the split.
 */
bool HandleTowerNumber(uint16_t towerNumber);

/*
 * @brief Sends the Tower Number packet.
 * @return bool TRUE if packet was successfully put into the TxFIFO buffer.
 */
bool TowerNumber_Send(void);

/*
 * @brief Sends a Tower Startup packet.
 * @return bool TRUE if packet was successfully put into the TxFIFO buffer.
 */
bool TowerStartup_Send(void);

/*
 * @brief Sends a Tower Version packet.
 * @return bool TRUE if packet was successfully put into the TxFIFO buffer.
 */
bool TowerVersion_Send(void);

/*
 * @brief Sends an ACK Response if there was an ACK Request.
 * @return void.
 */
void ACK_Check(void);

/*
 * @brief Sends a NAK Response if there was an ACK Request.
 * @return void.
 */
void NAK_Check(void);

/* Variable Definitions */
uint8_t NumBytes,
	Tower_Number_MSB,
	Tower_Number_LSB,
	Packet_Command,
	Packet_Parameter1,
	Packet_Parameter2,
	Packet_Parameter3,
	Packet_Checksum;

bool Packet_Init(const uint32_t baudRate, const uint32_t moduleClk){

  /* Initialise Tower Number MSB and LSB*/
  Tower_Number_MSB = 0;
  Tower_Number_LSB = 0;
  HandleTowerNumber(2820);

  /* Initialise Packet Variables*/
  NumBytes = 0;
  Packet_Command = 0;
  Packet_Parameter1 = 0;
  Packet_Parameter2 = 0;
  Packet_Parameter3 = 0;
  Packet_Checksum = 0;

  /* Initialise UART, Send Tower Startup Packet, Tower Version Packet and TowerNumber Packet*/
  return UART_Init(baudRate,moduleClk) && TowerStartup_Send() && TowerVersion_Send() && TowerNumber_Send();
}

bool Packet_Get(void){
  // Receives packet and returns true if successful
  if(UART_InChar(&Packet_Checksum)){
    NumBytes++;
    // Reset counter if valid packet is received
    if(NumBytes == 5){
      if(Check_Checksum()){
	NumBytes = 0;
	Packet_Checksum = 0;
	return true;
      }
	else
	  NumBytes--;
    }
    // Shifts the bytes forward if any one fails to transmit
    Packet_Command = Packet_Parameter1;
    Packet_Parameter1 = Packet_Parameter2;
    Packet_Parameter2 = Packet_Parameter3;
    Packet_Parameter3 = Packet_Checksum;
  }
  return false;
}

bool Packet_Put(const uint8_t command, const uint8_t parameter1, const uint8_t parameter2, const uint8_t parameter3){
  // Puts each byte into the FIFO and returns true if all is successful
  return(UART_OutChar(command) && UART_OutChar(parameter1) && UART_OutChar(parameter2) && UART_OutChar(parameter3) && UART_OutChar(command ^ parameter1 ^ parameter2 ^ parameter3));
}

bool Check_Checksum(void){
  // Does a parity check and returns true if matches up with the checksum
  return (Packet_Checksum == (Packet_Command ^ Packet_Parameter1 ^ Packet_Parameter2 ^ Packet_Parameter3));
}

void HandlePacket(void){

  /* Performs special command Get startup values */
  /* Calls ACK if successful; Calls NAK if fails */
  if((Packet_Command & 0x7f) == CMD_TOWER_STARTUP && Packet_Parameter1 == 0 && Packet_Parameter2 == 0 && Packet_Parameter3 == 0){
    if(TowerStartup_Send() && TowerVersion_Send() && TowerNumber_Send())
      ACK_Check();
    else
      NAK_Check();
  }

  /* Performs special command Get version */
  /* Calls ACK if successful; Calls NAK if fails */
  if((Packet_Command & 0x7f) == CMD_TOWER_VERSION && Packet_Parameter1 == 118 && Packet_Parameter2 == 120 && Packet_Parameter3 == 13){
    if(TowerVersion_Send())
      ACK_Check();
    else
      NAK_Check();
  }

  /* Sets and displays tower number */
  /* Calls ACK if successful; Calls NAK if fails */
  if((Packet_Command & 0x7f) == CMD_TOWER_NUMBER){
    if(Packet_Parameter1 == 1 && Packet_Parameter2 == 0 && Packet_Parameter3 == 0){
      if(TowerNumber_Send())
	ACK_Check();
      else
	NAK_Check();
    }
    if(Packet_Parameter1 == 2){
      uint16_t towerNumber = ((uint16_t)Packet_Parameter3 << 8) + Packet_Parameter2;
      if(HandleTowerNumber(towerNumber))
	ACK_Check();
      else
	NAK_Check();
    }
  }
}

/* Sends tower number */
bool TowerNumber_Send(void){
  return Packet_Put(CMD_TOWER_NUMBER,1,Tower_Number_LSB,Tower_Number_MSB);
}

/* Sends tower version */
bool TowerVersion_Send(void){
  return Packet_Put(CMD_TOWER_VERSION,118,1,0);
}

/* Sends tower startup packet */
bool TowerStartup_Send(void){
  return Packet_Put(CMD_TOWER_STARTUP,0,0,0);
}

/* Sets tower number */
/* Splits binary into MSB and LSB */
bool HandleTowerNumber(uint16_t towerNumber){
  Tower_Number_MSB = (uint8_t)((towerNumber & 0xFF00)>>8);
  Tower_Number_LSB = (uint8_t)(towerNumber & 0xFF);
  return true;
}

/* Checks the eighth bit for an ACK request */
/* Sends ACK if there is a request */
void ACK_Check(void){
  if((Packet_Command & 0x80) == 0x80)
    Packet_Put(Packet_Command |= 0x80,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
}

/* Sends NAK if there is a request */
void NAK_Check(void){
  if((Packet_Command & 0x80) == 0x80)
    Packet_Put(Packet_Command &= ~0x80,Packet_Parameter1,Packet_Parameter2,Packet_Parameter3);
}
