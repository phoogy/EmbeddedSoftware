/*
 * UART.c
 *
 *  Created on: 22 Mar 2017
 *      Author: 10692820
 */


#include "UART.h"							// Header file for UART module
#include "FIFO.h"							// Header file for FIFO module
#include "MK70F12.h"							// Header file containing peripheral memory map of the MK70F12

static TFIFO TxFIFO, RxFIFO;

bool UART_Init(const uint32_t baudRate, const uint32_t moduleClk){

  /* Enable the clock to UART2 */
  SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;

  /* Enable Port E */
  SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;

  /* Set Port E 16 and 17 to transfer and receive */
  PORTE_PCR16 |= PORT_PCR_MUX(3);
  PORTE_PCR17 |= PORT_PCR_MUX(3);

  /* Disable receiver and transmitter */
  UART2_C2 &= ~UART_C2_RE_MASK;
  UART2_C2 &= ~UART_C2_TE_MASK;

  /* Calculate baud settings */
  uint16_t baudDivisor = moduleClk / (16 * baudRate);

  /* Set baudRate register high & low bits and turn off everything else in BDH register*/
  UART2_BDH = (uint8_t)((baudDivisor & UART_BDH_SBR_MASK)>>8);
  UART2_BDL = (uint8_t)(baudDivisor & UART_BDL_SBR_MASK);
  UART2_BDH &= ~UART_BDH_LBKDIE_MASK;					// Disable LIN Break Detect Interrupt by setting bit to 0
  UART2_BDH &= ~UART_BDH_RXEDGIE_MASK;				// Disable RxD Input Active Edge Interrupt by setting bit to 0

  /* Determine if fineAdjust is needed to get closer to the baud rate */
  uint16_t fineAdjust = (((moduleClk*32)/(baudRate * 16)) - (baudDivisor * 32));

  /* Set fineAdjust And turn off everything else in C4*/
  UART2_C4 |= UART_C4_BRFA(fineAdjust);
  UART2_C4 &= ~UART_C4_M10_MASK;
  UART2_C4 &= ~UART_C4_MAEN2_MASK;
  UART2_C4 &= ~UART_C4_MAEN1_MASK;

  /*Turn off everything in C1*/
  UART2_C1 &= ~UART_C1_LOOPS_MASK;
  UART2_C1 &= ~UART_C1_UARTSWAI_MASK;
  UART2_C1 &= ~UART_C1_RSRC_MASK;
  UART2_C1 &= ~UART_C1_M_MASK;
  UART2_C1 &= ~UART_C1_WAKE_MASK;
  UART2_C1 &= ~UART_C1_ILT_MASK;
  UART2_C1 &= ~UART_C1_PE_MASK;
  UART2_C1 &= ~UART_C1_PT_MASK;

  /* Turn off everything in C2*/
  UART2_C2 &= ~UART_C2_SBK_MASK;
  UART2_C2 &= ~UART_C2_RWU_MASK;
  UART2_C2 &= ~UART_C2_ILIE_MASK;
  UART2_C2 &= ~UART_C2_RIE_MASK;
  UART2_C2 &= ~UART_C2_TCIE_MASK;
  UART2_C2 &= ~UART_C2_TIE_MASK;
  UART2_C2 &= ~UART_C2_RE_MASK;
  UART2_C2 &= ~UART_C2_TE_MASK;

  /* Turn off everything in C3*/
  UART2_C3 &= ~UART_C3_PEIE_MASK;
  UART2_C3 &= ~UART_C3_FEIE_MASK;
  UART2_C3 &= ~UART_C3_NEIE_MASK;
  UART2_C3 &= ~UART_C3_ORIE_MASK;
  UART2_C3 &= ~UART_C3_TXINV_MASK;
  UART2_C3 &= ~UART_C3_TXDIR_MASK;
  UART2_C3 &= ~UART_C3_T8_MASK;
  UART2_C3 &= ~UART_C3_R8_MASK;

  /* Turn off everything in C3*/
    UART2_C5 &= ~UART_C5_RDMAS_MASK;
    UART2_C5 &= ~UART_C5_TDMAS_MASK;

/* Enable receiver and transmitter */
  UART2_C2 |= UART_C2_RE_MASK;
  UART2_C2 |= UART_C2_TE_MASK;

  /* Initialise FIFO */
  FIFO_Init(&RxFIFO);
  FIFO_Init(&TxFIFO);

    return true;
}

bool UART_InChar(uint8_t * const dataPtr){
  /* Returns whether getting data from RxFIFO buffer failed or succeeded */
  return FIFO_Get(&RxFIFO,dataPtr);
}

bool UART_OutChar(const uint8_t data){
  /* Returns whether putting data into TxFIFO buffer failed or succeeded */
  return FIFO_Put(&TxFIFO,data);
}

void UART_Poll(void){
  /* If the RDRF flag is set, put data from UART data register into RxFIFO buffer */
  if (UART2_S1 & UART_S1_RDRF_MASK){
      FIFO_Put(&RxFIFO,UART2_D);	//  Need to fix. Upon tower sending data to pc, instead of 0x00 it sends 0x80. Not sure why
  }

  /* If the TDRE flag is set, get data from TxFIFO into UART data register */
  if(UART2_S1 & UART_S1_TDRE_MASK){
      FIFO_Get(&TxFIFO,(uint8_t *)&UART2_D);
  }
}
