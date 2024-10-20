#include <mcp_can.h>
#include <SPI.h>
#include "defaults.h"

// Can bus related variables
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];
#define CAN0_INT 2
MCP_CAN CAN0(10);

// Status
Status status;
SeatbeltsStatus seatbelts_status;
SomethingStatus something_status;
EngineStatus engine_status;

// Camera system
bool reverse_trigger_override = false;
bool reverse_gear_engaged = false;
unsigned long reverse_gear_engaged_start = 0;
bool reversed_gear_engaged_before = false;

void setup()
{
  Serial.begin(115200);

  pinMode(out_illum_pin, OUTPUT);
  pinMode(reverse_trigger_pin, OUTPUT);
  pinMode(park_brake_trigger_pin, OUTPUT);
  pinMode(cameras_relay, OUTPUT);
  pinMode(in_second_cam_button_pin, INPUT);

  // Defaults
  digitalWrite(reverse_trigger_pin, HIGH);
  digitalWrite(park_brake_trigger_pin, LOW);

  // Initiate canbus
  if (CAN0.begin(MCP_ANY, CAN_125KBPS, MCP_8MHZ) == CAN_OK)
  {
    Serial.println("MCP2515 Initialized Successfully!");
  }
  else
  {
    Serial.println("Error Initializing MCP2515...");
  }
  CAN0.setMode(MCP_NORMAL);
  pinMode(CAN0_INT, INPUT);
}

int camera_button_is_pressed() {
  return !digitalRead(in_second_cam_button_pin);
}

bool camera_countdown_active() {
  return reverse_gear_engaged_start > 0;
}

bool ten_seconds_passed_with_camera_on() {
  return millis() > reverse_gear_engaged_start + 10 * 1000;
}

void control_reverse_trigger() {
  reverse_gear_engaged = status.reverse_en;
  if (reverse_gear_engaged) {
    // Driver engaged reverse, switch to reverse even if the button is on.
    // Serial.println("Turning on reverse camera.");
    digitalWrite(cameras_relay, LOW);
    reverse_gear_engaged_start = millis();
    digitalWrite(reverse_trigger_pin, ACTIVE);
  }

  reversed_gear_engaged_before = status.reverse_en;

  if (camera_countdown_active() && ten_seconds_passed_with_camera_on()) {
    // Serial.println("Countdown finish turning off camera.");
    digitalWrite(reverse_trigger_pin, INACTIVE);
    reverse_gear_engaged_start = 0;
    return;
  }

  // If reverse override is active then don't turn off the trigger for the reverse.
  if (reverse_trigger_override) return;
}

void read_second_camera_button()
{
  // Don't do anything if reverse gear is active
  // driver parking has priority.
  if (reverse_gear_engaged) return;
  if (camera_countdown_active()) {
    // Serial.println("Camera countdown active, will not switch to baby.");
    return;
  }

  if (camera_button_is_pressed())
  {
    digitalWrite(cameras_relay, HIGH);
    digitalWrite(reverse_trigger_pin, ACTIVE);
    reverse_trigger_override = true;
  }
  else
  {
    digitalWrite(cameras_relay, LOW);
    reverse_trigger_override = false;
    digitalWrite(reverse_trigger_pin, INACTIVE);
  }
}

void read_canbus()
{
  if (!digitalRead(CAN0_INT))
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);
    if (rxId == CAN_ID_STATE_VEHICLE)
    {
      memcpy(status.bytes, rxBuf, sizeof(status));

      digitalWrite(park_brake_trigger_pin, !status.parking_en);
      digitalWrite(out_illum_pin, !status.lights_on);
      control_reverse_trigger();
    }
    else if (rxId == CAN_ID_STATE_SEATBELTS)
    {
      memcpy(seatbelts_status.bytes, rxBuf, sizeof(seatbelts_status));
    }
    else if (rxId == CAN_ID_STATE_ENGINE)
    {
      memcpy(engine_status.bytes, rxBuf, sizeof(engine_status));
      // Serial.print("RPM: ");
      // Serial.print(engine_status.rpm / 4);
      // Serial.print("Speed: ");
      // Serial.print(engine_status.speed);
      // Serial.print(" Throttle: ");
      // Serial.println(engine_status.throttle);
      // speed = something
      // odo = something
    }
    else if (rxId == CAN_ID_STATE_SOMETHING)
    {
      memcpy(something_status.bytes, rxBuf, sizeof(something_status));
    }
  }
}

void loop()
{
  read_second_camera_button();
  read_canbus();
}
