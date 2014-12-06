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
volatile int switchon = 0;
volatile int left, bar, right;
volatile int turned = 0;
volatile int turnstop=0;
volatile char barcode[5];

/*
 * 
 */

void DelayTurn() {

	// TODO: Use Timer 2 to delay for precisely (as precise as possible) usDelay
	// microseconds provided by the input variable.
	//
	// Hint: Determine the configuration for the PR1 setting that provides for a
	// one microsecond delay, and multiply this by the input variable.
	// Be sure to user integer values only.
/**********************************************/

        PR4=0xFFFF;
        PR5=0x1C1;
	TMR4 = 0;
        TMR5 = 0;
        IFS1bits.T5IF=0;
        IEC1bits.T5IE=0;
        T4CONbits.T32=1;
        T4CONbits.TON=1;
        T4CONbits.TCKPS0=1;
        T4CONbits.TCKPS1=1;


        while(IFS1bits.T5IF==0){}
        T4CONbits.TON = 0;
        turnstop=3;


/*****************************************************/
}
int main() {

    char adcValueStr[8];
    int period = 14745600/10000-1;
    double ocValue1 = 0;
    double ocValue2 = 0;
    int i=0, j=0;
    LCDInitialize();
    bar=0;
    barcode[0]=' ';
    barcode[1]=' ';
    barcode[2]=' ';barcode[3]=' ';


    TRISBbits.TRISB2 = 1; //left
    TRISAbits.TRISA1 = 1;//barcode
    TRISBbits.TRISB3 = 1;//right
    //1111 1111 1100 1101
    AD1PCFG &= 0xFFCD;
    //ad1con2 0000 0100 0000 1000
    AD1CON2=0x0408;
    AD1CON1 = 0x20E4;
    //0000 0100 0000 0100;
    AD1CON3 = 0x0101;
  //  0000 0000 0011 0010
    AD1CSSL = 0x0032;

    TRISBbits.TRISB10 = 0;
    OC1CON=0x0006; //pwm1, left
    OC1R=0;
    OC1RS=0;
    RPOR5bits.RP11R = 18;

    TRISBbits.TRISB11 = 0;
    OC2CON=0x0006; //pwm2, right
    OC2R=0;
    OC2RS=0;
    RPOR5bits.RP10R = 19;
    
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


   // TRISBbits.TRISB0 = 0;
   // TRISBbits.TRISB10 = 0;
    TRISBbits.TRISB8 = 0;
    TRISBbits.TRISB9 = 0;
    LATBbits.LATB8 = 0;
    LATBbits.LATB9 = 0;
    TRISAbits.TRISA0 = 0;
    LATAbits.LATA0=0;
    //AD1PCFGbits.PCFG0 =1;
    //AD1PCFGbits.PCFG1 =1;
   
    turnstop=0;
    while(1) {
        
        if(converted==1 && switchon==1) {
            
            LCDMoveCursor(0,0);
            sprintf(adcValueStr," %4d",ADC1BUF0);
            LCDPrintString(adcValueStr);
           /* LCDMoveCursor(1,0);
            sprintf(adcValueStr," %4d",ADC1BUF1);
            LCDPrintString(adcValueStr);
            //LCDPrintString(" ");
            sprintf(adcValueStr, " %4d", ADC1BUF2);
            LCDPrintString(adcValueStr);*/
        
                bar=ADC1BUF0;
              
                right=ADC1BUF1;

                left=ADC1BUF2;

                if(left<100 && right >200) {//top white
                    PORTAbits.RA0=1;
                    
                   
                        ocValue1=0.70;
                    ocValue2=0.15;
                    
                } else if(left>200 && right <100){//top black
                    PORTAbits.RA0=1;
                    
                    
                        ocValue1=0.15;
                    ocValue2=0.70;
                    
                    
                } else if(right>200 && left >200) {//both black
                    PORTAbits.RA0=1;
                  

                       if(turnstop==0) {

                            turnstop=1;
                            ocValue1=0;
                            ocValue2=0.77;
                        
                       } else if(turnstop==2){
                           ocValue1=0.64;
                           ocValue2=0.0;
                           turnstop=3;
                            
                       } else if(turnstop==425) {
                           ocValue1=0.77;
                           ocValue2=0;
                           turnstop=650;
                       }


                
                    
                } else if(right<100 && left<100) {//both white
                    PORTAbits.RA0=1;
                    
                    if(turnstop==1) {
                        
                        ocValue1=0.65;
                        ocValue2=0.67;
                        turnstop=2;
                    } else if(turnstop==3) {
                        ocValue1=0.64;
                        ocValue2=0;
                        turnstop++;
                    }else if(turnstop>3 && turnstop <425) {
                        ocValue1=0.64;
                        ocValue2=0;
                        turnstop++;
                    } else {

                      
                        ocValue1=0.65;
                        ocValue2=0.67;
                       
                    }
                   
                }
               converted = 0;
           
            OC1RS=(int)(ocValue1*period)/2;
            OC2RS=(int)(ocValue2*period)/2;
           // barcode[0]='0'; barcode[0]='0'; barcode[0]='0'; barcode[0]='1';
             //           LCDMoveCursor(1,0);
               //         LCDPrintString(barcode);

/*
            if(bar>400) {
                i=1;
            }
            if(i==1) {
                if(bar>100) {
                    if(bar>400) {barcode[j]='0';j++;}
                    if(bar<400&&bar>300) {barcode[j]='1';j++;}
                }
            }
            if(j==4) {i=0;j=0;
            LCDMoveCursor(1,0);
            barcode[4]='\0';
                    LCDPrintString(barcode);}*/


            /*Barcode Scanning*/
            i=0;
            switch(state) {
                case 0:
                    if(bar>400) state=1;
                    LCDMoveCursor(1,0);
                   // LCDPrintString("s0");
                    break;
                case 1:
                   // LCDMoveCursor(1,0);
                   // LCDPrintString("s1");
                    if(i==4) {
                        state=0;
                        
                    } else
                    if(bar<100) state=2;
                    break;
                case 2:
                    //LCDMoveCursor(1,0);
                    //LCDPrintString("s2");
                    if(bar>400) {
                        LCDMoveCursor(1,i);
                        LCDPrintChar('2');
                        i++;
                        state=1;
                        
                        
                    }
                    if(bar<400&&bar>300) {
                        LCDMoveCursor(1,i+3);
                    LCDPrintChar('1');i++;state=1;}

                    
                    
                    break;
                
            }
        }


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
        
        if(switchon==0) switchon=1;
        else {switchon=0;
        OC1RS=0;
        OC2RS=0;
        }
        //if(state==3) state=0;
       // else state=state+1;
    }

}

void __attribute__((interrupt,auto_psv)) _T4Interrupt(void) {
    IFS1bits.T5IF=0;
    T4CONbits.TON = 0;
    //turned =1;
}

