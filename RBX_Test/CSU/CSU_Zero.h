/**********************************************************************
    Nexcom Co., Ltd.
    Filename         : CSU_Zero.h
    Description      : Encoder Zero/Reset Sequence & Manual Control
    Last Updated     : 2026. 02. 02.
**********************************************************************/

#ifndef CSU_ZERO_H
#define CSU_ZERO_H

/* ************************** [[   include  ]]  *********************************************************** */
#include "main.h"


/* ************************** [[   define   ]]  *********************************************************** */
#define GPIO_ENC_ZERO_SET    24u
#define GPIO_ENC_ZERO_RST    25u
#define GPIO_ENC_PWR_EN      26u

/* ************************** [[   enum or struct   ]]  *************************************************** */
/* 제로 셋 시퀀스 상태 정의 */
typedef enum {
    eZERO_IDLE = 0,
    eZERO_PWR_OFF,      /* 전원 차단 및 신호 설정 */
    eZERO_WAIT_2SEC,    /* 전원 OFF 상태로 2초 대기 */
    eZERO_PWR_ON_WAIT,  /* 전원 투입 후 5초 유지 */
    eZERO_DONE_WAIT     /* 완료 비트 1초 유지 */
} eZeroStep;



/* ************************** [[   global   ]]  *********************************************************** */



/* ************************** [[  function  ]]  *********************************************************** */

/* 외부 호출 함수 */
void initGPIODoutZero(void);
void initEncoderZero(void);
void procEncoderZero(void);
void manualCtrlZero(void);


#endif	// #ifndef CSU_ZERO_H

