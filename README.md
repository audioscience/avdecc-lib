avdecc-lib
==========

Simple C++ library for implementing IEEE1722.1 (AVB Device Enumeration, Discovery and Control)


Introduction
------------

This library aims to simplify development of an AVDECC controller based on the IEEE1722.1 specification.
It provides a simple c++ object interface to 1722.1 objects and implements device discovery and enumeration
as a background procoess.

The repository contains source to build a Windows DLL or a Linux library and a command line application for
exercising the library.

The overall philosophy of AVDECC LIB is to implement a simple set of commands that allow an application to
discover and and control AVDECC capable endpoints. The internals of the library are designed to be single threaded.
The library supports notification events (callbacks) that are triggered on the success (or failure) of a command. 
It is up to the application to process the notifications in a useful manner. Asynchronously control updates from an
endpoint are also supported. A control notification does not have data about the updated descriptor values embedded
in it. Instead the AVDECC application should query the control class to obtain the updated values.

Directory layout
----------------

::

 lib\
   bin\
   doc\
   binding\
     python\
   build\
     linux\
     msvc\
     binding\
   include\ (contains public header files)
   src\ (contains private header files and c++ source code)
 app\
   bin\
   doc\
   build\
     linux\
     msvc\
   cmdline\
     src\
   test\
     strings\
     adp\
     logging\ 
     notify\

  
Object heirachy
---------------

::

AVDECC Sytem
    Endpoint[1..N]
        Entity[1..N]
            Configuration[1..N]
                Audio Unit[1..N]
                Stream Input[1..N]
                Stream Output[1..N]
                AVB Interface[1..N]
                Clock Source[1..N]
                Jack Input[1..N]
                Jack Output[1..N]
                Clock Domain[1..N]


Building
--------

For a Windows build the following environment variables must be defined:

* WPCAP_DIR the directory where Win PCAP is installed
* 
* JDKAVDECC_DIR the directory where JeffK?'s 1722.1 C library is installed 

Operations
----------

AVDECC Endpoint Discovery
~~~~~~~~~~~~~~~~~~~~~~~~~

When the AVDECC system receives a AVDECC advertise message from an endpoint, it proceeds to
enumerate the endpoint's complete object model, if it hasn't done so already. Upon completion
of the enumeration process, a notification message is sent to the application.

AVDECC descriptor reads
~~~~~~~~~~~~~~~~~~~~~~~

A descriptor read by referencing the object the object of interest. Since the AVDECC system has
already read all descriptors, the read operation is completed without producing any network traffic.

To read the name of the first input jack, one would go::

 avdeccsys->endpoint(0)->entity(0)->configuration(0)->input_stream(0)->get_name(name) 

AVDECC commands
~~~~~~~~~~~~~~~

An AVDECC command is sent to the target object, ie::

 istream = avdeccsys->endpoint(0)->entity(0)->configuration(0)->input_stream(0); istream->set_format(unsigned int *id,format,...);

Completion results in a notification message of success or failure via the callback mechanism.

Callbacks
~~~~~~~~~

The following callback functions should be supplied.

void avdecc_log_fn(void *user_obj, int32_t log_level, char msg[256], uint32_t time_stamp_ms);

void avdecc_notify(void *user_obj, int32_t notification, int32_t endpoint_index, int32_t obj, int32_t index, int32_t cmd, int32_t id); 

With notifications of:
* No Match Found
* Endpoint Discovered
* Endpoint Connected
* Endpoint Disconnected
* Invalid Command
* Command Timeout
* Command Resent
* Command Success


