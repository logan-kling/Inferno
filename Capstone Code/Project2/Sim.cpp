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
	return ((charge * weight) / (consumption * inclineFactor + speed * resistance));
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
		speeds[j] = (double)((charge * weight) / (consumption * inclineFactor + itterator * resistance));
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