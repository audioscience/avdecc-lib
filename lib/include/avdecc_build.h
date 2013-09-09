/**
 * avdecc_build.h
 *
 * Header of the avdecc_build, which defines the API to be used.
 *
 * (c) AudioScience, Inc. 2013
 */

#pragma once
#ifndef _AVDECC_BUILD_H_
#define _AVDECC_BUILD_H_

#ifdef AVDECC_LIB_EXPORTS
#define AVDECC_LIB_API __declspec(dllexport)
#else
#define AVDECC_LIB_API __declspec(dllimport)
#endif

#endif