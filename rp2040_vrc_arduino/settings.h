//Pins for RC PWM input
const uint8_t PWM_pins[4] = {0, 1, 2, 3};

//game list
typedef enum{
  vrc = 1,
  circuit_superstars = 2,
  wrap
}gm;

//Specify default game
gm game = vrc; 