/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2013 Gordon Williams <gw@pur3.co.uk>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * Misc utils and cheapskate stdlib implementation
 * ----------------------------------------------------------------------------
 */
#ifndef JSUTILS_H_
#define JSUTILS_H_

#include "platform_config.h"

#include <stddef.h>
#ifndef FAKE_STDLIB
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#endif
#include <stdarg.h> // for va_args
#include <stdint.h>
#include <stdbool.h>

#if defined(LINUX) || defined(ARDUINO_AVR)
#include <math.h>
#else
// these are in maths, but are used all over the place
extern int isnan ( double );
extern int isfinite ( double );
#define NAN (((JsVarFloat)0)/(JsVarFloat)0)
#define INFINITY (((JsVarFloat)1)/(JsVarFloat)0)
#endif



#ifndef BUILDNUMBER
#define JS_VERSION "1v81"
#else
#define JS_VERSION "1v81." BUILDNUMBER
#endif
/*
  In code:
  TODO - should be fixed
  FIXME - will probably break if used
  OPT - potential for speed optimisation
*/

#if defined(ARM) || defined(AVR)
#define alloca(x) __builtin_alloca(x)
#endif

#if !defined(__USB_TYPE_H) && !defined(CPLUSPLUS) && !defined(__cplusplus) // it is defined in this file too!
#undef FALSE
#undef TRUE
//typedef enum {FALSE = 0, TRUE = !FALSE} bool;
#define FALSE 0
#define TRUE 1
//typedef unsigned char bool;
//#define TRUE (1)
//#define FALSE (0)
#endif

// Not needed because including stdbool.h instead.
/*#ifndef Arduino_h
#define true (1)
#define false (0)
#endif*/

#define DBL_MIN 2.2250738585072014e-308
#define DBL_MAX 1.7976931348623157e+308

/* Number of Js Variables allowed and Js Reference format. 

   JsVarRef = uint8_t -> 15 bytes/JsVar   so JSVAR_CACHE_SIZE = (RAM - 3000) / 15
   JsVarRef = uint16_t -> 20 bytes/JsVar   so JSVAR_CACHE_SIZE = (RAM - 3000) / 20
   JsVarRef = uint32_t -> 26 bytes/JsVar   so JSVAR_CACHE_SIZE = (RAM - 3000) / 26

   NOTE: JSVAR_CACHE_SIZE must be at least 2 less than the number we can fit in JsVarRef 
         See jshardware.c FLASH constants - all this must be able to fit in flash


*/

#ifdef RESIZABLE_JSVARS
 //  probably linux - allow us to allocate more blocks of variables
  typedef uint32_t JsVarRef;
  typedef int32_t JsVarRefSigned;
  #define JSVARREF_MIN (-2147483648)
  #define JSVARREF_MAX 2147483647
  #define JSVARREF_SIZE 4
#else
   /** JsVerRef stores References for variables - We treat 0 as null
   *  NOTE: we store JSVAR_DATA_STRING_* as actual values so we can do #if on them below
   *
   */
  #if JSVAR_CACHE_SIZE <= 254
    typedef uint8_t JsVarRef;
    typedef int8_t JsVarRefSigned;
    #define JSVARREF_MIN (-128)
    #define JSVARREF_MAX 127
    #define JSVARREF_SIZE 1
  #elif JSVAR_CACHE_SIZE <= 1023
    /* for this, we use 10 bit refs. GCC can't do that so store refs as
     * single bytes and then manually pack an extra 2 bits for each of
     * the 4 refs into a byte called 'pack'
     *
     * Note that JsVarRef/JsVarRefSigned are still 2 bytes, which means
     * we're only messing around when loading/storing refs - not when
     * passing them around.
     */
    #define JSVARREF_PACKED_BITS 2
    typedef uint16_t JsVarRef;
    typedef int16_t JsVarRefSigned;
    #define JSVARREF_MIN (-512)
    #define JSVARREF_MAX 511
    #define JSVARREF_SIZE 1
  #else
    typedef uint16_t JsVarRef;
    typedef int16_t JsVarRefSigned;
    #define JSVARREF_MIN (-32768)
    #define JSVARREF_MAX 32767
    #define JSVARREF_SIZE 2
  #endif
