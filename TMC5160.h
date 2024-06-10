#ifndef TMC5160_H
#define TMC5160_H

#include "main.h"

/* TMC5160 register addresses */
#define TMC5160_GCONF 0x80  //0x00
#define TMC5160_CHOPCONF 0xEC  //0x6C
#define TMC5160_PWMCONF 0xF0  //0x70
#define TMC5160_COOLCONF 0xED  //0x6D
#define TMC5160_DRV_CONF 0x8A  //0x0A
#define TMC5160_GSTAT 0x81  //0x01
#define TMC5160_IHOLD_IRUN 0x90  //0x10
#define TMC5160_RAMPMODE 0xA0  //0x20
#define TMC5160_XACTUAL 0xA1  //0x21
#define TMC5160_VSTART 0xA3  //0x23
#define TMC5160_VMAX 0xA7  //0x27
#define TMC5160_A1 0xA4  //0x24
#define TMC5160_V1 0xA5  //0x25
#define TMC5160_AMAX 0xA6  //0x26
#define TMC5160_DMAX 0xA8  //0x28
#define TMC5160_D1 0xAA  //0x2A
#define TMC5160_VSTOP 0xAB  //0x2B
#define TMC5160_XTARGET 0xAD  //0x2D
#define TMC5160_TZEROWAIT 0xAC  //0x2C
#define TMC5160_TPOWERDOWN 0x91  //0x11
#define TMC5160_GLOBAL_SCALER 0x8B  //0x0B

/* Function prototypes */
void TMC5160_WriteRegister(uint8_t address, uint32_t value);
uint32_t TMC5160_ReadRegister(uint8_t address);
void TMC5160_Init(void);
int32_t speedFromHz(float speedHz);
int32_t accelFromHz(float accelHz);
void setRampSpeeds(float startSpeed, float stopSpeed, float transitionSpeed);
void setMaxSpeed(float speed);
void setAccelerations(float maxAccel, float maxDecel, float startAccel, float finalDecel);
void setTargetPosition(float position);
void setCurrentPosition(float position);

#endif /* TMC5160_H */
