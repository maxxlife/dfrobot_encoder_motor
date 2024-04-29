//The sample code for driving one way motor encoder

//Updated by Maksim Masalski maxxliferobot@gmail.com
//Program calculates angle in radians and angular velocity in rad/s
//Output example:
//Wheel Speed: 5 rad/s
//Wheel Angle: 6.28 rad

//When your wheel makes 1 revolution or 360 degrees,
//wheel angle will be 6.28 radians, what is equal 2*PI according to the math laws
//Speed can be different

//Constant value below stores pulses per one revolution of the output gearbox shaft
//of the DFRobot Motor with encoder FIT0450
//https://wiki.dfrobot.com/Micro_DC_Motor_with_Encoder-SJ01_SKU__FIT0450
//This motor has encoder plastic wheel installed on the rear side of the motor shaft
//This encoder wheel has 16 magnets inside, installed on the radius of the plastic encoder wheel
//Motor has a gearbox with gear ratio 1:120, so to make one revolution of the gearbox shaft,
//motor will make 120 revolutions,
//meaning that hall sensor will register number of pulses=120*16=1920 pulses per one wheel revolution
#define PULSES_PER_REVOLUTION 1920

const byte encoder0pinA = 2;//A pin -> the interrupt pin 0
const byte encoder0pinB = 3;//B pin -> the digital pin 3
byte encoder0PinALast;
int duration;//the number of the pulses
boolean Direction;//the rotation direction
unsigned long this_time;
unsigned long last_time;
unsigned long elapsed;
double wheel_angle;
double wheel_velocity;
double wheel_position;
double _initial_angle;
volatile long* _encoderPosition;

double ticks2Angle(long position) {
  //function to convert ticks into angle
	return position * ((double)2 * M_PI / PULSES_PER_REVOLUTION );
}

double getAngle() {
    double current_angle = ticks2Angle(*_encoderPosition);
    return current_angle - _initial_angle;
}

void setup()
{
  _initial_angle=ticks2Angle(*_encoderPosition);
  Serial.begin(57600);//Initialize the serial port
  EncoderInit();//Initialize the module
}

void loop()
{
  //we are using micros to calculate angular velocity
  //because we need a period of time, to see change in angle
  this_time = micros();
  elapsed = this_time-last_time;
  last_time = this_time;
  //calculating delta of the wheel angle
  double delta_wheel = wheel_angle - wheel_position;
  wheel_position += delta_wheel;

  //calculating speed by doing simple physics equation (velocity = distance/time_passed)
  //multiply by 1000000 converts angular peed from rad/microseconds to rad/seconds
  wheel_velocity = (delta_wheel / elapsed) * 1000000;

  //printing values
  Serial.print("\nWheel Speed: ");
  Serial.print(wheel_velocity);
  Serial.println(" rad/s");
  Serial.print("Wheel Angle: ");
  Serial.print(wheel_angle);
  Serial.println(" rad");
  delay(100);
}

void EncoderInit()
{
  Direction = true;//default -> Forward
  pinMode(encoder0pinB,INPUT);
  attachInterrupt(0, wheelSpeed, CHANGE);
}

void wheelSpeed()
{
  //default function to calculate ticks of the motor
  int Lstate = digitalRead(encoder0pinA);
  if((encoder0PinALast == LOW) && Lstate==HIGH)
  {
    int val = digitalRead(encoder0pinB);
    if(val == LOW && Direction)
    {
      Direction = false; //Reverse
    }
    else if(val == HIGH && !Direction)
    {
      Direction = true;  //Forward
    }
  }
  encoder0PinALast = Lstate;

  if(!Direction)  duration++;
  else  duration--;

  //calculating final angle of the wheel
  wheel_angle = ticks2Angle(duration);
}