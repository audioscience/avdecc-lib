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

The overall philosophy of AVDECC LIB is to implement a thin layer of commands that allow an application to
discover and and control AVDECC capable endpoints. The internal operations of the library are designed to be single threaded,
although multiple threads are used to queue operations to be performed by the single threaded "engine" portion of the library.
The library supports notification events (callbacks) that are triggered on the success (or failure) of a command. 
It is up to the application to process the notifications in a useful manner. Asynchronously control updates from an
endpoint are also supported. A control notification does not have data about the updated descriptor values embedded
in it. Instead the AVDECC application should query the control class to obtain the updated values.

Directory layout
---------------

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
      linux\ (linux specific files)
		msvc\ (Microsoft Visual Studio specific files)
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
----------------

    AVDECC System
      Endpoint[1..N]
        Entity[1..N]
            Configuration[1..N]
                Audio Unit[0..N]
                Stream Input[0..N]
                Stream Output[0..N]
                AVB Interface[0..N]
                Clock Source[0..N]
                Jack Input[0..N]
                Jack Output[0..N]
                Clock Domain[0..N]

**association_id**

If ASSOCIATION_ID_SUPPORTED and ASSOCIATION_ID_VALID flags are set in the ADP entity_capabilities field, then multiple entites will be represented as a single logical entity in the hierarchy.
*AL: How closely do we want to stick to 1722.1 terminology here e.g. The term for endpoint in the standard is 'end station'. Should this be reflected here?*
*AGE: will change to use endstation terminology*

Building
--------

**Windows**

Prerequisites

1. MSVC 2010 or later
1. jdksavdecc-c git repository from <https://github.com/jdkoftinoff/jdksavdecc-c>
1. winpcap from <http://www.winpcap.org/>

The following environment variables must be defined:
* WINPCAP_DIR the directory where WinPcap is installed
* JDKAVDECC_DIR the directory where JDKSAVDECC-C git 1722.1 C library is installed

**Linux**

ToDo

Operations
==========

AVDECC Endpoint Discovery
-------------------------

When the AVDECC system receives a AVDECC advertise message from an endpoint, it proceeds to
enumerate the endpoint's complete object model if it hasn't done so already and the following hold true:

* The AEM_SUPPORTED flag in the ADP entity_capabilities field is set 
* The GENERAL_CONTROLLER_IGNORE flag in the ADP entity_capabilities field is not set
* The ENTITY_NOT_READY flag in the ADP entity_capabilities field is not set

When an end station is discovered, a notification message is sent to the application. Additionally,
upon completion of the enumeration process, a second notification message is sent to the application.

*AL: Is there a case for separating the discovery and enumeration notifications? The user may want to connect entities that do not support AEM*
*AGE: changed - see above*

AVDECC descriptor reads
-----------------------

A descriptor read by referencing the object the object of interest. Since the AVDECC system has
already read all descriptors, the read operation is completed without producing any network traffic.

To read the name of the first input jack, one would go::

    avdeccsys->endpoint(0)->entity(0)->configuration(0)->stream_input(0)->get_name(name) 
    
*AL: This example might be a bit confusing, as there is also a GET_NAME command.
I'd expect the above to issue an AECP GET_NAME command rather than just read the string from the descriptor.
I presume the fields in the descriptor could just be accessed like:*

*AGE: agreed - I'll change it*

    avdeccsys->endpoint(0)->entity(0)->configuration(0)->stream_input(0)->object_name
    
*However, should the following return the actual contents of the descriptor, or return the correct string from the STRING descriptor?*

    avdeccsys->endpoint(0)->entity(0)->configuration(0)->stream_input(0)->localized_description
    
A thought:

    avdeccsys->get_entity(0x0022970102030000)->configuration(0)->stream_input(0)->localized_description


*AGE we are having internal discussions about "flattening" the API.*
*The controller would have a method something like*
*root->in_stream_set_format(end station GUID, entity index, config index, in_stream_index, parameters)*


AVDECC commands
---------------

An AVDECC command is sent to the target object, ie

    istream = avdeccsys->endpoint(0)->entity(0)->configuration(0)->stream_input(0);
    if (istream) {
    	istream->set_format(unsigned int *id,format,...);
    }

Completion results in a notification message of success or failure via the callback mechanism.

**Relative Implementation Priorities**

Command/Response | Priority
-----------------|---------
ACQUIRE_ENTITY | P1
LOCK_ENTITY | P1
ENTITY_AVAILABLE | P1
CONTROLLER_AVAILABLE | P1
READ_DESCRIPTOR | P1
WRITE_DESCRIPTOR | P3
SET_CONFIGURATION | P2
GET_CONFIGURATION | P2
SET_STREAM_FORMAT | P1
GET_STREAM_FORMAT | P1
SET_VIDEO_FORMAT | P4
GET_VIDEO_FORMAT | P4
SET_SENSOR_FORMAT | P4
GET_SENSOR_FORMAT | P4
SET_STREAM_INFO | P1
GET_STREAM_INFO | P1
SET_NAME | P2
GET_NAME | P2
SET_ASSOCIATION_ID | P3
GET_ASSOCIATION_ID | P3
SET_SAMPLING_RATE | P1
GET_SAMPLING_RATE | P1
SET_CLOCK_SOURCE | P1
GET_CLOCK_SOURCE | P1
SET_CONTROL | P2
GET_CONTROL | P2
INCREMENT_CONTROL | P3
DECREMENT_CONTROL | P3
SET_SIGNAL_SELECTOR | P3
GET_SIGNAL_SELECTOR | P3
SET_MIXER | P2
GET_MIXER | P2
SET_MATRIX | P3
GET_MATRIX | P3
START_STREAMING | P1
STOP_STREAMING | P1
REGISTER_UNSOLICITED_NOTIFICATION | P2
DEREGISTER_UNSOLICITED_NOTIFICATION | P2
IDENTIFY_NOTIFICATION | P2
GET_AVB_INFO | P2
GET_AS_PATH | P2
GET_COUNTERS | P3
REBOOT | P2
GET_AUDIO_MAP | P3
ADD_AUDIO_MAPPINGS | P3
REMOVE_AUDIO_MAPPINGS | P3
GET_VIDEO_MAP | P4
ADD_VIDEO_MAPPINGS | P4
REMOVE_VIDEO_MAPPINGS | P4
GET_SENSOR_MAP | P4
ADD_SENSOR_MAPPINGS | P4
REMOVE_SENSOR_MAPPINGS | P4
START_OPERATION | P3
ABORT_OPERATION | P3
OPERATION_STATUS | P3

Callbacks
---------

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


