/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"
#include "port.h"
#include "constant.h"
#include "ledControl.h"
#include "wheelControl.h"
#include "music.h"
#include "UART2.h"

#define MSG_COUNT 1

volatile uint8_t receivedData; // command sent by UART

osEventFlagsId_t main_Id;
osEventFlagsId_t green_led_running_Id, green_led_stop_Id, red_led_running_Id, red_led_stop_Id, music_running_Id, music_ending_Id;

volatile int green_led_index = 0;
volatile int hasStopped = 0;

/*----------------------------------------------------------------------------
 * UART Interrupt hand
 *---------------------------------------------------------------------------*/
void UART1_IRQHandler(void)
{
	NVIC_ClearPendingIRQ(UART1_IRQn);

	if (UART1->S1 & UART_S1_RDRF_MASK)
	{
		osEventFlagsSet(main_Id, 0x01);
		receivedData = UART1->D;
	}
}

/*----------------------------------------------------------------------------
 * Green led running thread
 *---------------------------------------------------------------------------*/
void led_green_running_thread(void *argument)
{

	int green_led_ports[8] = {8, 9, 10, 11, 2, 3, 4, 5};

	for (green_led_index; green_led_index < 8; green_led_index++)
	{
		osEventFlagsWait(green_led_running_Id, 0x01, osFlagsNoClear, osWaitForever);
		offAllLED();
		if (green_led_index < 4)
		{
			PTB->PDOR |= MASK(green_led_ports[green_led_index]);
			osDelay(GREEN_LED_RUNNING_MODE_osDelay);
			if (!hasStopped)
			{
				PTB->PDOR &= ~MASK(green_led_ports[green_led_index]);
			}
		}
		else
		{
			PTE->PDOR |= MASK(green_led_ports[green_led_index]);
			osDelay(GREEN_LED_RUNNING_MODE_osDelay);
			if (!hasStopped)
			{
				PTE->PDOR &= ~MASK(green_led_ports[green_led_index]);
			}
		}
		if (green_led_index == 7)
		{
			green_led_index = -1;
		}
	}
}

void led_green_stop_thread(void *argument)
{
	for (;;)
	{
		osEventFlagsWait(green_led_stop_Id, 0x01, osFlagsNoClear, osWaitForever);
		frontGreenLEDStationaryMode();
	}
}

void led_red_running_thread(void *argument)
{
	for (;;)
	{
		osEventFlagsWait(red_led_running_Id, 0x01, osFlagsNoClear, osWaitForever);
		rearRedLEDRunningMode();
	}
}

void led_red_stop_thread(void *argument)
{
	for (;;)
	{
		osEventFlagsWait(red_led_stop_Id, 0x01, osFlagsNoClear, osWaitForever);
		rearRedLEDStationaryMode();
	}
}

void music_ending_thread(void *argument)
{
	int tempo = 80;
	// this calculates the duration of a whole note in ms
	int wholenote = (60000 * 4) / tempo;
	int divider = 0;
	int noteDuration = 0;
	for (int thisNote = 0; thisNote < 260; thisNote += 2)
	{
		osEventFlagsWait(music_ending_Id, 0x01, osFlagsNoClear, osWaitForever);

		// calculates the duration of each note
		divider = thrones[thisNote + 1];
		if (divider > 0)
		{
			// regular note, just proceed
			noteDuration = wholenote / divider;
		}
		else
		{
			// dotted notes are represented with negative durations!!
			noteDuration = 1.5 * wholenote / (-divider); // increases the duration in half for dotted notes
		}

		// we only play the note for 90% of the duration, leaving 10% as a pause
		tone(thrones[thisNote], noteDuration * 0.9);
		// Wait for the specief duration before playing the next note.
		osDelay(noteDuration);
		if (thisNote == 258)
		{
			thisNote = 0;
		}
	}
}

