#include <string>
#pragma warning(disable:4996)
#include <vector>
#include <sstream>
#include <iostream>

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
	void	MakeElvGraph(std::vector<double> vectorX, std::vector<double> vectorY);	//Elevation graph maker

private:
	void	OnRadioBoxChange(wxCommandEvent& event);								//Radio Box Listener
	void	PrepElvGraph();															//Elevation graph setup

private:
	wxStaticText *i_test;
	wxTextCtrl *consumption, *weight, *resistance, *charge, *incline, *speed;
	wxSizer *i_sizer;
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
private:
	wxStaticText *o_test;
	wxTextCtrl *o_v1;
	wxSizer *o_sizer;
	mpWindow *outputGraph;
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
	void OnSave(wxCommandEvent& event);
	void OnLoad(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};

/* wxIDs for menu items */
enum {
	Edit_Run,
	Edit_Import,
	File_Save,
	File_Load
};

/* Event tables for menu items! */
wxBEGIN_EVENT_TABLE(MinFrame, wxFrame)
	EVT_MENU(Edit_Run, MinFrame::OnRun)
	EVT_MENU(Edit_Import, MinFrame::OnImport)
	EVT_MENU(File_Save, MinFrame::OnSave)
	EVT_MENU(File_Load, MinFrame::OnLoad)
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
	fileMenu->Append(File_Load, "&Load_Run", "Load a previous run");
	fileMenu->Append(File_Save, "&Save_Run", "Save the current run");
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_ANY, "&Import_Settings", "Import variables");
	fileMenu->Append(wxID_ANY, "&Export_Settings", "Export variables");
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, "&Exit", "Quit the program");
	menuBar->Append(fileMenu, "&File");

	// Edit Menu
	editMenu->Append(wxID_ANY, "&Clear_Forms", "Empty all the form fields");
	editMenu->Append(Edit_Run, "&Run/Cancel", "Begin or end the simulation");
	editMenu->Append(Edit_Import, "&Import_Elevation", "Import elevation data from Google");
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

	// Sets the window size explicitly!
	SetSize(600, 600);
}


// This is basically the 'main()' function, program starts here

bool MinApp::OnInit() {
	MinFrame* frame = new MinFrame("Basic");
	frame->Show(true);
	return true;
}

