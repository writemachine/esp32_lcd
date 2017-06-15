# ESP32 LCD Component 

A small library for communicating with an LCD using the GPIO pins of an ESP32 Development board. This only supports 4 bit mode at presentand has no concept of screen dimensions. It should work with any LCD up to 20 x 4.

To use it include this component in the components directory of your app and then include the header "lcd.h". A simple example of its use:

```
#include <stdio.h>
#include "lcd.h"

void app_main()
{
  struct lcd_pins pins = { 22, 23, 19, 18, 17, 16 }; 
  struct lcd_display * display = lcd_init(&pins);

  lcd_set_cursor(display, 0, 0);
  lcd_write_char(display, 'A');

  lcd_destroy(display);
}
```

Depends on v2.0 of the ESP IDF.
