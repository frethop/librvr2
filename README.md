# librvr2
C Library for programming a Sphero RVR from a Raspberry Pi

This repository holds the C source for a library that supports programing a Sphero RVR from a Raspoberry Pi.  There are two directories:
* src: This is the actual source code for the library.  It can be compiled with using the Makefile.  The Makefile builds "librvr.a", which can be used in the C builds.
* tests: This holds a number of tests for the C library.  There's a Makefile in there as well.

There is no documentation, sorry!  I'll work on that.

The proper way to start a code sequence:

    init_port();
    set_logging_level(NOLOGGING);  //or VERBOSE or VERYVERBOSE
    
    wake();
    
The code should refuse to start without "init_port()" being called....

A note should be made about sensors and notifications.  "init_port()" starts a thread that fields all the input from the RVR.  Sensor data and notifications are registered and accessible via "get_..." calls.  See sensor.h for a list of calls and sensortest files for examples.