// The default constructor for our input panel
InputPanel::InputPanel(wxWindow * parent)
	: wxPanel(parent, wxID_ANY){
	
	wxArrayString choices;
	choices.Add("Get Distance");
	choices.Add("Get Speed");

	vectorLayer = new mpFXYVector(_("Vector"));
	elevationGraph = new mpWindow(this, -1, wxPoint(0, 0), wxSize(300, 300), wxSUNKEN_BORDER);

	PrepElvGraph();

	i_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Input");
	consumption = new wxTextCtrl(this, -1, "0", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	weight = new wxTextCtrl(this, -1, "0", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	resistance = new wxTextCtrl(this, -1, "0", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	charge = new wxTextCtrl(this, -1, "0", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	speed = new wxTextCtrl(this, -1, "0", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	incline = new wxTextCtrl(this, -1, "0", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);

	speed->SetEditable(0);
	incline->SetEditable(0);
	
	buttonGroup = new wxRadioBox(this, wxID_RADIOBOX, "Distance/Speed:", wxDefaultPosition, wxDefaultSize, choices, 2, wxHORIZONTAL);
	
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

	i_sizer->Add(elevationGraph, 1, wxEXPAND);

	SetSizer(i_sizer);
	i_sizer->SetSizeHints(this);
}

// This is a helper function for saving the input forms to a file.
std::vector<std::string> InputPanel::SaveInputForms()
{
	std::vector<std::string> toSave;
	toSave.push_back(std::to_string(this->GetCharge()));
	toSave.push_back(std::to_string(this->GetConsumption()));
	toSave.push_back(std::to_string(this->GetResistance()));
	toSave.push_back(std::to_string(this->GetSpeed()));
	toSave.push_back(std::to_string(this->GetWeight()));
	return toSave;
}


/*	Use two vectors of x and y coords to make a plot and add it to the graph
	in the inputPannel
*/
void InputPanel::MakeElvGraph(std::vector<double> vectorX, std::vector<double> vectorY)
{
	
	vectorLayer->SetData(vectorX, vectorY);
	vectorLayer->SetContinuity(true);
	wxPen vectorpen(*wxBLUE, 5, wxPENSTYLE_SOLID);
	vectorLayer->SetPen(vectorpen);
	vectorLayer->SetDrawOutsideMargins(false);
	elevationGraph->SetMargins(10, 10, 30, 60);
	elevationGraph->AddLayer(vectorLayer);
	elevationGraph->Fit();
	wxMessageBox("Elevation Graph Set");
}

// Using no input,
// This function prepares the elevation graph by setting up the x and y axis,
// And scaling the graph window.
void InputPanel::PrepElvGraph() {
	wxFont graphFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	mpScaleX* xaxis = new mpScaleX(wxT("X"), mpALIGN_BOTTOM, true, mpX_NORMAL);
	mpScaleY* yaxis = new mpScaleY(wxT("Y"), mpALIGN_LEFT, true);
	xaxis->SetFont(graphFont);
	yaxis->SetFont(graphFont);
	xaxis->SetDrawOutsideMargins(false);
	yaxis->SetDrawOutsideMargins(false);
	elevationGraph->AddLayer(xaxis);
	elevationGraph->AddLayer(yaxis);
	elevationGraph->EnableDoubleBuffer(true);
	elevationGraph->SetMPScrollbars(true);
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
	outputGraph = new mpWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	o_sizer->Add(
		o_v1,
		0,
		wxALL,
		10);

	// Add the graph element
	o_sizer->Add(
		outputGraph,
		0,				// Yes stretch
		wxALL,
		10);
	
	SetSizer(o_sizer);
	o_sizer->SetSizeHints(this);
}

// Set the first output field to the given float value
void OutputPanel::SetOutputField(float out)
{
	o_v1->SetValue(std::to_string(out));
}

// Overload for multiple floats
void OutputPanel::SetOutputField(float* out)
{
	wxString outString;
	for (int i = sizeof(out); i > 0; i--) {
		outString.append(std::to_string(out[i]).append(" "));
	}
	o_v1->SetValue(outString);
}

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
		out_p->SetOutputField(getBestSpeed(in_p->GetConsumption(), 
			in_p->GetIncline(), 
			in_p->GetWeight(), 
			in_p->GetResistance(), 
			in_p->GetCharge()));
	}
}

// When the elevation event is generated:
// Open a file dialog to select which file to load from,
// parse the contents and send them to in_p memeber function:
// MakeElvGraph to create a graph from our vector.
void MinFrame::OnImport(wxCommandEvent & event) 
{
	wxString        file;
	wxFileDialog    fdlog(this);

	// show file dialog and get the path to
	// the file that was selected.
	if (fdlog.ShowModal() != wxID_OK) return;
	file.Clear();
	file = fdlog.GetPath();

	wxString        str;

	// open the file
	wxTextFile      tfile;
	tfile.Open(file);

	// read the first line
	str = tfile.GetFirstLine();

	std::string cString = str.ToStdString();
	std::stringstream ss(cString);
	std::vector<double> vectorY, vectorX;
	double i, j = 0;
	while (ss >> i) {
		vectorY.push_back(i);
		if (ss.peek() == ',' || ss.peek() == ' ') {
			ss.ignore();
		}
		vectorX.push_back(j);
		j++;
	}
	
	in_p->MakeElvGraph(vectorX, vectorY);

	/*std::string finishedString;
	for (i = 0; i < vectorY.size(); i++) {
		finishedString.append(std::to_string(vectorY.at(i)).append("\n"));
	}
	wxMessageBox(wxString(finishedString));*/
}

// When the OnSave event is generated:
// Open a file dialog and ask user for a name and a save path,
// Save the input fields to a file for later use.
void MinFrame::OnSave(wxCommandEvent & event)
{

	wxFileDialog
		saveFileDialog(this, _("Save txt file"), "", "",
			"text files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (saveFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...

					// save the current contents in the file;
					// this can be done with e.g. wxWidgets output streams:
	wxFileOutputStream output_stream(saveFileDialog.GetPath());
	if (!output_stream.IsOk())
	{
		wxLogError("Cannot save current contents in file '%s'.", saveFileDialog.GetPath());
		return;
	}
	std::vector<std::string> savingThis = in_p->SaveInputForms();

	for (std::vector<std::string>::iterator it = savingThis.begin(); it != savingThis.end(); ++it) {
		output_stream.Write((*it).c_str(), sizeof((*it).c_str()));
	}
}

// When the OnLoad event is generated:
// Ask the user for a file to load from,
// Read from the file and place the contents in the appropriate input columns.
void MinFrame::OnLoad(wxCommandEvent & event) 
{

	wxFileDialog
		openFileDialog(this, _("Open txt file"), "", "",
			"text files (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...

					// proceed loading the file chosen by the user;
					// this can be done with e.g. wxWidgets input streams:
	wxFileInputStream input_stream(openFileDialog.GetPath());
	if (!input_stream.IsOk())
	{
		wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
		return;
	}

	/*wxFileDialog* OpenDialog = new wxFileDialog(this, _("Load a file"), wxEmptyString, wxEmptyString, _("Text files (*.txt)"), wxFD_OPEN, wxDefaultPosition);

	if (OpenDialog->ShowModal() == wxID_OK) {
		CurrentDocPath = OpenDialog->GetPath();
		MainEditBox->LoadFile(CurrentDocPath);
		SetTitle(wxString("Edit - ") << OpenDialog->GetFilename());
	}*/
}