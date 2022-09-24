Graphical user interface
========================

A physical calculator is constrained to fixed labels for the keys as these can't be updated when the
calculator mode changes. A graphical user interface representation of the calculator doesn't have
these restrictions. The hardware abstraction layer supports the ability to change the labels on the
keys dependent on the mode.

For the implementer of these functions, there is no need to keep mode state since the correct function
will be called everytime the mode changes. There is also no need to send any different values when keys
are pressed. Physical calculators will have empty functions here.

For the caller of these functions, every time the mode changes in a way that the keys should be displayed
differently the appropriate mode function should be called.

Functions
---------

.. doxygenfile:: hal/gui.h

Example
-------

When the mode changes the appropriate function should be called to ensure the labels on the keys are
correct.

.. code-block:: C

   switch(newMode) {
     case CM_NORMAL:
       calcModeNormalGui();
       ...
       break;
     case CM_AIM:
       calcModeAimGui();
       ...
       break;
   }
