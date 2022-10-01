LCD
===

The display of the calculator is a 400 (width) by 240 (height) LCD. The hardware
abstraction layer provides functions to draw a filled rectangle and to manipulate
individual pixels. On a simulator these should only modify the contents of the
LCD part of the simulator (which should still be 400 by 240). All functions use
the origin at the top left.

Functions
---------

.. doxygenfile:: hal/lcd.h

Example
-------

LCD operations update the contents of the screen such that the latest operation
"wins". You can draw on top of previous state and the state remains until
changed.

.. code-block:: C

   // draw a black 20x20 rectangle starting offset just from the top right
   lcd_fill_rect(20, 20, 20, 20, LCD_SET_VALUE);
   // draw a white 10x10 rectangle inside this
   lcd_fill_rect(30, 30, 10, 10, LCD_EMPTY_VALUE);
