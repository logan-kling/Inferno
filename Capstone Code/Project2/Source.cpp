#include <string>
#pragma warning(disable:4996)
#include <sstream>
#include <iostream>
#include <cctype>

#include <wx/wx.h>
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/textfile.h>
#include <wx/msgdlg.h>
#include <wx/radiobox.h>
#include <wx/wfstream.h>

#include "mathplot.h"	// The library we are using to create graphs
#include "Sim.h"		// The file for our simulation functions
#include "SimIDs.h"		// Contains our custom wxIDs

class MinApp : public wxApp {
public:
	virtual bool OnInit();
};

// This class contains:
//  all the forms the user needs to fill out to run the program
//  a selector for the run option
//  a graph to display imported elevation data
class InputPanel : public wxPanel {
public:
	InputPanel(wxWindow * parent);

	// Get functions for private member content
	float	GetConsumption() const { return (float)atof(consumption->GetValue()); };
	float	GetWeight() const { return (float)atof(weight->GetValue()); };
	float	GetResistance() const { return (float)atof(resistance->GetValue()); };
	float	GetCharge() const { return (float)atof(charge->GetValue()); };
	float	GetIncline() const { return (float)atof(incline->GetValue()); };
	float	GetSpeed() const { return (float)atof(speed->GetValue()); };
	int		GetRunOption() { return buttonGroup->GetSelection(); };

	// Helper functions
	std::vector<std::string> SaveInputForms();										//Save form helper
	void	LoadInputForms(std::vector<std::string> loaded);

	void	MakeElvGraph(std::vector<double> vectorX, std::vector<double> vectorY);	//Elevation graph maker
	void	SetElvFields(float distance, float samples);
	void    SetLoadedValues(std::vector<double> loadvec);

private:
	void	OnRadioBoxChange(wxCommandEvent& event);								//Radio Box Listener
	void	PrepElvGraph();															//Elevation graph setup

private:
	wxStaticText *i_test;
	wxTextCtrl *consumption, *weight, *resistance, *charge, *incline, *speed, *routeDistance, *routeSamples;
	wxSizer *i_sizer, *route_sizer;
	wxRadioBox *buttonGroup;	//Radio buttons for selecting run mode
	mpWindow *elevationGraph;
	mpFXYVector *vectorLayer;	//layer for our elevation plot added to 'elevationGraph' in 'MakeElvGraph()'
	wxDECLARE_EVENT_TABLE();
};

// This class contains:
//  the output fields to display our run data
//  a graph for display use of data after run
class OutputPanel : public wxPanel {
public:
	OutputPanel(wxWindow * parent);
	void SetOutputField(float out);
	void SetOutputField(float * out);
	// Helper functions
	void	makeSpeedsGraph(std::vector<double> distances);	//Elevation graph maker
	void	HandleMainCalcs(double charge, double weight, double resistance);

	float samples, distance;
	std::vector<double> elevations;
private:
	wxStaticText *o_test;
	wxTextCtrl *o_v1;
	wxSizer *o_sizer;
	mpWindow *outputGraph;
	mpFXYVector *vectorLayer, *batteryLayer, *changeLayer;	//layer for our elevation plot added to 'speedGraph' in 'MakeSpeedsGraph()'
	void	prepareSpeedsGraph();
};

// This class contains:
//  The overall layout of our program
//  The menu bar
//  The input and output panels
//  This class is the parent of the other classes.
class MinFrame : public wxFrame {
public:
	MinFrame(const wxString& title);

	InputPanel *in_p;
	OutputPanel *out_p;

private:
	wxMenuBar *menuBar;
	wxMenu *fileMenu;
	wxMenu *editMenu;
	wxMenu *helpMenu;
	wxString CurrentDocPath;
	wxTextCtrl *MainEditBox;

