/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2016, Augusto Fraga Giachero
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "st7529_io.h"
#include "st7529.h"
#include "fonts.h"

uint32_t GBUFFER[1200];

void GLCD_CLEAR()
{
	int PixelCount;
	GLCD_SEND_CMD(0x15);
	GLCD_SEND_DATA(0x00);
	GLCD_SEND_DATA(0x4F);
	GLCD_SEND_CMD(0x75);
	GLCD_SEND_DATA(0x00);
	GLCD_SEND_DATA(0x9F);
	GLCD_SEND_CMD(0x5C);
	for (PixelCount = 25600; PixelCount > 0; PixelCount--)
	{
		GLCD_SEND_DATA(0x00);
	}
}

void GLCD_INIT()
{
	GLCD_INIT_IO();
	GLCD_SEND_CMD(0x30);        // ext0 set
	GLCD_SEND_CMD(0x94);        // exit sleep mode
	GLCD_SEND_CMD(0xd1);        // OSC on
	GLCD_SEND_CMD(0x20);
	GLCD_SEND_DATA(0x08);       // VB=1 (Booster on first)

	GLCD_SEND_CMD(0x20);
	GLCD_SEND_DATA(0x0b);       // VB=VF=VR=1(Booster,follower,reference on)
	GLCD_SEND_CMD(0xca);
	GLCD_SEND_DATA(0x04);       // CLD=1(clk/2)
	GLCD_SEND_DATA(0x27);       // DT=39=160/4-1
	GLCD_SEND_DATA(0x00);       // FI=0;LF=0;

	GLCD_SEND_CMD(0xbb);
	GLCD_SEND_DATA(0x01);       // com scan direction 0-79,159-80
	GLCD_SEND_CMD(0xbc);
	GLCD_SEND_DATA(0x01);       // LI=1(Inverse), CI=0(normal), C/L=0(along column)
	GLCD_SEND_DATA(0x00);       // CLR=0 normal order;
	GLCD_SEND_DATA(0x01);       // GS=1 2b3p
	GLCD_SEND_CMD(0x31);        // ext1 set
	GLCD_SEND_CMD(0x32);
	GLCD_SEND_DATA(0x05);       // with CLD=1, Frame Freq= 62.5Hz
	GLCD_SEND_DATA(0x00);       // BoosterFreq=3k
	GLCD_SEND_DATA(0x02);       // bias=1/12
	GLCD_SEND_CMD(0x30);        //ext0 set
	GLCD_SEND_CMD(0xaf);        //display on

	GLCD_SEND_CMD(0x81);
	GLCD_SEND_DATA(0x2D);
	GLCD_SEND_DATA(0x03);       // contrast set vop=15V  1d 4

	int ContContrast;
	for(ContContrast = 34; ContContrast > 0; ContContrast --)
	{
		GLCD_SEND_CMD(0xD6);
	}

	GLCD_SEND_CMD(0xA7);
	GLCD_CLEAR();
}
#define PIXEL_0_MASK 0xF800
#define PIXEL_1_MASK 0x0780
#define PIXEL_2_MASK 0x001F
void GLCD_LOAD_IMG(const uint32_t *img_ptr)
{
	const uint8_t *img_b_ptr = (uint8_t*)img_ptr;
	int ByteCount, PixelCount;
	uint32_t WordData;
	uint16_t DSend;
	
	GLCD_SEND_CMD(0x15);
	GLCD_SEND_DATA(0x00);
	GLCD_SEND_DATA(0x4F);
	GLCD_SEND_CMD(0x75);
	GLCD_SEND_DATA(0x00);
	GLCD_SEND_DATA(0x9F);
	GLCD_SEND_CMD(0x5C);
	
	GLCD_CS_LOW();
	GLCD_RS_HIGH();	

	for (ByteCount = 0; ByteCount < 1600; ByteCount++)
	{
		WordData = img_b_ptr[ByteCount*3] | (img_b_ptr[ByteCount*3 + 1] << 8) | (img_b_ptr[ByteCount*3 + 2] << 16);
		for(PixelCount = 0; PixelCount < 8; PixelCount++)
		{
			DSend = 0;
			if(WordData & 0x01)
			{
				DSend |= PIXEL_0_MASK;
			}
			
			if(WordData & 0x02)
			{
				DSend |= PIXEL_1_MASK;
			}

			if(WordData & 0x04)
			{
				DSend |= PIXEL_2_MASK;
			}
			GLCD_FAST_IO(DSend >> 8);
			GLCD_FAST_IO(DSend & 0xFF);
			
			WordData = WordData >> 3;
		}
	}
	GLCD_CS_HIGH();
}

void GLCD_UPDATE()
{
	GLCD_LOAD_IMG(GBUFFER);
}

