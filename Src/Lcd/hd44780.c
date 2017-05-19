/*
 * @file hd44780.c
 * @author Salvatore Barone
 * @email salvator.barone@gmail.com
 * @copyright
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the License, or any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "hd44780.h"
#include <assert.h>
#include <stdlib.h>

// Le macro seguenti definiscono i comandi che e' possibile inviare al display
#define HD44780_clear			0x01
#define HD44780_home			0x02
#define HD44780_row1			0x80
#define HD44780_row2			0xC0
#define HD44780_cursor_r		0x14
#define HD44780_cursor_l		0x10
#define HD44780_display_off		0x08
#define HD44780_cursor_off		0x0C
#define HD44780_cursor_on		0x0E
#define HD44780_cursor_blink	0x0F
#define HD44780_clear			0x01
#define HD44780_dec_no_shift	0x04
#define HD44780_dec_shift		0x05
#define HD44780_inc_no_shift	0x06
#define HD44780_inc_shift		0x07

#define timer_wait_ms(ms)   usleep(ms<<10)
#define timer_wait_us(us)	usleep(us)
#define lcd_command(lcd)	GPIO_setValue(lcd->gpio, lcd->RS, GPIO_reset)
#define lcd_data(lcd)		GPIO_setValue(lcd->gpio, lcd->RS, GPIO_set)
#define lcd_write(lcd)		GPIO_setValue(lcd->gpio, lcd->RW, GPIO_reset)
#define lcd_read(lcd)		GPIO_setValue(lcd->gpio, lcd->RW, GPIO_set)
#define lcd_enable(lcd)		GPIO_setValue(lcd->gpio, lcd->E, GPIO_set); \
							timer_wait_us(100); \
							GPIO_setValue(lcd->gpio, lcd->E, GPIO_reset)

void HD44780_SetByte(HD44780_LCD_t* lcd, uint8_t byte) {
	int i;
	if (lcd->iface_mode == HD44780_INTERFACE_8bit) {
		GPIO_setValue(lcd->gpio, lcd->Data7, ((byte & 0x80) == 0 ? GPIO_reset : GPIO_set));
		GPIO_setValue(lcd->gpio, lcd->Data6, ((byte & 0x40) == 0 ? GPIO_reset : GPIO_set));
		GPIO_setValue(lcd->gpio, lcd->Data5, ((byte & 0x20) == 0 ? GPIO_reset : GPIO_set));
		GPIO_setValue(lcd->gpio, lcd->Data4, ((byte & 0x10) == 0 ? GPIO_reset : GPIO_set));
		GPIO_setValue(lcd->gpio, lcd->Data3, ((byte & 0x08) == 0 ? GPIO_reset : GPIO_set));
		GPIO_setValue(lcd->gpio, lcd->Data2, ((byte & 0x04) == 0 ? GPIO_reset : GPIO_set));
		GPIO_setValue(lcd->gpio, lcd->Data1, ((byte & 0x02) == 0 ? GPIO_reset : GPIO_set));
		GPIO_setValue(lcd->gpio, lcd->Data0, ((byte & 0x01) == 0 ? GPIO_reset : GPIO_set));
		lcd_enable(lcd);
	}
	else {
		GPIO_setValue(lcd->gpio, lcd->Data7, ((byte & 0x80) == 0 ? GPIO_reset : GPIO_set));
		GPIO_setValue(lcd->gpio, lcd->Data6, ((byte & 0x40) == 0 ? GPIO_reset : GPIO_set));
		GPIO_setValue(lcd->gpio, lcd->Data5, ((byte & 0x20) == 0 ? GPIO_reset : GPIO_set));
		GPIO_setValue(lcd->gpio, lcd->Data4, ((byte & 0x10) == 0 ? GPIO_reset : GPIO_set));
		lcd_enable(lcd);
		GPIO_setValue(lcd->gpio, lcd->Data7, ((byte & 0x08) == 0 ? GPIO_reset : GPIO_set));
		GPIO_setValue(lcd->gpio, lcd->Data6, ((byte & 0x04) == 0 ? GPIO_reset : GPIO_set));
		GPIO_setValue(lcd->gpio, lcd->Data5, ((byte & 0x02) == 0 ? GPIO_reset : GPIO_set));
		GPIO_setValue(lcd->gpio, lcd->Data4, ((byte & 0x01) == 0 ? GPIO_reset : GPIO_set));
		lcd_enable(lcd);
	}
}

void HD44780_WriteCommand(HD44780_LCD_t* lcd, uint8_t command) {
	assert(lcd);
	assert(lcd->gpio);
	assert(HD44780_ValidatePair(lcd));
	lcd_write(lcd);
	lcd_command(lcd);
	HD44780_SetByte(lcd, command);
	timer_wait_ms(2);
}

void HD44780_WriteData(HD44780_LCD_t* lcd, uint8_t data) {
	assert(lcd);
	assert(lcd->gpio);
	assert(HD44780_ValidatePair(lcd));
	lcd_write(lcd);
	lcd_data(lcd);
	HD44780_SetByte(lcd, data);
	timer_wait_ms(2);
}

int HD44780_ValidatePair(HD44780_LCD_t* lcd)
{
	int array_dim = (lcd->iface_mode == HD44780_INTERFACE_8bit ? 11 : 7);
	int i, j;
	const GPIO_mask pair[] = {	lcd->RS, lcd->RW, lcd->E,
								lcd->Data7, lcd->Data6, lcd->Data5, lcd->Data4,
								lcd->Data3, lcd->Data2, lcd->Data1, lcd->Data0};
	for (i = 0; i < array_dim; i++) {
		if (pair[i] == 0)
			return 0;
		for (j = i+1; j < array_dim; j++) {
			if (pair[j] == 0)
			return 0;
			if (pair[i] == pair[j])
			return 0;
		}
	}
	return 1;
}

void HD44780_ConfigurePin(HD44780_LCD_t* lcd) {
	GPIO_setMode(lcd->gpio, lcd->RS | lcd->RW | lcd->E |
							lcd->Data7 | lcd->Data6 | lcd->Data5 | lcd->Data4 |
							lcd->Data3 | lcd->Data2 | lcd->Data1 | lcd->Data0, GPIO_write);
	GPIO_setValue(lcd->gpio, lcd->RS | lcd->RW | lcd->E |
							lcd->Data7 | lcd->Data6 | lcd->Data5 | lcd->Data4 |
							lcd->Data3 | lcd->Data2 | lcd->Data1 | lcd->Data0, GPIO_reset);
}

void HD44780_Init8(	HD44780_LCD_t	*lcd,
					GPIO_t			*gpio,
					GPIO_mask		RS,
					GPIO_mask		RW,
					GPIO_mask		E,
					GPIO_mask		Data7,
					GPIO_mask 		Data6,
					GPIO_mask 		Data5,
					GPIO_mask 		Data4,
					GPIO_mask 		Data3,
					GPIO_mask 		Data2,
					GPIO_mask 		Data1,
					GPIO_mask 		Data0) {
	assert(lcd);
	assert(gpio);
	lcd->iface_mode = HD44780_INTERFACE_8bit;
	lcd->gpio = gpio;
	lcd->RS = RS;
	lcd->RW = RW;
	lcd->E = E;
	lcd->Data7 = Data7;
	lcd->Data6 = Data6;
	lcd->Data5 = Data5;
	lcd->Data4 = Data4;
	lcd->Data3 = Data3;
	lcd->Data2 = Data2;
	lcd->Data1 = Data1;
	lcd->Data0 = Data0;
	assert(HD44780_ValidatePair(lcd));
	HD44780_ConfigurePin(lcd);
	// sequenza di inizializzazione del device
	timer_wait_ms(50);
	GPIO_setValue(lcd->gpio, lcd->RS, GPIO_reset);
	GPIO_setValue(lcd->gpio, lcd->RW, GPIO_reset);
	GPIO_setValue(lcd->gpio, lcd->Data7, GPIO_reset);
	GPIO_setValue(lcd->gpio, lcd->Data6, GPIO_reset);
	GPIO_setValue(lcd->gpio, lcd->Data5, GPIO_set);
	GPIO_setValue(lcd->gpio, lcd->Data4, GPIO_set);
	lcd_enable(lcd);
	timer_wait_ms(10);
	lcd_enable(lcd);
	timer_wait_ms(10);
	lcd_enable(lcd);
	timer_wait_ms(10);
	HD44780_SetByte(lcd, 0x38);
	timer_wait_ms(10);
	HD44780_SetByte(lcd, 0x08);
	timer_wait_ms(10);
	HD44780_SetByte(lcd, 0x01);
	timer_wait_ms(10);
	HD44780_SetByte(lcd, 0x06);
	timer_wait_ms(10);
	HD44780_SetByte(lcd, 0x0F);
	timer_wait_ms(10);
}

void HD44780_Init4(	HD44780_LCD_t	*lcd,
					GPIO_t			*gpio,
					GPIO_mask		RS,
					GPIO_mask		RW,
					GPIO_mask		E,
					GPIO_mask		Data7,
					GPIO_mask 		Data6,
					GPIO_mask 		Data5,
					GPIO_mask 		Data4) {
	assert(lcd);
	assert(gpio);
	lcd->iface_mode = HD44780_INTERFACE_4bit;
	lcd->gpio = gpio;
	lcd->RS = RS;
	lcd->RW = RW;
	lcd->E = E;
	lcd->Data7 = Data7;
	lcd->Data6 = Data6;
	lcd->Data5 = Data5;
	lcd->Data4 = Data4;
	lcd->Data3 = 0;
	lcd->Data2 = 0;
	lcd->Data1 = 0;
	lcd->Data0 = 0;
	assert(HD44780_ValidatePair(lcd));
	HD44780_ConfigurePin(lcd);
	// sequenza di inizializzazione del device
	timer_wait_ms(50);
	GPIO_setValue(lcd->gpio, lcd->RS, GPIO_reset);
	GPIO_setValue(lcd->gpio, lcd->RW, GPIO_reset);
	GPIO_setValue(lcd->gpio, lcd->Data7, GPIO_reset);
	GPIO_setValue(lcd->gpio, lcd->Data6, GPIO_reset);
	GPIO_setValue(lcd->gpio, lcd->Data5, GPIO_set);
	GPIO_setValue(lcd->gpio, lcd->Data4, GPIO_set);
	lcd_enable(lcd);
	timer_wait_ms(10);
	lcd_enable(lcd);
	timer_wait_ms(10);
	lcd_enable(lcd);
	timer_wait_ms(10);
	GPIO_setValue(lcd->gpio, lcd->Data7, GPIO_reset);
	GPIO_setValue(lcd->gpio, lcd->Data6, GPIO_reset);
	GPIO_setValue(lcd->gpio, lcd->Data5, GPIO_set);
	GPIO_setValue(lcd->gpio, lcd->Data4, GPIO_reset);
	lcd_enable(lcd);
	timer_wait_ms(5);
	HD44780_SetByte(lcd, 0x28);
	timer_wait_ms(10);
	HD44780_SetByte(lcd, 0x08);
	timer_wait_ms(10);
	HD44780_SetByte(lcd, 0x01);
	timer_wait_ms(10);
	HD44780_SetByte(lcd, 0x06);
	timer_wait_ms(10);
	HD44780_SetByte(lcd, 0x0F);
	timer_wait_ms(10);
}

void HD44780_Printc(HD44780_LCD_t* lcd, char c) {
	HD44780_WriteData(lcd, (uint8_t)c);
}

void HD44780_Print(HD44780_LCD_t* lcd, const char *s) {
	int i = 0;
	while ((uint8_t)s[i] != 0)
		HD44780_WriteData(lcd, (uint8_t)s[i++]);
}

void HD44780_printBinary8(HD44780_LCD_t *lcd, uint8_t b) {
	int i;
	for (i = 7; i != -1; i--)
		HD44780_Printc(lcd, ((b>>i)&1)+'0');
}

void HD44780_printBinary32(HD44780_LCD_t *lcd, uint32_t w) {
	int i;
	for (i = 31; i != -1; i--)
		HD44780_Printc(lcd, ((w>>i)&1)+'0');
}

void HD44780_printBinary64(HD44780_LCD_t *lcd, uint64_t b) {
	int i;
	for (i = 63; i != -1; i--)
		HD44780_Printc(lcd, ((b>>i)&1)+'0');
}

void HD44780_printHex8(HD44780_LCD_t *lcd, uint8_t b) {
	int i;
	for (i = 7; i != -1; i-=4)
		HD44780_Printc(lcd, ((b>>i)&0xF) + (((b>>i)&0xF) < 10 ? '0' : 'A'));
}

void HD44780_printHex32(HD44780_LCD_t *lcd, uint32_t w) {
	int i;
	for (i = 31; i != -1; i-=4)
		HD44780_Printc(lcd, ((w>>i)&0xF) + (((w>>i)&0xF) < 10 ? '0' : 'A'));
}

void HD44780_printHex64(HD44780_LCD_t *lcd, uint64_t b) {
	int i;
	for (i = 63; i != -1; i-=4)
		HD44780_Printc(lcd, ((b>>i)&0xF) + (((b>>i)&0xF) < 10 ? '0' : 'A'));
}

void HD44780_Clear(HD44780_LCD_t* lcd) {
	HD44780_WriteCommand(lcd, HD44780_clear);
}

void HD44780_Home(HD44780_LCD_t* lcd) {
	HD44780_WriteCommand(lcd, HD44780_home);
}

void HD44780_MoveToRow1(HD44780_LCD_t* lcd) {
	HD44780_WriteCommand(lcd, HD44780_row1);
}

void HD44780_MoveToRow2(HD44780_LCD_t* lcd) {
	return HD44780_WriteCommand(lcd, HD44780_row2);
}

void HD44780_MoveCursor(HD44780_LCD_t* lcd, HD44780_Direction_t dir) {
	HD44780_WriteCommand(lcd, (dir == HD44780_CursorLeft ? HD44780_cursor_l : HD44780_cursor_r));
}

void HD44780_DisplayOff(HD44780_LCD_t* lcd) {
	HD44780_WriteCommand(lcd, HD44780_display_off);
}

void HD44780_CursorOff(HD44780_LCD_t* lcd) {
	HD44780_WriteCommand(lcd, HD44780_cursor_off);
}

void HD44780_CursorOn(HD44780_LCD_t* lcd) {
	HD44780_WriteCommand(lcd, HD44780_cursor_on);
}

void HD44780_CursorBlink(HD44780_LCD_t* lcd) {
	HD44780_WriteCommand(lcd, HD44780_cursor_blink);
}
