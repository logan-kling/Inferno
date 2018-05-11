#include "Source.h"


wxIMPLEMENT_APP(MinApp);

// The default constructor for our frame
// This is the highest level window. Everything in the GUI is
// some level of child of 'this'
MinFrame::MinFrame(const wxString& title) 
	: wxFrame(NULL, wxID_ANY, title)
{

	// ---------------------------------------------------
	// MENU IMPLEMENTATION
	// ---------------------------------------------------

	menuBar = new wxMenuBar();
	fileMenu = new wxMenu();
	editMenu = new wxMenu();
	helpMenu = new wxMenu();

	// File Menu
	fileMenu->Append(File_Load, "&Load Car Settings", "Load a previous run");
	fileMenu->Append(File_Save, "&Save Car Settings", "Save the current run");
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, "&Exit", "Quit the program");
	menuBar->Append(fileMenu, "&File");

	// Edit Menu
	editMenu->Append(Edit_Clear, "&Clear Forms", "Empty all the form fields");
	editMenu->Append(Edit_Run, "&Run", "Begin or end the simulation");
	editMenu->Append(Edit_Launch_Map, "&Open Elevation Map", "Open the browser to get elevation data");
	editMenu->Append(Edit_Import, "&Import Elevation", "Import elevation data from Google");
	menuBar->Append(editMenu, "&Edit");

	// Help Menu
	helpMenu->Append(wxID_ABOUT, "&About", "Version Info");
	helpMenu->Append(Help_Walkthrough, "&Walkthrough", "Guide for program use");
	menuBar->Append(helpMenu, "&Help");

	SetMenuBar(menuBar);

	// ---------------------------------------------------
	// DUAL PANEL IMPLEMENTATION (Input/Output)
	// ---------------------------------------------------

	// Create a split window so we can have input and output side by side
	wxSplitterWindow *splitter = new wxSplitterWindow(this, 
		wxID_ANY, wxDefaultPosition, wxSize(400, 400), wxSP_3D);

	// Create input and output pannels with the splitter window as their parent
	in_p = new wxPanel(splitter);
	out_p = new wxPanel(splitter);
	splitter->SplitVertically(in_p, out_p, 0.5);
	splitter->SetMinimumPaneSize(20);

	// ---------------------------------------------------
	// INPUT PANEL IMPLEMENTATION
	// ---------------------------------------------------

	wxArrayString choices;
	choices.Add("Get Distance");
	choices.Add("Get Best Speed");
	choices.Add("Get Course");

	/*	Create and prepare elevation graph	*/
	
	elevationGraph = new Graph("Elevation", "X: Distance(meters), Y: Height(meters)",in_p);
	
	//elevationLayer = new mpFXYVector(_("Elevation"));
	//elevationGraph = new mpWindow(in_p, -1, wxPoint(0, 0), wxSize(300, 300), wxSUNKEN_BORDER);
	//prepareGraph(elevationGraph);


	/*	Create the form fields	*/
	i_sizer = new wxStaticBoxSizer(wxVERTICAL, in_p, "Input");
	course_sizer = new wxGridSizer(2, 5, 10);
	consumption = new Field("Motor Consumption:", in_p);
	weight = new Field("Weight (kg):", in_p);
	resistance = new Field("Vehicle Drag (const):", in_p);
	charge = new Field("Charge (WattHours):", in_p);
	speed = new Field("Speed (km/h):", in_p);
	incline = new Field("Incline:", in_p);
	routeDistance = new Field("Route Distance (meters):", in_p);
	routeSamples = new Field("Number of samples:", in_p);


	// These inputs are only used in 'Get Course' so they are in their own sizer
	solarInput = new Field("Solar input (WattHours):", in_p); 
	solarStrength = new Field("Solar Strength (% 1.00 - 0.0):", in_p);
	efficiency = new Field("Motor Efficiency (% 1.00 - 0.0):", in_p);
	horsepower = new Field("Motor Horsepower (Hp):", in_p);
	solarInput->asizer(course_sizer);
	solarStrength->asizer(course_sizer);
	horsepower->asizer(course_sizer);
	efficiency->asizer(course_sizer);

	/*	Create the radio button selector and set it  */
	buttonGroup = new wxRadioBox(in_p, wxID_RADIOBOX, "Run Mode:", wxDefaultPosition, wxDefaultSize, choices, 3, wxVERTICAL);
	buttonGroup->SetSelection(1);
	i_sizer->Add(buttonGroup);

	/*  Add all form fields to our sizer  */
	i_sizer->Add(consumption->label);
	i_sizer->Add(consumption->field,0, wxALL, 10); 

	i_sizer->Add(weight->label);
	i_sizer->Add(weight->field, 0, wxALL, 10);

	i_sizer->Add(resistance->label);
	i_sizer->Add(resistance->field, 0, wxALL, 10);

	i_sizer->Add(charge->label);
	i_sizer->Add(charge->field, 0, wxALL, 10);

	i_sizer->Add(speed->label);
	i_sizer->Add(speed->field, 0, wxALL, 10);

	i_sizer->Add(incline->label);
	i_sizer->Add(incline->field, 0, wxALL, 10);

	//Add the course sizer
	i_sizer->Add(course_sizer, 0, wxALL, 10);

	elevationGraph->wrapper->Add(routeDistance->label);
	elevationGraph->wrapper->Add(routeDistance->field, 0, wxALL, 10);
	elevationGraph->wrapper->Add(routeSamples->label);
	elevationGraph->wrapper->Add(routeSamples->field, 0, wxALL, 10);

	i_sizer->Add(elevationGraph->wrapper, 0, wxEXPAND, 10);

	in_p->SetSizer(i_sizer, wxEXPAND);
	i_sizer->SetSizeHints(in_p);

	// ---------------------------------------------------
	// OUTPUT PANEL IMPLEMENTATION
	// ---------------------------------------------------

	o_sizer = new wxStaticBoxSizer(wxVERTICAL, out_p, "Output");
	bestSpeed = new Field("Best Speed(km/h):", out_p);
	bestDistance = new Field("Distance(meters):", out_p);
	tripTime = new Field("Total Trip Time(hours):", out_p);
	chargeTime = new Field("Time Spent Recharging(hours):", out_p);

	//Assign a graph element to the outputGraph variable
	outputGraph = new Graph("Speed", "X: Distance(km), Y: Speed(km/h)", out_p);
	outputGraph2 = new Graph("Battery", "X: Distance(km), Y: Charge(KWh)", out_p);

	o_sizer->Add(bestSpeed->label, 0, wxALL, 10);
	o_sizer->Add(bestSpeed->field, 0, wxALL, 10);
	o_sizer->Add(bestDistance->label, 0, wxALL, 10);
	o_sizer->Add(bestDistance->field, 0, wxALL, 10);

	o_sizer->Add(tripTime->label, 0, wxALL, 10);
	o_sizer->Add(tripTime->field, 0, wxALL, 10);
	o_sizer->Add(chargeTime->label, 0, wxALL, 10);
	o_sizer->Add(chargeTime->field, 0, wxALL, 10);

	// Add the graph element
	o_sizer->Add(outputGraph->wrapper, 1, wxEXPAND);
	o_sizer->Add(outputGraph2->wrapper, 1, wxEXPAND);

	out_p->SetSizer(o_sizer, wxEXPAND);
	o_sizer->SetSizeHints(out_p);

	// ---------------------------------------------------
	// INITIALIZE PROGRAM SIZING
	// ---------------------------------------------------

	SetMinClientSize(wxSize(600, 600));
	SetClientSize(600, 650);

	//These items need to be hidden since we are starting on radio button 1
	// and unfortunately initializing the radio button selection does not call
	// our OnRadioBoxChange!
	routeDistance->field->SetEditable(0);
	routeSamples->field->SetEditable(0);
	speed->hide();
	elevationGraph->hide();
	outputGraph2->hide();
	tripTime->hide();
	chargeTime->hide();
	i_sizer->Hide(course_sizer, true);

	in_p->Layout();
	out_p->Layout();
}

