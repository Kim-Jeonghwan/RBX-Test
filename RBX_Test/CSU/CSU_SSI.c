/**********************************************************************
    Nexcom Co., Ltd.
    Filename         : CSU_SSI.c
    Description      : SSI Encoder Data Polling and Decoding
    Last Updated     : 2026. 02. 02.
**********************************************************************/

/* ************************** [[   include  ]]  *********************************************************** */
#include "CSU_SSI.h"


/* ************************** [[   define   ]]  *********************************************************** */
// #define SSI_CS_LOW		GpioDataRegs.GPBCLEAR.bit.GPIO66	= 1u
// #define SSI_CS_HIGH		GpioDataRegs.GPBSET.bit.GPIO66		= 1u


/* ************************** [[   global   ]]  *********************************************************** */
stSpiEnc xEncoderA;

// CSU_IPC.c에 정의된 전역 변수를 참조합니다.
extern stRcvIpcMsg1 xRcvIpcMsg1; 
extern stXmtIpcMsg1 xXmtIpcMsg1;

/* ************************** [[  static prototype  ]]  *************************************************** */
static void updateEncoderState(void);
static uint16_t Calculate_CRC7(uint32_t data_26bit);



/* ************************** [[  function  ]]  *********************************************************** */

/**
 * @brief 엔코더 데이터 및 IPC 관련 상태 초기화
 */
void initEncoderSSI(void)
{
    uint16_t i;

    // 1. 내부 변수 초기화
    for(i = 0; i < 3u; i++)
    {
        xEncoderA.RAW[i] = 0u;
    }
    xEncoderA.PD       = 0u;
    xEncoderA.ZPD      = false;
    xEncoderA.PV       = false;
    xEncoderA.PS       = false;
    xEncoderA.SD       = false;
    xEncoderA.ANGLE    = 0.0f;
    xEncoderA.IS_VALID = false;
    
    /* IPC 송신 구조체 초기화 */
    xXmtIpcMsg1.EncoderAngle = 0u;
    xXmtIpcMsg1.EncoderRawPD = 0u;
    xXmtIpcMsg1.IsValid      = false;
    xXmtIpcMsg1.ReadStatus   = false;
    xXmtIpcMsg1.Err_PV       = false;
    xXmtIpcMsg1.Err_CRC      = false;
}

/**
 * @brief SSI 엔코더 데이터 폴링 및 데이터 갱신 (10ms 주기)
 * @details 전원 상태와 PC 명령을 확인하여 SPI 통신을 수행합니다.
 */
void PollingEncoderSSI(void)
{
    uint16_t i;

    /* 조건: 전원 인가(EncPwrStat) 및 PC 읽기 명령(ReadEn) 활성 시 */
    /* 시퀀스 진행 중(EncBusy == 1)에는 읽기 동작을 차단하여 시퀀스 제어 우선 */
    if((xXmtIpcMsg1.EncPwrStat == true) && (xRcvIpcMsg1.Command.bit.ReadEn == true) && (xXmtIpcMsg1.EncBusy == false))
    {
        xXmtIpcMsg1.ReadStatus = true; // 동작 중 피드백

        // 1. SPI RX 버퍼 정리 (Flush)
        while(SpiaRegs.SPISTS.bit.INT_FLAG != 0u) 
        {
            (void)SpiaRegs.SPIRXBUF;
        }

        // 2. 48비트 프레임 수신 (16bit x 3회)
        // SSI_CS_LOW; // 필요 시 활성화
        for(i = 0; i < 3u; i++)
        {
            SPI_writeDataBlockingNonFIFO(SPIA_BASE, 0xFFFF);
            xEncoderA.RAW[i] = SPI_readDataBlockingNonFIFO(SPIA_BASE);
        }
        // SSI_CS_HIGH; // 필요 시 활성화

        // 3. 수신 데이터 해석
        updateEncoderState(); 
    }
    else
    {
        // 전원이 꺼져 있거나 읽기 중지 상태인 경우
        xXmtIpcMsg1.ReadStatus = false;
        xXmtIpcMsg1.IsValid    = false; 
        
        if(xXmtIpcMsg1.EncPwrStat == false)
        {
            xEncoderA.IS_VALID = false;
        }
    }
}

