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

#include "mathplot.h"
#include "Sim.h"
#include "SimIDs.h"

class MinApp : public wxApp {
public:
	virtual bool OnInit();
};

class InputPanel : public wxPanel {
public:
	InputPanel(wxWindow * parent);
	float	GetConsumption() const { return (float)atof(consumption->GetValue()); };
	float	GetWeight() const { return (float)atof(weight->GetValue()); };
	float	GetResistance() const { return (float)atof(resistance->GetValue()); };
	float	GetCharge() const { return (float)atof(charge->GetValue()); };
	float	GetIncline() const { return (float)atof(incline->GetValue()); };
	float	GetSpeed() const { return (float)atof(speed->GetValue()); };
	int		GetRunOption() { return buttonGroup->GetSelection(); };
	std::vector<std::string> SaveInputForms();
	void	MakeElvGraph(std::vector<double> vectorX, std::vector<double> vectorY);

private:
	void	OnRadioBoxChange(wxCommandEvent& event);
	void	PrepElvGraph();

private:
	int runOption;
	wxStaticText *i_test;
	wxTextCtrl *consumption, *weight, *resistance, *charge, *incline, *speed;
	wxSizer *i_sizer;
	wxRadioBox *buttonGroup;
	mpWindow *elevationGraph;
	mpFXYVector *vectorLayer;
	wxDECLARE_EVENT_TABLE();
};

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
	in_p = new InputPanel(splitter);
	out_p = new OutputPanel(splitter);
	splitter->SplitVertically(in_p, out_p);
	splitter->SetMinimumPaneSize(20);
}


// This is basically the 'main()' function, program starts here

bool MinApp::OnInit() {
	MinFrame* frame = new MinFrame("Basic");
	frame->Show(true);
	return true;
}

InputPanel::InputPanel(wxWindow * parent)
	: wxPanel(parent, wxID_ANY){
	
	wxArrayString choices;
	choices.Add("Get Distance");
	choices.Add("Get Speed");

	vectorLayer = new mpFXYVector(_("Vector"));
	elevationGraph = new mpWindow(this, -1, wxPoint(0, 0), wxSize(300, 300), wxSUNKEN_BORDER);

	PrepElvGraph();

	i_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Input");
	consumption = new wxTextCtrl(this, -1, "consumption", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	weight = new wxTextCtrl(this, -1, "weight", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	resistance = new wxTextCtrl(this, -1, "resistance", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	charge = new wxTextCtrl(this, -1, "charge", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	speed = new wxTextCtrl(this, -1, "speed", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
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

	i_sizer->Add(new wxStaticText(this, wxID_ANY, "Weight in kg:"));
	i_sizer->Add(weight, 0, wxALL, 10);

	i_sizer->Add(new wxStaticText(this, wxID_ANY, "Coefficient of resistance:"));
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
	//elevationGraph->Fit();
	wxMessageBox("Elevation Graph Set");
}

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

OutputPanel::OutputPanel(wxWindow * parent)
	: wxPanel(parent, wxID_ANY) {

	o_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Output");
	o_sizer->Add(new wxStaticText(this, wxID_ANY, "Miles on availible Kwh:"));
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

void OutputPanel::SetOutputField(float out)
{
	o_v1->SetValue(std::to_string(out));
}

void OutputPanel::SetOutputField(float* out)
{
	wxString outString;
	for (int i = sizeof(out); i > 0; i--) {
		outString.append(std::to_string(out[i]));
	}
	o_v1->SetValue(outString);
}

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