#endif

#if defined(__WORDSIZE) && __WORDSIZE == 64
// 64 bit needs extra space to be able to store a function pointer
#define JSVAR_DATA_STRING_LEN  8
#else
#define JSVAR_DATA_STRING_LEN  4
#endif
#define JSVAR_DATA_STRING_MAX_LEN (JSVAR_DATA_STRING_LEN+(3*JSVARREF_SIZE)+JSVARREF_SIZE) // (JSVAR_DATA_STRING_LEN + sizeof(JsVarRef)*3 + sizeof(JsVarRefCounter))

typedef int32_t JsVarInt;
typedef uint32_t JsVarIntUnsigned;
#ifdef USE_FLOATS
typedef float JsVarFloat;
#else
typedef double JsVarFloat;
#endif

#define JSSYSTIME_MAX 0x7FFFFFFFFFFFFFFFLL
typedef int64_t JsSysTime;
#define JSSYSTIME_INVALID ((JsSysTime)-1)

#define JSLEX_MAX_TOKEN_LENGTH  64
#define JS_ERROR_BUF_SIZE 64 // size of buffer error messages are written into
#define JS_ERROR_TOKEN_BUF_SIZE 16 // see jslTokenAsString

#define JS_NUMBER_BUFFER_SIZE 66 // 64 bit base 2 + minus + terminating 0

#define JSPARSE_MAX_SCOPES  8
// Don't restrict number of iterations now
//#define JSPARSE_MAX_LOOP_ITERATIONS 8192

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)
#define NOT_USED(x) ( (void)(x) )

// javascript specific names
#define JSPARSE_RETURN_VAR "return" // variable name used for returning function results
#define JSPARSE_PROTOTYPE_VAR "prototype"
#define JSPARSE_CONSTRUCTOR_VAR "constructor"
#define JSPARSE_INHERITS_VAR "__proto__"
// internal names that hopefully nobody will be able to access
#define JS_HIDDEN_CHAR '\xFF' // initial character of var name determines that we shouldn't see this stuff
#define JS_HIDDEN_CHAR_STR "\xFF"
#define JSPARSE_FUNCTION_CODE_NAME JS_HIDDEN_CHAR_STR"cod" // the function's code!
#define JSPARSE_FUNCTION_SCOPE_NAME JS_HIDDEN_CHAR_STR"sco" // the scope of the function's definition
#define JSPARSE_FUNCTION_THIS_NAME JS_HIDDEN_CHAR_STR"ths" // the 'this' variable - for bound functions
#define JSPARSE_FUNCTION_NAME_NAME JS_HIDDEN_CHAR_STR"nam" // for named functions (a = function foo() { foo(); })
#define JSPARSE_FUNCTION_LINENUMBER_NAME JS_HIDDEN_CHAR_STR"lin" // The line number offset of the function
#define JS_EVENT_PREFIX "#on"

#define JSPARSE_EXCEPTION_VAR "except" // when exceptions are thrown, they're stored in the root scope
#define JSPARSE_STACKTRACE_VAR "sTrace" // for errors/exceptions, a stack trace is stored as a string
#define JSPARSE_MODULE_CACHE_NAME "modules"

#if !defined(NO_ASSERT)
 #ifdef __STRING
   #define assert(X) if (!(X)) jsAssertFail(__FILE__,__LINE__,__STRING(X));
 #else
   #define assert(X) if (!(X)) jsAssertFail(__FILE__,__LINE__,"");
 #endif
#else
 #define assert(X) {}
#endif

