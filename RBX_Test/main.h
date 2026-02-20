/**********************************************************************
    Nexcom Co., Ltd.
    Filename         : main.h
    Description      : 
    Last Updated     : 2026. 01. 30.
**********************************************************************/

#ifndef MAIN_H
#define MAIN_H

/* ************************** [[   include  ]]  *********************************************************** */
/* 표준 라이브러리 */
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

/* Driverlib 및 Device 기본 정의 */
// _DUAL_HEADERS가 선언되어 있어야 두 방식을 병행 가능합니다.
#include "driverlib.h"
#include "device.h"

/* Bit-field 헤더 포함 */
// f28x_project.h 내부의 Uint16 등과 충돌을 피하기 위해 
// 프로젝트 전체에서 사용하는 타입을 여기서 정리합니다.
#include "f28x_project.h"

// // 이미 device.h에서 uint16_t로 선언된 것과 globalprototypes.h의 충돌을 막기 위함
// #define f2838x_globalprototypes_H

#include "DevCommon.h"
#include "DevDspInit.h"
#include "DevSci.h"
#include "DevSpi.h"
#include "DevTimer.h"
#include "DevAdc.h"

#include "CSU_IPC.h"
#include "CSU_LED.h"
#include "CSU_SSI.h"
#include "CSU_Zero.h"
#include "CSU_eQEP.h"
#include "CSU_Adc.h"


/* ************************** [[   define   ]]  *********************************************************** */
typedef unsigned char   Uint8; 



/* ************************** [[   enum or struct   ]]  *************************************************** */



/* ************************** [[   global   ]]  *********************************************************** */



/* ************************** [[  function  ]]  *********************************************************** */
// DSP program entry point
void main(void);


#endif	// #ifndef MAIN_H