void GLCD_SET_PIXEL(uint8_t Px, uint8_t Py, uint8_t Color)
{
	char *b_ptr = (char*)GBUFFER;
	int pixel = Py*240 + Px;
	char byte = b_ptr[pixel/8];
	if(Color)
	{
		byte |= 1 << (pixel%8);
	}
	else
	{
		byte &= ~(1 << (pixel%8));
	}
	
	b_ptr[pixel/8] = byte;
}

void GLCD_DRAW_RECT(uint8_t P1x, uint8_t P1y, uint8_t P2x, uint8_t P2y, uint8_t Color, uint8_t Fill)
{
	uint8_t TmpPx, TmpPy;
	if(P1x > P2x)
	{
		TmpPx = P2x;
		P2x = P1x;
		P1x = TmpPx;
	}
	
	if(P1y > P2y)
	{
		TmpPy = P2y;
		P2y = P1y;
		P1y = TmpPy;
	}
	
	if(Fill)
	{
		for(TmpPy = P1y; TmpPy <= P2y; TmpPy++)
		{
			for(TmpPx = P1x; TmpPx <= P2x; TmpPx++)
			{
				GLCD_SET_PIXEL(TmpPx, TmpPy, Color);
			}
		}
	}
	else
	{
		TmpPy = P1y;
		for(TmpPx = P1x; TmpPx <= P2x; TmpPx++)
		{
			GLCD_SET_PIXEL(TmpPx, TmpPy, Color);
		}
		TmpPy = P2y;
		for(TmpPx = P1x; TmpPx <= P2x; TmpPx++)
		{
			GLCD_SET_PIXEL(TmpPx, TmpPy, Color);
		}
		TmpPx = P1x;
		for(TmpPy = P1y; TmpPy <= P2y; TmpPy++)
		{
			GLCD_SET_PIXEL(TmpPx, TmpPy, Color);
		}
		TmpPx = P2x;
		for(TmpPy = P1y; TmpPy <= P2y; TmpPy++)
		{
			GLCD_SET_PIXEL(TmpPx, TmpPy, Color);
		}
	}
}

void GLCD_BUFFER_CLEAR()
{
	int word;
	for(word = 0; word < 1200; word++)
	{
		GBUFFER[word] = 0;
	}
}

void GLCD_LOAD_IMG_BUFFER(const uint32_t *img_ptr)
{
	int word;
	for(word = 0; word < 1200; word++)
	{
		GBUFFER[word] = img_ptr[word];
	}
}

void GLCD_INC_CONTRAST()
{
	GLCD_SEND_CMD(0xD6);
}

void GLCD_DEC_CONTRAST()
{
	GLCD_SEND_CMD(0xD7);
}

void GLCD_WRITE_CHAR(char data, uint8_t Px, uint8_t Py)
{
	data -= 0x10;
	int Hcount;
	uint8_t c = data;
	for(Hcount = 0; Hcount < 5; Hcount++)
	{
		GLCD_SET_PIXEL(Px + Hcount, Py, (ext_ascii_8x5[c][Hcount] & 0b00000001) ? 1 : 0);
		GLCD_SET_PIXEL(Px + Hcount, Py + 1, (ext_ascii_8x5[c][Hcount] & 0b00000010) ? 1 : 0);
		GLCD_SET_PIXEL(Px + Hcount, Py + 2, (ext_ascii_8x5[c][Hcount] & 0b00000100) ? 1 : 0);
		GLCD_SET_PIXEL(Px + Hcount, Py + 3, (ext_ascii_8x5[c][Hcount] & 0b00001000) ? 1 : 0);
		GLCD_SET_PIXEL(Px + Hcount, Py + 4, (ext_ascii_8x5[c][Hcount] & 0b00010000) ? 1 : 0);
		GLCD_SET_PIXEL(Px + Hcount, Py + 5, (ext_ascii_8x5[c][Hcount] & 0b00100000) ? 1 : 0);
		GLCD_SET_PIXEL(Px + Hcount, Py + 6, (ext_ascii_8x5[c][Hcount] & 0b01000000) ? 1 : 0);
		GLCD_SET_PIXEL(Px + Hcount, Py + 7, (ext_ascii_8x5[c][Hcount] & 0b10000000) ? 1 : 0);
	}
}

void GLCD_WRITE_STR(const char *str, uint8_t Px, uint8_t Py)
{
	int ofx, ofy, count;
	ofx = ofy = 0;
	for(count = 0; str[count] != 0; count++)
	{
		if((str[count] == '\n') || (str[count] == '\r'))
		{
			ofx = 0;
			ofy += 9;
		}
		else
		{
			GLCD_WRITE_CHAR(str[count], ofx, ofy);
			ofx += 6;
		}
	}
}
