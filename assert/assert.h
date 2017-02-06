/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
// Ryabinin (c) 2012-2015
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
#pragma once
#ifndef ASSERT_H_
#define ASSERT_H_
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

#include <types.h>

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

/// Более легкие ассерты
#define ASSERT_LITE							(TRUE)

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

#if defined(USE_FULL_ASSERT)  // debug на железе

	extern volatile const char* ErrorFunc;
	extern volatile u16 ErrorLine;
	extern volatile float ErrorVal;

#if (ASSERT_LITE == FALSE)
	extern void assert_param_f(const char* func, u32 line);
	#define assert_param(expr) 		((expr) ? (void)0 : assert_param_f(__FUNCTION__, __LINE__))
	#define trace_error(val)	{ ErrorFunc = __FUNCTION__; ErrorLine = __LINE__; ErrorVal = val; }
#else
	extern void assert_param_f2(u32 line);
	#define assert_param(expr) 		((expr) ? (void)0 : assert_param_f2(__LINE__))
	#define trace_error(val)	{ ErrorFunc = NULL; ErrorLine = __LINE__; ErrorVal = val; }
#endif

/// http://ideone.com/N8AFQc
#define ASSERT_REENTER()													\
	static uint8_t __reent_counter__;										\
	void __exit__(uint8_t * pvariable) { 									\
		(void) pvariable; 													\
		--__reent_counter__; 												\
		} 																	\
	uint8_t __reent_dummy__  __attribute__((cleanup (__exit__))); 			\
	++__reent_counter__; 													\
	assert_param(__reent_counter__ <= 1)

#elif defined(UNIT_TEST)  // debug на компе

	#include <stdio.h>
	#include <stdlib.h>
	#include <assert.h>

	#define assert_param(expr)				assert(expr)
	#define trace_error(val) \
	do{ \
		printf("err:%s/%s():%d?%f\n", __FILE__, __FUNCTION__, __LINE__, (double)val);\
	} while(0)

#define ASSERT_REENTER()													\
	static uint8_t __reent_counter__;										\
	void __exit__(uint8_t * pvariable) { 									\
		(void) pvariable; 													\
		--__reent_counter__; 												\
		} 																	\
	uint8_t __reent_dummy__  __attribute__((cleanup (__exit__))); 			\
	++__reent_counter__; 													\
	assert_param(__reent_counter__ <= 1)

#else // release
	#define assert_param(expr)			// убираем assert вообще, эти проверки много весят
	#define trace_error(val)		//Log_write(__FUNCTION__, __LINE__, (val))

#define ASSERT_REENTER()

#endif /* USE_FULL_ASSERT */

/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
#endif
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