/// Used when we have enums we want to squash down
#define PACKED_FLAGS  __attribute__ ((__packed__))  

/// Used before functions that we want to ensure are not inlined (eg. "void NO_INLINE foo() {}")
#define NO_INLINE __attribute__ ((noinline))

/// Put before functions that we always want inlined
#if defined(LINK_TIME_OPTIMISATION) && !defined(SAVE_ON_FLASH) && !defined(DEBUG)
#define ALWAYS_INLINE inline __attribute__((always_inline))
#elif defined(__GNUC__)
// By inlining in GCC we avoid shedloads of warnings
#define ALWAYS_INLINE inline
#else
// clang seems to hate being asked to inline when the definition
// isn't in the same file
#define ALWAYS_INLINE
#endif

/// Maximum amount of locks we ever expect to have on a variable (this could limit recursion) must be 2^n-1
#define JSV_LOCK_MAX  15

/// preprocessor power of 2 - suitable up to 16 bits
#define NEXT_POWER_2(X) \
   (((X) | (X)>>1 | (X)>>2 | (X)>>3 | \
    (X)>>4 | (X)>>5 | (X)>>6 | (X)>>7 | \
    (X)>>8 | (X)>>9 | (X)>>10 | (X)>>11 | \
    (X)>>12 | (X)>>13 | (X)>>14 | (X)>>15)+1)
/// Proprocessor get bit number
#define GET_BIT_NUMBER(X) \
  (((X)==    1)? 0: \
   ((X)==    2)? 1: \
   ((X)==    4)? 2: \
   ((X)==    8)? 3: \
   ((X)==   16)? 4: \
   ((X)==   32)? 5: \
   ((X)==   64)? 6: \
   ((X)==  128)? 7: \
   ((X)==  256)? 8: \
   ((X)==  512)? 9: \
   ((X)== 1024)?10: \
   ((X)== 2048)?11: \
   ((X)== 4096)?12: \
   ((X)== 8192)?13: \
   ((X)==16384)?14: \
   ((X)==32768)?15:10000/*error*/)

// To handle variable size bit fields
#define BITFIELD_DECL(BITFIELD, N) uint32_t BITFIELD[(N+31)/32]
#define BITFIELD_GET(BITFIELD, N) ((BITFIELD[(N)>>5] >> ((N)&31))&1)
#define BITFIELD_SET(BITFIELD, N, VALUE) (BITFIELD[(N)>>5] = (BITFIELD[(N)>>5]& (uint32_t)~(1 << ((N)&31))) | (uint32_t)((VALUE)?(1 << ((N)&31)):0)  )


static inline bool isWhitespace(char ch) {
    return (ch==0x09) || // \t - tab
           (ch==0x0B) || // vertical tab
           (ch==0x0C) || // form feed
           (ch==0x20) || // space
           (((unsigned char)ch)==0xA0) || // no break space
           (ch=='\n') ||
           (ch=='\r');
}

static inline bool isNumeric(char ch) {
    return (ch>='0') && (ch<='9');
}

static inline bool isHexadecimal(char ch) {
    return ((ch>='0') && (ch<='9')) ||
           ((ch>='a') && (ch<='f')) ||
           ((ch>='A') && (ch<='F'));
}
static inline bool isAlpha(char ch) {
    return ((ch>='a') && (ch<='z')) || ((ch>='A') && (ch<='Z')) || ch=='_';
}


bool isIDString(const char *s);

/** escape a character - if it is required. This may return a reference to a static array, 
so you can't store the value it returns in a variable and call it again. */
const char *escapeCharacter(char ch);
/// Convert a character to the hexadecimal equivalent (or -1)
int chtod(char ch);
/* convert a number in the given radix to an int. if radix=0, autodetect */
long long stringToIntWithRadix(const char *s, int radix, bool *hasError);
/* convert hex, binary, octal or decimal string into an int */
long long stringToInt(const char *s);

