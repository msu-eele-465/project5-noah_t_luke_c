#include "intrinsics.h"
#include <msp430.h>
#include "keypad.h"

#define unlock_code "1738"
unsigned char data = 0x16;
int lock_status = 1;
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    P1SEL1 &= ~BIT2 & BIT3;
    P1SEL0 |= BIT2 | BIT3;                  // I2C pins
    
    keypadInit();

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure USCI_B0 for I2C mode
    UCB0CTLW0 |= UCSWRST;                   // Software reset enabled
    UCB0CTLW0 |= UCSSEL__SMCLK;
    UCB0BRW = 10;
    UCB0CTLW0 |= UCMODE_3 | UCMST | UCTR; // I2C mode, Master mode, TX
    UCB0CTLW1 |= UCASTP_2;                  // Automatic stop generated
                                            // after UCB0TBCNT is reached
    UCB0TBCNT = 0x0001;                     // number of bytes to be received
    UCB0I2CSA = 0x00A;                     // Slave address

    // Setup Timer B0
    TB0CTL |= TBCLR;  // Clear timer and dividers
    TB0CTL |= TBSSEL__ACLK;  // Use ACLK
    TB0CTL |= MC__UP;  // Up counting mode
    TB0CCR0 = 32768;    // Compare value

    // Set up timer compare IRQs
    TB0CCTL0 &= ~CCIFG;  // Clear CCR0 flag
    TB0CCTL0 |= CCIE;  // Enable flag
    

    // Disable low-power mode / GPIO high-impedance
    PM5CTL0 &= ~LOCKLPM5;
    UCB0CTLW0 &= ~UCSWRST;

    
    
    UCB0IE |= UCTXIE0;  
    __enable_interrupt();
    UCB0CTLW0 |= UCTXSTT;
    
    
    while (1)
    {
        if(lock_status == 1)
        {
            lock_keypad(unlock_code);
            lock_status = 0;
        }
    }
}


#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){
    UCB0TXBUF = data;
}

#pragma vector = PORT3_VECTOR
__interrupt void ISR_PORT3_S2(void) {
    char input = scanPad();
        switch(input){
            case 'D':   data = 0x16;
                        UCB0CTLW0 |= UCTXSTT;
                        lock_status = 1;
                        break;
            case '1':   data = 0x1;
                        UCB0CTLW0 |= UCTXSTT;
                        break;
            case '2':   data = 0x2;
                        UCB0CTLW0 |= UCTXSTT;
                        break;
            case '3':   data = 0x3;
                        UCB0CTLW0 |= UCTXSTT;
                        break;
            case '5':   data = 0x6;
                        UCB0CTLW0 |= UCTXSTT;
                        break;
            case 'A':   data = 0x4;
                        UCB0CTLW0 |= UCTXSTT;
                        break;
            case 'B':   data = 0x8;
                        UCB0CTLW0 |= UCTXSTT;
                        break;
            case '9':   data = 0x11;
                        UCB0CTLW0 |= UCTXSTT;
                        break;
            case 'C':   data = 0x12;
                        UCB0CTLW0 |= UCTXSTT;
                        break;
            case '0':   data = 0x14;
                        UCB0CTLW0 |= UCTXSTT;
                        break;
                       
        }

    P3IFG &= ~BIT0;  // Clear the interrupt flag
    P3IFG &= ~BIT1;  // Clear the interrupt flag
    P3IFG &= ~BIT2;  // Clear the interrupt flag
    P3IFG &= ~BIT3;  // Clear the interrupt flag
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void) {
    P1OUT ^= BIT0;
    TB0CCTL1 &= ~CCIFG;
}