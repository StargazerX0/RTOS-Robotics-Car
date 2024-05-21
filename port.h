/*----------------------------------------------------------------------------
 * UART_TRANSMISSION PORT
 *---------------------------------------------------------------------------*/
#define UART_TX_PORTE0 0
#define UART_RX_PORTE1 1
#define UART1_INT_PRIO 128

/*----------------------------------------------------------------------------
 * LED_CONTROL PORT
 *---------------------------------------------------------------------------*/
//FRONT_GREEN_LED
#define G_LED_1 8 //PTB8
#define G_LED_2 9 //PTB9
#define G_LED_3 10 //PTB10
#define G_LED_4 11 //PTB11
#define G_LED_5 2 //PTE2
#define G_LED_6 3 //PTE3
#define G_LED_7 4 //PTE4
#define G_LED_8 5 //PTE5
//REAR_RED_LED
#define R_LED 20 //PTE20

/*----------------------------------------------------------------------------
 * WPM_WHELL_CONTROL PORT
 *---------------------------------------------------------------------------*/
#define RIGHT_FRONT_WHEEL 5 //PTA5
#define RIGHT_BACK_WHEEL 4 //PTA4
#define LEFT_FRONT_WHEEL 12 //PTA12
#define LEFT_BACK_WHEEL 13 //PTA13

#define BUZZER_POS 2 // + : PTB2
#define BUZZER_NEG 3 // - : PTB3

#define MASK(x) (1 << (x))
#define Q_SIZE (32)
