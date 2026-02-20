/**********************************************************************
    Nexcom Co., Ltd.
    Filename         : CSU_Zero.c
    Description      : Encoder Zero/Reset Sequence & Manual Control
    Last Updated     : 2026. 02. 02.
**********************************************************************/


/* ************************** [[   include  ]]  *********************************************************** */
#include "CSU_Zero.h"


/* ************************** [[   define   ]]  *********************************************************** */



/* ************************** [[   global   ]]  *********************************************************** */
// CSU_IPC.c에 정의된 전역 변수를 참조합니다.
extern stRcvIpcMsg1 xRcvIpcMsg1; 
extern stXmtIpcMsg1 xXmtIpcMsg1;


/* ************************** [[  static prototype  ]]  *************************************************** */
/* 정적 변수 */
static eZeroStep xZeroStep = eZERO_IDLE;
static uint32_t u32TickCnt = 0u;
static uint16_t bIsSetAction = 0u; // 1: Set동작, 0: Rst동작 플래그


/* ************************** [[  function  ]]  *********************************************************** */

/**
 * @brief Zero 관련 GPIO 초기화
 */
void initGPIODoutZero(void)
{
    // DSP_DO_ENC_ZERO_SET
	GPIO_SetupPinMux(24u, GPIO_MUX_CPU1, 0u);
	GPIO_SetupPinOptions(24u, GPIO_OUTPUT, GPIO_PUSHPULL);

	// DSP_DO_ENC_ZERO_RESET
	GPIO_SetupPinMux(25u, GPIO_MUX_CPU1, 0u);
	GPIO_SetupPinOptions(25u, GPIO_OUTPUT, GPIO_PUSHPULL);
	
	// DSP_DO_ENC_PWR_EN
	GPIO_SetupPinMux(26u, GPIO_MUX_CPU1, 0u);
	GPIO_SetupPinOptions(26u, GPIO_OUTPUT, GPIO_PUSHPULL);
}

/**
 * @brief 초기화 (상시 전원 On 상태)
 */
void initEncoderZero(void)
{
    xZeroStep = eZERO_IDLE;
    u32TickCnt = 0u;

    // 신호선은 Low로 시작
    GPIO_writePin(GPIO_ENC_ZERO_SET, 0u);
    GPIO_writePin(GPIO_ENC_ZERO_RST, 0u);
    
    // 전원은 상시 On (1u)
    GPIO_writePin(GPIO_ENC_PWR_EN, 1u); 
    
    // 전원 상태 초기화 (GPIO 상태 반영)
    xXmtIpcMsg1.EncPwrStat = (uint16_t)GPIO_readPin(GPIO_ENC_PWR_EN);
    xXmtIpcMsg1.EncBusy    = 0u;
    xXmtIpcMsg1.EncDone    = 0u;
}

/**
 * @brief 장치 제어 처리 (10ms 주기 호출)
 */