// forward decl
struct JsLex;
// ------------
typedef enum {
  JSET_STRING,
  JSET_ERROR,
  JSET_SYNTAXERROR,
  JSET_TYPEERROR,
  JSET_INTERNALERROR,
  JSET_REFERENCEERROR
} JsExceptionType;

void jsError(const char *fmt, ...);
void jsExceptionHere(JsExceptionType type, const char *fmt, ...);
void jsWarn(const char *fmt, ...);
void jsWarnAt(const char *message, struct JsLex *lex, size_t tokenPos);
void jsAssertFail(const char *file, int line, const char *expr);

// ------------
typedef enum {
  JSERR_NONE = 0,
  JSERR_RX_FIFO_FULL = 1, ///< The IO buffer (ioBuffer in jsdevices.c) is full and data was lost. Happens for character data and watch events
  JSERR_BUFFER_FULL = 2, ///< eg. Serial1's buffer exceeded the max size. Doesn't happen if you have an on('data') callback
  JSERR_CALLBACK = 4, ///< A callback (on data/watch/timer) caused an error and was removed
  JSERR_LOW_MEMORY = 8, ///< Memory is running low - Espruino had to run a garbage collection pass or remove some of the command history
  JSERR_MEMORY = 16, ///< Espruino ran out of memory and was unable to allocate some data that it needed.
} PACKED_FLAGS JsErrorFlags;

/** Error flags for things that we don't really want to report on the console,
 * but which are good to know about */
extern JsErrorFlags jsErrorFlags;


#ifdef FAKE_STDLIB
char *strncat(char *dst, const char *src, size_t c);
char *strncpy(char *dst, const char *src, size_t c);
size_t strlen(const char *s);
int strcmp(const char *a, const char *b);
void *memcpy(void *dst, const void *src, size_t size);
void *memset(void *dst, int c, size_t size);
#define RAND_MAX (0x7FFFFFFFU) // needs to be unsigned!
int rand();
void srand(unsigned int seed);
#endif

JsVarFloat stringToFloatWithRadix(const char *s, int forceRadix);
JsVarFloat stringToFloat(const char *str);

void itostr_extra(JsVarInt vals,char *str,bool signedVal,unsigned int base); // like itoa, but uses JsVarInt (good on non-32 bit systems)
static ALWAYS_INLINE void itostr(JsVarInt val,char *str,unsigned int base) {
    itostr_extra(val, str, true, base);
}

char itoch(int val);

// super ftoa that does fixed point and radix
void ftoa_bounded_extra(JsVarFloat val,char *str, size_t len, int radix, int fractionalDigits);
// normal ftoa with bounds checking
void ftoa_bounded(JsVarFloat val,char *str, size_t len);

/// Wrap a value so it is always between 0 and size (eg. wrapAround(angle, 360))
JsVarFloat wrapAround(JsVarFloat val, JsVarFloat size);


typedef void (*vcbprintf_callback)(const char *str, void *user_data);
/** Espruino-special printf with a callback
 * Supported are:
 *   %d = int
 *   %0#d = int padded to length # with 0s
 *   %x = int as hex
 *   %L = JsVarInt
 *   %Lx = JsVarInt as hex
 *   %f = JsVarFloat
 *   %s = string (char *)
 *   %c = char
 *   %v = JsVar * (doesn't have to be a string - it'll be converted)
 *   %q = JsVar * (in quotes, and escaped)
 *   %j = Variable printed as JSON
 *   %t = Type of variable
 *   %p = Pin
 *
 * Anything else will assert
 */
void vcbprintf(vcbprintf_callback user_callback, void *user_data, const char *fmt, va_list argp);

/// This one is directly usable..
void cbprintf(vcbprintf_callback user_callback, void *user_data, const char *fmt, ...);

/** get the amount of free stack we have, in bytes */
size_t jsuGetFreeStack();

#endif /* JSUTILS_H_ */
