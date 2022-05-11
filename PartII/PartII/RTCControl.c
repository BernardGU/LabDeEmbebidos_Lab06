#include "sam.h"
#include "stdbool.h"
#include "RTCControl.h"

void RTCInit() {
	/* Initialize the SAM system */
	SystemInit();
	
	/* Switch to 8MHz clock (disable prescaler) */
	SYSCTRL->OSC8M.bit.PRESC = 0;
	
	/* port mux configuration */
	PORT->Group[0].PINCFG[PIN_PA22].reg = PORT_PINCFG_PMUXEN | PORT_PINCFG_INEN | PORT_PINCFG_PULLEN; /* SDA */
	PORT->Group[0].PINCFG[PIN_PA23].reg = PORT_PINCFG_PMUXEN | PORT_PINCFG_INEN | PORT_PINCFG_PULLEN; /* SCL */
	
	/* PMUX: even = n/2, odd: (n-1)/2 */
	PORT->Group[0].PMUX[11].reg |= 0x02u;
	PORT->Group[0].PMUX[11].reg |= 0x20u;
	
	/* APBCMASK */
	PM->APBCMASK.reg |= PM_APBCMASK_SERCOM3;

	/*GCLK configuration for sercom3 module*/
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID (SERCOM3_GCLK_ID_CORE) |
	GCLK_CLKCTRL_ID (SERCOM3_GCLK_ID_SLOW) |
	GCLK_CLKCTRL_GEN(4) |
	GCLK_CLKCTRL_CLKEN;
	GCLK->GENCTRL.reg |= GCLK_GENCTRL_SRC_OSC8M|GCLK_GENCTRL_GENEN|GCLK_GENCTRL_ID(4);

	/* set configuration for SERCOM3 I2C module */
	SERCOM3->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN; /* smart mode enable */
	while (SERCOM3->I2CM.SYNCBUSY.reg); // waiting loading
	
	/* calculate BAUDRATE */
	uint64_t tmp_baud =((8000000/100000)-10-(8000000*250 /1000000000))/2;
	SERCOM3->I2CM.BAUD.bit.BAUD = SERCOM_I2CM_BAUD_BAUD((uint32_t)tmp_baud);
	while (SERCOM3->I2CM.SYNCBUSY.reg); // waiting loading
	// value equals 0x22 or decimal 34
	
	SERCOM3->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_ENABLE   |/* enable module */
	SERCOM_I2CM_CTRLA_MODE_I2C_MASTER |		/* i2c master mode */
	SERCOM_I2CM_CTRLA_SDAHOLD(3);		 /* SDA hold time to 600ns */
	while (SERCOM3->I2CM.SYNCBUSY.reg);  /* waiting loading */

	SERCOM3->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_BUSSTATE(1); /* set to idle state */
	while (SERCOM3->I2CM.SYNCBUSY.reg);  /* waiting loading */
}

void sendI2CDataArray(uint8_t SlAddr, uint8_t ptr, uint8_t *ptrData, uint32_t Size){
	/*Begin Tx by sending slave address with write signal (0)*/
	SERCOM3->I2CM.ADDR.reg = (SlAddr << 1) | 0; /* Sending slave address in write mode */
	while(!SERCOM3->I2CM.INTFLAG.bit.MB);		/* MB = 1 if slave NACKS the address */
	// Send the address of the RTC register to begin the write operation
	SendI2CData(ptr);
	// Send the data byte by byte
	for(int i=0; i<Size; i++) SendI2CData(ptrData[i]);

	/* After transferring the last byte stop condition will be sent */
	StopCond();
}

void SendI2CData(uint8_t Data){
	/* Place the data in the DATA register to send it*/
	SERCOM3->I2CM.DATA.reg = Data;
	while(!SERCOM3->I2CM.INTFLAG.bit.MB){}; /* MB = 1 if slave NACKS the address */
}

void StopCond() {
	SERCOM3->I2CM.CTRLB.bit.CMD = 0x3;
}

void receiveI2CDataArray(uint8_t SlAddr, uint8_t ptr, uint8_t *ptrData, uint32_t Size) {
	/*Condigure internal pointer for read operation*/
	sendI2CDataArray(SlAddr, ptr, ptrData, 0);
	/* slave address with read signal (1) */
	SERCOM3->I2CM.ADDR.reg = (SlAddr << 1) | 1;
	while(SERCOM3->I2CM.INTFLAG.bit.SB==0){};
	/*Read bits from slave device*/
	for(int i=0; i<Size-1; i++) {
		ReadI2CData(ptrData + i);
	}
	/* After receiving the last byte stop condition will be sent */
	StopCond();
}

void ReadI2CData(uint8_t *Data) {
	*Data = SERCOM3->I2CM.DATA.reg;
	while(SERCOM3->I2CM.INTFLAG.bit.SB==0){};
}

void TurnOffI2C() {
	// Turn off the I2C
	SERCOM3->I2CM.CTRLA.bit.ENABLE = 0x0u;
}


