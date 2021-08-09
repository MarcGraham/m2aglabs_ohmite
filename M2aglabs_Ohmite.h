/*
  m2aglabs_Ohmite.h - Library for Ohmite FSP0(1/2/3)CE devices.
  Created by Marc Graham, July 2019.
  BSD License. 
  Version 1.0.1 -- fix for non samd boards
*/
// m2aglabs_Ohmite.h

#ifndef _M2AGLABS_OHMITE_h
#define _M2AGLABS_OHMITE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/*
	type:
	0 = Round
	1 = short linear
	2 = long linear
*/

class M2aglabs_Ohmite
{
public:
	/**
		Constructor - Builds an instance of a FSP03CE sensor

		@param WIPER - pin 4 of sensor. Connects Analog and divider
		@param V_REF - Connects to other side of voltage divider for WIPER
		@param D_0  - pin 2 of connector. This is the 0 degree reference
		@param D_120 - pin 3 of connector. This is the 120 degree reference
		@param D_240 - pin 1 of connector. This is the 240 degree reference. 
		@return  an instantiated sensor object of the round type
	*/
	M2aglabs_Ohmite(int WIPER, int V_REF, int D_0, int D_120, int D_240); //Round sensor
	/**
		Constructor - Builds an instance of a FSP0(1/2)CE

		@param WIPER - pin 3 of sensor. Connects Analog and divider
		@param V_REF - Connects to other side of voltage divider for WIPER
		@param V_1 - pin 1 of connector 
		@param V_2 - pin 2 of connector
		@param type - boolean true for 55mm sensor (FSP01CE), false for 100 mm FSP02CE 
	*/
	M2aglabs_Ohmite(int WIPER, int V_REF, int V_1, int V_2, boolean type = true); 
	/**
		getForce -- get the force reading from the sensor. Uses private helpers to get force depending on type (3 or 1/2)
		
		@retrun a float value representing the force applied to the sensor. 
	*/
	float getForce();
	/**
		getPosition -- will return number of mm from 0 for linear sensors, 0 - 360 for round

		@param tailToTip -- optional for linear sensors. If true (default) will read 0 at tail (connector) end
							and max at tip. If false, 0 is at the tip with max at the tail. 
							Has no effect on round sensors. 
		@return -- a 16 bit integer representing the position of the touch on the sensors active surface. 
				   Round - 0 to 360 degrees
				   Linear - 0 to max in mm. Currently 55 mm for the FSP01CE and 100 mm for the FSP02CE 

	*/
	int16_t getPosition(boolean tailToTip = true);
	/**
		begin - Initializes device. Pins are initialized here. 

		@param analogResolution  -- defaults to 10. This is used to calculate the voltage of the inputs to 
									derive the return values. SAMD boards may be up to 12 -- most avr and arm are 10. 
	*/
	void begin(int16_t analogResolution = 10, float voltage = 3.3);

	/*
		zeroOffset -- used to setup sensors low voltage setting. Usually not zero. Defaults are included
					  in the constructors for type. 

		@param zero_offset -- The stored value is a positive number. Negatives will return current value.
		@returns the current value. 
	*/
	uint16_t zeroOffset(int16_t zero_offset = -1)
	{
		if (zero_offset > -1) {
			_ZERO_OFFSET = zero_offset;
		}

		return _ZERO_OFFSET; 
	}

	/*
		readRange -- used to setup sensors range. Usually not zero. Defaults are included
					  in the constructors for type.

		@param read_range -- The stored value is a positive number. Negatives will return current value.
		@returns the current value.
	*/
	uint16_t readRange(int16_t read_range = -1)
	{
		if (read_range > -1) {
			_READ_RANGE = read_range;
		}

		return _READ_RANGE;
	}

private:
	float _getAngle(int16_t low, int16_t high, int16_t off, int16_t analog);
	float _getVoltage(float value);
  int16_t _getRoundPosition();
	float _getRoundForce();
	float _getLinearForce();
  int16_t _getLinearPosition(boolean tailToTip); 
	uint16_t _getMillimeters(uint16_t  volatge);

	/* Variables */
	uint8_t _type; // 0 = round, 1 = short , 2 = long 
	/*
		_tailToTip -- for linear only. Defines where 0 is. 
	*/
	boolean _tailToTip; 
	/*
		_ANALOG_RESOLUTION -- set to default for platform. If values are off try a different setting. 
	*/
	uint16_t  _ANALOG_RESOLUTION = 10; 
	float _VOLTAGE = 3.3 ;
	uint16_t _ZERO_OFFSET;
	uint16_t _READ_RANGE; 
	//Round
	int16_t _ANALOGIN;
	int16_t _D_0;
	int16_t _D_120;
	int16_t _D_240;
	//Linear
	int16_t _WIPER;
	int16_t _V_1;
	int16_t _V_2; 
	int16_t _V_REF; 
};


#endif
