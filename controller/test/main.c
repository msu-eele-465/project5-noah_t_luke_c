#include "intrinsics.h"
#include <msp430.h>
#include "keypad.h"

#define unlock_code "1738"

unsigned char data = 0x00;
int lock_status = 1;
int window = 0;
char key_pressed;

int letters_set_pattern[] = {0b01010011, 0b01100101, 0b01110100, 0b01000000, 0b01010000, 0b01100001, 0b01110100, 0b01110100, 0b01100101, 0b01110000, 0b01101110};
int letters_set_window[] = {0b01010011, 0b01100101, 0b01110100, 0b01000000, 0b01010111, 0b01101001, 0b01101110, 0b01100100, 0b01101111, 0b01110111, 0b01000000, 0b01010011, 0b01101001, 0b01111010, 0b01100101};
int letters_pattern_static[] = {0b01010011, 0b01110100, 0b01100001, 0b01110100, 0b01101001, 0b01100011};
int letters_pattern_toggle[] = {0b01010100, 0b01111010, 0b01100111, 0b01100111, 0b01101100, 0b01100101};
int letters_pattern_up_counter[] = {0b01010101, 0b01110000, 0b01000000, 0b01100011, 0b01110101, 0b01101110, 0b01110100, 0b01100101, 0b01110000};
int letters_pattern_in_and_out[] = {0b01001001, 0b01101110, 0b01000000, 0b01100001, 0b01101110, 0b01100110, 0b01000000, 0b01100011, 0b01110100};


void i2c_config(){
    // Configure USCI_B0 for I2C mode
    UCB0CTLW0 |= UCSWRST;                   // Software reset enabled
    UCB0CTLW0 |= UCSSEL__SMCLK;
    UCB0BRW = 10;
    UCB0CTLW0 |= UCMODE_3 | UCMST | UCTR;   // I2C mode, Master mode, TX
    UCB0CTLW1 |= UCASTP_2;                  // Automatic stop generated
                                            // after UCB0TBCNT is reached

    UCB0TBCNT = 0x0001;                     // number of bytes to be sent
    UCB0I2CSA = 0x00;                       // Slave address
                                            // Two slaves are being used, 0x0A is the LEDbar, 0x0B is the LCD
                                            // When one of those keys is pressed, update slave address, send data, set back to 0x00  
    // I2C pins, 1.2 SDA, 1.3 SCL
    P1SEL1 &= ~(BIT2 & BIT3);
    P1SEL0 |= BIT2 | BIT3;     
}



void write_set_pattern(){
    UCB0I2CSA = 0x0B;                      // LCD slave address
    data = 0x00;                           // Send 0 so LCD writes to top line
    UCB0CTLW0 |= UCTXSTT;                  // Transmit
    __delay_cycles(200);
    int i = 0;
    for (i = 0; i < sizeof(letters_set_pattern); i++) {
        data = letters_set_pattern[i];                 // Set data to be transmited to next letter code
        UCB0CTLW0 |= UCTXSTT;              // Transmit
        __delay_cycles(200);
    }
    __delay_cycles(200);
    UCB0I2CSA = 0x00;                      // Reset slave address
}

void write_set_window(){
    UCB0I2CSA = 0x0B;                      // LCD slave address
    data = 0x00;                           // Send 0 so LCD writes to top line
    UCB0CTLW0 |= UCTXSTT;                  // Transmit
    int i = 0;
    for (i = 0; i < sizeof(letters_set_window); i++) {
        data = letters_set_window[i];                 // Set data to be transmited to next letter code
        UCB0CTLW0 |= UCTXSTT;              // Transmit
    }
    UCB0I2CSA = 0x00;                      // Reset slave address
}

void write_pattern_static(){
    UCB0I2CSA = 0x0B;                      // LCD slave address
    data = 0x00;                           // Send 0 so LCD writes to top line
    UCB0CTLW0 |= UCTXSTT;                  // Transmit
    int i = 0;
    for (i = 0; i < sizeof(letters_pattern_static); i++) {
        data = letters_pattern_static[i];  // Set data to be transmited to next letter code
        UCB0CTLW0 |= UCTXSTT;              // Transmit
    }
    UCB0I2CSA = 0x00;                      // Reset slave address
}

void write_pattern_toggle(){
    UCB0I2CSA = 0x0B;                      // LCD slave address
    data = 0x00;                           // Send 0 so LCD writes to top line
    UCB0CTLW0 |= UCTXSTT;                  // Transmit
    int i = 0;
    for (i = 0; i < sizeof(letters_pattern_toggle); i++) {
        data = letters_pattern_toggle[i];  // Set data to be transmited to next letter code
        UCB0CTLW0 |= UCTXSTT;              // Transmit
    }
    UCB0I2CSA = 0x00;                      // Reset slave address
}

void write_pattern_up_counter(){
    UCB0I2CSA = 0x0B;                      // LCD slave address
    data = 0x00;                           // Send 0 so LCD writes to top line
    UCB0CTLW0 |= UCTXSTT;                  // Transmit
    int i = 0;
    for (i = 0; i < sizeof(letters_pattern_up_counter); i++) {
        data = letters_pattern_up_counter[i];  // Set data to be transmited to next letter code
        UCB0CTLW0 |= UCTXSTT;                  // Transmit
    }
    UCB0I2CSA = 0x00;                          // Reset slave address
}