// This is basically the 'main()' function, program starts here
bool MinApp::OnInit() 
{
	MinFrame* frame = new MinFrame("SolarSim");
	frame->Show(true);
	return true;
}

int MinApp::OnExit()
{
	return 0;
}


// This is a helper function for saving the input forms to a file.
// If this function is changed, make sure it matches InputPanel::LoadInputForms()
std::vector<std::string> MinFrame::SaveInputForms()
{
	std::vector<std::string> toSave;
	toSave.clear();
	toSave.push_back(consumption->gets());
	toSave.push_back(weight->gets());
	toSave.push_back(resistance->gets());
	toSave.push_back(charge->gets());
	toSave.push_back(incline->gets());
	toSave.push_back(speed->gets());
	toSave.push_back(solarInput->gets());
	toSave.push_back(solarStrength->gets());
	toSave.push_back(efficiency->gets());
	toSave.push_back(horsepower->gets());
	return toSave;
}

// This is a helper function for loading the input forms from a file
// If this function is changed, make sure it matches InputPanel::SaveInputForms()
void MinFrame::LoadInputForms(std::vector<std::string> loaded)
{
	consumption->set(loaded[0]);
	weight->set(loaded[1]);
	resistance->set(loaded[2]);
	charge->set(loaded[3]);
	incline->set(loaded[4]);
	speed->set(loaded[5]);
	solarInput->set(loaded[6]);
	solarStrength->set(loaded[7]);
	efficiency->set(loaded[8]);
	horsepower->set(loaded[9]);

}

