/*
  m2aglabs_Ohmite.cpp - Library for Ohmite FSP0(1/2/3)CE devices.
  Copyright (c) 2021 m2ag.labs (https://m2aglabs.com)
  MIT License
*/

#include "M2aglabs_Ohmite.h"


M2aglabs_Ohmite::M2aglabs_Ohmite(int ANALOGIN, int WIPER, int D_0, int D_120, int D_240)
{
	_type = 0; 
	_ZERO_OFFSET = 800; 
	_READ_RANGE = 1450;
	_ANALOGIN = ANALOGIN;
	_WIPER = WIPER;
	_D_0 = D_0; 
	_D_120 = D_120;
	_D_240 = D_240;
}

M2aglabs_Ohmite::M2aglabs_Ohmite(int WIPER, int V_REF, int V_1, int V_2,  boolean type) {

	if (type) {
		_ZERO_OFFSET = 200;
		_READ_RANGE = 2600;
		_type = 1;
	}
	else {
		_type = 2;
		_ZERO_OFFSET = 500;
		_READ_RANGE = 1900;
	}

	_WIPER = WIPER; 
	_V_1 = V_1; 
	_V_2 = V_2; 
	_V_REF = V_REF; 


}

float M2aglabs_Ohmite::getForce()
{
	float force = 0.0;

	if (_type == 0) {
		force = _getRoundForce(); 
	}
	else {
		force = _getLinearForce();
	}
	return force; 
}


int16_t M2aglabs_Ohmite::getPosition(boolean tailToTip)
{
	int position = 0; 

	if (_type == 0) {//round 
		position = _getRoundPosition(); 
	}
	else { //it is either round or it is not. 
		position = _getLinearPosition(tailToTip);
	}

	return position; 
}

void M2aglabs_Ohmite::begin(int16_t analogResolution, float voltage) 
{
	_ANALOG_RESOLUTION = analogResolution; 
	_VOLTAGE = voltage;

	if (_type == 0) {
		// For the round sensor
		pinMode(_WIPER, INPUT);
		pinMode(_D_0, OUTPUT);
		pinMode(_D_120, OUTPUT);
		pinMode(_D_240, OUTPUT);
	}
	else {
		//linear Sensor
		pinMode(_V_1, OUTPUT);
		pinMode(_V_2, OUTPUT);
		pinMode(_WIPER, OUTPUT);
		pinMode(_V_REF, OUTPUT);
		//Set all low to clear the sensor 
		digitalWrite(_V_1, LOW);
		digitalWrite(_V_2, LOW);
		digitalWrite(_WIPER, LOW);
		digitalWrite(_V_REF, LOW);
	}
}



// private stuff 
int16_t M2aglabs_Ohmite::_getRoundPosition() {

	float v0 = 0.0, v120 = 0.0, v240 = 0.0;
	float angle = 0.0;
	//Step 1:
	/* read the value of all positions */
	digitalWrite(_D_0, LOW);
	digitalWrite(_D_120, HIGH);
	digitalWrite(_D_240, HIGH);
	delayMicroseconds(10);
	v0 = _getVoltage(analogRead(_ANALOGIN));

	digitalWrite(_D_0, HIGH);
	digitalWrite(_D_120, LOW);
	digitalWrite(_D_240, HIGH);
	delayMicroseconds(10);
	v120 = _getVoltage(analogRead(_ANALOGIN));

	digitalWrite(_D_0, HIGH);
	digitalWrite(_D_120, HIGH);
	digitalWrite(_D_240, LOW);
	delayMicroseconds(10);
	v240 = _getVoltage(analogRead(_ANALOGIN));

	//Determine which is closest to the touch 
	if (v0 < v120 && v0 < v240) {
		if (v120 < v240) {
			//Serial.print("v0:v120 :");
			angle = _getAngle(_D_0, _D_120, _D_240, _ANALOGIN);
		}
		else {
			//Serial.print("v0:v240 :");
			angle =_getAngle(_D_240, _D_0, _D_120, _ANALOGIN);
			angle += 240;
		}
	}

	if (v120 < v0 && v120 < v240) {
		if (v0 < v240) {
			//Serial.print("v240:v0 :");
			angle = _getAngle(_D_0, _D_120, _D_240, _ANALOGIN);
		}
		else {
			//Serial.print("v120:v240 :");
			angle = _getAngle(_D_120, _D_240, _D_0, _ANALOGIN);
			angle += 120.0;
		}
	}

	if (v240 < v0 && v240 < v120) {
		if (v0 < v120) {
			//Serial.print("v240:v0 :");
			angle = _getAngle(_D_240, _D_0, _D_120, _ANALOGIN);
			angle += 240;
		}
		else {
			//Serial.print("v120:v240 :");
			angle = _getAngle(_D_120, _D_240, _D_0, _ANALOGIN);
			angle += 120;
		}
	}
	//Angles are offset from their starting points to give 360 degrees
	return int16_t(angle);

}

