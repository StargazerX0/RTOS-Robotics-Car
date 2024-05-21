#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "port.h"
#include "constant.h"
#include "wheelControl.h"
#include "command.h"

/*----------------------------------------------------------------------------
 * Initialise the UART2 PORT for data transmission
 *---------------------------------------------------------------------------*/
void initUART1(uint32_t baud_rate) {
	uint32_t divisor, bus_clock;
	
	//enable clock to UART2 and PORTE
	SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	//connect UART pins for PTE22, PTE23
	PORTE->PCR[UART_TX_PORTE0] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_TX_PORTE0] |= PORT_PCR_MUX(3);
	
	PORTE->PCR[UART_RX_PORTE1] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_RX_PORTE1] |= PORT_PCR_MUX(3);
	
	//Ensure Tx and Rx are disabled before configuration
	UART1->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

	//Set Baud Rate to desired value
	bus_clock = (DEFAULT_SYSTEM_CLOCK) / 2;
	divisor = bus_clock / (baud_rate * 16); //16 times of sampling
	UART1->BDH = UART_BDH_SBR(divisor >> 8);
	UART1->BDL = UART_BDL_SBR(divisor);
	
	UART1->C1 = 0;
	UART1->S2 = 0;
	UART1->C3 = 0;
	
	//interrupt code
	NVIC_SetPriority(UART1_IRQn, UART1_INT_PRIO);
	NVIC_ClearPendingIRQ(UART1_IRQn);
	NVIC_EnableIRQ(UART1_IRQn);
	
	//enable UART2 receiver and transmitter
	UART1->C2 |= ((UART_C2_TE_MASK) | (UART_C2_RE_MASK));
	//enable UART2 receiver interrupt
	UART1->C2 |= UART_C2_RIE_MASK;
}

