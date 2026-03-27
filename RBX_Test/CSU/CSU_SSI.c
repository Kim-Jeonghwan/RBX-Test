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
    /* 시퀀스 진행 중(EncBusy == true)에는 읽기 동작을 차단하여 시퀀스 제어 우선 */
    if((xXmtIpcMsg1.EncPwrStat == true) && (xRcvIpcMsg1.Command.bit.ReadEn == true) && (xXmtIpcMsg1.EncBusy == false))
    {
        xXmtIpcMsg1.ReadStatus = true; // 동작 중 피드백

        // 1. SPI RX 버퍼 정리 (Flush)
        while(SpiaRegs.SPISTS.bit.INT_FLAG != 0u) 
        {
            (void)SpiaRegs.SPIRXBUF;
        }

        // 2. 48비트 프레임 수신 (16bit x 3회)
        // [필수 보완] Zettlex IncOder 스펙 (타이밍 다이어그램): 
        // 바이트(또는 워드) 간 통신 유휴 시간(T_cki)이 10us를 초과하면 인코더가 프레임을 리셋하고 처음부터 다시 보냅니다.
        // for문 도중 인터럽트가 걸려 지연되는 것을 막기 위해 통신 중 인터럽트를 잠시 끕니다.
        DINT;
        for(i = 0; i < 3u; i++)
        {
            SPI_writeDataBlockingNonFIFO(SPIA_BASE, 0xFFFF);
            xEncoderA.RAW[i] = SPI_readDataBlockingNonFIFO(SPIA_BASE);
        }
        EINT;

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
        xXmtIpcMsg1.EncoderAngle = (uint32_t)(xEncoderA.ANGLE * 10000.0f + 0.5f); // 소수점 4자리 유효화를 위한 x10000 스케일링 및 반올림
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
 * @brief Zettlex IncOder 실제 하드웨어 방식(Augmented CRC-7) 적용
 */
static uint16_t Calculate_CRC7(uint32_t data_26bit)
{
    uint8_t crc = 0;
    int32_t i;
    
    // Zettlex 매뉴얼의 "32 bit word..." 텍스트는 오기재이거나 특정 하드웨어 구조를 오해하게 적어둔 것 같음.
    // (예: 텍스트 그대로 연산하면 19, 실제 수신 RECV 값은 50)

    // 1. 26비트 데이터 처리 (MSB인 D32부터 차례대로)
    for (i = 25; i >= 0; i--)
    {
        uint8_t bit = (data_26bit >> i) & 1;
        uint8_t inv = (crc & 0x40) ? 1 : 0;
        
        crc = ((crc << 1) | bit) & 0x7F;
        if (inv) { crc ^= 0x5B; }
    }
    
    // 2. 7개의 '0' 비트를 추가로 밀어넣음 (Augmented Zero Padding)
    // 이 과정을 거쳐야 실제 수신된 CRC_RECV 값과 완벽히 일치합니다.
    for (i = 0; i < 7; i++)
    {
        uint8_t inv = (crc & 0x40) ? 1 : 0;
        
        crc = (crc << 1) & 0x7F;
        if (inv) { crc ^= 0x5B; }
    }

    return (uint16_t)crc;
}

