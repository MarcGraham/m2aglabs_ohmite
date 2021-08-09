/*
  m2aglabs_Ohmite.ino - Library for Ohmite FSP0(1/2/3)CE devices.
  Copyright (c) 2021 m2ag.labs (https://m2aglabs.com)
  MIT License.

  Version 1.0.1 -- fix for non SAMD boards
  This file sets up a long and round sensor by default. To add a third you can uncomment 
  the appropriate lines. 
  SAMD boards may have more than 10 bits analog readresolution. Override the default 
  by passing parameters in the begin() call.  
*/
#include "M2aglabs_Ohmite.h"




//#define ANALOG_RESOLUTION 12
#define LINEAR_THRESHOLD  0.3
#define ROUND_THRESHOLD  0.3


/*
	Round sensor
	WIPER, VREF, D120, D0, D240

	Linear Sensor
	WIPER, VREF, V1, V2, true/false (Short or Long)
	 
    WIPER and VREF are on two sides of a resistor. VREF floats for position measurements,
	PULLS low for force. 

*/

M2aglabs_Ohmite roundSensor(A1, 28, 18, 19, 17);
//M2aglabs_Ohmite sLinear(A2, 7, 6, 10, true);  //true means short
M2aglabs_Ohmite lLinear(A0, 22, 21, 20, false);  //false is long sensor 



void setup() {

	Serial.begin(115200);
	/*
		The lib is set for a default of 10 bits for analog resolution and 3.3. for voltage. If the voltage is 5.0,
		set it here. SAMD boards may have more bits resolution. 
	*/
	//analogReadResolution(ANALOG_RESOLUTION);
	roundSensor.begin();
	//sLinear.begin();
	lLinear.begin(); 

	//Set options --
	/*
	Serial.println(roundSensor.readRange());
	Serial.println(roundSensor.readRange(1500)); 
	Serial.println(roundSensor.zeroOffset());
	Serial.println(roundSensor.zeroOffset(500));
	*/

}

void loop() {
	roundSensorActions();
	linearSensorActions();
}




void linearSensorActions() {

	int lpos; //Position is an integer 
	float flp; //Force is a float

  /*
    int spos
	float fsp
	fsp = sLinear.getForce(); 
	
	if (fsp > LINEAR_THRESHOLD) { 
		//False reads from tail to tip. 
		spos = sLinear.getPosition(false);
		Serial.print("s: ");
		Serial.print(fsp);
		Serial.print(" : ");
		Serial.println(spos);

	}
 */

	flp = lLinear.getForce(); 
	if (flp > LINEAR_THRESHOLD) {
		lpos = lLinear.getPosition(false);	
		Serial.print("l: ");
		Serial.print(flp);
		Serial.print(" : ");
		Serial.println(lpos);
	}

}



void roundSensorActions() {

	//Get the force from the round sensor 
	float force = roundSensor.getForce();


	//IF it looks like we are touching it, calculate the position. 
	if (force > ROUND_THRESHOLD) {

		int angle = roundSensor.getPosition(); 
		Serial.print(" raw angle: ");
		Serial.println(angle);

		angle = constrain(angle, 0.0, 360.0);

		Serial.print(" adjusted: ");
		Serial.println(angle);

	}
	

	return;

} 
