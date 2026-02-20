/**********************************************************************
    Nexcom Co., Ltd.
    Filename         : CSU_LED.h
    Description      : System Status LED Control (Green / Orange)
    Last Updated     : 2026. 01. 30.
**********************************************************************/

#ifndef CSU_LED_H
#define CSU_LED_H

/* ************************** [[   include  ]]  *********************************************************** */
#include "main.h"



/* ************************** [[   define   ]]  *********************************************************** */
#define LED_OFF		false
#define LED_ON		true

#define LED_NONE	false
#define LED_TOGGLE	true

/* READSTATUS 상태 표시요 LED(GPIO 33)*/
#define READSTATUS           33u

/* 시퀀스 상태 표시용 LED (GPIO 34~36) */
#define GPIO_LED_STEP_1      34u // 
#define GPIO_LED_STEP_2      35u // 
#define GPIO_LED_STEP_3      36u // 


/* ************************** [[   enum or struct   ]]  **************************************************** */

/**
 * @brief LED 인덱스 정의 (GPIO 번호 매핑)
 */
typedef enum
{
	eLED_GREEN					= 31u,
	eLED_ORANGE					= 32u,
    eLED_READSTATUS             = 33u,
	eLED_PWRON					= 34u,
    eLED_SETGPIO                = 35u,
    eLED_RSTGPIO                = 36u,
    eLED_STEP1					= 37u,
	eLED_STEP2					= 38u,
	eLED_STEP3					= 39u

}eLed;

/**
 * @brief 개별 LED 제어 속성 구조체
 */
typedef struct
{
    uint16_t Index:8u;    // GPIO Index (eLed 타입 저장)
    uint16_t Time:8u;     // Toggle 주기 설정
    uint16_t Temp:8u;     // 카운트 다운용 임시 변수
    bool     State:1u;    // 현재 점등 상태 (false: Off, true: On)
    bool     Toggle:1u;   // 토글 모드 활성 (false: None, true: Toggle)
    uint16_t Reserved:14u;
} stLed;

/**
 * @brief 시스템 전체 LED 상태 관리 구조체
 */
typedef struct
{
	stLed	ledGreen;
	stLed	ledOrange;
    stLed   ledReadstatus;
    stLed	ledPwrOn;    
    stLed	ledSetGpio;
    stLed   ledRstGpio;
    stLed	ledStep1;
    stLed	ledStep2;
    stLed	ledStep3;

}stLedStatus;



/* ************************** [[   global   ]]  *********************************************************** */
extern stLedStatus xLed;



/* ************************** [[  function  ]]  *********************************************************** */

/**
 * @brief LED 제어를 위한 GPIO 방향 및 Mux 설정
 */
void initGpioDoutLed(void);

/**
 * @brief LED 변수 초기화 및 기본 동작 설정
 */
void Initial_LED(void);

/**
 * @brief LED 동작 상태 업데이트 (Main Loop 호출)
 */
void updateLedStatus(void);

/**
 * @brief LED의 On/Off 상태를 직접 설정 (토글 중단)

 */
void setLedStatus(stLed *pLed, bool State);

/**
 * @brief LED 토글 모드 활성화 및 주기 설정
 */
void setLedModeToggle(stLed *pLed, bool State, uint16_t Time);

/**
 * @brief IsValid 상태에 따라 Orange LED 제어
 */
void updateOrangeLed(void);


void updateSetRstGpioLed();


#endif	// #ifndef CSU_LED_H

