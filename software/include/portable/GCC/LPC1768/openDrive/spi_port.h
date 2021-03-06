#ifndef __openDrive_SPI_PORT_H 
#define __openDrive_SPI_PORT_H

#include "openDrive/LPC1768_bitdef.h"

/*** SPI CS Multiplexing
 * P2.0 -> CS; P2.1 -> CS_A; P2.2 -> CS_B; P2.3 -> CS_C
 * P2.0 must be low to use mutliplexer
 * P2.1/2/3 used to select port
 * See 74HC138 Datasheet for more informations
 ***/

#define SPICS_FIRSTBIT	0
#define SPICS_ALLHIGH	LPC_GPIO2->FIOSET = (0b1111 << SPICS_FIRSTBIT);
#define SPICS_Y0LOW		SPICS_ALLHIGH; LPC_GPIO2->FIOCLR = (~(0b0000) << SPICS_FIRSTBIT);
#define SPICS_Y1LOW		SPICS_ALLHIGH; LPC_GPIO2->FIOCLR = (~(0b0010) << SPICS_FIRSTBIT);
#define SPICS_Y2LOW		SPICS_ALLHIGH; LPC_GPIO2->FIOCLR = (~(0b0100) << SPICS_FIRSTBIT);
#define SPICS_Y3LOW		SPICS_ALLHIGH; LPC_GPIO2->FIOCLR = (~(0b0110) << SPICS_FIRSTBIT);
#define SPICS_Y4LOW		SPICS_ALLHIGH; LPC_GPIO2->FIOCLR = (~(0b1000) << SPICS_FIRSTBIT);
#define SPICS_Y5LOW		SPICS_ALLHIGH; LPC_GPIO2->FIOCLR = (~(0b1010) << SPICS_FIRSTBIT);
#define SPICS_Y6LOW		SPICS_ALLHIGH; LPC_GPIO2->FIOCLR = (~(0b1100) << SPICS_FIRSTBIT);
#define SPICS_Y7LOW		SPICS_ALLHIGH; LPC_GPIO2->FIOCLR = (~(0b1110) << SPICS_FIRSTBIT);


#define TMC428_SPI_RA_Active	SPICS_Y0LOW
#define TMC428_SPI_RA_Inactive	SPICS_ALLHIGH


//SPI Status Register
#define SPI_ABRT 0b00000100
#define SPI_MODF 0b00001000
#define SPI_ROVR 0b00010000
#define SPI_WCOL 0b00100000
#define SPI_SPIF 0x80

//SSPn Status Register
#define SSPN_TFE 0b00001		// Transmit FIFO Empty
#define SSPN_TNF 0b00010		// Transmit FIFO Not Full
#define SSPN_RNE 0b00100		// Receive FIFO Not Empty
#define SSPN_RFF 0b01000		// Receive FIFO Full
#define SSPN_BSY 0b10000		// Busy

#define SPI_SSEL_START
#define SPI_SSEL_STOP

#define SSP0_SSEL_HIGH		LPC_GPIO0->FIOSET = (1 << 16)
#define SSP0_SSEL_LOW		LPC_GPIO0->FIOCLR = (1 << 16)

#define SSP1_SSEL_HIGH		LPC_GPIO0->FIOSET = (1 << 6)
#define SSP1_SSEL_LOW		LPC_GPIO0->FIOCLR = (1 << 6)

