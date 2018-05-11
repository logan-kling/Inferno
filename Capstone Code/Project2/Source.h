#include <string>
#pragma warning(disable:4996)
#pragma warning(disable:4003)
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


#include "mathplot.h"	// The library we are using to create graphs
#include "Sim.h"		// The file for our simulation functions
#include "SimIDs.h"		// Contains our custom wxIDs


/*
This class is intended as a more user friendly way of pairing a text box with a label.
Now they can be hidden and shown together.
*/
class Field
{
public:

	/*
	@param name: the name you give to this Field
	@param parent: the wxPanel this Field will be contained in
	*/
	Field(std::string name, wxPanel *parent);

	// Return the field value as a double
	double get();

	// Return the field value as a string
	std::string gets();

	// Set the field value from a double
	void set(double v);

	// Set the field value from a string
	void set(std::string s);

	//For adding specifically to a grid sizer, this keeps the field and label together.
	void asizer(wxGridSizer *siz);

	// Hide the element
	void hide();

	// Show the element
	void show();

	
	wxPanel *parent_ref;		// Reference the parent so we can get the holding sizer from inside this class
	wxTextCtrl *field;			// The text field
	wxStaticText *label;		// The label for the text field
};

/*
This class is intended as a more user friendly way of handling graphs.
This lets them keep a label and a description and easily be shown/hidden and set
*/
class Graph
{
public:
	
	/*
	@param name: the name you want to give this graph (can not be changed later)
	@param description: How this graph should be read, We use "X: 'units of x', Y: 'units of y'" (can be changed later)
	@param parent: the wxPanel this Graph will be contained in
	*/
	Graph(std::string name, std::string description, wxPanel *parent);

	// Update the description of this graph (name can not be changed)
	void updateText(std::string new_legend);

	// Set the graph using x and y vectors and the color to draw the graph in
	void setGraph(std::vector<double> vectorX, std::vector<double> vectorY, wxColor color);

	// Prepare the graph, set up the X and Y axis, margins, etc
	void prepareGraph();

	// Hide the element
	void hide();

	// Show the element
	void show();

	wxPanel *parent_ref;		// Reference the parent so we can get the holding sizer from inside this class
	wxStaticBoxSizer* wrapper;	// A sizer to keep everything in and provide a name to the graph
	wxStaticText* legend;		// Description of the graph
	mpFXYVector* vector;		// The vector that will be drawn on the graph
	mpWindow* graph;			// The graph element
};

class MinApp : public wxApp {
public:
	virtual bool OnInit() wxOVERRIDE;
	virtual int OnExit() wxOVERRIDE;
};

/*
	This class is the meat of the program
	Called by MinApp every wx-element is a child or gran-child etc of this class
*/
class MinFrame : public wxFrame {
public:
	MinFrame(const wxString& title);



private:

	// ---------------------------------------------------
	// VARIABLES
	// ---------------------------------------------------

	/*
		Menu Variables
	*/
	wxMenuBar *menuBar;
	wxMenu *fileMenu;
	wxMenu *editMenu;
	wxMenu *helpMenu;
	wxString CurrentDocPath;
	wxTextCtrl *MainEditBox;
	
	/*
		Split Windows
	*/
	wxPanel *in_p, *out_p;
	
	/*
		Input Variables
	*/
	Field *consumption, *weight, *resistance, *charge, *incline, *speed, *routeDistance, *routeSamples; // Used by multiple run modes
	Field *solarInput, *solarStrength, *efficiency, *horsepower; // Specific to GetCourse
	wxSizer *i_sizer;
	wxGridSizer *course_sizer;
	wxRadioBox *buttonGroup;	// Radio buttons for selecting run mode
	
	/*
		Output Variables
	*/
	Field *bestSpeed, *bestDistance, *tripTime, *chargeTime;
	wxSizer *o_sizer;

	/*
		Graph Variables:
	*/
	Graph* outputGraph, *outputGraph2, *elevationGraph;
	float samples, distance;
	std::vector<double> distance_list;
	std::vector<double> elevations;

	// ---------------------------------------------------
	// FUNCTIONS
	// ---------------------------------------------------

	// Callled by OnSave to get the values of all our input fields
	std::vector<std::string> SaveInputForms();

	// Called by OnLoad to set the values of all our input fields
	void	LoadInputForms(std::vector<std::string> loaded);

	// Called by OnImport to set the elevation related fields
	void	SetElvFields(float distance, float samples);

	// Called by OnRun to handle the 'Get Course' option specifically
	void	HandleMainCalcs(double charge, double weight, double resistance);
	
	/*
		EVENT FUNCTIONS:
	*/
	void OnRun(wxCommandEvent& event);		// Checks radio box selection and runs the propper function	
	void OnImport(wxCommandEvent& event);	// Imports elevation data from a save file generated by route.html
	void OnClear(wxCommandEvent& event);	// sets all inputs to 0.0
	void OnSave(wxCommandEvent& event);		// Saves the current state of all inputs (excluding elevation) to a file
	void OnLoad(wxCommandEvent& event);		// Loads saved state of all inputs (excluding elevation) from a file
	void OnQuit(wxCommandEvent& event);		/* CALL graph->Destroy() FOR ALL GRAPHS HERE OR THE PROGRAM BREAKS!!!*/
	void OnAbout(wxCommandEvent& event);	// Brings up the about window
	void OnHelp(wxCommandEvent& event);		// Brings up a small walkthrough
	void OnRadioBoxChange(wxCommandEvent& event);								//Radio Box Listener

	wxDECLARE_EVENT_TABLE();
};

/* wxIDs for menu items */
enum {
	Edit_Run,
	Edit_Import,
	Edit_Clear,
	File_Save,
	File_Load,
	Help_Walkthrough
};

/* Event table */
wxBEGIN_EVENT_TABLE(MinFrame, wxFrame)
EVT_MENU(Edit_Run, MinFrame::OnRun)
EVT_MENU(Edit_Import, MinFrame::OnImport)
EVT_MENU(File_Save, MinFrame::OnSave)
EVT_MENU(File_Load, MinFrame::OnLoad)
EVT_MENU(Edit_Clear, MinFrame::OnClear)
EVT_MENU(wxID_EXIT, MinFrame::OnQuit)
EVT_MENU(wxID_ABOUT, MinFrame::OnAbout)
EVT_MENU(Help_Walkthrough, MinFrame::OnHelp)
EVT_RADIOBOX(wxID_RADIOBOX, MinFrame::OnRadioBoxChange)
wxEND_EVENT_TABLE()