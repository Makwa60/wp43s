Input/output
============

File operations are supported in the hardware abstraction layer using an abstraction that roughly
maps onto standard file operations on most platforms. The significant difference is that paths are
represented by an enumeration rather than a string. This is so that the appropriate location can
be chosen for the given platform dependent on the purpose of the file. Note also that the file
handle is the first argument to the functions rather than the last. (This is to better represent
a more object-oriented C style of programming.)

Functions
---------

.. doxygenfile:: hal/io.h

Example
-------

File operations should always close the file after it has been opened. The file handle shouldn't
be used after it has been closed. Here is a read-only example:

.. code-block:: C

   ioFile_t *file = ioFileOpen(IOPATH_SAVEFILE, IOMODE_READ);
   char buffer[256];
   ioFileRead(file, buffer, 256);
   ioFileClose(file);
