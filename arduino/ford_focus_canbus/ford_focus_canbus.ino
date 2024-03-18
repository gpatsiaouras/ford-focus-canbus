#include <mcp_can.h>
#include <SPI.h>
#include "PWM.hpp"
#include "defaults.h"

PWM illum_pwm(in_illum_pin);

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
#define CAN0_INT 2
MCP_CAN CAN0(10);
Status status;


void setup() {
  Serial.begin(115200);
  
  pinMode(out_illum_pin, OUTPUT);
  pinMode(reverse_trigger_pin, OUTPUT);
  pinMode(park_brake_trigger_pin, OUTPUT);
  
  // Defaults
  digitalWrite(reverse_trigger_pin, HIGH);
  digitalWrite(park_brake_trigger_pin, LOW);
  
  // Initiate illumination pwm reader
  illum_pwm.begin(false);

  // Initiate canbus
  if(CAN0.begin(MCP_ANY, CAN_125KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.println("MCP2515 Initialized Successfully!");
  } else {
    Serial.println("Error Initializing MCP2515...");
  }
  CAN0.setMode(MCP_NORMAL);
  pinMode(CAN0_INT, INPUT);
}

void debug_print_illum() {
  Serial.print(illum_pwm.getValue());
  Serial.print(" - ");
  Serial.print(illum_pwm.getAge());
  Serial.print(" - ");
}

void set_illum() {
  if (illum_pwm.getValue() > 0 && illum_pwm.getAge() < 100000) {
    digitalWrite(out_illum_pin, LOW);
  } else {
    digitalWrite(out_illum_pin, HIGH);
  }
}

void loop() {
  set_illum();
  // debug_print_illum();

  if(!digitalRead(CAN0_INT)) {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);
    if (rxId == CAN_ID_STATE_PARK_REV) {
      memcpy(status.bytes, rxBuf, sizeof(status));
      digitalWrite(park_brake_trigger_pin, status.parking_en);
      digitalWrite(reverse_trigger_pin, ! status.reverse_en);
    }
  }
}