static inline uint32_t xSSP0Port_Init(void) {

/*
	P0.16 => SSEL0
	P0.15 => SCK0
	P0.17 => MISO0
	P0.18 => MOSI0

	CPOL = 0
	CPHA = 0

	SSEL1 = active low
    SCK1 = active high
	MISO/MOSI = low
*/

	extern uint32_t SystemCoreClock;
	uint32_t PCLK = SystemCoreClock / 4;
	uint32_t SCR = 4;
	uint32_t CPSR = 20;

    //Reset PinFunctions to 0b00
	LPC_PINCON->PINSEL0 &= ~((0b11 << LPC_PINCON_PINSEL0_P0_15));
    LPC_PINCON->PINSEL1 &= ~((0b11 << LPC_PINCON_PINSEL1_P0_16) | (0b11 << LPC_PINCON_PINSEL1_P0_17) | (0b11 << LPC_PINCON_PINSEL1_P0_18));

	//Reset PinModes to 0b00
	LPC_PINCON->PINMODE0 &= ~((0b11 << LPC_PINCON_PINMODE0_P0_15));
    LPC_PINCON->PINMODE1 &= ~((0b11 << LPC_PINCON_PINMODE1_P0_16) | (0b11 << LPC_PINCON_PINMODE1_P0_17) | (0b11 << LPC_PINCON_PINMODE1_P0_18));

	//Reset PinDirections to 0b0
    LPC_GPIO0->FIODIR &= ~((1 << 15) | (1 << 16) | (1 << 17) | (1 << 18));
 
	
	//Set PinDirections
	LPC_GPIO0->FIODIR |= (1 << 15) |									// P0.15 (SCK0) => Output
	                     (1 << 16) |									// P0.16 (SSEL0) => Output
						 (0 << 17) |									// P0.17 (MISO0) => Input
						 (1 << 18);										// P0.18 (MOSI0) => Output
	
	//Set PinFunctions
	LPC_PINCON->PINSEL0 |=  (0b10 << LPC_PINCON_PINSEL0_P0_15);			// Config P0.15 = SCK0
	LPC_PINCON->PINSEL1 |=  (0b00 << LPC_PINCON_PINSEL1_P0_16) |		// Config P0.16 = GPIO (SSEL0)
                            (0b10 << LPC_PINCON_PINSEL1_P0_17) |		// Config P0.17 = MISO0
                            (0b10 << LPC_PINCON_PINSEL1_P0_18);			// Config P0.18 = MOSI0

	//Set PinModes
	LPC_PINCON->PINMODE0 |= (0b00 << LPC_PINCON_PINMODE0_P0_15);		// P0.15 (SCK0) = PullDown	
	LPC_PINCON->PINMODE1 |= (0b00 << LPC_PINCON_PINMODE1_P0_16) |		// P0.16 (SSEL0) = PullUp
	                        (0b11 << LPC_PINCON_PINMODE1_P0_17) |		// P0.17 (MISO0) = PullDown
                            (0b00 << LPC_PINCON_PINMODE1_P0_18);		// P0.18 (MOSI0) = PullDown	

	LPC_SC->PCONP |= (1 << LPC_SC_PCONP_PCSSP0);						// SSP0 power/clock control bit.
    LPC_SC->PCLKSEL1 |= (0b00 << LPC_SC_PCLKSEL1_PCLK_SSP0);			// Prescaler PCLK = CCLK/4

	LPC_SSP0->CR0 = ((8-1) << LPC_SSPn_CR0_DSS)							// Data Size Select => 8-bit transfer (frames)
	              | (0b00 << LPC_SSPn_CR0_FRF)							// Frame Format => SPI
	              | (0 << LPC_SSPn_CR0_CPOL)							// Clock Out Polarity => active low
	              | (0 << LPC_SSPn_CR0_CPHA)							// Clock Out Phase
				  | (SCR << LPC_SSPn_CR0_SCR);							// Serial Clock Rate => PCLK / (CPSDVSR × [SCR+1])

	LPC_SSP0->CR1 = (0 << LPC_SSPn_CR1_LBM)								// Loop Back Mode => normal operation
	              | (1 << LPC_SSPn_CR1_SSE)								// SSP Enable => enabled
				  | (0 << LPC_SSPn_CR1_MS)								// Master/Slave Mode => master
				  | (0 << LPC_SSPn_CR1_SOD);							// Slave Output Disable

	LPC_SSP0->CPSR = CPSR;												// Clock Prescale Register

	SSP0_SSEL_HIGH; //inactive

	uint32_t SSP0_CLK = (PCLK/(CPSR*(SCR+1)))/1000;	
	debug_printf("SSP0 initialized @%dkHz\n", SSP0_CLK);

	return(1);
}


static inline unsigned char xSSP0Port_Transmit(unsigned char SendChar) {

	while(!(LPC_SSP0->SR & 0b10)); //Wait until we can send to Fifo
	
	LPC_SSP0->DR = SendChar;

	unsigned char data = LPC_SSP0->DR;
	//(void)data;

	return(data);
}

