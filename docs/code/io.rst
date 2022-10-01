Input/output
============

File operations are supported in the hardware abstraction layer using an
abstraction that roughly maps onto standard file operations on most platforms.
The file operations only support a single file open at one time so no file
handle is passed in to any of the operations. Files must still be opened before
they are used and closed when they are finished with (to allow subsequent file
operations).

Instead of a file path an enumeration is used which represents a particular file
that should be saved. This is so an appropriate location can be chosen for the
given platform.

Functions
---------

.. doxygenfile:: hal/io.h

Example
-------

File operations should always close the file after it has been opened. Here is a
read-only example:

.. code-block:: C

   ioFileOpen(ioPathSaveFile, ioModeRead);
   char buffer[256];
   ioFileRead(buffer, 256);
   ioFileClose();
