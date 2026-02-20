/**********************************************************************
    Nexcom Co., Ltd.
    Filename         : CSU_SSI.h
    Description      : SSI Encoder Data Polling and Decoding
    Last Updated     : 2026. 02. 02.
**********************************************************************/

#ifndef CSU_SSI_H
#define CSU_SSI_H

/* ************************** [[   include  ]]  *********************************************************** */
#include "main.h"


/* ************************** [[   define   ]]  *********************************************************** */

/** * @brief 엔코더 분해능 및 연산 상수
 * 20-bit Resolution (2^20 = 1,048,576)
 */
#define ENCODER_RES_20BIT    1048576.0f  /* 2^20 해상도 */
#define SSI_FRAME_SIZE       3u          /* 16-bit * 3 = 48-bit */

/* ************************** [[   enum or struct   ]]  *************************************************** */

/**
 * @brief SSI 48-bit 프레임 비트 필드 맵 (참고용)
 * D29-D08: PD  (20bit)
 * D06-D00: CRC (CRC-7, 다항식 0x5B)
 */
typedef enum
{
    eSpiEnc_SBZ = 0,    /* Should Be Zero (D47-D33) */
    eSpiEnc_ZPD,        /* Zero Point Default (D32) */
    eSpiEnc_PV,         /* Position Valid (D31) */
    eSpiEnc_PS,         /* Position Synchronised (D30) */
    eSpiEnc_PD,         /* Position Data (D29-D8) */
    eSpiEnc_SD,         /* Stale Data (D7) */
    eSpiEnc_CRC         /* Cyclic Redundancy Check (D6-D0) */
} eSpiEnc;

/**
 * @brief 엔코더 통합 관리 구조체
 */
typedef struct
{
    uint16_t RAW[SSI_FRAME_SIZE]; /* SPI 수신 원본 (16bit x 3) */
    uint32_t PD;                  /* 20비트 위치 데이터 (D29-D08) */
    bool     ZPD;                 /* 제로 포인트 설정 여부 (D32) */
    bool     PV;                  /* 위치 유효성 (1:정상, 0:에러) */
    bool     PS;                  /* 위치 동기화 상태 (1:동기화됨) */
    bool     SD;                  /* 데이터 갱신 여부 (0:신규, 1:이전값) */
    uint16_t CRC_RECV;            /* 수신된 CRC-7 값 */ 
    uint16_t CRC_CALC;            /* MCU에서 계산한 CRC-7 값 */
    float32_t ANGLE;               /* 도(Degree) 단위 환산 각도 */
    bool     IS_VALID;            /* PV==1 && CRC 일치 시 true */
} stSpiEnc;

/* ************************** [[   global   ]]  *********************************************************** */

/**
 * @brief 엔코더 실시간 데이터 전역 변수
 * CSU_SSI.c에 정의되어 있으며, 외부 모듈에서 참조 가능
 */
extern stSpiEnc xEncoderA;


/* ************************** [[  function  ]]  *********************************************************** */

/**
 * @brief SSI 엔코더 모듈 초기화
 */
void initEncoderSSI(void);

/**
 * @brief SSI 엔코더 데이터 폴링 및 파싱 실행 (10ms 주기 권장)
 */
void PollingEncoderSSI(void);

#endif	// #ifndef CSU_SSI_H

