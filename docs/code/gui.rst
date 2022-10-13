Graphical user interface
========================

A physical calculator is constrained to fixed labels for the keys as these can't
be updated when the calculator mode changes. A graphical user interface
representation of the calculator doesn't have these restrictions. The hardware
abstraction layer supports the ability to change the labels on the keys
dependent on the mode.

For the implementer of this function, there is no need to keep mode state
since the function will be called everytime the layout should change.
There is also no need to send any different values for callbacks when keys are
pressed. Physical calculators will have an empty implementation for this
function.

For the caller of this function, every time the mode changes in a way that the
keys should be displayed differently the appropriate layout should be selected.

Functions
---------

.. doxygenfile:: hal/gui.h

Example
-------

When the mode changes the appropriate layout should be used as an argument to
the ``guiSetLayout`` function to ensure the labels on the keys are correct.

.. code-block:: C

   switch(newMode) {
     case cmNormal:
       guiSetLayout(glNormal);
       ...
       break;
     case cmAim:
       guiSetLayout(glAim);
       ...
       break;
   }
