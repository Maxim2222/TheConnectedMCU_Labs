// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************
#include <xc.h>          /* Defines special function registers, CP0 regs  */
#include <stdint.h>          /* For uint32_t definition                       */
#include <sys/attribs.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "user.h"            /* variables/params used by user.c               */
#include "OLED.h"
#include "OledChar.h"
#include "OledGrph.h"
#include "UART.h"
#include <string.h>
#include <stdlib.h>

#define X_MAX 128
#define Y_MAX 28
#define X_center 64
#define Y_center 14


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************


/******************************************************************************/
/* User Functions                                                             */

/******************************************************************************/

volatile int x,y,x_factor,y_factor,y_kill;
volatile    int n, i=0;

void InitGPIO(void) {
    // LED output
    // Disable analog mode for G6
    ANSELGbits.ANSG6 = 0;
    // Set direction to output for G6
    TRISGbits.TRISG6 = 0;

    // Initialize outputs for other LEDs
    ANSELBbits.ANSB11 = 0;
    ANSELGbits.ANSG15 = 0;

    TRISBbits.TRISB11 = 0;
    TRISGbits.TRISG15 = 0;
    TRISDbits.TRISD4 = 0;

    // Turn off LEDs for initialization
    LD1_PORT_BIT = 0;
    LD2_PORT_BIT = 0;
    LD3_PORT_BIT = 0;
    LD4_PORT_BIT = 0;

    // Initilalize input for BTN1
    // Disable analog mode
    ANSELAbits.ANSA5 = 0;
    // Set direction to input
    TRISAbits.TRISA5 = 1;

    // Initialize input for BTN2
    TRISAbits.TRISA4 = 1;
}

void InitBIOSGPIO(void) {
    /* Setup functionality and port direction */
    // LED outputs
    // Disable analog mode
    // Set directions to output
    TRISE = 0;
   
    // Test LEDs
    LATE = 0xff;        
    
    // Turn off LEDs for initialization
    LATE = 0;

    // Button inputs
    ANSELGbits.ANSG7 = 0;
    
    
    
    ANSELEbits.ANSE8 = 0;
    ANSELEbits.ANSE8 = 0;
 
    ANSELCbits.ANSC1 = 0;
    
    // Set directions to input
    TRISGbits.TRISG7 = 1;
    TRISDbits.TRISD5 = 1;
    TRISFbits.TRISF1 = 1;
    TRISAbits.TRISA2 = 1; 
    
    TRISEbits.TRISE8 = 1;
    TRISEbits.TRISE9 = 1;
    TRISAbits.TRISA14 = 1;
    TRISCbits.TRISC1 = 1; 
    
    /*
    #define BIOS_SW1_PORT_BIT       PORTEbits.RE8
#define BIOS_SW2_PORT_BIT       PORTEbits.RE9
#define BIOS_SW3_PORT_BIT       PORTAbits.RA14
#define BIOS_SW4_PORT_BIT       PORTCbits.RC1*/
}

void InitApp(void) {
    // Initialize peripherals
    InitGPIO();
    InitBIOSGPIO();
    
    OledHostInit();
    OledDspInit();
    OledDvrInit();
    
    // TODO: Add UART4 Initialization call here
    UART4_init();
}

void Task1(void * pvParameters) {
    uint8_t displayed_str[16];
    memset(displayed_str, ' ', sizeof(displayed_str));
    displayed_str[0] = '*';
    displayed_str[15] = '\0';
    uint32_t pos = 0;
    while (1) {
        
        if (!BIOS_SW3_PORT_BIT) {
            xSemaphoreTake(xMutexOLED, portMAX_DELAY);
            OledSetCursor(0,1);
            OledPutString(displayed_str);
            if (pos < 15) {
                displayed_str[pos] = '*';
            } else {
                memset(displayed_str, ' ', sizeof(displayed_str));
                displayed_str[15] = '\0';
            }
            pos = pos==15? 0 : pos+1;
            DelayMs(200);
          xSemaphoreGive(xMutexOLED);
        }
        
        vTaskDelay(1);   
        }
          
    }

void Task2(void * pvParameters) {
   uint8_t displayed_str[16];
    memset(displayed_str, ' ', sizeof(displayed_str));
    displayed_str[14] = '*';
    displayed_str[15] = '\0';
    uint32_t pos = 14;
    
    while (1) {
        if (BIOS_SW3_PORT_BIT) {
            xSemaphoreTake(xMutexOLED, portMAX_DELAY);
            OledSetCursor(0,1);
            OledPutString(displayed_str);
            if (pos > 0) {
                displayed_str[pos] = '*';
            } else {
                memset(displayed_str, ' ', sizeof(displayed_str));
                displayed_str[15] = '\0';
            }
            pos = pos > 0? pos -1 : 14;
            DelayMs(200);
            xSemaphoreGive(xMutexOLED);
        }
        vTaskDelay(1); 
        }
           
    }

// TODO: Define function ClockTask here

// TODO: Define SerialInTask here

void DelayMs(int t) {
    volatile long int count = t*33356;
    
    while (count--)
        ;
}