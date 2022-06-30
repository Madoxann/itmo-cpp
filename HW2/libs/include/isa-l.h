
/**
 *  @file isa-l.h
 *  @brief Include for ISA-L library
 */

#ifndef _ISAL_H_
#define _ISAL_H_

#define ISAL_MAJOR_VERSION 2
#define ISAL_MINOR_VERSION 30
#define ISAL_PATCH_VERSION 0
#define ISAL_MAKE_VERSION(maj, min, patch)  ((maj) * 0x10000 + (min) * 0x100 + (patch))
#define ISAL_VERSION ISAL_MAKE_VERSION(ISAL_MAJOR_VERSION, ISAL_MINOR_VERSION, ISAL_PATCH_VERSION)

#include <crc.h>
#include <crc64.h>
#include <erasure_code.h>
#include <gf_vect_mul.h>
#include <igzip_lib.h>
#include <mem_routines.h>
#include <raid.h>
#endif //_ISAL_H_
