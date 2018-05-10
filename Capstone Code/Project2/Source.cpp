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
	helpMenu->Append(wxID_ANY, "&Walkthrough", "Guide for program use");
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
	elevationLayer = new mpFXYVector(_("Vector"));
	elevationGraph = new mpWindow(in_p, -1, wxPoint(0, 0), wxSize(300, 300), wxSUNKEN_BORDER);
	prepareGraph(elevationGraph);


	/*	Create the form fields	*/
	i_sizer = new wxStaticBoxSizer(wxVERTICAL, in_p, "Input");
	route_sizer = new wxStaticBoxSizer(wxVERTICAL, in_p, "Route Information");
	consumption = new Field("Motor Consumption:", in_p);
	weight = new Field("Weight (kg):", in_p);
	resistance = new Field("Vehicle Drag (const):", in_p);
	charge = new Field("Charge (KWh):", in_p);
	speed = new Field("Speed (Mph):", in_p);
	incline = new Field("Incline:", in_p);
	routeDistance = new Field("Route Distance (meters):", in_p);
	routeSamples = new Field("Number of samples:", in_p);

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

	route_sizer->Add(elevationGraph, 1, wxEXPAND);
	route_sizer->Add(routeDistance->label);
	route_sizer->Add(routeDistance->field, 0, wxALL, 10);
	route_sizer->Add(routeSamples->label);
	route_sizer->Add(routeSamples->field, 0, wxALL, 10);

	i_sizer->Add(route_sizer, 0, wxEXPAND, 10);

	i_sizer->Hide(speed->label, true);
	i_sizer->Hide(speed->field, true);
	i_sizer->Hide(route_sizer, true);

	in_p->SetSizer(i_sizer, wxEXPAND);
	route_sizer->SetSizeHints(in_p);
	i_sizer->SetSizeHints(in_p);

	// ---------------------------------------------------
	// OUTPUT PANEL IMPLEMENTATION
	// ---------------------------------------------------

	o_sizer = new wxStaticBoxSizer(wxVERTICAL, out_p, "Output");
	o_t1 = new wxStaticText(out_p, wxID_ANY, "Best Speed:");
	o_v1 = new wxTextCtrl(out_p, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	o_t2 = new wxStaticText(out_p, wxID_ANY, "Distance:");
	o_v2 = new wxTextCtrl(out_p, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);

	//Assign a graph element to the outputGraph variable
	speedLayer = new mpFXYVector(_("Speed"));
	batteryLayer = new mpFXYVector(_("Battery"));
	outputGraph = new mpWindow(out_p, wxID_ANY, wxPoint(0, 0), wxSize(300, 300), wxSUNKEN_BORDER);
	outputGraph2 = new mpWindow(out_p, wxID_ANY, wxPoint(0, 0), wxSize(300, 300), wxSUNKEN_BORDER);
	prepareGraph(outputGraph2);

	o_sizer->Add(o_t1, 0, wxALL, 10);
	o_sizer->Add(o_v1, 0, wxALL, 10);
	o_sizer->Add(o_t2, 0, wxALL, 10);
	o_sizer->Add(o_v2, 0, wxALL, 10);

	// Add the graph element
	o_sizer->Add(
		outputGraph,
		1,				// Yes stretch
		wxEXPAND);
	o_sizer->Add(outputGraph2, 1, wxEXPAND);

	out_p->SetSizer(o_sizer, wxEXPAND);
	o_sizer->SetSizeHints(out_p);

	Layout();

	SetMinClientSize(wxSize(600, 600));
	SetClientSize(600, 650);
}

