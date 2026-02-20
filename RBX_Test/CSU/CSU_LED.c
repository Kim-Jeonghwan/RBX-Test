/**********************************************************************
    Nexcom Co., Ltd.
    Filename         : CSU_LED.c
    Description      : System Status LED Control (Green / Orange)
    Last Updated     : 2026. 01. 30.
**********************************************************************/

/* ************************** [[   include  ]]  *********************************************************** */
#include "CSU_LED.h"
#include "CSU_IPC.h"  // For xXmtIpcMsg1 access


/* ************************** [[   define   ]]  *********************************************************** */



/* ************************** [[   global   ]]  *********************************************************** */
stLedStatus xLed;


/* ************************** [[  static prototype  ]]  *************************************************** */
static void HW_writeLedPin(uint16_t Index, bool State); 
static void HW_toggleLedPin(uint16_t Index);


/* ************************** [[  function  ]]  *********************************************************** */

/**
 * @brief LED 관련 GPIO 초기화
 */
void initGpioDoutLed(void)
{
	// LED GREEN (GPIO31)
	GPIO_SetupPinMux(eLED_GREEN, GPIO_MUX_CPU1, 0u);
	GPIO_SetupPinOptions(eLED_GREEN, GPIO_OUTPUT, GPIO_PUSHPULL);

	// LED ORANGE (GPIO32)
	GPIO_SetupPinMux(eLED_ORANGE, GPIO_MUX_CPU1, 0u);
	GPIO_SetupPinOptions(eLED_ORANGE, GPIO_OUTPUT, GPIO_PUSHPULL);

    // LED PWRON (GPIO33)
	GPIO_SetupPinMux(eLED_PWRON, GPIO_MUX_CPU1, 0u);
	GPIO_SetupPinOptions(eLED_PWRON, GPIO_OUTPUT, GPIO_PUSHPULL);
    
	// LED READSTATUS (GPIO34)
	GPIO_SetupPinMux(eLED_READSTATUS, GPIO_MUX_CPU1, 0u);
	GPIO_SetupPinOptions(eLED_READSTATUS, GPIO_OUTPUT, GPIO_PUSHPULL);

    // LED SETGPIO (GPIO35)
	GPIO_SetupPinMux(eLED_SETGPIO, GPIO_MUX_CPU1, 0u);
	GPIO_SetupPinOptions(eLED_SETGPIO, GPIO_OUTPUT, GPIO_PUSHPULL);
    
    // LED RSTGPIO (GPIO36)
	GPIO_SetupPinMux(eLED_RSTGPIO, GPIO_MUX_CPU1, 0u);
	GPIO_SetupPinOptions(eLED_RSTGPIO, GPIO_OUTPUT, GPIO_PUSHPULL);

    // LED STEP1 (GPIO37)
	GPIO_SetupPinMux(eLED_STEP1, GPIO_MUX_CPU1, 0u);
	GPIO_SetupPinOptions(eLED_STEP1, GPIO_OUTPUT, GPIO_PUSHPULL);

    // LED STEP2 (GPIO38)
	GPIO_SetupPinMux(eLED_STEP2, GPIO_MUX_CPU1, 0u);
	GPIO_SetupPinOptions(eLED_STEP2, GPIO_OUTPUT, GPIO_PUSHPULL);

    // LED STEP3 (GPIO39)
	GPIO_SetupPinMux(eLED_STEP3, GPIO_MUX_CPU1, 0u);
	GPIO_SetupPinOptions(eLED_STEP3, GPIO_OUTPUT, GPIO_PUSHPULL);


}

/**
 * @brief LED 초기값 설정
 * @details Green LED는 시스템 정상 동작 확인용으로 1초 토글 설정을 기본으로 합니다.
 */
