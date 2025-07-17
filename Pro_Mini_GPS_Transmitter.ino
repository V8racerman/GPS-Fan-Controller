#include "main.h"

const long watchdog_limit = 30000;
long watchdog_time;
long now_time;
const float trigger_speed = 40.0;
const float minimum_speed = 20.0;
const long fan_delay_time = 30000;
long fan_time;
const float hysteresis = 5;
const long warmup_time = 180000;
bool fan_delay = false;
bool fan_control_enable = false;
bool tx = true;
uint32_t new_speed;
uint32_t old_speed;
long start_time;

void calcChecksum(unsigned char* CK) {
  memset(CK, 0, 2);
  for (int i = 0; i < (int)sizeof(NAV_PVT); i++) {
    CK[0] += ((unsigned char*)(&pvt))[i];
    CK[1] += CK[0];
  }
}

void setup_UBLOX() {
   
  // send configuration data in UBX protocol
  for(unsigned int i = 0; i < sizeof(UBLOX_INIT); i++) {                        
    Serial.write( pgm_read_byte(UBLOX_INIT+i) );
    delay(5); // simulating a 38400baud pace (or less), otherwise commands are not accepted by the device.
  }
}

bool processGPS() {
  static int fpos = 0;
  static unsigned char checksum[2];
  const int payloadSize = sizeof(NAV_PVT);

  while ( Serial.available() ) {
    byte c = Serial.read();
    if ( fpos < 2 ) {
      if ( c == UBX_HEADER[fpos] )
        fpos++;
      else
        fpos = 0;
    }
    else {      
      if ( (fpos-2) < payloadSize )
        ((unsigned char*)(&pvt))[fpos-2] = c;

      fpos++;

      if ( fpos == (payloadSize+2) ) {
        calcChecksum(checksum);
      }
      else if ( fpos == (payloadSize+3) ) {
        if ( c != checksum[0] )
          fpos = 0;
      }
      else if ( fpos == (payloadSize+4) ) {
        fpos = 0;
        if ( c == checksum[1] ) {
          return true;
        }
      }
      else if ( fpos > (payloadSize+4) ) {
        fpos = 0;
      }
    }
  }
  return false;
}

void send_packet(packet pk) {
  driver.send((uint8_t*)&pk, sizeof(pk));
  driver.waitPacketSent();
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);  // turn fan off
  Serial.begin(9600);
  pkt.spd = 0;
  pkt.numSV = 0;
  pkt.state = 0;
  old_pkt.spd = 0;
  old_pkt.numSV = 0;
  old_pkt.state = 1;
  old_speed = 0;
  new_speed = 0;
  tx = true;
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  driver.init();
  delay(100);
  setup_UBLOX();
  start_time = millis();
  watchdog_time = start_time;
  }
void loop() {
  char c;
  now_time = millis();
  if (now_time - start_time > warmup_time) fan_control_enable = true;  // fan control enabled after "warmup_time" from power on
  if (now_time - watchdog_time > watchdog_limit) {  // watchdog timer failsafe turns fan on if the GPS serial messages are not being received 
    // old_pkt.state = pkt.state;  // not implemented as no way of testing now, but might be required
    pkt.state = 1; 
    digitalWrite(RELAY, LOW); // turn fan on
    fan_delay = false;
    }
  if ( processGPS() ) {
    digitalWrite(LED_BUILTIN, HIGH);
    watchdog_time = now_time;
    pkt.spd = (uint32_t)pvt.gSpeed;
    // old_pkt.numSV = pkt.numSV;  // not implemented as no way of testing now, but might be required
    pkt.numSV = (uint8_t)pvt.numSV;
    old_speed = new_speed;
    new_speed = SPEED_CONVERSION*pvt.gSpeed;
    if (fan_control_enable) {
      if ((pkt.state == 0) && (new_speed < minimum_speed ))  { 
          // old_pkt.state = pkt.state;  // not implemented as no way of testing now, but might be required
          pkt.state = 1; 
          digitalWrite(RELAY, LOW); // turn fan on
          fan_delay = false;
        }
      if ((pkt.state == 0) && (new_speed < trigger_speed - hysteresis ))  { 
        if (fan_delay == false) {
          fan_delay = true;
          fan_time = millis(); }
        else {
          if (millis() - fan_time > fan_delay_time) {
          // old_pkt.state = pkt.state;  // not implemented as no way of testing now, but might be required  
          pkt.state = 1; 
          digitalWrite(RELAY, LOW); // turn fan on
          fan_delay = false;
          }
        }
      }
      if ((pkt.state == 1) && (new_speed > trigger_speed + hysteresis ))  { 
        // old_pkt.state = pkt.state;  // not implemented as no way of testing now, but might be required
        pkt.state = 0; 
        digitalWrite(RELAY, HIGH);// turn fan off
      }
    if ((pkt.state == 0) && (new_speed > trigger_speed + hysteresis ))  { fan_delay = false; }  // reset the fan turn-on delay
    if (pkt.state != old_pkt.state) {  tx = true; }
    if (pkt.numSV != old_pkt.numSV) {  tx = true; }
    if (new_speed != old_speed ) { tx = true; }
    if (tx) { 
      send_packet(pkt);
      old_pkt.spd = pkt.spd;
      old_pkt.numSV = pkt.numSV;
      old_pkt.state = pkt.state;
      tx = false;
      } else { delay(200); } 
    }  
    digitalWrite(LED_BUILTIN, LOW);  
  }
}
