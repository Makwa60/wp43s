Timer
=====

The hardware abstraction layer provides functions for managing callbacks based on
timer expiry. These timers should be run from the main loop but also from
``systemProcessEvents`` when called within the WP43 code.

Functions
---------

.. doxygenfile:: hal/timer.h

Example
-------

The configuration of the timers should be done early on in initialisation after
first calling ``timerReset``:

.. code-block:: C

   timerReset();
   timerConfig(tidAutoRepeat, cbAutoRepeat);
   timerConfig(tidShowNop,    cbShowNop);

where ``cbShowNop`` is the callback function for when the ``tidShowNop`` timer
expires.

When the timer expires the timer callback function will be called once (not
repeatedly). If the timer should be called repeatedly then the callback can
start the timer again.

.. code-block:: C

   void cbShowNop(uint16_t param) {
     // ...
     // Restart timer to get callback in another 200 milliseconds
     timerStart(tidShowNop, param, 200);
   }

   void myFunction(void) {
     timerStart(tidShowNop, 0, 200);
   }
