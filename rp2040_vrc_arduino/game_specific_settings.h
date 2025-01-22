//game list
enum game_t{
  vrc = 1,
  circuit_superstars = 2,
  wrap
}game = vrc; //Specify default game

//game-dependent input-output translation
void update_gp_by_game(uint8_t _changeflags, hid_custom_gamepad_report_t &_gp, uint32_t *_pwm_in, bool no_signal){
  switch(game){
    case vrc:
      if(no_signal){
        for(uint8_t axis = 0; axis < 6; ++axis){
          _gp.axis[axis] = MID_HID_SIGNAL;
        }
      }
      else{
        for(uint8_t axis = 0; axis < 4; ++axis){
          if(_changeflags>>axis & 0x01){
            _gp.axis[axis] = scale_pwm(axis, MIN_HID_SIGNAL, MAX_HID_SIGNAL);
          }
        }
        _gp.axis[4] = MID_HID_SIGNAL;
        _gp.axis[5] = MID_HID_SIGNAL;
      }
      break;

    case circuit_superstars:
      if(no_signal){
        for(uint8_t axis = 0; axis < 6; ++axis){
          _gp.axis[axis] = MID_HID_SIGNAL;
        }
      }
      else{
        _gp.axis[0] = _pwm_in[0];
        _gp.axis[1] = MID_HID_SIGNAL;
        if(_pwm_in[1] > MID_HID_SIGNAL){
          _gp.axis[2] = _pwm_in[1];
          _gp.axis[3] = MID_HID_SIGNAL;
        }
        else{
          _gp.axis[3] = MAX_HID_SIGNAL-_pwm_in[1];
          _gp.axis[2] = MID_HID_SIGNAL;   
        }
        _gp.axis[4] = MID_HID_SIGNAL;
        _gp.axis[5] = MID_HID_SIGNAL;
      }
      break;
  }
}