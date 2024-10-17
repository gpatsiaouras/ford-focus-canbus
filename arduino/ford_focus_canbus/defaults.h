// Connections
const int in_illum_pin = 3;
const int in_second_cam_button_pin = 4;
const int out_illum_pin = 9;
const int reverse_trigger_pin = 8;
const int park_brake_trigger_pin = 7;
const int can_int_pin = 2;
const int cameras_relay = 5;

// Constants
// Due to optocouplers values are reversed
const int ACTIVE = 0;
const int INACTIVE = 1;

union Status
{
  struct __attribute__((__packed__)) //packed means that the struct will have exactly the ssize of bytes it contains
  {
    uint8_t unknown_1:1;
    uint8_t unknown_2:1;
    uint8_t unknown_3:1;
    uint8_t trunk_open:1;
    uint8_t codrivers_rear_door_open:1;
    uint8_t drivers_rear_door_open:1;
    uint8_t codrivers_door_open:1;
    uint8_t drivers_door_open:1;

    uint8_t second_byte;
    uint8_t third_byte;

    //fourth byte split two bits
    uint8_t parking_en:1;
    uint8_t reverse_en:1;
    uint8_t engine_on:1;
    uint8_t ac_on:1;
    uint8_t clima_on:1;
    uint8_t rear_window_defrost_on:1;
    uint8_t high_beam_on:1;
    uint8_t lights_on:1;
    // uint8_t:6; //reserved
    
    uint8_t fifth_byte;

    uint8_t:4; //reserved
    uint8_t lock_en:1;
    uint8_t unlock_en:1;
    uint8_t:2; //reserved

    uint8_t seventh_byte;
    uint8_t eighth_byte;
  };
  uint8_t bytes[8];
};

union EngineStatus
{
  struct __attribute__((__packed__)) //packed means that the struct will have exactly the ssize of bytes it contains
  {
    uint16_t rpm; // x4
    uint8_t third_byte;
     // speed=(XX-10,000)/100; 0x2710=stopped	
    uint8_t fourth_byte;
    uint16_t speed;
    uint16_t throttle;
  };
  uint8_t bytes[8];
};

union SeatbeltsStatus
{
  struct __attribute__((__packed__)) //packed means that the struct will have exactly the ssize of bytes it contains
  {
    uint8_t first_byte;

    //second byte split two bits
    uint8_t bit_1:1;
    uint8_t bit_2:1;
    uint8_t bit_3:1;
    uint8_t bit_4:1;
    uint8_t bit_5:1;
    uint8_t drivers_seatbelt:1;
    uint8_t codriver_seatbelt:1;
    uint8_t bit_8:1;
    // uint8_t:6; //reserved
    
    uint8_t third_byte;
    uint8_t fourth_byte;
    uint8_t fifth_byte;
    uint8_t sixth_byte;
    uint8_t seventh_byte;
    uint8_t eighth_byte;
  };
  uint8_t bytes[8];
};

union SomethingStatus
{
  struct __attribute__((__packed__)) //packed means that the struct will have exactly the ssize of bytes it contains
  {
    uint8_t first_bit_1:1;
    uint8_t first_bit_2:1;
    uint8_t first_bit_3:1;
    uint8_t first_bit_4:1;
    uint8_t first_bit_5:1;
    uint8_t first_bit_6:1;
    uint8_t first_bit_7:1;
    uint8_t first_bit_8:1;
    uint8_t second_bit_1:1;
    uint8_t second_bit_2:1;
    uint8_t second_bit_3:1;
    uint8_t second_bit_4:1;
    uint8_t second_bit_5:1;
    uint8_t second_bit_6:1;
    uint8_t second_bit_7:1;
    uint8_t second_bit_8:1;
    uint8_t third_bit_1:1;
    uint8_t hazard_lights_off:1; //meaning when on bit is 0
    uint8_t third_bit_3:1;
    uint8_t unlock_key_command:1;
    uint8_t trunk_open_command:1;
    uint8_t third_bit_6:1;
    uint8_t third_bit_7:1;
    uint8_t third_bit_8:1;
    
  };
  uint8_t bytes[3];
};

// CanBus
const unsigned long CAN_ID_STATE_VEHICLE = 0x433;
const unsigned long CAN_ID_STATE_ENGINE = 0x201;
// 7th and 8th byte is throttle pedal position
const unsigned long CAN_ID_STATE_PEDALS = 0x285;
// 1st byte 8 bit cluch pedal
// 2nd byte 8 bit brake pedal
const unsigned long CAN_ID_STATE_LOCK = 0x210;
const unsigned long CAN_ID_STATE_SEATBELTS = 0x460;
const unsigned long CAN_ID_STATE_SOMETHING = 0x501;