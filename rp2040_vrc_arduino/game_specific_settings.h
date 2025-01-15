//RP2040 pins for RC PWM input (in order)
const uint8_t PWM_pins[4] = {0, 1, 2, 3};

//game list
enum game_t{
  vrc = 1,
  circuit_superstars = 2,
  wrap
}game = vrc; //Specify default game

//game-dependent input-output translation

void update_gp_by_game(uint8_t _changeflags, hid_custom_gamepad_report_t &_gp, uint32_t *_pwm_in){
  switch(game){
    case vrc:
      for(uint8_t axis = 0; axis < 4; ++axis){
        _gp.axis[axis] = _pwm_in[axis];
      }
      _gp.axis[4] = MAX_HID_SIGNAL/2;
      _gp.axis[5] = MAX_HID_SIGNAL/2;
      break;

    case circuit_superstars:
      _gp.axis[0] = _pwm_in[0];
      _gp.axis[1] = MAX_HID_SIGNAL/2;
      if(_pwm_in[1] > MAX_HID_SIGNAL/2){
        _gp.axis[2] = _pwm_in[1];
        _gp.axis[3] = MAX_HID_SIGNAL/2;
      }
      else{
        _gp.axis[3] = MAX_HID_SIGNAL-_pwm_in[1];
        _gp.axis[2] = MAX_HID_SIGNAL/2;    
      }
      _gp.axis[4] = MAX_HID_SIGNAL/2;
      _gp.axis[5] = MAX_HID_SIGNAL/2;
      break;
  }
}