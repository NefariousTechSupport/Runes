#pragma once

#include <stdint.h>

uint16_t crc16(char* pData, int length);
uint8_t calculateBCC(uint32_t serial);