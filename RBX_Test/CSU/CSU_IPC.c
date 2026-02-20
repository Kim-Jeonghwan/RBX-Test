/**********************************************************************
    Nexcom Co., Ltd.
    Filename         : CSU_IPC.c
    Description      : PC Interface Communication (IPC) Protocol Definition
    Last Updated     : 2026. 01. 30.
**********************************************************************/

/* ************************** [[   include  ]]  *********************************************************** */
#include "CSU_IPC.h"



/* ************************** [[   define   ]]  *********************************************************** */
#define IPC_SOF		0x7Eu
#define IPC_EOT		0x0Du
#define IPC_MSG1	0x10u


/* ************************** [[   global   ]]  *********************************************************** */
stRcvIpcMsg1	xRcvIpcMsg1;
stXmtIpcMsg1	xXmtIpcMsg1;



/* ************************** [[  static prototype  ]]  *************************************************** */




/* ************************** [[  function  ]]  *********************************************************** */


/**
 * @brief PC로부터 수신된 IPC 메시지 해석 및 구조체 업데이트
 * @details ID 0x10 패킷을 수신하여 시퀀스 번호, 제어 명령(Command), 예비 데이터를 파싱합니다.
 * @param ID 수신된 메시지의 식별 번호 (0x10u)
 * @param Data[] 수신된 데이터 배열 (바이트 단위)
 */
void recvIpcMessage(uint16_t ID, uint16_t Data[])
{
    volatile uint16_t pos = 0u;

    onConv32 on32;
    onConv16 on16;
    
    switch(ID)
    {
    case 0x10u:
        /* 1. Sequence Number 저장 */
        xRcvIpcMsg1.IncNumber = Data[pos++];

        /* 2. Command 저장 (Little Endian: LSB First) */
        on16.byte.B0 = (uint8_t)Data[pos++]; // 첫 번째 바이트를 하위(B0)로 저장
        on16.byte.B1 = (uint8_t)Data[pos++]; // 두 번째 바이트를 상위(B1)로 저장
        
        xRcvIpcMsg1.Command.all = on16.u16;

        /* 3. Reserved1 (float32_t - Little Endian) */
        on32.byte.B0 = (uint8_t)Data[pos++];
        on32.byte.B1 = (uint8_t)Data[pos++];
        on32.byte.B2 = (uint8_t)Data[pos++];
        on32.byte.B3 = (uint8_t)Data[pos++];
        xRcvIpcMsg1.Reserved1 = on32.f32;

        /* 4. Reserved2 (float32_t - Little Endian) */
        on32.byte.B0 = (uint8_t)Data[pos++];
        on32.byte.B1 = (uint8_t)Data[pos++];
        on32.byte.B2 = (uint8_t)Data[pos++];
        on32.byte.B3 = (uint8_t)Data[pos++];
        xRcvIpcMsg1.Reserved2 = on32.f32;

    break;
    
    default:
    break;
    }
}

/**
 * @brief 엔코더 상태 및 데이터를 PC로 전송 (10ms 주기)
 * @details 전체 16바이트 패킷을 구성하며, 순수 데이터 길이는 10바이트입니다.
 * 패킷 구조: SOF(1) + ID(1) + LEN(1) + DATA(10) + Checksum(1) + EOT(1)
 */
void sendIpcMessage1(void)
{
    volatile uint16_t pos = 0u;
    onConv32 on32;
    onConv16 on16;
    uint16_t i = 0u;
    uint16_t Buf[25u] = {0u, }; 
    uint16_t CheckSum = 0u;

    /* 1. 헤더 구성 */
    Buf[pos++] = IPC_SOF;           // Buf[0]: 0x7E
    Buf[pos++] = IPC_MSG1;          // Buf[1]: 0x10 (ID)
    Buf[pos++] = 0u;                // Buf[2]: Length (데이터 개수 11이 들어갈 자리)
    
    /* 2. Sequence Number */
    Buf[pos++] = (uint16_t)(xXmtIpcMsg1.IncNumber++ & 0xFFu); // Buf[3]

    /* 3. Status Bit Fields 직접 조립 (가장 안전한 방법) */
    on16.u16 = 0u; // 필수: 쓰레기 값 방지
    on16.u16 |= (xXmtIpcMsg1.IsValid    == true ? 1u : 0u) << 0u;
    on16.u16 |= (xXmtIpcMsg1.Err_PV     == true ? 1u : 0u) << 1u;
    on16.u16 |= (xXmtIpcMsg1.Err_CRC    == true ? 1u : 0u) << 2u;
    on16.u16 |= (xXmtIpcMsg1.ReadStatus == true ? 1u : 0u) << 3u;
    on16.u16 |= (xXmtIpcMsg1.EncBusy    == true ? 1u : 0u) << 4u;
    on16.u16 |= (xXmtIpcMsg1.EncDone    == true ? 1u : 0u) << 5u;
    on16.u16 |= (xXmtIpcMsg1.EncPwrStat == true ? 1u : 0u) << 6u;
    on16.u16 |= (xXmtIpcMsg1.ManualMode == true ? 1u : 0u) << 7u;
    
    Buf[pos++] = on16.byte.B0; // Buf[4]: Status Byte (PC expects 1 byte)

    /* 4. EncoderAngle (uint32_t - 4 bytes, Little Endian) */
    on32.u32 = xXmtIpcMsg1.EncoderAngle;
    Buf[pos++] = on32.byte.B0; // Buf[5]
    Buf[pos++] = on32.byte.B1; // Buf[6]
    Buf[pos++] = on32.byte.B2; // Buf[7]
    Buf[pos++] = on32.byte.B3; // Buf[8]

    /* 5. EncoderRawPD (uint32_t - 4 bytes, Little Endian) */
    on32.u32 = xXmtIpcMsg1.EncoderRawPD;
    Buf[pos++] = on32.byte.B0; // Buf[9]
    Buf[pos++] = on32.byte.B1; // Buf[10]
    Buf[pos++] = on32.byte.B2; // Buf[11]
    Buf[pos++] = on32.byte.B3; // Buf[12]

    /* 6. 데이터 길이(LEN) 계산 및 업데이트 */
    Buf[2] = (uint16_t)(pos - 2u);

    /* 7. CheckSum 계산 (Length인 Buf[2]부터 데이터 끝인 Buf[pos-1]까지) */
    CheckSum = 0u;
    for(i = 2u; i < pos; i++)
    {
        CheckSum += (Buf[i] & 0xFFu);
    }

    Buf[pos++] = (uint16_t)(CheckSum & 0xFFu); // Buf[13]
    Buf[pos++] = IPC_EOT;                   // Buf[14]: 0x0D


    /* 8. 최종 전송 (pos는 현재 15) */
    xmtScib_IPC(Buf, pos);
}