void music_running_thread(void *argument)
{
	int tempo = 80;
	// this calculates the duration of a whole note in ms
	int wholenote = (60000 * 4) / tempo;
	int divider = 0;
	int noteDuration = 0;
	for (int thisNote = 0; thisNote < 1206; thisNote += 2)
	{
		osEventFlagsWait(music_running_Id, 0x01, osFlagsNoClear, osWaitForever);

		// calculates the duration of each note
		divider = elise[thisNote + 1];
		if (divider > 0)
		{
			// regular note, just proceed
			noteDuration = wholenote / divider;
		}
		else
		{
			// dotted notes are represented with negative durations!!
			noteDuration = 1.5 * wholenote / (-divider); // increases the duration in half for dotted notes
		}

		// we only play the note for 90% of the duration, leaving 10% as a pause
		tone(elise[thisNote], noteDuration * 0.9);
		// Wait for the specief duration before playing the next note.
		osDelay(noteDuration);
		if (thisNote == 1204)
		{
			thisNote = 0;
		}
	}
}

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void app_main(void *argument)
{
	for (;;)
	{
		osEventFlagsWait(main_Id, 0x01, osFlagsWaitAny, osWaitForever);
		if (receivedData == STOP)
		{
			hasStopped = 1;
			stop();
			osEventFlagsClear(green_led_running_Id, 0x01);
			osEventFlagsSet(green_led_stop_Id, 0x01);
			osEventFlagsClear(red_led_running_Id, 0x01);
			osEventFlagsSet(red_led_stop_Id, 0x01);
			green_led_index = -1;
		}
		else if (receivedData == ENDOFTASK)
		{
			osEventFlagsClear(music_running_Id, 0x01);
			osEventFlagsSet(music_ending_Id, 0x01);
			hasStopped = 1;
			osEventFlagsClear(green_led_running_Id, 0x01);
			osEventFlagsSet(green_led_stop_Id, 0x01);
			osEventFlagsClear(red_led_running_Id, 0x01);
			osEventFlagsSet(red_led_stop_Id, 0x01);
			green_led_index = -1;
			stop();
		}
		else
		{
			hasStopped = 0;
			osEventFlagsSet(green_led_running_Id, 0x01);
			osEventFlagsClear(green_led_stop_Id, 0x01);
			osEventFlagsSet(red_led_running_Id, 0x01);
			osEventFlagsClear(red_led_stop_Id, 0x01);
			if (receivedData == FORWARD)
			{
				forward();
			}
			else if (receivedData == BACK)
			{
				back();
			}
			else if (receivedData == RIGHTTRUN)
			{
				rightTurn();
			}
			else if (receivedData == LEFTTRUN)
			{
				leftTurn();
			}
			else if (receivedData == FORWARDFULLSPEED)
			{
				forwardFullSpeed();
			}
			else if (receivedData == FORWARDSLOWSPEED)
			{
				forwardSlowSpeed();
			}
			else if (receivedData == SHARPRIGHTTRUN)
			{
				sharpRightTurn();
			}
			else if (receivedData == SHARPLEFTTRUN)
			{
				sharpLeftTurn();
			}
			else
			{
				break;
			}
		}
	}
}

int main(void)
{
	// System Initialization
	SystemCoreClockUpdate();
	initLEDGPIO();
	initWheelPWM();
	initSoundPWM();
	initUART1(BAUD_RATE);

	// ...
	osKernelInitialize(); // Initialize CMSIS-RTOS

	main_Id = osEventFlagsNew(NULL);
	green_led_running_Id = osEventFlagsNew(NULL);
	green_led_stop_Id = osEventFlagsNew(NULL);
	red_led_running_Id = osEventFlagsNew(NULL);
	red_led_stop_Id = osEventFlagsNew(NULL);
	music_running_Id = osEventFlagsNew(NULL);
	music_ending_Id = osEventFlagsNew(NULL);

	osEventFlagsSet(green_led_stop_Id, 0x01);
	osEventFlagsSet(red_led_stop_Id, 0x01);
	osEventFlagsSet(music_running_Id, 0x01);
	osThreadNew(app_main, NULL, NULL); // Create application main thread
	osThreadNew(led_green_running_thread, NULL, NULL);
	osThreadNew(led_green_stop_thread, NULL, NULL);
	osThreadNew(led_red_running_thread, NULL, NULL);
	osThreadNew(led_red_stop_thread, NULL, NULL);
	osThreadNew(music_running_thread, NULL, NULL);
	osThreadNew(music_ending_thread, NULL, NULL);

	osKernelStart(); // Start thread execution

	for (;;)
	{
	}
}