void procEncoderZero(void)
{
    // 시퀀스 진행 중(IDLE이 아님)에 들어오는 트리거 및 메뉴얼 명령은 무시하고 클리어
    if(xZeroStep != eZERO_IDLE)
    {
        xRcvIpcMsg1.Command.bit.SetTrig  = 0u;
        xRcvIpcMsg1.Command.bit.RstTrig  = 0u;
        xRcvIpcMsg1.Command.bit.ManualEn = 0u; // 메뉴얼 전환 차단
    }

    // 0. 수동 제어 처리
    manualCtrlZero();

    // 1. 자동 시퀀스 스테이트 머신
    switch(xZeroStep)
    {
        case eZERO_IDLE:
            // 트리거 확인 (메뉴얼 모드가 아닐 때만 작동)
            if(xRcvIpcMsg1.Command.bit.ManualEn == 0u)
            {
                if(xRcvIpcMsg1.Command.bit.SetTrig == 1u)
                {
                    bIsSetAction = 1u; // Set 시퀀스 결정
                    xZeroStep = eZERO_PWR_OFF;
                }
                else if(xRcvIpcMsg1.Command.bit.RstTrig == 1u)
                {
                    bIsSetAction = 0u; // Rst 시퀀스 결정
                    xZeroStep = eZERO_PWR_OFF;
                }
            }

            // 시퀀스가 시작되었거나, 메뉴얼 모드인데 트리거가 들어온 경우 비트 클리어
            if(xZeroStep == eZERO_PWR_OFF || xRcvIpcMsg1.Command.bit.ManualEn == 1u)
            {
                xRcvIpcMsg1.Command.bit.SetTrig = 0u;
                xRcvIpcMsg1.Command.bit.RstTrig = 0u;

                if(xZeroStep == eZERO_PWR_OFF)
                {
                    xXmtIpcMsg1.EncBusy = 1u;
                    xXmtIpcMsg1.EncDone = 0u;
                }
            }
            break;

        case eZERO_PWR_OFF:


            // [단계 1] 전원 차단 및 결정된 신호(SET/RST) 출력
            GPIO_writePin(GPIO_ENC_PWR_EN, 0u);

            if(bIsSetAction == 1u) {
                GPIO_writePin(GPIO_ENC_ZERO_SET, 1u);
                GPIO_writePin(GPIO_ENC_ZERO_RST, 0u);
            } else {
                GPIO_writePin(GPIO_ENC_ZERO_SET, 0u);
                GPIO_writePin(GPIO_ENC_ZERO_RST, 1u);
            }

            u32TickCnt = 0u;
            xZeroStep = eZERO_WAIT_2SEC;
            break;

        case eZERO_WAIT_2SEC:
            setLedStatus(&xLed.ledStep1, LED_ON); // 37번 ON

            // [단계 2] 전원 OFF 상태로 2초 대기 (10ms * 200)
            if(++u32TickCnt >= 200u)
            {
                // 2초 경과 후 전원 재투입
                GPIO_writePin(GPIO_ENC_PWR_EN, 1u);
                u32TickCnt = 0u;
                xZeroStep = eZERO_PWR_ON_WAIT;
                

            }
            break;

        case eZERO_PWR_ON_WAIT:
            setLedStatus(&xLed.ledStep1, LED_OFF); // 37번 OFF
            setLedStatus(&xLed.ledStep2, LED_ON); // 38번 ON
        

            // [단계 3] 전원 ON 상태에서 5초 패킷 유지 (10ms * 500)
            if(++u32TickCnt >= 500u)
            {
                // 5초 경과 후 신호 해제 (SET/RST Low)
                GPIO_writePin(GPIO_ENC_ZERO_SET, 0u);
                GPIO_writePin(GPIO_ENC_ZERO_RST, 0u);
                
                xXmtIpcMsg1.EncBusy = 0u;
                xXmtIpcMsg1.EncDone = 1u;
                u32TickCnt = 0u;
                xZeroStep = eZERO_DONE_WAIT;

            }
            break;

        case eZERO_DONE_WAIT:
            setLedStatus(&xLed.ledStep2, LED_OFF); // 38번 OFF
            setLedStatus(&xLed.ledStep3, LED_ON); // 39번 ON    
            // [단계 4] 완료 비트 1초 유지 (10ms * 100)
            if(++u32TickCnt >= 100u)
            {
                xXmtIpcMsg1.EncDone = 0u;
                xZeroStep = eZERO_IDLE;

            setLedStatus(&xLed.ledStep1, LED_OFF); // 37번 OFF
            setLedStatus(&xLed.ledStep2, LED_OFF); // 38번 OFF
            setLedStatus(&xLed.ledStep3, LED_OFF); // 39번 OFF      
                
            
            }
            break;

        default:
            setLedStatus(&xLed.ledStep1, LED_OFF); // 37번 OFF
            setLedStatus(&xLed.ledStep2, LED_OFF); // 38번 OFF
            setLedStatus(&xLed.ledStep3, LED_OFF); // 39번 OFF            
            break;
    }

    // [중요] 항상 실제 GPIO 출력 상태를 읽어서 IPC 상태 비트에 반영
    xXmtIpcMsg1.EncPwrStat = (uint16_t)GPIO_readPin(GPIO_ENC_PWR_EN);


}

/**
 * @brief 수동 제어 처리
 */
void manualCtrlZero(void)
{
    // 시퀀스 동작 중이 아닐 때만 수동 제어 처리
    if(xZeroStep == eZERO_IDLE)
    {
        // 수동 모드 상태 비트 업데이트
        xXmtIpcMsg1.ManualMode = xRcvIpcMsg1.Command.bit.ManualEn;

        if(xRcvIpcMsg1.Command.bit.ManualEn == 1u)
        {
            // 수동 모드: PC 명령을 하드웨어에 직접 반영
            GPIO_writePin(GPIO_ENC_ZERO_SET, (uint32_t)xRcvIpcMsg1.Command.bit.SetForce);
            GPIO_writePin(GPIO_ENC_ZERO_RST, (uint32_t)xRcvIpcMsg1.Command.bit.RstForce);
            GPIO_writePin(GPIO_ENC_PWR_EN, (uint32_t)xRcvIpcMsg1.Command.bit.PwrEn);
        }
        else
        {
            // 일반 모드: 기본 상태 유지 (전원 항상 ON)
            GPIO_writePin(GPIO_ENC_ZERO_SET, 0u);
            GPIO_writePin(GPIO_ENC_ZERO_RST, 0u);
            GPIO_writePin(GPIO_ENC_PWR_EN, 1u);
        }
    }
    else
    {
        // 시퀀스 중에는 메뉴얼 상태 비트를 무조건 0으로 유지 (IDLE 아닐 때 진입 방지)
        xXmtIpcMsg1.ManualMode = 0u;
    }
}
