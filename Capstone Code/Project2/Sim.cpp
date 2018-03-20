#include "Sim.h"


// Everything math-y and not GUI can go here

float testIToO(float in1, float in2, float in3) {
	return (in1 + in2 * in3);
}

/*
* Using assumptions made here:
*  http://www.ev-propulsion.com/EV-calculations.html
* for practice/test functions
*/

// Taking in:
//	Volts in the battery pack
//  Kwh (Kiloawatt hours) of the battery pack
// Assuming:
//  Our car will use 90 amps at 50MPH
//  And goes 50 MPH the whole distance
// Output:
//  Distance traveled by vehicle on the charge
float testMiles(float bat_voltage, float bat_size) {

	float wh_per_mile;

	// The 90/50 is the amperage draw divided by the speed that requires that amperage draw
	wh_per_mile = bat_voltage * (90.f / 50.f);

	// Now given bat_size which is our availible Kwh we can calculate the distance that can be traveled
	return (bat_size / wh_per_mile) * 1000.f; // * 1000 to convert to miles
}

float getDistance(float charge, //The initial charge of the car
	float consumption, //How many Watt hours the car consumes per mile driven
	float incline, //The incline of the road the car's driving on (-50 < incline < 50)
	float weight, //The weight of the car
	float speed, //The speed the car will travel
	float resistance /*The coefficient of resistance of the car*/) {
	float inclineFactor; //turns incline into a number between 0.2 & 4
	if (incline == 0) //If the road is flat
		inclineFactor = 1;
	else if (incline > 0) //If the car goes uphill
		inclineFactor = incline / 50 + 1;
	else if (incline < 0) //If the car goes downhill
		inclineFactor = -1 * incline / 50;
	return ((charge / weight) / (consumption * inclineFactor + speed * resistance));
}

std::vector<double> getBestSpeed(float consumption, //How many Watt hours the car consumes per mile driven
	float incline, //The incline of the road the car's driving on (-50 < incline < 50)
	float weight, //The weight of the car
	float resistance, /*The coefficient of resistance of the car*/
	float charge  /*The initial charge of the car*/) {
	float inclineFactor; //turns incline into a number between 0.2 & 4
	if (incline == 0) //If the road is flat
		inclineFactor = 1;
	else if (incline > 0) //If the car goes uphill
		inclineFactor = incline / 50 + 1;
	else if (incline < 0) //If the car goes downhill
		inclineFactor = -1 * incline / 50;
	std::vector<double> speeds(51);// = new float[50];
	for (uint_fast8_t itterator = 15, j = 0; itterator <= 65; itterator++, j++) {
		speeds[j] = (double)((charge / weight) / (consumption * inclineFactor + itterator * resistance));
	}
	return speeds;
}

/* 
Drag Function from https://physics.info/drag/
pressure drag: R=1/2qCAv^2 or simple drag: R=-bv
a typical car has C of 0.25 to 0.35

Relating power: 
Simple drag: P=bv^(n+1)
Pressure drag: P=1/2qCAv^3

Variables: 
q is the fluid density,
C is the coefficient of drag,
A is the area,
v of course is the speed
*/

/*
Power or work to go up an given height function
P=W/t
W=(Sum of forces)(distance)+(mass)(gravity)(change in height)

(Power)(efficiency of the motor) = (mass)(gravity)(sin of the incline)
(Watts)(in %, the old car had 95%)=(kg  )(m/s^2  )(sin of the degrees)
*/

MyCar::MyCar()
{
}

MyCar::MyCar(double charge, double weight, double efficiency, double drag, double area)
{
	batteryCharge = charge;
	mass = weight;
	motorEfficiency = efficiency;
	dragCoef = drag;
	crossSec = area;
}

void MyCar::doMainCalcs(double charge, double weight, double drag, float distance, float samples, std::vector<double> elevations)
{
	batteryCharge = charge;
	mass = weight;
	motorEfficiency = 0.95;
	dragCoef = drag;
	crossSec = 1.;

	float secDistance = distance / samples;

	for (int i = 0; i < samples - 1; i++) {
		int roadAngle;
		int elvChange = elevations[i] - elevations[i + 1];
		
		roadAngle = sin(secDistance / elvChange);

		double velocity = bestVelocity(roadAngle);

		//To make sure our car does not exceed resonable speeds
		if (velocity > maxSpeed) {
			velocity = maxSpeed;
		}
		else if (velocity < minSpeed) {
			velocity = minSpeed;
		}

		velocities.push_back(velocity);

		double directPower = wheelEnergy(velocity, secDistance, roadAngle);

		double powerLoss = directPower * motorEfficiency;
		ChangeCharge(powerLoss);
		charges.push_back(batteryCharge);
		changes.push_back(i);
	}
}

double MyCar::bestVelocity(float roadAngle) {
	return ((0.95 * wattPower) / (mass * sin(roadAngle)));
}

double MyCar::wheelEnergy(double velocity, double distance, float roadAngle) {
	double energy;

	// The two zeros near the end are for rolling mass and acceleration which we are neglecting for now (will be updated later)
	energy = (1. / 3600.) * (mass * GRAVITY * (rollingResist * cos(roadAngle) + sin(roadAngle)) + 
		0.0386 * (AIR_DENSITY * dragCoef * crossSec * pow(velocity, 2.)) + (mass + 0.)*0.) * distance;
	
	return energy;
}

//function returns power in Watts needed to travel
double doPower(MyCar *car, double acceleration, double velocity, double roadAngle) {
	double powerMotor;
	double powerTravel;
	double powerAcceleration;

	powerMotor = car->motorEfficiency * pow((car->mass * acceleration + car->dragCoef * car->crossSec * pow(velocity, 2) +
		car->rollingResist * car->mass * GRAVITY + car->mass * GRAVITY * sin(roadAngle)), 2);

	powerTravel = velocity * (car->dragCoef * car->crossSec * pow(velocity , 2) +
		car->rollingResist * car->mass * GRAVITY + car->mass * GRAVITY * sin(roadAngle));

	powerAcceleration = car->mass * acceleration * velocity;

	return powerMotor + powerTravel + powerAcceleration;
}

/*
Conversion functions:
Hp to Watt
Watt to Hp
*/

double HpToWatt(double Hp) {
	return Hp * (double)745.699872;	//From Google
}

double WattToHp(double Watt) {
	return Watt * (double)0.00134102; //From Google
}
