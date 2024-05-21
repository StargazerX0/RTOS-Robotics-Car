#include "port.h"
#include "constant.h"
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

/*----------------------------------------------------------------------------
 * Initialise the GPIO for all 9 pins used for LED_CONTROL
 *---------------------------------------------------------------------------*/
void initLEDGPIO(void) {
	// Activate timer for portB and portE
	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTE_MASK));

	// Configure MUX settings to make all 9 pins GPIO
	PORTE->PCR[R_LED] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[R_LED] |= PORT_PCR_MUX(1);
	for (int i = 0; i < 4; i++) {
		PORTB->PCR[G_LED_1 + i] &= ~PORT_PCR_MUX_MASK;
		PORTB->PCR[G_LED_1 + i] |= PORT_PCR_MUX(1);
	}
	for (int i = 0; i < 4; i++) {
		PORTE->PCR[G_LED_5 + i] &= ~PORT_PCR_MUX_MASK;
		PORTE->PCR[G_LED_5 + i] |= PORT_PCR_MUX(1);
	}

	// Set Data Direction Registers for PortB and PortD as OUTPUT
	PTB->PDDR |= (MASK(G_LED_1) | MASK(G_LED_2) | MASK(G_LED_3) | MASK(G_LED_4));
	PTE->PDDR |= (MASK(R_LED) | MASK(G_LED_6) | MASK(G_LED_5) | MASK(G_LED_7) | MASK(G_LED_8));
}

void offAllLED(void) {
	PTB->PDOR &= ~(MASK(G_LED_1) | MASK(G_LED_2) | MASK(G_LED_3) | MASK(G_LED_4));
	PTE->PDOR &= ~(MASK(R_LED) | MASK(G_LED_5) | MASK(G_LED_6) | MASK(G_LED_7) | MASK(G_LED_8));
}

void frontGreenLEDStationaryMode(void) {
	
	PTB->PDOR |= MASK(G_LED_1);
	PTB->PDOR |= MASK(G_LED_2);
	PTB->PDOR |= MASK(G_LED_3);
	PTB->PDOR |= MASK(G_LED_4);
	PTE->PDOR |= MASK(G_LED_5);
	PTE->PDOR |= MASK(G_LED_6);
	PTE->PDOR |= MASK(G_LED_7);
	PTE->PDOR |= MASK(G_LED_8);
	osDelay(RED_LED_RUNNING_MODE_osDelay);
}

void rearRedLEDRunningMode(void) {
	PTE->PDOR |= MASK(R_LED);
	osDelay(RED_LED_RUNNING_MODE_osDelay);
	PTE->PDOR &= ~MASK(R_LED);
	osDelay(RED_LED_RUNNING_MODE_osDelay);
}

void rearRedLEDStationaryMode(void) {
	PTE->PDOR |= MASK(R_LED);
	osDelay(RED_LED_STATIONARY_MODE_osDelay);
	PTE->PDOR &= ~MASK(R_LED);
	osDelay(RED_LED_STATIONARY_MODE_osDelay);
}