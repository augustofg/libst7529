#include <stdint.h>

void GLCD_INIT();
void GLCD_CLEAR();
void GLCD_BUFFER_CLEAR();
void GLCD_UPDATE();
void GLCD_LOAD_IMG(const uint32_t *img_ptr);
void GLCD_LOAD_IMG_BUFFER(const uint32_t *img_ptr);
void GLCD_SET_PIXEL(uint8_t Px, uint8_t Py, uint8_t Color);
void GLCD_DRAW_RECT(uint8_t P1x, uint8_t P1y, uint8_t P2x, uint8_t P2y, uint8_t Color, uint8_t Fill);
void GLCD_INC_CONTRAST();
void GLCD_DEC_CONTRAST();
void GLCD_WRITE_CHAR(char data, uint8_t Px, uint8_t Py);
void GLCD_WRITE_STR(const char *str, uint8_t Px, uint8_t Py);
