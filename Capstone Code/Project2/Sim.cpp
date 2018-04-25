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
extern float testMiles(float bat_voltage, float bat_size) {

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
	return (float)((1000 * charge * log2((float)speed / 8 - (float)incline / 28 + 1)) / (consumption * inclineFactor * log(weight + 1) + speed * resistance));
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
	std::vector<double> speeds;
	speeds.reserve(53);
	speeds.emplace_back((double)((1000 * charge * log2((double)15 / 8 - (double)incline / 28 + 1)) / (consumption * inclineFactor * log(weight + 1) + 15 * resistance)));
	speeds.emplace_back((double)15);
	speeds.emplace_back(speeds.front());
	for (uint_fast8_t itterator = 16; itterator <= 65; itterator++) {
		speeds.emplace_back((double)((1000 * charge * log2((double)itterator / 8 - (double)incline / 28 + 1)) / (consumption * inclineFactor * log(weight + 1) + itterator * resistance)));
		if (speeds.back() > speeds.front()) {
			speeds.front() = speeds.back();
			speeds[1] = (double)itterator;
		}
	}
	return speeds;
}


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
	maxBatteryCharge = charge;
	mass = weight;
	motorEfficiency = efficiency;
	dragCoef = drag;
	crossSec = area;
}

void MyCar::doMainCalcs(double charge, double weight, double drag, float distance, float samples, std::vector<double> elevations)
{
	maxBatteryCharge = charge;
	batteryCharge = charge;
	mass = weight;
	motorEfficiency = 0.95;
	dragCoef = drag;
	crossSec = 1.;
	tripTime = 0.;
	solarWattsPeak = 300.; // (Watt) 0.3kW, this would be an input of 0.3kW per hour or 0.3kWh
	// solarSystemEfficiency is default to 80% so we will leave it at that
	// This means about 80% of whatever sunlight we get is lost in the system
	// solarStrength is set to 100%, this is the ammount of sunlight hitting the solar panels

	std::vector<double> velChoose;
	double numSpeeds = 10;
	double diffVel = maxSpeed - minSpeed;
	double changeVel = diffVel / numSpeeds;
	for (int i = 1; i <= numSpeeds; i++) {
		velChoose.push_back(minSpeed + changeVel * i);
	}

	float secDistance = distance / samples;

	for (int i = 0; i < samples - 1; i++) {
		int roadAngle;
		int elvChange = elevations[i] - elevations[i + 1];
		double secTime;
		
		roadAngle = sin(secDistance / elvChange);

		// The velocity for this section, limited by horsepower(wattpower) up a hill
		//double velocity = bestVelocity(roadAngle);

		//To make sure our car does not exceed resonable speeds
		/*if (velocity > maxSpeed) {
			velocity = maxSpeed;
		}
		else if (velocity < minSpeed) {
			velocity = minSpeed;
		}*/

		double directPower;
		double temp;
		int chosen = 0;
		directPower = wheelEnergy(velChoose[0], secDistance, roadAngle);
		for (int j = 1; j < velChoose.size() - 1; j++) {
			temp = wheelEnergy(velChoose[j], secDistance, roadAngle);
			if (temp < directPower) {
				directPower = temp;
				chosen = j;
			}
		}
		// velocity in m/s, distance in m, secTime in hours
		secTime = ((secDistance / velChoose[chosen]) / 60) / 60;

		
		double solarGain = (solarWattsPeak * solarStrength * solarSystemEfficiency) * secTime;
		double powerLoss = directPower / motorEfficiency;

		// If our current battery charge would drop below the minimum
		if ((batteryCharge - powerLoss) < (maxBatteryCharge * minCharge)) {
			WaitForRecharge(); //Then we should wait for the battery to charge back up
		}

		ChangeCharge(solarGain);
		ChangeCharge(-1 * powerLoss);

		charges.push_back(batteryCharge);
		velocities.push_back(velChoose[chosen]);

		//changes is just for testing purposes
		//changes.push_back(i);
	}
}

/* Take solar input from panels and the current charge, calculate the time
	until the current charge reaches the minimum charge. */
void MyCar::WaitForRecharge() {
	//For now I am just setting the battery to recharged, the wait will be implemented later.
	batteryCharge = maxBatteryCharge * targetCharge;
}

//not in use
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

//not in use
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
