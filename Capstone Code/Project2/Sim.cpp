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
	wh_per_mile = bat_voltage * (90.f/50.f); 

	// Now given bat_size which is our availible Kwh we can calculate the distance that can be traveled
	return (bat_size / wh_per_mile) * 1000.f; // * 1000 to convert to miles
}