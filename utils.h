#ifndef  __UTILS_H__
#define  __UTILS_H__
#include "stdbool.h"
#include "stdint.h"


typedef struct
{
    bool up;
    uint32_t start;
    uint32_t value;
}utils_timer_t;


#ifndef   IS_POWER_OF_TWO
#define   IS_POWER_OF_TWO(A)   (((A) != 0) && ((((A) - 1) & (A)) == 0))
#endif

#ifndef   MIN
#define   MIN(A,B)             ((A) > (B) ? (B) :(A))
#endif

#ifndef   MAX
#define   MAX(A,B)             ((A) > (B) ? (A) :(B))
#endif

#ifndef  UTILS_ASSERT
#define  UTILS_ASSERT(x)                            \
do {                                                \
    if ((void *)(x) == (void *)0) {                 \
        while(1);                                   \
    }                                               \
}while (0)                                         
#endif


#define IS_CAP_LETTER(c)    (((c) >= 'A') && ((c) <= 'F'))
#define IS_LC_LETTER(c)     (((c) >= 'a') && ((c) <= 'f'))
#define IS_09(c)            (((c) >= '0') && ((c) <= '9'))
#define ISVALIDHEX(c)       (IS_CAP_LETTER(c) || IS_LC_LETTER(c) || IS_09(c))
#define ISVALIDDEC(c)       IS_09(c)
#define CONVERTDEC(c)       (c - '0')

#define CONVERTHEX_ALPHA(c) (IS_CAP_LETTER(c) ? ((c) - 'A'+10) : ((c) - 'a'+10))
#define CONVERTHEX(c)       (IS_09(c) ? ((c) - '0') : CONVERTHEX_ALPHA(c))



/*字节转换成HEX字符串*/
 void dump_hex_str(const char *src,char *dest,uint16_t src_len);
   
/**
  * @brief  Convert an Integer to a string
  * @param  p_str: The string output pointer
  * @param  intnum: The integer to be converted
  * @retval None
  */
void Int2Str(uint8_t *p_str, uint32_t intnum);

/**
  * @brief  Convert a string to an integer
  * @param  p_inputstr: The string to be converted
  * @param  p_intnum: The integer value
  * @retval 1: Correct
  *         0: Error
  */
uint32_t Str2Int(uint8_t *p_inputstr, uint32_t *p_intnum);
   
/* 函数：utils_timer_init
*  功能：自定义定时器初始化
*  参数：timer 定时器指针
*  参数：timeout 超时时间
*  参数：up 定时器方向-向上计数-向下计数
*  返回: 0：成功 其他：失败
*/ 
int utils_timer_init(utils_timer_t *timer,uint32_t timeout,bool up);

/* 函数：utils_timer_value
*  功能：定时器现在的值
*  返回：>=0：现在时间值 其他：失败
*/ 
uint32_t utils_timer_value(utils_timer_t *timer);

/* 函数：utils_get_str_addr_by_num
*  功能：获取字符串中第num个str的地址
*  参数：buffer 字符串地址
*  参数：str    查找的字符串
*  参数：num    第num个str
*  参数：addr   第num个str的地址
*  返回：0：成功 其他：失败
*/ 
int utils_get_str_addr_by_num(char *buffer,const char *str,const uint8_t num,char **addr);

/* 函数：utils_get_str_value_by_num
*  功能：获取字符串中第num和第num+1个str之间的字符串
*  参数：buffer 字符串地址
*  参数：dst    存储地址
*  参数：str    字符串
*  参数：num    第num个字符串
*  返回：0：成功 其他：失败
*/
int utils_get_str_value_by_num(char *buffer,char *dst,const char *str,uint8_t num);


   
   
   
   
   
   
#endif