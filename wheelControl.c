#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "port.h"
#include "constant.h"
#include "command.h"

/*----------------------------------------------------------------------------
 * Initialise the PWM output for 4 wheels
 *---------------------------------------------------------------------------*/
void initWheelPWM(void) {
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	PORTA->PCR[RIGHT_FRONT_WHEEL] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[RIGHT_FRONT_WHEEL] |= PORT_PCR_MUX(3); // tpm0-ch2
	PORTA->PCR[RIGHT_BACK_WHEEL] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[RIGHT_BACK_WHEEL] |= PORT_PCR_MUX(3); // tpm0-ch1
	
	PORTA->PCR[LEFT_FRONT_WHEEL] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[LEFT_FRONT_WHEEL] |= PORT_PCR_MUX(3); // tpm1-ch0
	PORTA->PCR[LEFT_BACK_WHEEL] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[LEFT_BACK_WHEEL] |= PORT_PCR_MUX(3); // tpm1-ch1
	
	//enable timer for tpm0 and tpm1
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK | SIM_SCGC6_TPM1_MASK;
	
	//clear TPMSRC bits
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	//select the internal clock
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	//48_000_000 / 128 = 37500; 
	TPM0->MOD = 7500;
	TPM1->MOD = 7500;
	
	TPM0->SC &= ~(TPM_SC_CMOD_MASK | TPM_SC_PS_MASK);
	TPM0->SC |= TPM_SC_CMOD(1) | TPM_SC_PS(7);
	TPM0->SC &= ~(TPM_SC_CPWMS_MASK);
	
	TPM1->SC &= ~(TPM_SC_CMOD_MASK | TPM_SC_PS_MASK);
	TPM1->SC |= TPM_SC_CMOD(1) | TPM_SC_PS(7);
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
	
	TPM0_C1SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
	TPM0_C1SC |= TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1);
	TPM0_C2SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
	TPM0_C2SC |= TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1);
	
	TPM1_C0SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
	TPM1_C0SC |= TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1);
	TPM1_C1SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
	TPM1_C1SC |= TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1);
}

void stop() {
	TPM0_C1V = 0;
	TPM0_C2V = 0;
	
	TPM1_C0V = 0;
	TPM1_C1V = 0;
}

void forward() {
	TPM0_C1V = 7500;
	TPM0_C2V = 0; //CPM_C2V high, moving back
			
	TPM1_C0V = WHEEL_FORWARD_FULL_SPEED; // PTB12/13
	TPM1_C1V = 0;
}

void back() {
	TPM0_C1V = 0;
	TPM0_C2V = 5000;
			
	TPM1_C0V = 0;
	TPM1_C1V = 5000;
}

void forwardFullSpeed() {
	TPM0_C1V = WHEEL_FORWARD_FULL_SPEED;
	TPM0_C2V = 0; //CPM_C2V high, moving back
			
	TPM1_C0V = WHEEL_FORWARD_FULL_SPEED; // PTB12/13
	TPM1_C1V = 0;
}

void forwardSlowSpeed() {
	TPM0_C1V = 0;
	TPM0_C2V = WHEEL_FORWARD_SLOW_SPEED;
			
	TPM1_C0V = WHEEL_FORWARD_SLOW_SPEED; // PTB12/13
	TPM1_C1V = 0;
}

void sharpRightTurn() {
	TPM0_C1V = WHEEL_FORWARD_FULL_SPEED;
	TPM0_C2V = 0; //CPM_C2V high, moving back
			
	TPM1_C0V = 0;
	TPM1_C1V = WHEEL_FORWARD_FULL_SPEED; // PTB12/13
}

void sharpLeftTurn() {
	TPM0_C1V = 0;
	TPM0_C2V = WHEEL_FORWARD_FULL_SPEED;
			
	TPM1_C0V = WHEEL_FORWARD_FULL_SPEED;
	TPM1_C1V = 0;
}

void rightTurn() {
	TPM0_C1V = WHEEL_FORWARD_FULL_SPEED;
	TPM0_C2V = 0;

	TPM1_C1V = WHEEL_FORWARD_SLOW_SPEED;
	TPM1_C0V = 0;
}

//?????
void leftTurn() {
	TPM0_C1V = 0;
	TPM0_C2V = WHEEL_FORWARD_SLOW_SPEED;

	TPM1_C1V = 0;
	TPM1_C0V = WHEEL_FORWARD_FULL_SPEED;
}
