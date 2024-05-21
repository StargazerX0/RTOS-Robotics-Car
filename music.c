//#include "MKL25Z4.h"

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "port.h"

void tone(int note, volatile uint32_t duration) {
    int factor = 37500 / note;
    TPM2->MOD = factor;
    TPM2_C0V = factor / 2;
    osDelay(duration);
}

void initSoundPWM(void) {
	//Enable cloclk gating for PORTB
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;

    // Configure PWM pin operation to Mod 3
    //PORTB->PCR[BUZZER_POS] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[BUZZER_POS] |= PORT_PCR_MUX(3);

    //PORTB->PCR[BUZZER_NEG] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[BUZZER_NEG] |= PORT_PCR_MUX(3);

    // Enable clock gating for Timer1
    SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;

    // Select clock for TPM module
    //SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    //SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

    // 48000000 / 128 = 375000; 375000 / 7500 = 50Hz
     TPM2->MOD = 750;

    // Edge-aligned PWM
    // Update SnC register: CMOD = 01, PS = 0b111 (128)
    TPM2->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
    TPM2->SC |= ((TPM_SC_CMOD(1)) | (TPM_SC_PS(7)));
    TPM2->SC &= ~(TPM_SC_CPWMS_MASK);

    // Enable PWM on TPM0 Channel 3: PTE30
    TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
    TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

    // Enable PWM on TPM0 Channel 4: PTE31
    TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
    TPM2_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}

