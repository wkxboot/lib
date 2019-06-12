#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "stdbool.h"
#include "utils.h"
#include "cmsis_os.h"


/*
* @brief 数组转成HEX字符串
* @param
* @param
* @return 
* @note
*/
 void dump_hex_str(const char *src,char *dest,uint16_t src_len)
 {
    char temp;
    char hex_digital[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    for (uint16_t i = 0; i < src_len; i++){  
        temp = src[i];  
        dest[2 * i] = hex_digital[(temp >> 4) & 0x0f ];  
        dest[2 * i + 1] = hex_digital[temp & 0x0f];  
    }
    dest[src_len * 2] = '\0';
}
/**
  * @brief  Convert an Integer to a string
  * @param  p_str: The string output pointer
  * @param  intnum: The integer to be converted
  * @retval None
  */
void Int2Str(uint8_t *p_str, uint32_t intnum)
{
  uint32_t i, divider = 1000000000, pos = 0, status = 0;

  for (i = 0; i < 10; i++)
  {
    p_str[pos++] = (intnum / divider) + 48;

    intnum = intnum % divider;
    divider /= 10;
    if ((p_str[pos-1] == '0') & (status == 0))
    {
      pos = 0;
    }
    else
    {
      status++;
    }
  }
}

/**
  * @brief  Convert a string to an integer
  * @param  p_inputstr: The string to be converted
  * @param  p_intnum: The integer value
  * @retval 1: Correct
  *         0: Error
  */
uint32_t Str2Int(uint8_t *p_inputstr, uint32_t *p_intnum)
{
  uint32_t i = 0, res = 0;
  uint32_t val = 0;

  if ((p_inputstr[0] == '0') && ((p_inputstr[1] == 'x') || (p_inputstr[1] == 'X')))
  {
    i = 2;
    while ( ( i < 11 ) && ( p_inputstr[i] != '\0' ) )
    {
      if (ISVALIDHEX(p_inputstr[i]))
      {
        val = (val << 4) + CONVERTHEX(p_inputstr[i]);
      }
      else
      {
        /* Return 0, Invalid input */
        res = 0;
        break;
      }
      i++;
    }

    /* valid result */
    if (p_inputstr[i] == '\0')
    {
      *p_intnum = val;
      res = 1;
    }
  }
  else /* max 10-digit decimal input */
  {
    while ( ( i < 11 ) && ( res != 1 ) )
    {
      if (p_inputstr[i] == '\0')
      {
        *p_intnum = val;
        /* return 1 */
        res = 1;
      }
      else if (((p_inputstr[i] == 'k') || (p_inputstr[i] == 'K')) && (i > 0))
      {
        val = val << 10;
        *p_intnum = val;
        res = 1;
      }
      else if (((p_inputstr[i] == 'm') || (p_inputstr[i] == 'M')) && (i > 0))
      {
        val = val << 20;
        *p_intnum = val;
        res = 1;
      }
      else if (ISVALIDDEC(p_inputstr[i]))
      {
        val = val * 10 + CONVERTDEC(p_inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
      i++;
    }
  }

  return res;
}

/* 函数：utils_timer_init
*  功能：自定义定时器初始化
*  参数：timer 定时器指针
*  参数：timeout 超时时间
*  参数：up 定时器方向-向上计数-向下计数
*  返回: 0：成功 其他：失败
*/ 
int utils_timer_init(utils_timer_t *timer,uint32_t timeout,bool up)
{
    if(timer == NULL){
        return -1;
    }

    timer->up = up;
    timer->start = osKernelSysTick();  
    timer->value = timeout;  

    return 0;
}

/* 函数：utils_timer_value
*  功能：定时器现在的值
*  返回：>=0：现在时间值 其他：失败
*/ 
uint32_t utils_timer_value(utils_timer_t *timer)
{
    uint32_t time_elapse;

    if (timer == NULL){
        return 0;
    }  
    time_elapse = osKernelSysTick() - timer->start; 

    if (timer->up == true) {
        return  timer->value > time_elapse ? time_elapse : timer->value;
    }

    return  timer->value > time_elapse ? timer->value - time_elapse : 0; 
}
/*
* @brief 判断是否是数字字符
* @param s字符串
* @param 
* @return 浮点数
* @note
*/
static uint8_t is_digit(char c)
{
    if (c >= '0' && c <= '9') {
        return 1;
    }

    return 0;
}
/*
* @brief 字符串转浮点数
* @param s字符串
* @param 
* @return 浮点数
* @note
*/
double utils_atof(char *s)
{ 
    double tf;
    int nc,i,neg;

    tf = 0.0;
    if (s[0] == '-') { 
        neg = 1;
    } else { 
        neg = 0;
    }

    /* search for decimal point */
    for (nc=neg;( is_digit(s[nc]) && (s[nc] != '.') && (s[nc] != '\0')); nc++) ;
    for (i=neg; ( (is_digit(s[i]) || (s[i] == '.')) && (s[i] != '\0') ) ;i++) {   
    switch((nc-i))
    {
      case 0:break; /* we're at the decimal point */
      case 1:tf += (s[i] - '0');  break; /* one's place */
      case 2:tf += (s[i] - '0') * 10; break;
      case 3:tf += (s[i] - '0') * 100; break;
      case 4:tf += (s[i] - '0') * 1000; break;
      case 5:tf += (s[i] - '0') * 10000; break;
      case 6:tf += (s[i] - '0') * 100000; break;
      case 7:tf += (s[i] - '0') * 1000000; break;
      case 8:tf += (s[i] - '0') * 10000000; break;
      case 9:tf += (s[i] - '0') * 100000000.0; break;
      case 10:tf += (s[i] - '0') * 1000000000.0; break;
      case 11:tf += (s[i] - '0') * 10000000000.0;  break; 
      case 12:tf += (s[i] - '0') * 100000000000.0; break;
      case 13:tf += (s[i] - '0') * 1000000000000.0; break;
      case 14:tf += (s[i] - '0') * 10000000000000.0; break;
      case 15:tf += (s[i] - '0') * 100000000000000.0; break;
      case 16:tf += (s[i] - '0') * 1000000000000000.0; break;
      case 17:tf += (s[i] - '0') * 10000000000000000.0; break;
      case 18:tf += (s[i] - '0') * 100000000000000000.0; break;
      case 19:tf += (s[i] - '0') * 1000000000000000000.0; break;
      case -1:tf += (s[i] - '0') * 0.1; break;
      case -2:tf += (s[i] - '0') * 0.01; break;
      case -3:tf += (s[i] - '0') * 0.001; break;
      case -4:tf += (s[i] - '0') * 0.0001; break;
      case -5:tf += (s[i] - '0') * 0.00001; break;
      case -6:tf += (s[i] - '0') * 0.000001; break;
      case -7:tf += (s[i] - '0') * 0.0000001; break;
      case -8:tf += (s[i] - '0') * 0.00000001; break;
      case -9:tf += (s[i] - '0') * 0.000000001; break;
      case -10:tf += (s[i] - '0') * 0.0000000001; break;
      case -11:tf += (s[i] - '0') * 0.00000000001; break;
      case -12:tf += (s[i] - '0') * 0.000000000001; break;
      case -13:tf += (s[i] - '0') * 0.0000000000001; break;
      case -14:tf += (s[i] - '0') * 0.00000000000001; break;
      case -15:tf += (s[i] - '0') * 0.000000000000001; break;
      case -16:tf += (s[i] - '0') * 0.0000000000000001; break;
      case -17:tf += (s[i] - '0') * 0.00000000000000001; break;
      case -18:tf += (s[i] - '0') * 0.000000000000000001; break;
      case -19:tf += (s[i] - '0') * 0.0000000000000000001; break;
      default : break;
    }
  }

    if (neg) {
        tf *= -1.0;
    }

    return(tf);
}

