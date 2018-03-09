#pragma once
#include <stdint.h>
#include <vector>

//Air density at 1 atm of pressure at 15 degrees Celcius (about 59 degrees Ferhente )
#define AIR_DENSITY 1.225

class MyCar {
public: 
	MyCar();					//Default constructor
	MyCar(double charge, double mass, double efficiency, double drag, double area);

	void ChangeCharge(double diff) { batteryCharge += diff; };	//Add or subtract from the charge

private:
	double batteryCharge;		//In KilowattHours	(KWh)
	double motorEfficiencty;	//In percent		(%)
	double mass;				//In Kilograms		(kg)
	double dragCoef;			//Drag Coefficient or C, dimensionless
	double crossSec;			//Cross section area for drag calculations

	/* These are the bounds set by the user, not the car's hard limit. */
	double maxSpeed;			//Maximum speed		(kph)
	double minSpeed;			//Minimum speed		(kph)

	/*This is the MAXIMUM input with perfect sunlight conditions to the
	 *	car from ALL the solar pannels		*/
	double solarInput;			//In Watts			(Watt)
};

float testIToO(float in1, float in2, float in3);
float testMiles(float bat_voltage, float bat_size);

float getDistance(float charge, //The initial charge of the car
	float consumption, //How many Watt hours the car consumes per mile driven
	float incline, //The incline of the road the car's driving on (-50 < incline < 50)
	float weight, //The weight of the car
	float speed, //The speed the car will travel
	float resistance /*The coefficient of resistance of the car*/);

std::vector<double> getBestSpeed(float consumption, //How many Watt hours the car consumes per mile driven
	float incline, //The incline of the road the car's driving on (-50 < incline < 50)
	float weight, //The weight of the car
	float resistance, /*The coefficient of resistance of the car*/
	float charge = 90000.0 /*The initial charge of the car*/);

double HpToWatt(double Hp);
double WattToHp(double Watt);