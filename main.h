#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>

#define PTT     6   // not used
#define TXW     10
#define RXW     8  // RXW not used by this transmitter sketch. D7 is used in receiver sketch
// #define RXD1    2
// #define TXD1    3
#define RELAY   7

#include <SPI.h>
#include <Wire.h>
// #include <SoftwareSerial.h>
#include <RH_ASK.h>
#include "ubx.h"
#include "ask.h"

#define OLED_RESET -1

RH_ASK driver(BAUD_RATE, RXW, TXW, PTT, false);

// SoftwareSerial myport(RXD1,TXD1, false);

NAV_PVT pvt;
packet pkt, old_pkt;

#endif
