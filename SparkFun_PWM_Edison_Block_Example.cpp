/****************************************************************************
 * SparkFun_PWM_Edison_Block_Example.cpp
 * Example code showing how to use the SparkFun PWM Edison Block
 * Mike Hord @ SparkFun Electronics
 * 9 June 2015
 * <github repository address>
 *
 * This file is a demonstration program showing the various functions that we've
 * provided for working with the PCA9685 IC on the SparkFun PWM Edison Block.
 * It uses an RGB LED and a small servo motor to show what the library can do.
 *
 * Resources:
 * Requires Intel's MRAA framework. This can be downloaded from either the
 * GitHub site (https://github.com/intel-iot-devkit/mraa) or in pre-built form
 * from http://iotdk.intel.com/sdk/mraa-update/.
 *
 * Development environment specifics:
 * Developed in the Intel iot-ide-dk Eclipse on Win 7 (v1.0.0.201502201135)
 * Using lib-mraa v0.6.2
 * On Edison poky-linux image build ww18-15
 *
 * This code is beerware; if you see me (or any other SparkFun employee) at the
 * local, and you've found our code helpful, please buy us a round!
 * ****************************************************************************/

#include "mraa.hpp"
#include "SparkFun_pca9685_Edison.h"
#include <iostream>
#include <unistd.h>

using namespace std;

// These channel definitions mirror the ones used in the PWM Block hookup guide.
#define SERVO 0
#define RED 2
#define GREEN 3
#define BLUE 4
  
// Uncomment one or both of these defines to enable the appropriate demo. Do
//  note servo motors and LEDs are best used at different frequencies and
//  polarities, so hooking both at once will give you bad results.
#define SERVO_DEMO
//#define LED_DEMO

// main() runs once and completes; there's no infinite loop here. Do note,
//  though, that whatever settings you write to the PWM module will persist
//  after the code has completed.
int main()
{
  // Variables to be used elsewhere in the program.
  uint16_t startTime, stopTime;

  mraa::I2c* pwm_i2c;   // We need to create an I2c object that we can pass to
                        //  the pca9685 constructor. If you have more than one
                        //  PCA9685 device on your bus (either by stacking more
                        //  than one PWM block or by adding external boards via
                        //  the I2C Expansion Block), you'll need to create a
                        //  different I2c object for each one!
  pwm_i2c = new mraa::I2c(1); // Tell the I2c object which bus it's on.

  pca9685 pwm(pwm_i2c, 0x40); // 0x40 is the default address for the PCA9685.

  // In general usage, you don't need to worry about getting or setting the
  //  mode registers or the prescaler register. I'm including these lines here
  //  just for example completeness purposes.
  cout<<"Current mode register values: 0x"<<hex<<pwm.readModeRegisters()<<endl;
  cout<<"Current prescaler: "<<dec<<static_cast<int16_t>(pwm.getPrescaler())<<endl;
  pwm.setPrescaler(121);

  // There are four generic functions allowing the user to change the start and
  //  stop times of the various channels. Generally, however, you shouldn't ever
  //  have to or want to use these, since there are more useful functions
  //  available which will be covered below.
  pwm.setChlTime(RED, 0, 0);
  pwm.setChlDuty(BLUE, 0);
  pwm.setChlStart(GREEN, 0);
  pwm.setChlStop(GREEN, 0);

  pwm.setChlDuty(SERVO, 0);

#ifdef LED_DEMO
  // When you call enableLEDMode(), you set the output to be approximately 400Hz
  //  and inverted. Thus, a 0 output will be a 100% high output. This allows us
  //  to use the output to drive common anode LEDs. See the tutorial for an
  //  example circuit.
  pwm.enableLEDMode();
  // Back to this, just so you can compare the settings in LED mode with the
  //  default (which is actually LED mode).
  cout<<"Current mode register values: "<<hex<<pwm.readModeRegisters()<<endl;
  cout<<"Current prescaler: "<<dec<<static_cast<int16_t>(pwm.getPrescaler())<<endl;
  for (uint8_t i = 0; i <= 100; i++)
  {
    // Check and print the start and stop times for the RED channel, then set
    //  all three to one percentage point higher. You'll note that the start
    //  time is always 0, and that the stop times increase logarithmically. This
    //  lets us get a visually linear brightness out of the LEDs.
	  pwm.getChlTime(RED, &startTime, &stopTime);
	  cout<<"Start time: "<<dec<<startTime<<endl;
	  cout<<"Stop time: "<<dec<<stopTime<<endl;
	  pwm.setChlLEDPercent(BLUE, i);
	  pwm.setChlLEDPercent(GREEN, i);
	  pwm.setChlLEDPercent(RED, i);
	  usleep(100000);
  }
  sleep(4);
  pwm.setChlTime(RED, 0, 0);
  pwm.setChlTime(GREEN, 0, 0);
  pwm.setChlTime(BLUE, 0, 0);
  cout<<"LED demo complete!"<<endl;
#endif

#ifdef SERVO_DEMO
  // We can set or get the minimum and maximum angles the angle set function
  //  expects to see.
  int16_t servoMinAngle, servoMaxAngle;
  pwm.getServoAngleLimits(&servoMinAngle, &servoMaxAngle);
  cout<<"Current servo min angle: "<<dec<<servoMinAngle<<endl;
  cout<<"Current servo max angle: "<<dec<<servoMaxAngle<<endl;

  // Likewise, we can set the min and max of pulse widths. Each count here is
  //  about 4.5us, depending on the clock's accuracy.
  uint16_t minServoPL, maxServoPL;
  pwm.getServoAnglePulseLimits(&minServoPL, &maxServoPL);
  cout<<"Current servo min pulse length: "<<dec<<minServoPL<<endl;
  cout<<"Current servo max pulse length: "<<dec<<maxServoPL<<endl;

  // These numbers are based on experimentation with SparkFun's generic
  //  sub-micro servo motor. You may find that they are too high or too low for
  //  your particular motor. The generic settings are fairly conservative and
  //  there is no need to use these functions unless you feel like you can get
  //  a wider range of motion by doing so.
  servoMinAngle = 0;
  servoMaxAngle = 160;
  minServoPL = 108;
  maxServoPL = 450;
  pwm.setServoAnglePulseLimits(minServoPL, maxServoPL);
  pwm.setServoAngleLimits(servoMinAngle, servoMaxAngle);

  // enabling servo mode makes the output active high and sets the frequency to
  //  approximately 50Hz.
  pwm.enableServoMode();

  // For comparison against default values or LED mode values.
  cout<<"Current mode register values: 0x"<<hex<<pwm.readModeRegisters()<<endl;
  cout<<"Current prescaler: "<<dec<<static_cast<int16_t>(pwm.getPrescaler())<<endl;

  // This steps through the full range of your servo's rotation. It also shows
  //  the start and stop time of the pulses, so you can see how those correspond
  //  to different positions.
  for (int16_t i = servoMinAngle; i <= servoMaxAngle; i++)
  {
	  pwm.getChlTime(SERVO, &startTime, &stopTime);
	  pwm.setChlAngle(SERVO, i);
	  cout<<"Start time: "<<dec<<startTime<<endl;
	  cout<<"Stop time: "<<dec<<stopTime<<endl;
	  usleep(100000);
  }
  cout<<"Servo demo complete!"<<endl;
#endif

  return MRAA_SUCCESS;
}