void MinFrame::SetElvFields(float distance, float samples)
{
	routeDistance->field->SetValue(wxString(std::to_string(distance)));
	routeSamples->field->SetValue(wxString(std::to_string(samples)));
}


void MinFrame::HandleMainCalcs(double charge, double weight, double resistance)
{
	MyCar *car = new MyCar();
	car->solarStrength = solarStrength->get();
	car->wattPower = HpToWatt(horsepower->get());
	car->motorEfficiency = efficiency->get();
	car->solarWattsPeak = solarInput->get();
	car->doMainCalcs(charge, weight, resistance, distance, samples, elevations);
	std::vector<double> xAxis;
	for (int i = 1; i <= samples-1; i++) {
		xAxis.push_back(i);
	}

	tripTime->set(car->driveTime);
	chargeTime->set(car->rechargeTime);

	outputGraph->setGraph(xAxis, car->velocities, *wxBLUE);
	outputGraph2->setGraph(xAxis, car->charges, *wxGREEN);

}


/* This Function looks at the radio button box in InputPanel and is called when
*  that changes. 
*  When called, depending on the option it should change which simulation function 
*  gets run and which fields are displayed.
*/
void MinFrame::OnRadioBoxChange(wxCommandEvent& event)
{
	if (buttonGroup->GetString(event.GetSelection()) == "Get Distance") {
		elevationGraph->hide();
		consumption->show();
		speed->show();
		incline->show();
		i_sizer->Hide(course_sizer, true);

		bestSpeed->hide();
		bestDistance->show();
		tripTime->hide();
		chargeTime->hide();
		outputGraph->hide();
		outputGraph2->hide();

		i_sizer->Layout();
		o_sizer->Layout();
		
	}
	else if (buttonGroup->GetString(event.GetSelection()) == "Get Best Speed") {
		elevationGraph->hide();
		consumption->show();
		speed->hide();
		incline->show();
		i_sizer->Hide(course_sizer, true);

		bestSpeed->show();
		bestDistance->show();
		tripTime->hide();
		chargeTime->hide();
		outputGraph->show();
		outputGraph2->hide();

		i_sizer->Layout();
		o_sizer->Layout();
	}
	else {
		elevationGraph->show();
		consumption->hide();
		speed->hide();
		incline->hide();
		i_sizer->Show(course_sizer, true);

		bestSpeed->hide();
		bestDistance->hide();
		tripTime->show();
		chargeTime->show();
		outputGraph->show();
		outputGraph2->show();

		i_sizer->Layout();
		o_sizer->Layout();
	}
}

