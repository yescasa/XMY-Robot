/* 
 * File:   lab3pa.c
 * Author: A_A
 *
 * Created on October 26, 2014, 6:54 PM
 */

#include "p24fj64ga002.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>


_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & 
	BKBUG_ON & COE_OFF & ICS_PGx1 & 
	FWDTEN_OFF & WINDIS_OFF & FWPSA_PR128 & WDTPS_PS32768 )


_CONFIG2( IESO_OFF & SOSCSEL_SOSC & WUTSEL_LEG & FNOSC_PRIPLL & FCKSM_CSDCMD & OSCIOFNC_OFF &
	IOL1WAY_OFF & I2C1SEL_PRI & POSCMOD_XT )


        volatile int state = 0;
volatile double adcValue = 0.0;
volatile int converted = 0;

/*
 * 
 */
int main() {

    char adcValueStr[8];
    int period = 14745600/10000-1;
    double ocValue1 = 0;
    double ocValue2 = 0;
    
    LCDInitialize();
    
    
    TRISBbits.TRISB2 = 1; //rb2/an4 for potentiometer
    AD1PCFG &= 0xFFEF;
    AD1CON1 = 0x20E4;
    AD1CON2 = 0;
    AD1CON3 = 0x0101;
    AD1CHS = 0x0004;
    AD1CSSL = 0;

    TRISBbits.TRISB1 = 0;
    OC1CON=0x0006; //pwm1, left
    OC1R=0;
    OC1RS=period;
   // RPOR0bits.RP0R = 18;

    TRISBbits.TRISB0 = 0;
    OC2CON=0x0006; //pwm2, right
    OC2R=0;
    OC2RS=period;
    //RPOR0bits.RP1R = 19;
    
    IEC0bits.AD1IE = 1;//interrupt for adc
    IFS0bits.AD1IF = 0;
    
    
    AD1CON1bits.ADON = 1;
    //pwm period timer
    T2CON =0x8000;
    TMR2 = 0;
    PR2 = 14745600/10000-1;


    TRISBbits.TRISB5 = 1;//switch
    CNEN2bits.CN27IE = 1;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;


    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB1 = 0;
    TRISBbits.TRISB8 = 0;
    TRISBbits.TRISB9 = 0;
    TRISAbits.TRISA0 = 0;
    //AD1PCFGbits.PCFG0 =1;
    //AD1PCFGbits.PCFG1 =1;
    LATBbits.LATB8 = 0;
    LATBbits.LATB9 = 0;

    while(1) {

        if(converted==1) {
            adcValue = (ADC1BUF0*3.3)/1024;
            

            if((adcValue/3.3)*100 < 49){
		ocValue1 = 1.0;
		ocValue2 = (adcValue / 1.65);
            } else if((adcValue/3.3)*100 > 51){
		ocValue1 = ((3.3-adcValue)/ 1.65);
		ocValue2 = 1.0;
            } else {														//Move straight
		ocValue1 = 1.0;
		ocValue2 = 1.0;
            }

//            OC1R=(int)(ocValue1*period);
//            OC2R=(int)(ocValue2*period);
            OC1RS=(int)(ocValue1*period)/2;
            OC2RS=(int)(ocValue2*period)/2;
          
            LCDMoveCursor(0,0);
            sprintf(adcValueStr,"%6.2f",adcValue);
            LCDPrintString(adcValueStr);

           
            //sprintf(adcValueStr,"%4d",(int)(ocValue1*period));
            //LCDPrintString(adcValueStr);
            //LCDPrintString(" ");
           // LCDMoveCursor(1,5);
           // sprintf(adcValueStr,"%4d",(int)(ocValue2*period));
            //LCDPrintString(adcValueStr);

            converted = 0;
        }


        
        switch(state) {
            case 0:
                //how to control motors?
                PORTAbits.RA0=0;
                break;
            case 1:
                PORTAbits.RA0=1;
                LATBbits.LATB8 = 0;
                LATBbits.LATB9 = 0;
                RPOR0bits.RP0R = 18;
                RPOR0bits.RP1R = 19;
                break;
            case 2:
                PORTAbits.RA0=0;
                break;
            case 3:
                PORTAbits.RA0=1;
                RPOR4bits.RP8R = 18;
                RPOR4bits.RP9R = 19;
                PORTBbits.RB0 = 0;
                PORTBbits.RB1 = 0;
                
                break;
        }
 LCDMoveCursor(1,0);
       sprintf(adcValueStr,"%1d",state);
            LCDPrintString(adcValueStr);
    }
    
    
    return (EXIT_SUCCESS);
}


void __attribute__ ((__interrupt__)) _ADC1Interrupt(void)
{
		IFS0bits.AD1IF = 0;
		converted = 1;

}


void __attribute__((interrupt)) _CNInterrupt(void){

    IFS1bits.CNIF = 0;
    if(PORTBbits.RB5 == 0){

        if(state==3) state=0;
        else state=state+1;
    }

}

