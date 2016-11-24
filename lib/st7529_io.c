#include "st7529_io.h"

void GLCD_SEND_CMD(char cmd)
{
	GLCD_CS_LOW();
	GLCD_RS_LOW();
	GLCD_DB_WRITE(cmd);
	GLCD_WR_LOW();
	asm volatile(
		"nop \n"
		"nop \n"
		"nop \n"
		);
	GLCD_WR_HIGH();
	GLCD_CS_HIGH();
}

void GLCD_SEND_DATA(char data)
{
	GLCD_CS_LOW();
	GLCD_RS_HIGH();
	GLCD_DB_WRITE(data);
	GLCD_WR_LOW();
	asm volatile(
		"nop \n"
		"nop \n"
		"nop \n"
		);
	GLCD_WR_HIGH();
	GLCD_CS_HIGH();
}