/**


*/
float M2aglabs_Ohmite::_getRoundForce() {

	float force = 0.0; 
	pinMode(_WIPER, OUTPUT);
	digitalWrite(_WIPER, LOW);
	digitalWrite(_D_0, HIGH);
	digitalWrite(_D_120, HIGH);
	digitalWrite(_D_240, HIGH);
	delayMicroseconds(10);
	force = _getVoltage(analogRead(_ANALOGIN));
	pinMode(_WIPER, INPUT);
	return force; 
}


float M2aglabs_Ohmite::_getLinearForce() {

	//using method 3 from implementation pdf

	float Awiper_1, Awiper_2, force = 0.0;
	digitalWrite(_V_REF, LOW);

	pinMode(_WIPER, INPUT);

	pinMode(_V_1, OUTPUT);
	digitalWrite(_V_1, HIGH);

	pinMode(_V_2, INPUT);

	delayMicroseconds(10);

	Awiper_1 = analogRead(_WIPER);

	digitalWrite(_V_1, LOW);

	pinMode(_V_2, OUTPUT);
	digitalWrite(_V_2, HIGH);

	pinMode(_V_1, INPUT);

	delayMicroseconds(10);
	Awiper_2 = analogRead(_WIPER);

	//Reset Pins (all to LOW):
	pinMode(_V_1, OUTPUT);
	pinMode(_WIPER, OUTPUT);

	digitalWrite(_V_1, LOW);
	digitalWrite(_V_2, LOW);
	digitalWrite(_WIPER, LOW);

	force = _getVoltage((Awiper_1 + Awiper_2) / 2);
	
	return force; 
}


int16_t M2aglabs_Ohmite::_getLinearPosition(boolean tailToTip) {
	//Measured from the tail (0) to tip (max -- 55mm for short, 100 mm) 
	int16_t value = 0; 
	pinMode(_WIPER, INPUT);
	pinMode(_V_REF, INPUT);
  
	if (tailToTip) {
		digitalWrite(_V_1, HIGH);
	}
	else {
		//Read from the tip end
		digitalWrite(_V_2, HIGH);
	}
	delayMicroseconds(10);
	value = int16_t(_getVoltage(analogRead(_WIPER)) * 1000.00);


	//Drain voltage from the sensor before next read
	//This effects accuracy. 
	digitalWrite(_V_1, LOW);
	digitalWrite(_V_2, LOW);
	pinMode(_V_REF, OUTPUT);
	digitalWrite(_V_REF, LOW);
	pinMode(_WIPER, OUTPUT);
	digitalWrite(_WIPER, LOW);

	return _getMillimeters(value);

}


uint16_t M2aglabs_Ohmite::_getMillimeters(uint16_t  voltage){
	int position;

	if (_type == 1) {
		//short 
		position = ((voltage - _ZERO_OFFSET) * 55) / _READ_RANGE;
		//position = ((voltage - 500) * 55) / 1900;
		position = constrain(position, 0, 55);
	}
	else {
		position = ((voltage - _ZERO_OFFSET) * 100) / _READ_RANGE;
		//position = ((voltage - 200) * 100) / 2600;
		position = constrain(position, 0, 100);
	}

	return position;
}


float M2aglabs_Ohmite::_getAngle(int16_t low, int16_t high, int16_t off, int16_t analogin) {
	
	float angle = 0.0;

	pinMode(off, INPUT);

	digitalWrite(high, HIGH);
	digitalWrite(low, LOW);
	delayMicroseconds(10);


	angle = _getVoltage(analogRead(analogin)) * 1000; //convert to millivolts

	//Looking at the range of the voltage swing when I press on an angle, min should be about 800
	//and max is 2.250 (800 + 1450), the 1450 being the range.
	angle = ((angle - _ZERO_OFFSET) * 120.0) / _READ_RANGE;

	pinMode(off, OUTPUT);

	return angle;
}

float M2aglabs_Ohmite::_getVoltage(float value)
{
	return (value * _VOLTAGE) / pow(2, _ANALOG_RESOLUTION);
}