/**
 * @brief 48비트 수신 프레임 해석 및 CRC 검증
 */
static void updateEncoderState(void)
{
    uint64_t full_frame;
    
    // 1. RAW 데이터 병합 (48-bit)
    full_frame = ((uint64_t)xEncoderA.RAW[0] << 32ULL) | 
                 ((uint64_t)xEncoderA.RAW[1] << 16ULL) | 
                 ((uint64_t)xEncoderA.RAW[2]);

    // 2. 비트맵 기반 필드 추출(Datasheet)
    xEncoderA.ZPD       = ((full_frame >> 32ULL) & 0x01ULL) ? true : false; 
    xEncoderA.PV        = ((full_frame >> 31ULL) & 0x01ULL) ? true : false; 
    xEncoderA.PS        = ((full_frame >> 30ULL) & 0x01ULL) ? true : false; 
    xEncoderA.PD        = (uint32_t)((full_frame >> 8ULL)  & 0xFFFFFULL); 
    xEncoderA.SD        = ((full_frame >> 7ULL)  & 0x01ULL) ? true : false; 
    xEncoderA.CRC_RECV  = (uint16_t)(full_frame & 0x7FULL);

    // 3. CRC 계산 (D7 ~ D32 영역, 총 26비트)
    uint32_t data_for_crc = (uint32_t)((full_frame >> 7ULL) & 0x3FFFFFFULL);
    xEncoderA.CRC_CALC = Calculate_CRC7(data_for_crc);

    // 4. 검증 및 IPC 데이터 갱신
    if ((xEncoderA.PV == true) && (xEncoderA.CRC_RECV == xEncoderA.CRC_CALC)) 
    {
        xEncoderA.IS_VALID = true;
        xEncoderA.ANGLE = ((float32_t)xEncoderA.PD / 1048576.0f) * 360.0f;

        // IPC 송신 패킷 업데이트
        xXmtIpcMsg1.IsValid      = true;
        xXmtIpcMsg1.EncoderAngle = (uint32_t)(xEncoderA.ANGLE * 10.0f + 0.5f); // 소수점 1자리 반올림 (예: 12.34 -> 123)
        xXmtIpcMsg1.EncoderRawPD = xEncoderA.PD;
        xXmtIpcMsg1.Err_PV       = false;
        xXmtIpcMsg1.Err_CRC      = false;
    } 
    else 
    {
        xEncoderA.IS_VALID = false;
        
        // 에러 상태 알림
        xXmtIpcMsg1.IsValid      = false;
        xXmtIpcMsg1.Err_PV       = (xEncoderA.PV == false) ? true : false;
        xXmtIpcMsg1.Err_CRC      = (xEncoderA.CRC_RECV != xEncoderA.CRC_CALC) ? true : false;
        xXmtIpcMsg1.EncoderRawPD = xEncoderA.PD; // 분석용 원본 데이터는 유지
    }
}

/**
 * @brief 다항식 0x5B를 사용한 CRC-7 계산
 */
static uint16_t Calculate_CRC7(uint32_t data_26bit)
{
    uint32_t i;
    // 데이터 시트: CRC는 D7이 LSB로 오도록 4바이트(32비트) 워드 내에서 정렬됨 
    // 26비트 데이터를 32비트 워드의 MSB 쪽으로 밀어서 계산 준비
    uint32_t remainder = data_26bit << (32 - 26); 
    uint32_t polynomial = (uint32_t)0x5B << (32 - 7); // 0x5B 다항식을 MSB에 정렬

    for (i = 0; i < 26; i++)
    {
        // 최상위 비트(MSB)가 1이면 다항식으로 XOR 연산
        if (remainder & 0x80000000)
        {
            remainder ^= polynomial;
        }
        remainder <<= 1;
    }

    // 최종 결과를 오른쪽으로 밀어 7비트만 남김 
    return (uint16_t)(remainder >> (32 - 7));
}
