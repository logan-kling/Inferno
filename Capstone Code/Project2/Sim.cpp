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

float* getBestSpeed(float consumption, //How many Watt hours the car consumes per mile driven
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
	float* speeds = new float[50];
	for (uint_fast8_t itterator = 15, j = 0; itterator <= 65; itterator++, j++) {
		speeds[j] = (charge * weight) / (consumption * inclineFactor + itterator * resistance);
	}
	return speeds;
}