// When the Run event is generated:
// Take the given input and the run option,
// Call the appropriate simulation function,
// Set the output field with the results of the run.
void MinFrame::OnRun(wxCommandEvent & event)
{
	//setspeedfield(testmiles(in_p->getconsumption(), in_p->getweight()));
	if (buttonGroup->GetSelection() == 0) {
		bestSpeed->set(0);
		bestDistance->set((getDistance(charge->get(),
			consumption->get(),
			incline->get(),
			weight->get(),
			speed->get(),
			resistance->get())));
	}
	else if (buttonGroup->GetSelection() == 1) {
		std::vector<double> speeds = getBestSpeed(consumption->get(),
			incline->get(),
			weight->get(),
			resistance->get(),
			charge->get());
		//read the max distance from the front of the vector
		bestDistance->set(speeds.front());

		//remove the first index of the vector entirely
		speeds.erase(speeds.begin());

		//read the max speed from the front of the vector
		bestSpeed->set(speeds.front());

		//remove the second (now first) index of the vector entirely
		speeds.erase(speeds.begin());

		std::vector<double> vec;
		for (int i = 16; i < speeds.size() + 16; i++) {
			vec.push_back((double)i);
		}

		//setGraph(outputGraph, speedLayer, vec, speeds, *wxBLUE);
		outputGraph->setGraph(vec, speeds, *wxBLUE);
	}
	else {
		HandleMainCalcs(charge->get(), weight->get(), resistance->get());
	}
}

// Open the Google Map webpage for the user to get elevation data from.
void MinFrame::OnMap(wxCommandEvent& event) {
	ShellExecute(NULL, L"open", L"file:///C\:\\Users\\tapoz\\Documents\\GitHub\\Inferno\\Capstone\ Code\\webviewCode\\route.html",
		NULL, NULL, SW_SHOWNORMAL);
}

// When the elevation event is generated:
// Open a file dialog to select which file to load from,
// parse the contents and send them to in_p memeber function:
// MakeElvGraph to create a graph from our vector.
void MinFrame::OnImport(wxCommandEvent & event) 
{
	wxString        file;						//File name
	wxFileDialog    fdlog(this);				//File load dialog object
	wxString        elv;		
	double			distVal, sampVal;			//to find the distance and samples 
	double			i, j = 0;					//iterators
	wxTextFile      tfile;						//file object
	std::vector<double> vectorY, vectorX;		//vectors for our graph


	// show file dialog and get the path to
	// the file that was selected.
	if (fdlog.ShowModal() != wxID_OK) return;
	file.Clear();
	file = fdlog.GetPath();
	

	// open the file
	tfile.Open(file);

	elv = tfile.GetFirstLine();
	std::string elvCString = elv.ToStdString();
	std::stringstream ss(elvCString);

	/* ### This section grabs FIRST the distance in meters from the file ###
	   ### THEN the number of samples from the file	### */
	ss >> i;
	distVal = i;
	if (ss.peek() == ',' || ss.peek() == ' ') {
		ss.ignore();
	}

	ss >> i;
	sampVal = i;
	if (ss.peek() == ',' || ss.peek() == ' ') {
		ss.ignore();
	}
	//wxMessageBox(wxString(std::to_string(distStr)));
	//wxMessageBox(wxString(std::to_string(sampStr)));
	SetElvFields(distVal, sampVal);
	double secDist = distVal / sampVal;
	/* ### This section reads in all the elevation points in the file ### */
	while (ss >> i) {
		vectorY.push_back(i);
		if (ss.peek() == ',' || ss.peek() == ' ') {
			ss.ignore();
		}
		vectorX.push_back(j * secDist);
		j++;
	}
	
	samples = sampVal;
	distance = distVal;
	elevations = vectorY;

	elevationGraph->setGraph(vectorX, vectorY, *wxBLUE);
	//setGraph(elevationGraph, elevationLayer, vectorX, vectorY, *wxBLUE);

}

