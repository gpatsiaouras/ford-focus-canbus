// Connections
const int in_illum_pin = 3;
const int out_illum_pin = 9;
const int reverse_trigger_pin = 8;
const int park_brake_trigger_pin = 7;

union Status
{
  struct __attribute__((__packed__)) //packed means that the struct will have exactly the ssize of bytes it contains
  {
    uint8_t first_byte;
    uint8_t second_byte;
    uint8_t third_byte;

    //fourth byte split two bits
    uint8_t parking_en:1;
    uint8_t reverse_en:1;
    uint8_t:6; //reserved
    
    uint8_t fifth_byte;
    uint8_t sixth_byte;
    uint8_t seventh_byte;
    uint8_t eighth_byte;
  };
  uint8_t bytes[8];
};

// CanBus
const unsigned long CAN_ID_STATE_PARK_REV = 433;