Time
====

Internal timers and the timer application rely on a concept of time. The hardware
abstraction layer provides functions for this purpose

Functions
---------

.. doxygenfile:: hal/time.h

Example
-------

In almost all cases it is better to use ``timeUptimeMs`` as it is simpler.

.. code-block:: C

   uint32_t firstTime = timeUptimeMs();
   ...
   uint32_t elapsedTime = timeUptimeMs() - firstTime;

This doesn't cope with the 32-bit wrap around. More work is needed in these cases.
