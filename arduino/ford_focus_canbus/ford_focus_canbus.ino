#include "PWM.hpp"
#include <mcp2515.h>
#include "defaults.h"

struct can_frame canMsg;

MCP2515 mcp2515(10);

PWM illum_pwm(in_illum_pin);

void setup() {
  pinMode(out_illum_pin, OUTPUT);
  pinMode(reverse_trigger_pin, OUTPUT);
  pinMode(park_brake_trigger_pin, OUTPUT);
  
  // Defaults
  digitalWrite(reverse_trigger_pin, HIGH);
  digitalWrite(park_brake_trigger_pin, LOW);

  // Serial.begin(115200);
  
  illum_pwm.begin(false);
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();
}

void debug_print_illum() {
  Serial.print(illum_pwm.getValue());
  Serial.print(" - ");
  Serial.print(illum_pwm.getAge());
  Serial.print(" - ");
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) // To receive data (Poll Read)
  {
    if (canMsg.can_id == CAN_ID_STATE_PARK_REV) {
      const int state_int = canMsg.data[3];
      if (state_int == 0x00) {
        digitalWrite(reverse_trigger_pin, DISENGAGED);
        digitalWrite(park_brake_trigger_pin, DISENGAGED);
      } else if (state_int == 0x01) {
        digitalWrite(reverse_trigger_pin, DISENGAGED);
        digitalWrite(park_brake_trigger_pin, ENGAGED);
      } else if (state_int == 0x02) {
        digitalWrite(reverse_trigger_pin, ENGAGED);
        digitalWrite(park_brake_trigger_pin, DISENGAGED);
      } else if (state_int == 0x03) {
        digitalWrite(reverse_trigger_pin, ENGAGED);
        digitalWrite(park_brake_trigger_pin, ENGAGED);
      }
    }
  }
  // debug_print_illum();
  if (illum_pwm.getValue() > 0 && illum_pwm.getAge() < 100000) {
    digitalWrite(out_illum_pin, LOW);
    Serial.println(LOW);
  } else {
    digitalWrite(out_illum_pin, HIGH);
    Serial.println(HIGH);
  }
  

  delay(100);
}
