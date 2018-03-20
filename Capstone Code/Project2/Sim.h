#pragma once
#include <stdint.h>
#include <vector>
#include <math.h>

//Air density at 1 atm of pressure at 15 degrees Celcius (about 59 degrees Ferhente )
#define AIR_DENSITY 1.225
#define GRAVITY 9.8 // acceleration due to gravity in m/s^2

class MyCar {
public: 
	MyCar();					//Default constructor
	MyCar(double charge, double weight, double efficiency, double drag, double area);
	
	void doMainCalcs(double charge, double weight, double drag, float distance, float samples, std::vector<double> elevations);

	double bestVelocity(float roadAngle);
	double wheelEnergy(double velocity, double distance, float roadAngle);

	void ChangeCharge(double diff) { batteryCharge += diff; };	//Add or subtract from the charge
	void WaitForRecharge();

	std::vector<double> velocities;
	std::vector<double> charges;

	std::vector<double> changes;

	double	wattPower = 149140;	// "horsepower" in watts (Watt)
	double	maxBatteryCharge;	// This is the maximum ammount the battery can be charged to in KilowattHours (KWh)
	double	batteryCharge;		// In KilowattHours	(KWh)
	double	targetCharge = 0.90;		// (%) When the battery hits minimum, the simulation should wait until the battery is recharged to this point
	double	minCharge = 0.05;			// (%) When the batter hits this level it pauses the car until it has recharged to the target
	double	motorEfficiency = 0.95;		// In percent		(%)
	double	mass;				// In Kilograms		(kg)
	double	dragCoef = 0.29;	// Drag Coefficient or C, dimensionless (Defaulting to a ford escape)
	double	crossSec = 1;		// Cross section area for drag calculations (m^3)
	double	rollingResist = 0.0055;		// Rolling resistance of the tires, default is the bmx tires used for solar cars

	/* These are the bounds set by the user, not the car's hard limit. */
	double maxSpeed = 96;			//Maximum speed		(kph)
	double minSpeed = 40;			//Minimum speed		(kph)

	/*This is the MAXIMUM input with perfect sunlight conditions to the
	 *	car from ALL the solar pannels		*/
	double solarWattsPeak;			//In Watts			(Watt)
	double solarSystemEfficiency = 0.80;   //in percent		(%)
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