void MinFrame::prepareGraph(mpWindow * graph)
{
	wxFont graphFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);//Set the font for the graph
	mpScaleX* xaxis = new mpScaleX(wxT("X"), mpALIGN_BOTTOM, true, mpX_NORMAL);		//Label the x axis
	mpScaleY* yaxis = new mpScaleY(wxT("Y"), mpALIGN_LEFT, true);					//Label the y axis
	xaxis->SetFont(graphFont);														//Set font for x axis
	yaxis->SetFont(graphFont);														//Set font for y axis
	xaxis->SetDrawOutsideMargins(false);											//Dont't draw the axis outside margins
	yaxis->SetDrawOutsideMargins(false);
	graph->AddLayer(xaxis);												//add the axis to the graph
	graph->AddLayer(yaxis);
	graph->EnableDoubleBuffer(true);										//reduces flicker when graph is drawn
	graph->SetMPScrollbars(true);											//adds scroll bars if the graph window is too small
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
	toSave.push_back(std::string(charge->field->GetValue()));
	toSave.push_back(std::string(consumption->field->GetValue()));
	toSave.push_back(std::string(resistance->field->GetValue()));
	toSave.push_back(std::string(speed->field->GetValue()));
	toSave.push_back(std::string(weight->field->GetValue()));
	return toSave;
}

// This is a helper function for loading the input forms from a file
// If this function is changed, make sure it matches InputPanel::SaveInputForms()
void MinFrame::LoadInputForms(std::vector<std::string> loaded)
{
	charge->field->SetValue( wxString(loaded[0]) );
	consumption->field->SetValue( wxString(loaded[1]) );
	resistance->field->SetValue( wxString(loaded[2]) );
	speed->field->SetValue( wxString(loaded[3]) );
	weight->field->SetValue( wxString(loaded[4]) );
}


/*	Use two vectors of x and y coords to make a plot and add it to the graph
	in the inputPannel */
void MinFrame::setGraph(mpWindow *graph, mpFXYVector *layer, std::vector<double> vectorX, std::vector<double> vectorY, wxColor color)
{
	
	layer->SetData(vectorX, vectorY);			//Adds the x and y coords to the layer
	layer->SetContinuity(true);				//Draw lines in between the points
	wxPen vectorpen(color, 5, wxPENSTYLE_SOLID);	//Set line size and color
	layer->SetPen(vectorpen);					//gives the pen to the layer
	layer->SetDrawOutsideMargins(false);		//Makes sure the graph isnt drawn outside of the graph bounds
	graph->SetMargins(10, 10, 30, 60);		//Sets our margins, top->right->bottom->left
	graph->AddLayer(layer);			//Adds the plotted x/y coordinates to our graph
	graph->Fit();							//Zoom the graph properly after everything has been added
}

void MinFrame::SetElvFields(float distance, float samples)
{
	routeDistance->field->SetValue(wxString(std::to_string(distance)));
	routeSamples->field->SetValue(wxString(std::to_string(samples)));
}

void MinFrame::SetLoadedValues(std::vector<double> loadvec)
{
	consumption->field->SetValue(wxString(std::to_string(loadvec[0])));
	weight->field->SetValue(wxString(std::to_string(loadvec[1])));
	resistance->field->SetValue(wxString(std::to_string(loadvec[2])));
	charge->field->SetValue(wxString(std::to_string(loadvec[3])));
	incline->field->SetValue(wxString(std::to_string(loadvec[4])));
	speed->field->SetValue(wxString(std::to_string(loadvec[5])));
}


void MinFrame::HandleMainCalcs(double charge, double weight, double resistance)
{
	MyCar *car = new MyCar();
	car->doMainCalcs(charge, weight, resistance, distance, samples, elevations);
	std::vector<double> xAxis;
	for (int i = 1; i <= samples-1; i++) {
		xAxis.push_back(i);
	}

	setGraph(outputGraph, speedLayer, xAxis, car->velocities, *wxBLUE);
	setGraph(outputGraph2, batteryLayer, xAxis, car->charges, *wxGREEN);

	/*speedLayer->SetData(xAxis, car->velocities);
	speedLayer->SetContinuity(true);
	wxPen vectorpen(*wxBLUE, 5, wxPENSTYLE_SOLID);
	speedLayer->SetPen(vectorpen);
	speedLayer->SetDrawOutsideMargins(true);
	outputGraph->SetMargins(10, 10, 30, 60);
	outputGraph->AddLayer(speedLayer);
	outputGraph->Fit();*/

	/*batteryLayer->SetData(xAxis, car->charges);
	batteryLayer->SetContinuity(true);
	wxPen vectorpen2(*wxGREEN, 5, wxPENSTYLE_SOLID);
	batteryLayer->SetPen(vectorpen2);
	batteryLayer->SetDrawOutsideMargins(true);
	outputGraph2->AddLayer(batteryLayer);
	outputGraph2->Fit();*/

}