void MinFrame::OnClear(wxCommandEvent & event)
{
	std::vector<std::string> zeroVec;
	for (int i = 0; i < 10; i++) {
		zeroVec.push_back(std::to_string(0.0));
	}
	LoadInputForms(zeroVec);
}

// When the OnSave event is generated:
// Open a file dialog and ask user for a name and a save path,
// Save the input fields to a file for later use.
void MinFrame::OnSave(wxCommandEvent & event)
{
	wxString        file;						//File name
	
	wxFileDialog
		saveFileDialog(this, _("Save txt file"), "", "",
			"text files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (saveFileDialog.ShowModal() != wxID_OK) return;
	file.Clear();
	file = saveFileDialog.GetPath();

	wxTextFile      saveFile(file);						//file object

	// First check if the file exists, if it does, clear it out and open it, if not then create it
	if (saveFile.Exists()) {
		saveFile.Open();
		saveFile.Clear();
	}
	else
		saveFile.Create();

	// Use the function to grab the inputs we need to save and store them in this string vector
	std::vector<std::string> savingThis = SaveInputForms();

	// Write those lines to the file
	for (std::vector<std::string>::iterator it = savingThis.begin(); it != savingThis.end(); ++it) {
		saveFile.AddLine(wxString(*it));
	}

	// Try to save the changes, if it fails give the user an error message
	if (!saveFile.Write())
		wxMessageBox("Failed to save file!");
}

// When the OnLoad event is generated:
// Ask the user for a file to load from,
// Read from the file and place the contents in the appropriate input columns.
void MinFrame::OnLoad(wxCommandEvent & event) 
{

	wxString        file;						//File name
	std::vector<std::string> loaded;

	wxFileDialog
		openFileDialog(this, _("Open txt file"), "", "",
			"text files (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() != wxID_OK) return;
	file.Clear();
	file = openFileDialog.GetPath();

	wxTextFile		openFile(file);						//file object

	if (!openFile.Exists()) {
		wxMessageBox("No such file exists");
		return;
	}

	openFile.Open();

	// Start at the first line
	loaded.push_back(openFile.GetFirstLine().ToStdString());
	// Then go from there
	while (!openFile.Eof()) {
		loaded.push_back(
			openFile.GetNextLine().ToStdString()
			);
	}

	if (loaded.size() < 10) {
		wxMessageBox(wxT("This load file was not valid!"));
	}
	else {
		LoadInputForms(loaded);
	}
		
}

void MinFrame::OnQuit(wxCommandEvent & event)
{
	elevationGraph->graph->Destroy();
	outputGraph->graph->Destroy();
	outputGraph2->graph->Destroy();
	Close(true);
}

void MinFrame::OnAbout(wxCommandEvent & event)
{
	wxMessageBox(wxT("Solar Car Simulation\n 2018 Senior Capstone\n Dennie Devito, Logan Kling, and Dakota Zaengle"), wxT("About"), wxICON_INFORMATION | wxOK);
}

void MinFrame::OnHelp(wxCommandEvent & event)
{
	wxString wlk = "This program has three run modes, selectable using the radio buttons.\n";
	wlk += "Get Distance uses a set speed and other variables to return the best case distance\n";
	wlk += "Get Best Speed uses an array of speeds from 16 to 65 to find the one that maximizes distance\n";
	wlk += "Get Course uses elevation input from the Google Maps webpage to calculate performance over long distance\n";
	wlk += " -The graph of charge will suddenly jump up when the charge reaches a minimum percent. ";
	wlk += "This is what happens when the car is low on power, the simulation calculates the time it would take to recharge the cells to a target percentage and then sets it to that percent. ";
	wlk += "\n\n";
	wlk += "To access the map, if you installed using the setup package, route.html will be located in your install folder. ";
	wlk += "If you are running from the github download, route.html will be in the project directory and the webviewCode folder. ";
	wxMessageBox(wlk, wxT("Walkthrough"), wxOK);
}

Field::Field(std::string name, wxPanel *parent)
{
	label = new wxStaticText(parent, -1, name);
	field = new wxTextCtrl(parent, -1, "0.0", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	parent_ref = parent;
}

double Field::get()
{
	return atof(field->GetValue());
}

std::string Field::gets()
{
	return field->GetValue();
}

void Field::set(double v)
{
	field->SetValue(std::to_string(v));
}

void Field::set(std::string s)
{
	field->SetValue(wxString(s));
}

void Field::asizer(wxGridSizer *siz)
{
	wxBoxSizer *box = new wxBoxSizer(wxVERTICAL);
	box->Add(label);
	box->Add(field);
	siz->Add(box);
}

void Field::hide()
{
	parent_ref->GetSizer()->Hide(field, true);
	parent_ref->GetSizer()->Hide(label, true);
}

void Field::show()
{
	parent_ref->GetSizer()->Show(label, true);
	parent_ref->GetSizer()->Show(field, true);
}

Graph::Graph(std::string name, std::string description, wxPanel * parent)
{
	parent_ref = parent;
	graph = new mpWindow(parent, wxID_ANY, wxPoint(0, 0), wxSize(300, 300), wxSUNKEN_BORDER);
	vector = new mpFXYVector(_("Vector"));
	wrapper = new wxStaticBoxSizer(wxVERTICAL, parent, name);
	legend = new wxStaticText(parent, -1, description);
	prepareGraph();

	wrapper->Add(legend);
	wrapper->Add(graph, 1, wxEXPAND);
}

void Graph::updateText(std::string new_legend)
{
	legend->SetLabelText(new_legend);
}

void Graph::setGraph(std::vector<double> vectorX, std::vector<double> vectorY, wxColor color)
{
	/* These two steps prevent the graph from being improperly deleted when you close the program after multiple runs */
	graph->DelAllLayers(false, true);			//Delete everything but the graph
	prepareGraph();								//Re-prepare the graph


	vector->SetData(vectorX, vectorY);			//Adds the x and y coords to the layer
	vector->SetContinuity(true);				//Draw lines in between the points
	wxPen vectorpen(color, 5, wxPENSTYLE_SOLID);	//Set line size and color
	vector->SetPen(vectorpen);					//gives the pen to the layer
	vector->SetDrawOutsideMargins(false);		//Makes sure the graph isnt drawn outside of the graph bounds
	graph->SetMargins(10, 10, 30, 60);			//Sets our margins, top->right->bottom->left
	graph->AddLayer(vector);					//Adds the plotted x/y coordinates to our graph
	graph->Fit();								//Zoom the graph properly after everything has been added
}

void Graph::prepareGraph()
{
	wxFont graphFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);//Set the font for the graph
	mpScaleX* xaxis = new mpScaleX(wxT("X"), mpALIGN_BOTTOM, true, mpX_NORMAL);		//Label the x axis
	mpScaleY* yaxis = new mpScaleY(wxT("Y"), mpALIGN_LEFT, true);					//Label the y axis
	xaxis->SetFont(graphFont);														//Set font for x axis
	yaxis->SetFont(graphFont);														//Set font for y axis
	xaxis->SetDrawOutsideMargins(false);											//Dont't draw the axis outside margins
	yaxis->SetDrawOutsideMargins(false);
	graph->AddLayer(xaxis);															//add the axis to the graph
	graph->AddLayer(yaxis);
	graph->EnableDoubleBuffer(true);												//reduces flicker when graph is drawn
	graph->SetMPScrollbars(true);													//adds scroll bars if the graph window is too small
}

void Graph::hide()
{
	parent_ref->GetSizer()->Hide(wrapper, true);
}

void Graph::show()
{
	parent_ref->GetSizer()->Show(wrapper, true);
}
