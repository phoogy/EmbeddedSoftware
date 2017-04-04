/*! @file main.c
 *  @version 1.0
 *  @brief
 *           Main module.
 *           This module contains user's application code.
 *  @author Duong Phong Au 10692820 & Emily Tiang 12016681
 *  @date 20 Mar 2017
 */

// CPU mpdule - contains low level hardware initialization routines
#include "Cpu.h"
#include "packet.h"
#include "UART.h"
/*lint -save  -e970 Disable MISRA rule (6.3) checking. */

//static TFIFO TxFIFO, RxFIFO;
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */
  /* Sets the baudRate to 38400 */
  uint32_t baudRate = 38400;

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */

  /* Initialise Packet */
  Packet_Init(baudRate,CPU_BUS_CLK_HZ);

  for(;;)
  {
    if(Packet_Get())
      HandlePacket();
    UART_Poll();
  }

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */

/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
