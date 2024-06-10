#include "tmc5160.h"
#include "math.h"

extern SPI_HandleTypeDef hspi2;

#define min(a, b) ((a) < (b) ? (a) : (b))

const uint16_t rev = 51200;
uint32_t _fclk = 12000000;
uint16_t _uStepCount = 256;

void TMC5160_WriteRegister(uint8_t address, uint32_t value) {
    uint8_t txData[5];
    txData[0] = address ;
    txData[1] = (value & 0xFF000000) >> 24;
    txData[2] = (value & 0xFF0000) >> 16;
    txData[3] = (value & 0xFF00) >> 8;
    txData[4] = value & 0xFF;

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, txData, 5, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
}

void TMC5160_Init(void) {
    // Set initial configuration for TMC5160
    TMC5160_WriteRegister(TMC5160_GCONF,  0x00000000); // Normal operation mode
    TMC5160_WriteRegister(TMC5160_GSTAT, 0b00000000000000000000000000000101);
    TMC5160_WriteRegister(TMC5160_GLOBAL_SCALER, 0b00000000000000000000000010111111); // Set global scaler
    TMC5160_WriteRegister(TMC5160_IHOLD_IRUN, 0b00000000000000000000111111000011); // Current settings
    TMC5160_WriteRegister(TMC5160_CHOPCONF, 0b00000000000000010000000111000101); // Chopper Configuration
    TMC5160_WriteRegister(TMC5160_PWMCONF, 0xC40C001E); // PWMCONF Reset
    TMC5160_WriteRegister(TMC5160_DRV_CONF, 0b00000000000000000001000010000000);
    TMC5160_WriteRegister(TMC5160_RAMPMODE, 0x00000000); // Positioning mode
    TMC5160_WriteRegister(TMC5160_TPOWERDOWN, 0x0000000A);
		setRampSpeeds(0,0.1,300);
}

uint32_t TMC5160_ReadRegister(uint8_t address) {
    uint8_t txData[5] = {address, 0, 0, 0, 0};
    uint8_t rxData[5] = {0};

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi2, txData, rxData, 5, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

    uint32_t value = (rxData[1] << 24) | (rxData[2] << 16) | (rxData[3] << 8) | rxData[4];
    return value;
}

int32_t speedFromHz(float speedHz) {
    return (int32_t)(speedHz / ((float)_fclk / (float)(1ul << 24)) * (float)_uStepCount);
}

int32_t accelFromHz(float accelHz) {
    return (int32_t)(accelHz / ((float)_fclk * (float)_fclk / (512.0 * 256.0) / (float)(1ul << 24)) * (float)_uStepCount);
}

void setRampSpeeds(float startSpeed, float stopSpeed, float transitionSpeed) {
    TMC5160_WriteRegister(TMC5160_VSTART, min(0x3FFFF, speedFromHz(fabs(startSpeed)))); // VSTART : 18 bits
    TMC5160_WriteRegister(TMC5160_VSTOP, min(0x3FFFF, speedFromHz(fabs(stopSpeed)))); // VSTOP : 18 bits
    TMC5160_WriteRegister(TMC5160_V1, min(0xFFFFF, speedFromHz(fabs(transitionSpeed)))); // V1 : 20 bits
}

void setMaxSpeed(float speed) {
    if(speed > 0) {
        TMC5160_WriteRegister(TMC5160_RAMPMODE, 0x00000001);
        TMC5160_WriteRegister(TMC5160_VMAX, min(0x7FFFFF, speedFromHz(fabs(speed)))); // VMAX : 23 bits
    } else {
        TMC5160_WriteRegister(TMC5160_RAMPMODE, 0x00000002);
        TMC5160_WriteRegister(TMC5160_VMAX, min(0x7FFFFF, speedFromHz(fabs(speed)))); // VMAX : 23 bits
    }
}

void setAccelerations(float maxAccel, float maxDecel, float startAccel, float finalDecel) {
    TMC5160_WriteRegister(TMC5160_AMAX, min(0xFFFF, accelFromHz(fabs(maxAccel)))); // AMAX, DMAX, A1, D1 : 16 bits
    TMC5160_WriteRegister(TMC5160_DMAX, min(0xFFFF, accelFromHz(fabs(maxDecel))));
    TMC5160_WriteRegister(TMC5160_A1, min(0xFFFF, accelFromHz(fabs(startAccel))));
    TMC5160_WriteRegister(TMC5160_D1, min(0xFFFF, accelFromHz(fabs(finalDecel))));
}

void setTargetPosition(float position) {
    TMC5160_WriteRegister(TMC5160_XTARGET, (int32_t)(position * (float)_uStepCount));
}

void setCurrentPosition(float position) {
    TMC5160_WriteRegister(TMC5160_XACTUAL, (int32_t)(position * (float)_uStepCount));
}
