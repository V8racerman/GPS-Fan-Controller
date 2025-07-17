#ifndef ASK_H
#define ASK_H
#include <Arduino.h>

struct packet {
  uint32_t     spd = 0;
  uint8_t      numSV = 0;
  uint8_t      state = 0;  
  };

#define BAUD_RATE 400
#define SPEED_CONVERSION 0.00223693629

#endif
