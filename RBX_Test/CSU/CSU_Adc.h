/**********************************************************************
    Nexcom Co., Ltd.
    Filename         : CSU_Adc.h
    Description      : ADC Application Logic (100ms Periodic Task)
    Last Updated     : 2026. 02. 12.
**********************************************************************/

#ifndef CSU_ADC_H
#define CSU_ADC_H

/* ************************** [[   include  ]]  *********************************************************** */
#include "main.h"

/* ************************** [[   define   ]]  *********************************************************** */


/* ************************** [[   enum or struct   ]]  *************************************************** */
typedef struct {
    float32_t PotenRaw;       /* ADCINA2 Raw 전압 */
    float32_t PotenMAVE;      /* ADCINA2 이동 평균 전압 */
} stAdcApp;


/* ************************** [[   global   ]]  *********************************************************** */
extern stAdcApp xAdcApp;


/* ************************** [[  function  ]]  *********************************************************** */
/**
 * @brief ADC 애플리케이션 초기화
 */
void Initial_CSU_Adc(void);

/**
 * @brief 100ms 주기로 실행되는 ADC 데이터 처리 루틴
 */
void updateAdcData(void);

#endif /* CSU_ADC_H */
