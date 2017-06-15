/* LCD / GPIO Library for ESP32
  
   Author: aliraza1944@gmail.com, scandox@gmail.com

   This example code is in the Public Domain

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#ifndef LCD_GPIO
#define LCD_GPIO

struct lcd_pins {
  int rs;
  int en;
  int d0; // remember could be d4
  int d1; // ... d5
  int d2; // ... d6
  int d3; // ... d7
};

struct lcd_display {
  struct lcd_pins * gpio;
};

struct lcd_display * lcd_init(struct lcd_pins * pins);
void lcd_destroy(struct lcd_display * display);
void lcd_clear(struct lcd_display * display);
void lcd_write(struct lcd_display * display, char * text);
void lcd_write_char(struct lcd_display * display, char letter);
void lcd_set_cursor(struct lcd_display * display, int line, int column);
void lcd_command(struct lcd_display * display, int command);
void lcd_shift_right(struct lcd_display * display);
void lcd_shift_left(struct lcd_display * display);

#endif