void Initial_LED(void)
{
    // Green LED (GPIO 31) 설정
    xLed.ledGreen.Index  = eLED_GREEN;
    setLedModeToggle(&xLed.ledGreen, LED_TOGGLE, 10u); // 1초 주기 토글

    // Orange LED (GPIO 32) 설정
    xLed.ledOrange.Index = eLED_ORANGE;
    setLedModeToggle(&xLed.ledOrange, LED_NONE, 0u);   // 기본 꺼짐
    setLedStatus(&xLed.ledOrange, LED_OFF);

    // PwrOn LED (GPIO 33) 설정
    xLed.ledPwrOn.Index = eLED_PWRON;
    setLedModeToggle(&xLed.ledPwrOn, LED_NONE, 0u);   // 기본 꺼짐
    setLedStatus(&xLed.ledPwrOn, LED_OFF);

    // Readstatus LED (GPIO 34) 설정
    xLed.ledReadstatus.Index = eLED_READSTATUS;
    setLedModeToggle(&xLed.ledReadstatus, LED_NONE, 0u);   // 기본 꺼짐
    setLedStatus(&xLed.ledReadstatus, LED_OFF);
    
    // SetGpio LED (GPIO 35) 설정
    xLed.ledSetGpio.Index = eLED_SETGPIO;
    setLedModeToggle(&xLed.ledSetGpio, LED_NONE, 0u);   // 기본 꺼짐
    setLedStatus(&xLed.ledSetGpio, LED_OFF);
    
    // RstGpio LED (GPIO 36) 설정
    xLed.ledRstGpio.Index = eLED_RSTGPIO;
    setLedModeToggle(&xLed.ledRstGpio, LED_NONE, 0u);   // 기본 꺼짐
    setLedStatus(&xLed.ledRstGpio, LED_OFF);

    // Step1 LED (GPIO 37) 설정
    xLed.ledStep1.Index = eLED_STEP1;
    setLedModeToggle(&xLed.ledStep1, LED_NONE, 0u);   // 기본 꺼짐
    setLedStatus(&xLed.ledStep1, LED_OFF);

    // Step2 LED (GPIO 38) 설정
    xLed.ledStep2.Index = eLED_STEP2;
    setLedModeToggle(&xLed.ledStep2, LED_NONE, 0u);   // 기본 꺼짐
    setLedStatus(&xLed.ledStep2, LED_OFF);

    // Step3 LED (GPIO 39) 설정
    xLed.ledStep3.Index = eLED_STEP3;
    setLedModeToggle(&xLed.ledStep3, LED_NONE, 0u);   // 기본 꺼짐
    setLedStatus(&xLed.ledStep3, LED_OFF);


}

/**
 * @brief LED 상태 머신 업데이트
 * @details 100ms 주기로 호출되어 토글 카운트를 관리하거나 On/Off 상태를 유지합니다.
 */
void updateLedStatus(void)
{
    uint16_t i = 0u;
    stLed *pLed[2];
    
    pLed[0] = &xLed.ledGreen;
    pLed[1] = &xLed.ledOrange;

    for(i = 0u; i < 2u; i++)
    {
        if(pLed[i]->Toggle == LED_TOGGLE)
		{
            if(pLed[i]->Temp == 0u)
            {
                HW_toggleLedPin(pLed[i]->Index); // 레지스터 직접 제어
                pLed[i]->Temp = pLed[i]->Time;
            }
            else
            {
                pLed[i]->Temp--;
            }
        }
        else
        {
            HW_writeLedPin(pLed[i]->Index, pLed[i]->State); // 레지스터 직접 제어
        }
    }
}


/**
 * @brief LED 강제 점등/소등 설정
 * @param pLed 대상 LED 구조체 포인터
 * @param State LED_ON(1) 또는 LED_OFF(0)
 */
void setLedStatus(stLed *pLed, bool State)
{
    if(pLed->State != State)
    {
        pLed->State = State;
        pLed->Toggle = LED_NONE; 
        HW_writeLedPin(pLed->Index, State);
    }
}


/**
 * @brief LED 토글 동작 설정
 * @param State LED_TOGGLE(1) 또는 LED_NONE(0)
 * @param Time 토글 주기 카운트
 */
void setLedModeToggle(stLed *pLed, bool State, uint16_t Time)
{
    pLed->Toggle = State;
    pLed->Time   = Time;
    pLed->Temp   = 0u;
}



/**
 * @brief 하드웨어 GPADAT 레지스터 직접 제어 (Internal Only)
 */