static inline unsigned char xSSP0Port_Receive(unsigned char *ReceivedChar) {

	uint32_t status = LPC_SSP0->SR;
	if(status & 0b100) {
		//Receive FIFO Not Empty.
		*ReceivedChar = LPC_SSP0->DR;
		return(1);
	}

	return(0);
}


static inline void xSSP0Port_ClearRxFifo(void) {

	unsigned char ReceivedChar;
	while(LPC_SSP0->SR & 0b100) {
		//Receive FIFO Not Empty.		
		ReceivedChar = LPC_SSP0->DR;
		(void)ReceivedChar;
	}
	
	return;
}

static inline void xSSP0Port_SendDummyFrame(uint32_t cnt) {
  int i;
	for(i=0; i<cnt; i++) {
		xSSP0Port_Transmit(0xFF);
	}
	
	return;
}


/**********************************************************************************/
/* SSP1 functions                                                                 */
/**********************************************************************************/

/*** SSP1 Init ***/
static inline uint32_t xSSP1Port_Init(void) {

/*
	P0.6 => SSEL1
	P0.7 => SCK1
	P0.8 => MISO1
	P0.9 => MOSI1

	CPOL = 1
	CPHA = 1

	SSEL1 = active low
    SCK1 = active high
	MISO/MOSI = low
*/

	// Serial Clock Rate => PCLK / (CPSDVSR × [SCR+1])
	extern uint32_t SystemCoreClock;
	uint32_t PCLK = SystemCoreClock / 1;
	uint32_t SCR = 7;
	uint32_t CPSR = 10;

    //Reset PinFunctions to 0b00
	LPC_PINCON->PINSEL0 &= ~((0b11 << LPC_PINCON_PINSEL0_P0_6) | (0b11 << LPC_PINCON_PINSEL0_P0_7) | (0b11 << LPC_PINCON_PINSEL0_P0_8) | (0b11 << LPC_PINCON_PINSEL0_P0_9));

	//Reset PinModes to 0b00
	LPC_PINCON->PINMODE0 &= ~((0b11 << LPC_PINCON_PINMODE0_P0_6) | (0b11 << LPC_PINCON_PINMODE0_P0_7) | (0b11 << LPC_PINCON_PINMODE0_P0_8) | (0b11 << LPC_PINCON_PINMODE0_P0_9));

	//Reset PinDirections to 0b0
    LPC_GPIO0->FIODIR &= ~((1 << 6) | (1 << 7) | (1 << 8) | (1 << 9));
 
	
	//Set PinDirections
	LPC_GPIO0->FIODIR |= (1 << 6) |										// P0.6 (SSEL1) => Output
	                     (1 << 7) |										// P0.7 (SCK1) => Output
						 (0 << 8) |										// P0.8 (MISO1) => Input
						 (1 << 9);										// P0.9 (MOSI1) => Output
	
	//Set PinFunctions
	LPC_PINCON->PINSEL0 |=  (0b00 << LPC_PINCON_PINSEL0_P0_6) |			// Config P0.06 = GPIO (SSEL1)
	                        (0b10 << LPC_PINCON_PINSEL0_P0_7) |			// Config P0.07 = SCK1
                            (0b10 << LPC_PINCON_PINSEL0_P0_8) |			// Config P0.08 = MISO1
                            (0b10 << LPC_PINCON_PINSEL0_P0_9);			// Config P0.09 = MOSI1

	//Set PinModes
	LPC_PINCON->PINMODE0 |= (0b00 << LPC_PINCON_PINMODE0_P0_6) |		// P0.06 (SSEL1) = PullUp
	                        (0b11 << LPC_PINCON_PINMODE0_P0_7) |		// P0.07 (SCK1) = PullDown
                            (0b00 << LPC_PINCON_PINMODE0_P0_8) |		// P0.08 (MISO1) = PullDown
                            (0b00 << LPC_PINCON_PINMODE0_P0_9);			// P0.09 (MOSI1) = PullDown	

	LPC_SC->PCONP |= (1 << LPC_SC_PCONP_PCSSP1);						// SSP1 power/clock control bit.
    LPC_SC->PCLKSEL0 |= (0b01 << LPC_SC_PCLKSEL0_PCLK_SSP1);			// Prescaler PCLK = CCLK/1

	LPC_SSP1->CR0 = ((8-1) << LPC_SSPn_CR0_DSS)							// Data Size Select => 8-bit transfer (frames)
	              | (0b00 << LPC_SSPn_CR0_FRF)							// Frame Format => SPI
	              | (1 << LPC_SSPn_CR0_CPOL)							// Clock Out Polarity => active low
	              | (1 << LPC_SSPn_CR0_CPHA)							// Clock Out Phase
				  | (SCR << LPC_SSPn_CR0_SCR);							// Serial Clock Rate => PCLK / (CPSDVSR × [SCR+1])

	LPC_SSP1->CR1 = (0 << LPC_SSPn_CR1_LBM)								// Loop Back Mode => normal operation
	              | (1 << LPC_SSPn_CR1_SSE)								// SSP Enable => enabled
				  | (0 << LPC_SSPn_CR1_MS)								// Master/Slave Mode => master
				  | (0 << LPC_SSPn_CR1_SOD);							// Slave Output Disable

	LPC_SSP1->CPSR = CPSR;												// Clock Prescale Register

	SSP1_SSEL_HIGH; //inactive

	uint32_t SSP1_CLK = (PCLK/(CPSR*(SCR+1)))/1000;	
	debug_printf("SSP1 initialized @%dkHz\n", SSP1_CLK);

	return(1);
}


