#include <string>
#pragma warning(disable:4996)
#include <sstream>
#include <iostream>
#include <cctype>
#include <Windows.h>

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
#include <string>


#include "mathplot.h"	// The library we are using to create graphs
#include "Sim.h"		// The file for our simulation functions
#include "SimIDs.h"		// Contains our custom wxIDs

#define LOCKED 0
#define UNLOCKED 1

/*
* enums for a text field or graph field for input output windows.
*/
const static enum displayType {
	Text,
	Graph
};



class TextField
{
public:

	/*
	* Default constructor, sets the text field to editable or not.
	*/
	TextField();
	TextField(wxWindow *parent, bool edit);

public:
	wxTextCtrl *input;
	bool editable;
};



class GraphField
{
public:
	GraphField();

protected:
	void prepareGraph();
	void setGraph(std::vector<double> vectorX, std::vector<double> vectorY);
	mpFXYVector *graphVector;
	mpWindow *graph;
};



/*
* Class for holding the different form types
* Inherits from TextField which contains more info on text box forms
* and from GraphField which contains more info on Graph displays
*/
class Field : public TextField, public GraphField
{
public:
	Field(std::string name, displayType type, wxWindow *parent, bool edit);

public:
	wxStaticText *title;
	std::string fieldName;
	displayType fieldType;
};



class Forms
{
public:
	Forms();
	Forms(wxWindow *parent);
	std::vector<Field*> formFields;
};


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

	Forms	inputForms;

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
	void setSpeedField(float out);
	void setSpeedField(std::string out);
	void setDistanceField(float out);
	void setDistanceField(std::string out);
	//void setSpeedField(float * out);
	// Helper functions
	void	makeSpeedsGraph(std::vector<double> distances);	//Elevation graph maker
	void	HandleMainCalcs(double charge, double weight, double resistance);

	float samples, distance;
	std::vector<double> elevations;
private:
	wxStaticText *o_t1,
		*o_t2;
	wxTextCtrl *o_v1,
		*o_v2;
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
	void OnMap(wxCommandEvent& event);
	void OnImport(wxCommandEvent& event);
	void OnClear(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnLoad(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

/* wxIDs for menu items */
enum {
	Edit_Run,
	Edit_Launch_Map,
	Edit_Import,
	Edit_Clear,
	File_Save,
	File_Load
};

/* Event tables for menu items! */
wxBEGIN_EVENT_TABLE(MinFrame, wxFrame)
EVT_MENU(Edit_Run, MinFrame::OnRun)
EVT_MENU(Edit_Launch_Map, MinFrame::OnMap)
EVT_MENU(Edit_Import, MinFrame::OnImport)
EVT_MENU(File_Save, MinFrame::OnSave)
EVT_MENU(File_Load, MinFrame::OnLoad)
EVT_MENU(Edit_Clear, MinFrame::OnClear)
wxEND_EVENT_TABLE()

/* Event table for Input Pannel! */
wxBEGIN_EVENT_TABLE(InputPanel, wxPanel)
EVT_RADIOBOX(wxID_RADIOBOX, InputPanel::OnRadioBoxChange)
wxEND_EVENT_TABLE()