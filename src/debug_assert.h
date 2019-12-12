#ifndef  __DEBUG_ASSERT_H__
#define  __DEBUG_ASSERT_H__
 
#ifdef  __cplusplus
#define  DEBUG_ASSERT_BEGIN extern "C" {
#define  DEBUG_ASSERT_END   }
#else
#define  DEBUG_ASSERT_BEGIN
#define  DEBUG_ASSERT_END
#endif
 
DEBUG_ASSERT_BEGIN

#define  DEBUG_ASSERT_ENABLE  1

#if defined(DEBUG_ASSERT_ENABLE) && DEBUG_ASSERT_ENABLE > 0
#define  DEBUG_ASSERT(x)                      \
do                                            \
{                                             \
    if ((void *)(x) == (void *)0) {           \
        while (1);                            \
    }                                         \
} while (0)
#else
#define  DEBUG_ASSERT(x)
#endif

DEBUG_ASSERT_END
#endif