static void HW_writeLedPin(uint16_t Index, bool State)
{
	switch(Index)
	{
	
	case eLED_GREEN:
		GpioDataRegs.GPADAT.bit.GPIO31 = State;
	break;

	case eLED_ORANGE:
		GpioDataRegs.GPBDAT.bit.GPIO32 = State;
	break;

    case eLED_PWRON:
		GpioDataRegs.GPBDAT.bit.GPIO33 = State;
	break;

	case eLED_READSTATUS:
		GpioDataRegs.GPBDAT.bit.GPIO34 = State;
	break;

    case eLED_SETGPIO:
		GpioDataRegs.GPBDAT.bit.GPIO35 = State;
	break;

    case eLED_RSTGPIO:
		GpioDataRegs.GPBDAT.bit.GPIO36 = State;
	break;   

    case eLED_STEP1:
		GpioDataRegs.GPBDAT.bit.GPIO37 = State;
	break;

    case eLED_STEP2:
		GpioDataRegs.GPBDAT.bit.GPIO38 = State;
	break;

    case eLED_STEP3:
		GpioDataRegs.GPBDAT.bit.GPIO39 = State;
	break;

 

	default:
		// MISRA
	break;
	}
}

/**
 * @brief 하드웨어 GPATOGGLE 레지스터 직접 제어 (Internal Only)
 */
static void HW_toggleLedPin(uint16_t Index)
{
	switch(Index)
	{

	case eLED_GREEN:
		GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1u;
	break;

	case eLED_ORANGE:
		GpioDataRegs.GPBTOGGLE.bit.GPIO32 = 1u;
	break;

    case eLED_PWRON:
		GpioDataRegs.GPBTOGGLE.bit.GPIO33 = 1u;
	break;

    case eLED_READSTATUS:
		GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1u;
	break;

	case eLED_SETGPIO:
		GpioDataRegs.GPBTOGGLE.bit.GPIO35 = 1u;
	break;

	case eLED_RSTGPIO:
		GpioDataRegs.GPBTOGGLE.bit.GPIO36 = 1u;
	break;

	case eLED_STEP1:
		GpioDataRegs.GPBTOGGLE.bit.GPIO37 = 1u;
	break;

	case eLED_STEP2:
		GpioDataRegs.GPBTOGGLE.bit.GPIO38 = 1u;
	break;

	case eLED_STEP3:
		GpioDataRegs.GPBTOGGLE.bit.GPIO39 = 1u;
	break;


	default:
		// MISRA
	break;
	}
}



/**
 * @brief 시스템 상태(IsValid)에 따른 LED 제어 로직 처리
 * @details 100ms 주기로 호출되어 IsValid 상태에 따라 Orange LED를 On/Off 합니다.
 *          IsValid가 0(에러)이면 Orange LED ON, 1(정상)이면 OFF
 */
void updateOrangeLed(void)
{
    if (xXmtIpcMsg1.IsValid == false)
    {
        setLedStatus(&xLed.ledOrange, LED_ON);
    }
    else
    {
        setLedStatus(&xLed.ledOrange, LED_OFF);
    }
}

void updateSetRstGpioLed(void)
{

    // GPIO_ENC_PWR_EN(26번) 상태에 따라 33번 LED 제어
    setLedStatus(&xLed.ledPwrOn, (GPIO_readPin(GPIO_ENC_PWR_EN) == LED_ON));

    // ReadStatus 상태에 따라 34번 LED 제어
    setLedStatus(&xLed.ledReadstatus, (xXmtIpcMsg1.ReadStatus == LED_ON));

    // GPIO_ENC_ZERO_SET(24번) 상태에 따라 35번 LED 제어
    setLedStatus(&xLed.ledSetGpio, (GPIO_readPin(GPIO_ENC_ZERO_SET) == LED_ON));

    // GPIO_ENC_ZERO_RST(25번) 상태에 따라 36번 LED 제어
    setLedStatus(&xLed.ledRstGpio, (GPIO_readPin(GPIO_ENC_ZERO_RST) == LED_ON));



}