/* This Function looks at the radio button box in InputPanel and is called when
*  that changes. 
*  When called, depending on the option it should change which simulation function gets run.
*/
void MinFrame::OnRadioBoxChange(wxCommandEvent& event)
{
	if (buttonGroup->GetString(event.GetSelection()) == "Get Distance") {
		i_sizer->Hide(route_sizer, true);
		i_sizer->Show(speed->label, true);
		i_sizer->Show(speed->field, true);
		i_sizer->Show(incline->label, true);
		i_sizer->Show(incline->field, true);
		Layout();
		
	}
	else if (buttonGroup->GetString(event.GetSelection()) == "Get Best Speed") {
		i_sizer->Hide(route_sizer, true);
		i_sizer->Hide(speed->label, true);
		i_sizer->Hide(speed->field, true);
		i_sizer->Show(incline->label, true);
		i_sizer->Show(incline->field, true);
		Layout();
	}
	else {
		i_sizer->Show(route_sizer, true);
		i_sizer->Hide(speed->label, true);
		i_sizer->Hide(speed->field, true);
		i_sizer->Hide(incline->label, true);
		i_sizer->Hide(incline->field, true);
		Layout();
	}
}

// When the Run event is generated:
// Take the given input and the run option,
// Call the appropriate simulation function,
// Set the output field with the results of the run.
void MinFrame::OnRun(wxCommandEvent & event)
{
	//setspeedfield(testmiles(in_p->getconsumption(), in_p->getweight()));
	if (buttonGroup->GetString(event.GetSelection()) == "Get Distance") {
		bestSpeed->set(0);
		bestDistance->set((getDistance(charge->get(),
			consumption->get(),
			incline->get(),
			weight->get(),
			speed->get(),
			resistance->get())));
	}
	else if (buttonGroup->GetString(event.GetSelection()) == "Get Best Speed") {
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
		for (int i = 0; i < speeds.size(); i++) {
			vec.push_back((double)i);
		}

		setGraph(outputGraph, speedLayer, speeds, vec, *wxBLUE);
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
	
	/* ### This section reads in all the elevation points in the file ### */
	while (ss >> i) {
		vectorY.push_back(i);
		if (ss.peek() == ',' || ss.peek() == ' ') {
			ss.ignore();
		}
		vectorX.push_back(j);
		j++;
	}
	
	samples = sampVal;
	distance = distVal;
	elevations = vectorY;

	setGraph(elevationGraph, elevationLayer, vectorX, vectorY, *wxBLUE);

}

void MinFrame::OnClear(wxCommandEvent & event)
{
	std::vector<double> zeroVec;
	for (int i = 0; i < 10; i++) {
		zeroVec.push_back(0.0);
	}
	SetLoadedValues(zeroVec);
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

	LoadInputForms(loaded);
		
}

void MinFrame::OnQuit(wxCommandEvent & event)
{
	Close(true);
}

void MinFrame::OnAbout(wxCommandEvent & event)
{
	wxMessageBox(wxT("Solar Car Simulation\n 2018 Senior Capstone\n Dennie Devito, Logan Kling, and Dakota Zaengle"), wxT("About"), wxICON_INFORMATION | wxOK);
}

Field::Field(std::string name, wxPanel *parent)
{
	label = new wxStaticText(parent, -1, name);
	field = new wxTextCtrl(parent, -1, "0.0", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
}

double Field::get()
{
	return atof(field->GetValue());
}

void Field::set(double v)
{
	field->SetValue(std::to_string(v));
}
