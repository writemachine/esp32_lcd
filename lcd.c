/* LCD / GPIO Library for ESP32
  
   Author: aliraza1944@gmail.com, scandox@gmail.com

   This example code is in the Public Domain

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "lcd.h"

/* Sleep an arbitary time in milliseconds

   If you end up with a fractional value less than 1 then there 
   will be no delay. So for example if the interrupt timer is set
   to 100HZ and you pass 1 ms, then you'll get 0.1 => 0.

   If you need millisecond precision (which we do here) then you'll 
   want a interrupt timer at 1000HZ.
*/
void delay(int ms)
{
  vTaskDelay(ms / portTICK_PERIOD_MS);
}

/* Necessary to pulse the clock between certain operations 

*/
void clock_pulse(struct lcd_display * display, int ms) {
  gpio_set_level(display->gpio->en, 1);
  delay(ms);
  gpio_set_level(display->gpio->en, 0);
}

/* Sets four bits on GPIO pins 4-7 or 0-3

*/
void write4(struct lcd_display * display, int command) {
  int d[4] = { 0,0,0,0 };

  if(command & 1) d[0] = 1;
  if(command & 2) d[1] = 1;
  if(command & 4) d[2] = 1;
  if(command & 8) d[3] = 1;

  gpio_set_level(display->gpio->d0, d[0]);
  gpio_set_level(display->gpio->d1, d[1]);
  gpio_set_level(display->gpio->d2, d[2]);
  gpio_set_level(display->gpio->d3, d[3]);
}

/* Writes 8 bits in 4 bit mode

*/
void write8_4bitmode(struct lcd_display * display, int command)
{
  gpio_set_level(display->gpio->rs, 1);
  write4(display, command >> 4);
  clock_pulse(display, 1);
  write4(display, command);
  clock_pulse(display, 1);
}

/* Sends a command to LCD 

*/
void lcd_command(struct lcd_display * display, int command)
{
  gpio_set_level(display->gpio->rs, 0);
  write4(display, command);
  clock_pulse(display, 2);
}

/* Clears the screen 

*/
void lcd_clear(struct lcd_display * display)
{
  lcd_command(display, 0);
  lcd_command(display, 1);
}

/* Sets cursor position 
  
   Line and column are zero indexed

*/
void lcd_set_cursor(struct lcd_display * display, int line, int column)
{
  int lower, upper, position; 

  switch (line) {
    case 0:
      position = 0x80 + column; 
      break;
    case 1:
      position = 0XC0 + column;
      break;
    case 2:
      position = 0x94 + column;
      break;
    case 3:
      position = 0xD4 + column;
      break;
    default:
      return;
      break;
  }

  lower = position >> 4;
  upper = position;
  lcd_command(display, lower);
  lcd_command(display, upper);
}

/* Initializes the pins on the specified display

*/
void lcd_gpio_init(struct lcd_display * display) {
  gpio_set_direction(display->gpio->rs, GPIO_MODE_OUTPUT);
  gpio_set_direction(display->gpio->en, GPIO_MODE_OUTPUT);
  gpio_set_direction(display->gpio->d0, GPIO_MODE_OUTPUT);
  gpio_set_direction(display->gpio->d1, GPIO_MODE_OUTPUT);
  gpio_set_direction(display->gpio->d2, GPIO_MODE_OUTPUT);
  gpio_set_direction(display->gpio->d3, GPIO_MODE_OUTPUT);
  delay(50);
}

/* Creates and initializes a new display

   Returns pointer to the display

*/
struct lcd_display * lcd_init(struct lcd_pins * pins)
{
  struct lcd_display * display = malloc(sizeof(struct lcd_display));
  display->gpio = malloc(sizeof(struct lcd_pins));
  memcpy(display->gpio, pins, sizeof(struct lcd_pins)); 

  lcd_gpio_init(display); 

  write8_4bitmode(display, 0x00);
  delay(20);
	
  /* Magic Reset Sequence */
  lcd_command(display, 0x03);
  delay(2);
  lcd_command(display, 0x03);
  delay(2);
  lcd_command(display, 0x03);
  delay(2);
  
  lcd_command(display, 0x02);
  delay(2);
  lcd_command(display, 0x02);
  lcd_command(display, 0x08);
  lcd_command(display, 0x00);

  // Display on, cursor on, blinking off
  lcd_command(display, 0x0E);

  lcd_command(display, 0x00);
  lcd_command(display, 0x06);

  return display;
}

/* Frees memory associated with a display

*/
void lcd_destroy(struct lcd_display * display) {
  free(display->gpio);
  free(display);
}

/* Write a piece of text

   Keeps sending characters until \0. Does not attempt
   to wrap text and has no sense of where the cursor is

*/
void lcd_write(struct lcd_display * display, char * text)
{
  int i;
  for(i=0; text[i]!='\0'; i++) {
    write8_4bitmode(display, text[i]);
  }
}

/* Write a single char

*/
void lcd_write_char(struct lcd_display * display, char letter)
{
  write8_4bitmode(display, letter);
}

void lcd_shift_right(struct lcd_display * display)
{
	lcd_command(display, 0x01);
	lcd_command(display, 0x0C);
}

void lcd_shift_left(struct lcd_display * display)
{
	lcd_command(display, 0x01);
	lcd_command(display, 0x08);
}
