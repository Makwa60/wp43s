Time
====

Internal timers and the timer application rely on a concept of time. The hardware
abstraction layer provides functions for this purpose

State and functions
-------------------

.. doxygenfile:: hal/time.h

Example
-------

In almost all cases it is better to use ``timeUptimeMs`` than ``timeCurrentMs``
as it is simpler.

.. code-block:: C

   uint32_t firstTime = timeUptimeMs();
   ...
   uint32_t elapsedTime = timeUptimeMs() - firstTime;

This doesn't cope with the 32-bit wrap around. More work is needed in these
cases.

To set the time/date of the calculator, ensure that all fields of the structure
are filled in. This functionality won't work with the simulator.

.. code-block:: C

   dateInfo_t date;
   date.year  = 2022;
   date.month = 1;
   date.day   = 30;
   timeSetDateInfo(&date);

To retrieve the time/date supply an allocated structure that will be written to.

.. code-block:: C

   timeInfo_t time;
   timeGetTimeInfo(&time);
   printf("This is the current hour: %d\n", time.hour);
