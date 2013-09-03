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

