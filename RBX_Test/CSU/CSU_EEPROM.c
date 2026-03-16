/**********************************************************************
    Nexcom Co., Ltd.
    Filename         : CSU_EEPROM.c
    Description      : EEPROM M95M04 SPI 통신 드라이버
    Last Updated     : 2026. 03. 13.
**********************************************************************/

/* ************************** [[   include  ]]  *********************************************************** */
#include "CSU_EEPROM.h"


/* ************************** [[   define   ]]  *********************************************************** */


/* ************************** [[   global   ]]  *********************************************************** */
uint16_t eeprom_read_val = 0;

/* ************************** [[  static prototype  ]]  *************************************************** */
static uint16_t SPIA_Transfer8(uint16_t byte_data);


/* ************************** [[  function  ]]  *********************************************************** */

/**
 * @brief EEPROM 스위치(GPIO64, 65) 핀 초기화 (GPIO_SetupPinMux 방식)
 */
void initGPIODinEEPROM(void)
{
    // GPIO 64 (Tactile Switch 1)
    GPIO_SetupPinMux(64, GPIO_MUX_CPU1, 0u);
    GPIO_SetupPinOptions(64, GPIO_INPUT, GPIO_ASYNC);

    // GPIO 65 (Tactile Switch 2)
    GPIO_SetupPinMux(65, GPIO_MUX_CPU1, 0u);
    GPIO_SetupPinOptions(65, GPIO_INPUT, GPIO_ASYNC);
}

/**
 * @brief EEPROM 관련 초기화
 */
void EEPROM_Init(void)
{
    // EEPROM 칩 셀렉트 핀은 DevSpi.c의 InitSpia() 에서 초기화됨 (High 대기)
    EEPROM_CS_HIGH();
}

/**
 * @brief SPIA 8-bit 송수신 함수
 * @param byte_data 송신할 8비트 데이터
 * @return 수신된 8비트 데이터
 */
static uint16_t SPIA_Transfer8(uint16_t byte_data)
{
    // C2000Ware SPI 8-bit 모드 시 TX 데이터는 좌측 정렬(Left Aligned)로 넣어야 함
    SPI_writeDataBlockingNonFIFO(SPIA_BASE, byte_data << 8);
    // RX 데이터는 우측 정렬(Right Aligned)되어 반환됨
    return SPI_readDataBlockingNonFIFO(SPIA_BASE) & 0xFFu;
}

/**
 * @brief EEPROM 쓰기 활성화 (WEL 비트 설정)
 */
void EEPROM_WriteEnable(void)
{
    EEPROM_CS_LOW();
    (void)SPIA_Transfer8(EEPROM_CMD_WREN);
    EEPROM_CS_HIGH();
}

/**
 * @brief EEPROM 상태 레지스터 읽기
 * @return 8-bit 상태 값
 */
uint16_t EEPROM_ReadStatus(void)
{
    uint16_t status = 0;

    EEPROM_CS_LOW();
    (void)SPIA_Transfer8(EEPROM_CMD_RDSR);
    status = SPIA_Transfer8(0x00u);
    EEPROM_CS_HIGH();

    return status;
}

/**
 * @brief EEPROM 1Byte 읽기
 * @param address 24-bit (실제 19-bit 사용) 주소 (0x000000 ~ 0x07FFFF)
 * @return 1Byte 데이터
 */
uint16_t EEPROM_ReadByte(uint32_t address)
{
    uint16_t rxData = 0;

    EEPROM_CS_LOW();
    
    // 읽기 명령어 전송
    (void)SPIA_Transfer8(EEPROM_CMD_READ);
    
    // 24-bit 주소 전송
    (void)SPIA_Transfer8((uint16_t)((address >> 16) & 0xFFu)); // A23-A16
    (void)SPIA_Transfer8((uint16_t)((address >> 8) & 0xFFu));  // A15-A8
    (void)SPIA_Transfer8((uint16_t)(address & 0xFFu));         // A7-A0
    
    // 데이터 수신
    rxData = SPIA_Transfer8(0x00u);
    
    EEPROM_CS_HIGH();

    return rxData;
}

/**
 * @brief EEPROM 1Byte 쓰기
 * @param address 24-bit 주소
 * @param data 저장할 1Byte 데이터
 */
void EEPROM_WriteByte(uint32_t address, uint16_t data)
{
    // 쓰기 활성화(WEL 비트 셋) 
    EEPROM_WriteEnable();

    EEPROM_CS_LOW();
    
    // 쓰기 명령어 전송
    (void)SPIA_Transfer8(EEPROM_CMD_WRITE);
    
    // 24-bit 주소 전송
    (void)SPIA_Transfer8((uint16_t)((address >> 16) & 0xFFu)); // A23-A16
    (void)SPIA_Transfer8((uint16_t)((address >> 8) & 0xFFu));  // A15-A8
    (void)SPIA_Transfer8((uint16_t)(address & 0xFFu));         // A7-A0
    
    // 데이터 전송
    (void)SPIA_Transfer8(data & 0xFFu);
    
    EEPROM_CS_HIGH();
}

/**
 * @brief EEPROM 동작 테스트 함수
 */
void EEPROM_Test(void)
{
    uint32_t test_addr = 0x000100u; // 임의의 테스트 주소
    uint16_t write_val = 0xA5u;     // 테스트용 쓸 데이터
    uint16_t read_val = 0;

    EEPROM_Init();

    // 입력한 주소에 데이터 쓰기
    EEPROM_WriteByte(test_addr, write_val);

    // 입력한 주소에서 데이터 읽어오기
    read_val = EEPROM_ReadByte(test_addr);

    if(read_val == write_val) {
        // 성공 시퀀스 (필요 시 추가)
    } else {
        // 실패 시퀀스 (필요 시 추가)
    }
}

/**
 * @brief EEPROM 스위치 (GPIO64, 65) 테스트 처리 태스크
 */
void EEPROM_Task(void)
{
    static uint32_t prev_gpio64 = 0;
    static uint32_t prev_gpio65 = 0;
    static bool toggle_val = false;
    
    // 현재 스위치 상태 읽기 (1: 눌림, 0: 뗌)
    uint32_t curr_gpio64 = GPIO_readPin(64);
    uint32_t curr_gpio65 = GPIO_readPin(65);
    
    uint32_t test_addr = 0x000100u; // 테스트할 주소

    // GPIO 64번 스위치 (누를 때마다 0xA5, 0xA6 번갈아가며 쓰기) - Rising Edge 감지
    if ((curr_gpio64 == 1) && (prev_gpio64 == 0)) {
        if (!toggle_val) {
            EEPROM_WriteByte(test_addr, 0xA5u);
        } else {
            EEPROM_WriteByte(test_addr, 0xA6u);
        }
        toggle_val = !toggle_val;
    }

    // GPIO 65번 스위치 (누를 때마다 읽기) - Rising Edge 감지
    if ((curr_gpio65 == 1) && (prev_gpio65 == 0)) {
        eeprom_read_val = EEPROM_ReadByte(test_addr);
    }

    // 이전 상태 업데이트
    prev_gpio64 = curr_gpio64;
    prev_gpio65 = curr_gpio65;
}
