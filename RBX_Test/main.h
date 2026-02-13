/**********************************************************************
    Nexcom Co., Ltd.
    Filename         : main.h
    Description      : 
    Last Updated     : 2026. 01. 30.
**********************************************************************/

#ifndef MAIN_H
#define MAIN_H

/* ************************** [[   include  ]]  *********************************************************** */
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "f28x_project.h"           // TI 제공 칩-시리즈 전체 포함 헤더 파일 (bit-field 방식)
#include "driverlib.h"              // TI 제공 Driver API 라이브러리 헤더 파일 (driverlib 방식)
#include "device.h"

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
typedef unsigned char  Uint8;



/* ************************** [[   enum or struct   ]]  *************************************************** */



/* ************************** [[   global   ]]  *********************************************************** */



/* ************************** [[  function  ]]  *********************************************************** */
// DSP program entry point
void main(void);


#endif	// #ifndef MAIN_H

