/**********************************************************************
    Nexcom Co., Ltd.
    Filename         : CSU_Adc.c
    Description      : ADC Application Logic (100ms Periodic Task)
    Last Updated     : 2026. 02. 12.
**********************************************************************/

/* ************************** [[   include  ]]  *********************************************************** */
#include "CSU_Adc.h"

/* ************************** [[   global   ]]  *********************************************************** */
stAdcApp xAdcApp;

/* ************************** [[  function  ]]  *********************************************************** */

/**
 * @brief ADC 애플리케이션 초기화
 */
void Initial_CSU_Adc(void)
{
    xAdcApp.PotenRaw = 0.0f;
    xAdcApp.PotenMAVE = 0.0f;
}

/**
 * @brief 100ms 주기로 실행되는 ADC 데이터 처리 루틴
 * @details 하드웨어 레지스터에서 직접 읽거나 DevAdc에서 처리된 데이터를 가공합니다.
 */
void updateAdcData(void)
{
    // 100ms 마다 현재 ADCINA2의 원본 전압 값을 업데이트
    // (DevAdc.c의 ISR에서도 업데이트될 수 있지만, CSU 레벨에서 100ms 주기로 읽기를 원함)
    xAdcApp.PotenRaw = (float32_t)AdcaResultRegs.ADCRESULT0 * CONV_ADC_3_3V;
    
    // 필요 시 100ms 주기에서의 추가 처리 로직을 여기에 작성
}