void write_pattern_in_and_out(){
    UCB0I2CSA = 0x0B;                      // LCD slave address
    data = 0x00;                           // Send 0 so LCD writes to top line
    UCB0CTLW0 |= UCTXSTT;                  // Transmit
    int i = 0;
    for (i = 0; i < sizeof(letters_pattern_in_and_out); i++) {
        data = letters_pattern_in_and_out[i];  // Set data to be transmited to next letter code
        UCB0CTLW0 |= UCTXSTT;                  // Transmit
    }
    UCB0I2CSA = 0x00;                          // Reset slave address
}

void transmit_pattern_led(){
    UCB0I2CSA = 0x0A;                       // LEDbar slave address
    switch (key_pressed) {
        case '0': 
            data = key_pressed - '0';       // Convert char to integer
            UCB0CTLW0 |= UCTXSTT;     
            write_pattern_static();                   
            break;
        case '1': 
            data = key_pressed - '0';       // Convert char to integer
            UCB0CTLW0 |= UCTXSTT;  
            write_pattern_toggle();                         
            break;
        case '2': 
            data = key_pressed - '0';       // Convert char to integer
            UCB0CTLW0 |= UCTXSTT;
            write_pattern_up_counter();                           
            break;
        case '3': 
            data = key_pressed - '0';       // Convert char to integer
            UCB0CTLW0 |= UCTXSTT;
            write_pattern_in_and_out();                           
            break;
    }
        UCB0I2CSA = 0x00;                   // Reset slave address
}

void lcd_window_size_transmit(){
    UCB0I2CSA = 0x0B;                       // LCD slave address
    switch (key_pressed) {
        case '0': 
            data = 0;       // Convert char to integer
            UCB0CTLW0 |= UCTXSTT;                        
            break;
        case '1': 
            data = 1;       // Convert char to integer
            UCB0CTLW0 |= UCTXSTT;                           
            break;
        case '2': 
            data = 2;       
            UCB0CTLW0 |= UCTXSTT;                           
            break;
        case '3': 
            data = 3;       // Convert char to integer
            UCB0CTLW0 |= UCTXSTT;                           
            break;
        case '4': 
            data = 4;       // Convert char to integer
            UCB0CTLW0 |= UCTXSTT;                        
            break;
        case '5': 
            data = 5;       // Convert char to integer
            UCB0CTLW0 |= UCTXSTT;                           
            break;
        case '6': 
            data = 6;       // Convert char to integer
            UCB0CTLW0 |= UCTXSTT;                           
            break;
        case '7': 
            data = 7;       // Convert char to integer
            UCB0CTLW0 |= UCTXSTT;                           
            break;
        case '8': 
            data = 8;       // Convert char to integer
            UCB0CTLW0 |= UCTXSTT;                           
            break;
        case '9': 
            data = 9;       // Convert char to integer
            UCB0CTLW0 |= UCTXSTT;                           
            break;
    }
    UCB0I2CSA = 0x00;                       // Reset slave address
}

int main(void)
{
    // Stop WDT
    WDTCTL = WDTPW | WDTHOLD;

    // Set up I2C
    i2c_config();

    // Set up I2C
    keypad_init();

    // Disable low power mode
    PM5CTL0 &= ~LOCKLPM5;
    UCB0CTLW0 &= ~UCSWRST;

    // I2C interrupt
    UCB0IE |= UCTXIE0;  

    // Enable interrupts
    __enable_interrupt();
    
    
    while (1)
    {       
        if(lock_status == 1)
        {
            lock_keypad(unlock_code);
            lock_status = 0;
        }
        while(key_pressed != 'A' && key_pressed != 'B') 
        {             
            // Just sit here until A or B is pressed
        }
        P2OUT ^= BIT7;
        if(key_pressed == 'A'){                    // A pressed
            
            write_set_pattern();

            while(key_pressed != '0' && key_pressed != '1' && key_pressed != '2' && key_pressed != '3')
            {
                // Wait here until valid key is pressed
            }
            P2OUT ^= BIT7;
            transmit_pattern_led();
        }
        if(key_pressed == 'B'){

            write_set_window();
            
            while(key_pressed != '0' && key_pressed != '1' && key_pressed != '2' && key_pressed != '3' && key_pressed != '4' && key_pressed != '5' && key_pressed != '6' && key_pressed != '7' && key_pressed != '8' && key_pressed != '9')
            {
                // Wait here until valid key is pressed     
            }

            lcd_window_size_transmit();
            
        }
    }
}


#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){
    UCB0TXBUF = data;
    P2OUT ^= BIT7;
}

#pragma vector = PORT3_VECTOR
__interrupt void ISR_PORT3_S2(void) {
    key_pressed = scanPad();
    P3IFG &= ~(BIT0 | BIT1 | BIT2 | BIT3);  // Clear the interrupt flag
}