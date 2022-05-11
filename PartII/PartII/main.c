#include "sam.h"
#include "RTCControl.h"
#include "myprintf.h"

#define BUFFER_SIZE 35

char buff[BUFFER_SIZE];

void initRTCTime();
void UARTInit(void);
void displayRTC();

int main(void) {
	buff[BUFFER_SIZE-1] = '\0';

	UARTInit();
	RTCInit();

	myprintf("> Initializing RTC...");
	initRTCTime();
	myprintf("> RTC initialized.");
	while (1) {
		displayRTC();
	}

	return 0;
	
}

void initRTCTime() {
	rtc_type rtc;

	rtc.sec.reg  = 0x00;
	rtc.min.reg  = 0x50;
	rtc.hrs.reg  = 0x04;
	rtc.day.reg  = 0x02;
	rtc.date.reg = 0x10;
	rtc.mon.reg  = 0x05;
	rtc.year.reg = 0x22;
	rtc.hrs.field_24hrs.h12 = 0; // 12-hour format

	sendI2CDataArray(RTC_SLAVE_ADDR, SEC_ADDR, (uint8_t*)&rtc, 7);
}

void displayRTC() {
	rtc_type rtc;

	receiveI2CDataArray(RTC_SLAVE_ADDR, SEC_ADDR, (uint8_t*)&rtc, 7);
	mysnprintf(buff, sizeof buff, "%02x-%02x-%02x %02x:%02x:%02x",
		rtc.year.reg,
		rtc.mon.reg,
		rtc.day.reg,
		(rtc.hrs.field_24hrs.dec_hours << 4) | rtc.hrs.field_24hrs.uni_hours,
		rtc.min.reg,
		rtc.sec.reg
	);
	myprintf("%s\n", buff);
}


void UARTInit(void) {
	/* Initialize the SAM system */
    SystemInit();

	/* Switch to 8MHz clock (disable prescaler) */
	SYSCTRL->OSC8M.bit.PRESC = 0;
	
	/* port mux configuration*/
	PORT->Group[0].DIR.reg |= (1 << 10);                  /* Pin 10 configured as output */
	PORT->Group[0].PINCFG[PIN_PA11].bit.PMUXEN = 1;       /* Enabling peripheral functions */
	PORT->Group[0].PINCFG[PIN_PA10].bit.PMUXEN = 1;       /* Enabling peripheral functions */
	
	/*PMUX: even = n/2, odd: (n-1)/2 */
	PORT->Group[0].PMUX[5].reg |= 0x02;                   /* Selecting peripheral function C */
	PORT->Group[0].PMUX[5].reg |= 0x20;                   /* Selecting peripheral function C */
	
	/* APBCMASK */
	//PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;			  /* SERCOM 0 enable*/
	PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;

	/*GCLK configuration for sercom0 module: using generic clock generator 0, ID for sercom0, enable GCLK*/

	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM0_GCLK_ID_CORE) |
						GCLK_CLKCTRL_GEN(0) |
						GCLK_CLKCTRL_CLKEN;

	
	/* configure SERCOM0 module for UART as Standard Frame, 8 Bit size, No parity, BAUDRATE:9600*/

	SERCOM0->USART.CTRLA.reg =
	SERCOM_USART_CTRLA_DORD | SERCOM_USART_CTRLA_MODE_USART_INT_CLK |
	SERCOM_USART_CTRLA_RXPO(3/*PAD3*/) | SERCOM_USART_CTRLA_TXPO(1/*PAD2*/);
	
	uint64_t br = (uint64_t)65536 * (8000000 - 16 * 9600) / 8000000;
	
	SERCOM0->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_CHSIZE(0/*8 bits*/);

	SERCOM0->USART.BAUD.reg = (uint16_t)br;

	SERCOM0->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
}