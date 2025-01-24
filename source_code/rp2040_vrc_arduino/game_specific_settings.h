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
          _gp.axis[axis] = 0;
        }
      }
      else{
        for(uint8_t axis = 0; axis < 4; ++axis){
          if((_changeflags>>axis) & 0x01){
            _gp.axis[axis] = scale_pwm(axis);
          }
        }
        _gp.axis[4] = 0;
        _gp.axis[5] = 0;
      }
      break;

    case circuit_superstars:
      if(no_signal){
        for(uint8_t axis = 0; axis < 6; ++axis){
          _gp.axis[axis] = 0;
        }
      }
      else{
        _gp.axis[0] = scale_pwm(0);
        _gp.axis[1] = 0;
        int16_t thr = scale_pwm(1);
        if(thr > 0){
          _gp.axis[2] = thr;
          _gp.axis[3] = 0;
        }
        else{
          _gp.axis[3] = -thr;
          _gp.axis[2] = 0;   
        }
        _gp.axis[4] = 0;
        _gp.axis[5] = 0;
      }
      break;
  }
}