	void OnRun(wxCommandEvent& event);
	void OnImport(wxCommandEvent& event);
	void OnClear(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnLoad(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

/* wxIDs for menu items */
enum {
	Edit_Run,
	Edit_Import,
	Edit_Clear,
	File_Save,
	File_Load
};

/* Event tables for menu items! */
wxBEGIN_EVENT_TABLE(MinFrame, wxFrame)
EVT_MENU(Edit_Run, MinFrame::OnRun)
EVT_MENU(Edit_Import, MinFrame::OnImport)
EVT_MENU(File_Save, MinFrame::OnSave)
EVT_MENU(File_Load, MinFrame::OnLoad)
EVT_MENU(Edit_Clear, MinFrame::OnClear)
wxEND_EVENT_TABLE()

/* Event table for Input Pannel! */
wxBEGIN_EVENT_TABLE(InputPanel, wxPanel)
	EVT_RADIOBOX(wxID_RADIOBOX, InputPanel::OnRadioBoxChange)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MinApp);

// The default constructor for our frame
// This is the highest level window. Everything in the GUI is
// some level of child of 'this'
MinFrame::MinFrame(const wxString& title) 
	: wxFrame(NULL, wxID_ANY, title)
{
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
	editMenu->Append(Edit_Import, "&Import Elevation", "Import elevation data from Google");
	menuBar->Append(editMenu, "&Edit");

	// Help Menu
	helpMenu->Append(wxID_ABOUT, "&About", "Version Info");
	helpMenu->Append(wxID_ANY, "&Walkthrough", "Guide for program use");
	menuBar->Append(helpMenu, "&Help");

	SetMenuBar(menuBar);

	// Menu bar is now done and placed

	// This is for everything else

	// Create a split window so we can have input and output side by side
	wxSplitterWindow *splitter = new wxSplitterWindow(this, 
		wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_NOBORDER);

	// Create input and output pannels with the splitter window as their parent
	in_p = new InputPanel(splitter);
	out_p = new OutputPanel(splitter);
	splitter->SplitVertically(in_p, out_p);
	splitter->SetMinimumPaneSize(200);

}


// This is basically the 'main()' function, program starts here

bool MinApp::OnInit() {
	MinFrame* frame = new MinFrame("SolarSim");
	frame->Show(true);
	return true;
}

// The default constructor for our input panel
InputPanel::InputPanel(wxWindow * parent)
	: wxPanel(parent, wxID_ANY){
	
	wxArrayString choices;
	choices.Add("Get Distance");
	choices.Add("Get Speed");
	choices.Add("Get Course");

	vectorLayer = new mpFXYVector(_("Vector"));
	elevationGraph = new mpWindow(this, -1, wxPoint(0, 0), wxSize(300, 300), wxSUNKEN_BORDER);

	PrepElvGraph();

	i_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Input");
	route_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Route Information");
	consumption = new wxTextCtrl(this, -1, "0", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	weight = new wxTextCtrl(this, -1, "0", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	resistance = new wxTextCtrl(this, -1, "0", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	charge = new wxTextCtrl(this, -1, "0", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	speed = new wxTextCtrl(this, -1, "0", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	incline = new wxTextCtrl(this, -1, "0", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	routeDistance = new wxTextCtrl(this, -1, "0", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	routeSamples = new wxTextCtrl(this, -1, "0", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);

	speed->SetEditable(0);
	incline->SetEditable(0);
	routeDistance->SetEditable(0);
	routeSamples->SetEditable(0);
	
	buttonGroup = new wxRadioBox(this, wxID_RADIOBOX, "Distance/Speed:", wxDefaultPosition, wxDefaultSize, choices, 3, wxHORIZONTAL);
	
	// Set the radio button
	buttonGroup->SetSelection(1);
	
	i_sizer->Add(buttonGroup);

	i_sizer->Add(new wxStaticText(this, wxID_ANY, "Motor Consumption:"));
	i_sizer->Add(
		consumption,
		0,            // not vertically stretchable
		wxALL,        // and make border all around
		10);          // set border width to 10

	i_sizer->Add(new wxStaticText(this, wxID_ANY, "Weight:"));
	i_sizer->Add(weight, 0, wxALL, 10);

	i_sizer->Add(new wxStaticText(this, wxID_ANY, "Coefficient of Air Resistance:"));
	i_sizer->Add(resistance, 0, wxALL, 10);

	i_sizer->Add(new wxStaticText(this, wxID_ANY, "Charge:"));
	i_sizer->Add(charge, 0, wxALL, 10);

	i_sizer->Add(new wxStaticText(this, wxID_ANY, "Speed:"));
	i_sizer->Add(speed, 0, wxALL, 10);

	i_sizer->Add(new wxStaticText(this, wxID_ANY, "Incline:"));
	i_sizer->Add(incline, 0, wxALL, 10);

	route_sizer->Add(elevationGraph, 1, wxEXPAND);
	route_sizer->Add(new wxStaticText(this, wxID_ANY, "Distance(Meters):"));
	route_sizer->Add(routeDistance, 0, wxALL, 10);
	route_sizer->Add(new wxStaticText(this, wxID_ANY, "Samples:"));
	route_sizer->Add(routeSamples, 0, wxALL, 10);

	i_sizer->Add(route_sizer, 0, wxEXPAND, 10);

	this->SetSizer(i_sizer);
}

// This is a helper function for saving the input forms to a file.
// If this function is changed, make sure it matches InputPanel::LoadInputForms()
std::vector<std::string> InputPanel::SaveInputForms()
{
	std::vector<std::string> toSave;
	toSave.clear();
	toSave.push_back(std::to_string(this->GetCharge()));
	toSave.push_back(std::to_string(this->GetConsumption()));
	toSave.push_back(std::to_string(this->GetResistance()));
	toSave.push_back(std::to_string(this->GetSpeed()));
	toSave.push_back(std::to_string(this->GetWeight()));
	return toSave;
}

// This is a helper function for loading the input forms from a file
// If this function is changed, make sure it matches InputPanel::SaveInputForms()
void InputPanel::LoadInputForms(std::vector<std::string> loaded)
{
	charge->SetValue( wxString(loaded[0]) );
	consumption->SetValue( wxString(loaded[1]) );
	resistance->SetValue( wxString(loaded[2]) );
	speed->SetValue( wxString(loaded[3]) );
	weight->SetValue( wxString(loaded[4]) );
}


/*	Use two vectors of x and y coords to make a plot and add it to the graph
	in the inputPannel */
void InputPanel::MakeElvGraph(std::vector<double> vectorX, std::vector<double> vectorY)
{
	
	vectorLayer->SetData(vectorX, vectorY);			//Adds the x and y coords to the layer
	vectorLayer->SetContinuity(true);				//Draw lines in between the points
	wxPen vectorpen(*wxBLUE, 5, wxPENSTYLE_SOLID);	//Set line size and color
	vectorLayer->SetPen(vectorpen);					//gives the pen to the layer
	vectorLayer->SetDrawOutsideMargins(false);		//Makes sure the graph isnt drawn outside of the graph bounds
	elevationGraph->SetMargins(10, 10, 30, 60);		//Sets our margins, top->right->bottom->left
	elevationGraph->AddLayer(vectorLayer);			//Adds the plotted x/y coordinates to our graph
	elevationGraph->Fit();							//Zoom the graph properly after everything has been added
	//wxMessageBox("Elevation Graph Set");
}

void InputPanel::SetElvFields(float distance, float samples)
{
	routeDistance->SetValue(wxString(std::to_string(distance)));
	routeSamples->SetValue(wxString(std::to_string(samples)));
}

void InputPanel::SetLoadedValues(std::vector<double> loadvec)
{
	consumption->SetValue(wxString(std::to_string(loadvec[0])));
	weight->SetValue(wxString(std::to_string(loadvec[1])));
	resistance->SetValue(wxString(std::to_string(loadvec[2])));
	charge->SetValue(wxString(std::to_string(loadvec[3])));
	incline->SetValue(wxString(std::to_string(loadvec[4])));
	speed->SetValue(wxString(std::to_string(loadvec[5])));
}

// Using no input,
// This function prepares the elevation graph by setting up the x and y axis,
// And scaling the graph window.
void InputPanel::PrepElvGraph() {
	wxFont graphFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);//Set the font for the graph
	mpScaleX* xaxis = new mpScaleX(wxT("X"), mpALIGN_BOTTOM, true, mpX_NORMAL);		//Label the x axis
	mpScaleY* yaxis = new mpScaleY(wxT("Y"), mpALIGN_LEFT, true);					//Label the y axis
	xaxis->SetFont(graphFont);														//Set font for x axis
	yaxis->SetFont(graphFont);														//Set font for y axis
	xaxis->SetDrawOutsideMargins(false);											//Dont't draw the axis outside margins
	yaxis->SetDrawOutsideMargins(false);
	elevationGraph->AddLayer(xaxis);												//add the axis to the graph
	elevationGraph->AddLayer(yaxis);
	elevationGraph->EnableDoubleBuffer(true);										//reduces flicker when graph is drawn
	elevationGraph->SetMPScrollbars(true);											//adds scroll bars if the graph window is too small
}


/*	Use two vectors of x and y coords to make a plot and add it to the graph
in the inputPannel */
void OutputPanel::makeSpeedsGraph(std::vector<double> distances)
{
	std::vector<double> speeds = {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65};
	vectorLayer->SetData(speeds, distances);			//Adds the x and y coords to the layer
	vectorLayer->SetContinuity(true);				//Draw lines in between the points
	wxPen vectorpen(*wxBLUE, 5, wxPENSTYLE_SOLID);	//Set line size and color
	vectorLayer->SetPen(vectorpen);					//gives the pen to the layer
	vectorLayer->SetDrawOutsideMargins(false);		//Makes sure the graph isnt drawn outside of the graph bounds
	outputGraph->SetMargins(10, 10, 30, 60);		//Sets our margins, top->right->bottom->left
	outputGraph->AddLayer(vectorLayer);			//Adds the plotted x/y coordinates to our graph
	outputGraph->Fit();							//Zoom the graph properly after everything has been added
	wxMessageBox("Distance versus Speed Graph Set");
}

void OutputPanel::HandleMainCalcs(double charge, double weight, double resistance)
{
	MyCar *car = new MyCar();
	car->doMainCalcs(charge, weight, resistance, distance, samples, elevations);
	std::vector<double> xAxis;
	for (int i = 1; i <= samples-1; i++) {
		xAxis.push_back(i);
	}
	vectorLayer->SetData(xAxis, car->velocities);
	vectorLayer->SetContinuity(true);
	wxPen vectorpen(*wxBLUE, 5, wxPENSTYLE_SOLID);
	vectorLayer->SetPen(vectorpen);
	vectorLayer->SetDrawOutsideMargins(true);
	outputGraph->SetMargins(10, 10, 30, 60);
	outputGraph->AddLayer(vectorLayer);

	batteryLayer->SetData(xAxis, car->charges);
	batteryLayer->SetContinuity(true);
	wxPen vectorpen2(*wxGREEN, 5, wxPENSTYLE_SOLID);
	batteryLayer->SetPen(vectorpen2);
	batteryLayer->SetDrawOutsideMargins(true);
	outputGraph->AddLayer(batteryLayer);

	changeLayer->SetData(xAxis, car->changes);
	changeLayer->SetContinuity(true);
	wxPen vectorpen3(*wxRED, 3, wxPENSTYLE_SOLID);
	changeLayer->SetPen(vectorpen3);
	changeLayer->SetDrawOutsideMargins(true);
	outputGraph->AddLayer(changeLayer);

	outputGraph->Fit();
}

// Using no input,
// This function prepares the graph of speeds by setting up the x and y axis,
// And scaling the graph window.
void OutputPanel::prepareSpeedsGraph() {
	wxFont graphFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);//Set the font for the graph
	mpScaleX* xaxis = new mpScaleX(wxT("X"), mpALIGN_BOTTOM, true, mpX_NORMAL);		//Label the x axis
	mpScaleY* yaxis = new mpScaleY(wxT("Y"), mpALIGN_LEFT, true);					//Label the y axis
	xaxis->SetFont(graphFont);														//Set font for x axis
	yaxis->SetFont(graphFont);														//Set font for y axis
	xaxis->SetDrawOutsideMargins(false);											//Dont't draw the axis outside margins
	yaxis->SetDrawOutsideMargins(false);
	outputGraph->AddLayer(xaxis);												//add the axis to the graph
	outputGraph->AddLayer(yaxis);
	outputGraph->EnableDoubleBuffer(true);										//reduces flicker when graph is drawn
	outputGraph->SetMPScrollbars(true);											//adds scroll bars if the graph window is too small
}

/* This Function looks at the radio button box in InputPanel and is called when
*  that changes. 
*  When called, depending on the option it should change which simulation function gets run.
*/
void InputPanel::OnRadioBoxChange(wxCommandEvent& event)
{
	if (buttonGroup->GetString(event.GetSelection()) == "Get Distance") {
		speed->SetEditable(1);
		
	}
	else if (buttonGroup->GetString(event.GetSelection()) == "Get Speed") {
		speed->SetEditable(0);
	}
}

// Default constructor for our OutputPanel.
OutputPanel::OutputPanel(wxWindow * parent)
	: wxPanel(parent, wxID_ANY) {

	o_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Output");
	o_sizer->Add(new wxStaticText(this, wxID_ANY, "Result:"));
	o_v1 = new wxTextCtrl(this, -1, "Output", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);

	//Assign a graph element to the outputGraph variable
	vectorLayer = new mpFXYVector(_("Speed"));
	batteryLayer = new mpFXYVector(_("Battery"));
	changeLayer = new mpFXYVector(_("changes"));
	outputGraph = new mpWindow(this, wxID_ANY, wxPoint(0, 0), wxSize(300, 300), wxSUNKEN_BORDER);
	prepareSpeedsGraph();
	o_sizer->Add(
		o_v1,
		0,
		wxALL,
		10);

	// Add the graph element
	o_sizer->Add(
		outputGraph,
		1,				// Yes stretch
		wxEXPAND);
	
	SetSizer(o_sizer);
}

// Set the first output field to the given float value
void OutputPanel::SetOutputField(float out)
{
	o_v1->SetValue(std::to_string(out));
}

/*/ Overload for multiple floats
void OutputPanel::SetOutputField(float* out)
{
	wxString outString;
	for (int i = sizeof(out); i > 0; i--) {
		outString.append(std::to_string(out[i]).append(" "));
	}
	o_v1->SetValue(outString);
}*/

// When the Run event is generated:
// Take the given input and the run option,
// Call the appropriate simulation function,
// Set the output field with the results of the run.
void MinFrame::OnRun(wxCommandEvent & event)
{
	//out_p->SetOutputField(testMiles(in_p->GetConsumption(), in_p->GetWeight()));
	if (in_p->GetRunOption() == 0) {
		out_p->SetOutputField(getDistance(in_p->GetCharge(), 
			in_p->GetConsumption(), 
			in_p->GetIncline(), 
			in_p->GetWeight(), 
			in_p->GetSpeed(), 
			in_p->GetResistance()));
	}
	else if (in_p->GetRunOption() == 1) {
		out_p->makeSpeedsGraph(getBestSpeed(in_p->GetConsumption(), 
			in_p->GetIncline(), 
			in_p->GetWeight(), 
			in_p->GetResistance(), 
			in_p->GetCharge()));
	}
	else if (in_p->GetRunOption() == 2) {
		out_p->HandleMainCalcs(in_p->GetCharge(), in_p->GetWeight(), in_p->GetResistance());
	}
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
	in_p->SetElvFields(distVal, sampVal);
	
	/* ### This section reads in all the elevation points in the file ### */
	while (ss >> i) {
		vectorY.push_back(i);
		if (ss.peek() == ',' || ss.peek() == ' ') {
			ss.ignore();
		}
		vectorX.push_back(j);
		j++;
	}
	
	out_p->samples = sampVal;
	out_p->distance = distVal;
	out_p->elevations = vectorY;

	in_p->MakeElvGraph(vectorX, vectorY);

}

void MinFrame::OnClear(wxCommandEvent & event)
{
	std::vector<double> zeroVec;
	for (int i = 0; i < 10; i++) {
		zeroVec.push_back(0.0);
	}
	in_p->SetLoadedValues(zeroVec);
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
	std::vector<std::string> savingThis = in_p->SaveInputForms();

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

	in_p->LoadInputForms(loaded);
		
}