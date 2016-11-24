#include "MKL25Z4.h"

static inline void GLCD_DB_WRITE(char data)
{
	GPIOC_PDOR = (GPIOC_PDOR & 0xFFFFFF00) | data;
}

static inline void GLCD_WR_HIGH()
{
	GPIOC_PSOR = (1 << 13);
}

static inline void GLCD_WR_LOW()
{
	GPIOC_PCOR = (1 << 13);
}

static inline void GLCD_RD_HIGH()
{
	GPIOC_PSOR = (1 << 16);
}

static inline void GLCD_RD_LOW()
{
	GPIOC_PCOR = (1 << 16);
}

static inline void GLCD_CS_HIGH()
{
	GPIOC_PSOR = (1 << 11);
}

static inline void GLCD_CS_LOW()
{
	GPIOC_PCOR = (1 << 11);
}

static inline void GLCD_RS_HIGH()
{
	GPIOC_PSOR = (1 << 12);
}

static inline void GLCD_RS_LOW()
{
	GPIOC_PCOR = (1 << 12);
}

static inline void GLCD_RST_HIGH()
{
	GPIOC_PSOR = (1 << 17);
}

static inline void GLCD_RST_LOW()
{
	GPIOC_PCOR = (1 << 17);
}

static inline void GLCD_INIT_IO()
{
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;

	PORTC_PCR0 = PORT_PCR_MUX(1);
	PORTC_PCR1 = PORT_PCR_MUX(1);
	PORTC_PCR2 = PORT_PCR_MUX(1);
	PORTC_PCR3 = PORT_PCR_MUX(1);
	PORTC_PCR4 = PORT_PCR_MUX(1);
	PORTC_PCR5 = PORT_PCR_MUX(1);
	PORTC_PCR6 = PORT_PCR_MUX(1);
	PORTC_PCR7 = PORT_PCR_MUX(1);
	PORTC_PCR11 = PORT_PCR_MUX(1);
	PORTC_PCR12 = PORT_PCR_MUX(1);
	PORTC_PCR13 = PORT_PCR_MUX(1);
	PORTC_PCR16 = PORT_PCR_MUX(1);
	PORTC_PCR17 = PORT_PCR_MUX(1);

	GPIOC_PDDR |= 0xFF | (1 << 11) | (1 << 12) | (1 << 13) |
		(1 << 16) | (1 << 17);

	GLCD_RD_HIGH();
	GLCD_WR_HIGH();
	GLCD_CS_HIGH();
	GLCD_RST_LOW();
	GLCD_RST_HIGH();
}

static inline void GLCD_FAST_IO(char data)
{
	GLCD_DB_WRITE(data);
	GLCD_WR_LOW();
	asm volatile(
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		"nop \n"
		);
	GLCD_WR_HIGH();
}

void GLCD_SEND_DATA(char data);
void GLCD_SEND_CMD(char cmd);
