#include "sam.h"
  
int main(void) { 
  
    /* Initialize the SAM system */ 
    SystemInit(); 
  
    //PORT configuration for general-purpose PIN 
    PORT->Group[0].PINCFG[17].reg = 0x0; 
    PORT->Group[0].DIRSET.reg = PORT_PA17;

    //TIMER configuration 
    //Configure the POWER MANAGER to enable the TC3 module 
    PM->APBCMASK.reg |= PM_APBCMASK_TC3; 

    // Configure the GENERIC CLOCK CONTROL used by the TC3 module
    GCLK->GENDIV.reg = 0;
    GCLK->GENCTRL.reg = 0x10600;
    GCLK->CLKCTRL.reg = 0x401B; // 0100

    //Determine the initial count
    //Time = Toscillator * (2^TIMERMODE - COUNT) * Prescaler 
    TC3->COUNT16.COUNT.reg = 65307;

    //Configure the TC3 peripheral
    TC3->COUNT16.CTRLA.reg = 0; 
    TC3->COUNT16.CTRLBSET.reg = 0;
    TC3->COUNT16.INTFLAG.reg = 0x1;

    //Enabling the TC3 peripheral
    TC3->COUNT16.CTRLA.reg |= 0x2;

    //Loop to sync the TC clock with the processor/main clock 
    while (TC3->COUNT16.STATUS.bit.SYNCBUSY) { } // Test SYNCBUSY
    
    //*************************************************************************** 
    //Loop to generate the square-wave signal 
    //EXTRA cycles to ADD to the initial count---it will be still a  
    //polling-error of +/- 7 us in the period of the signal 
    //LDRB(4) + ANDS(1) + BEQ(2) + LDRB(4) + ANDS(1) + BEQ(1) + STR(4) +  
    //STRB(4) + STRH(4) + B(2) = 27 cycles 

while (1) {
    while (!TC3->COUNT16.INTFLAG.bit.OVF) { }                // Test for OVF Flag
    PORT->Group[0].OUTTGL.bit.OUTTGL = ((uint32_t) 1 << 17); // Toggle the output pin 17 (BUILTIN_LED)
    TC3->COUNT16.INTFLAG.bit.OVF = 1;                        // Clear OVF Flag
    TC3->COUNT16.COUNT.reg = 65307;                          // Reset counter
}

    return 0;

}