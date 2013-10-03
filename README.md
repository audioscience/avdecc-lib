avdecc-lib
==========

Simple C++ library for implementing IEEE1722.1 (AVB Device Enumeration, Discovery and Control)


Introduction
------------

This library aims to simplify development of an AVDECC controller based on the IEEE1722.1 specification.
It provides a simple c++ object interface to 1722.1 objects and implements device discovery and enumeration
as a background process.

The repository contains source to build a Windows DLL or a Linux library and a command line application for
exercising the library.

The overall philosophy of AVDECC LIB is to implement a thin layer of commands that allow an application to
discover and and control AVDECC capable endpoints. The internal operations of the library are designed to be single threaded,
although multiple threads are used to queue operations to be performed by the single threaded "engine" portion of the library.
The library supports notification events (callbacks) that are triggered on the success (or failure) of a command. 
It is up to the application to process the notifications in a useful manner. Asynchronously control updates from an
end station are also supported. A control notification does not have data about the updated descriptor values embedded
in it. Instead the AVDECC application should query the control class to obtain the updated values.

Directory layout
---------------

 ::
		
	lib\
		bin\
		doc\
		binding\
			python\
		build\
			linux\
			msvc\
		include\ (contains public header files)
		src\ (contains private header files and c++ source code)
			linux\ (linux specific files)
			msvc\ (Microsoft Visual Studio specific files
			
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

Object hierarchy
---------------
 
  ::

	System
	Controller
		End Station[1..N]
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

* Windows *

Prerequisites

1. MSVC 2010 or later
1. jdksavdecc-c git repository from <https://github.com/jdkoftinoff/jdksavdecc-c>
1. winpcap from <http://www.winpcap.org/>

The following environment variables must be defined:
* WPCAP_DIR the directory where WinPcap is installed
* JDKSAVDECC_DIR the directory where JDKSAVDECC-C git 1722.1 C library is installed

* Linux *

ToDo

Operations
==========

AVDECC End Station Discovery
----------------------------

When the AVDECC system receives a AVDECC advertise message from an end station, it proceeds to
enumerate the end station's complete object model, if it hasn't done so already. Upon completion
of the enumeration process, a notification message is sent to the application.

AVDECC AEM descriptor reads
---------------------------

A descriptor read by referencing the object the object of interest. Since the AVDECC system has
already read all descriptors, the read operation is completed without producing any network traffic.

To read the name of the first input jack, one would go::

    controller->end_station(0)->entity(0)->configuration(0)->input_stream(0)->get_name(name) 

AVDECC AEM commands
-------------------

An AVDECC command is sent to the target object, ie::

    istream = controller->end_station(0)->entity(0)->configuration(0)->input_stream(0);
    istream->set_format(unsigned int *id,format,...);

Completion results in a notification message of success or failure via the callback mechanism.

Callbacks
---------

The following callback functions should be supplied.

 ::
 
	void log_callback(void *user_obj, int32_t log_level, const char *log_msg, int32_t time_stamp_ms);
	void notification_callback(void *user_obj, int32_t notification, uint64_t guid, uint16_t cmd_type, uint16_t desc_type, uint16_t desc_index, void *notification_id);

With logging levels of:
* ERROR
* WARNING
* NOTICE
* INFO
* DEBUG
* VERBOSE
	
With notifications of:
* NO MATCH FOUND
* END STATION DISCOVERED
* END STATION CONNECTED
* END STATION DISCONNECTED,
* COMMAND SENT
* COMMAND TIMEOUT
* COMMAND RESENT
* COMMAND SUCCESS
* RESPONSE RECEIVED

Source code style
-----------------

Use astyle_code_format file.			