static inline unsigned char xSSP1Port_Transmit(unsigned char SendChar) {

	//while(!(LPC_SSP1->SR & SSPN_TNF)); //Wait until we can send to Fifo
	
	LPC_SSP1->DR = SendChar;
	while((LPC_SSP1->SR & (SSPN_TFE|SSPN_RNE|SSPN_BSY)) != 0b00101); //wait until byte is send & received

	unsigned char data = LPC_SSP1->DR;
	//(void)data;

	return(data);
}

static inline unsigned char xSSP1Port_Receive(unsigned char *ReceivedChar) {

	uint32_t status = LPC_SSP1->SR;
	if(status & 0b100) {
		//Receive FIFO Not Empty.
		*ReceivedChar = LPC_SSP1->DR;
		return(1);
	}

	return(0);
}

static inline void xSSP1Port_ClearRxFifo(void) {

	unsigned char ReceivedChar;
	while(LPC_SSP1->SR & 0b100) {
		//Receive FIFO Not Empty.		
		ReceivedChar = LPC_SSP1->DR;
		(void)ReceivedChar;
	}
	
	return;
}

static inline void xSSP1Port_SendDummyFrame(uint32_t cnt) {
  int i;
	for(i=0; i<cnt; i++) {
		xSSP1Port_Transmit(0xFF);
	}
	
	return;
}


/**********************************************************************************/
/**********************************************************************************/


/*** initialize SPI ChipSelect Multiplexer on P2.0 - P2.3 ***/
static inline void xSSP_CSMP_Init(void) {
	
	// Set Pinfunctions
	LPC_PINCON->PINSEL4 &= ~((0b11 << LPC_PINCON_PINSEL4_P2_0)		// Pin2.0 => GPIO
	                        |(0b11 << LPC_PINCON_PINSEL4_P2_1)		// Pin2.1 => GPIO
							|(0b11 << LPC_PINCON_PINSEL4_P2_2)		// Pin2.2 => GPIO
							|(0b11 << LPC_PINCON_PINSEL4_P2_3));	// Pin2.3 => GPIO

	// Set Pindirection
    LPC_GPIO2->FIODIR |= (1 << LPC_GPIOn_FIODIR_Pn_0)		// Pin2.0 = Output
	                  |  (1 << LPC_GPIOn_FIODIR_Pn_1)		// Pin2.1 = Output
					  |  (1 << LPC_GPIOn_FIODIR_Pn_2)		// Pin2.2 = Output
					  |  (1 << LPC_GPIOn_FIODIR_Pn_3);		// Pin2.3 = Output

	
	SPICS_ALLHIGH;
}

#endif /* end __openDrive_SPI_PORT_H */

