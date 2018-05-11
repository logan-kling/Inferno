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

class Field
{
public:
	Field(std::string name, wxPanel *parent);

	double get();
	void set(double v);
	void set(std::string s);

	wxPanel *parent_ref;
	wxTextCtrl *field;
	wxStaticText *label;
};

class Graph
{
public:
	Graph(std::string name, std::string description, wxPanel *parent);

	void updateText(std::string rename, std::string new_legend);
	void setGraph(std::vector<double> vectorX, std::vector<double> vectorY, wxColor color);
	void prepareGraph();


	wxPanel *parent_ref;
	wxStaticBoxSizer* wrapper;
	wxStaticText* legend;
	mpFXYVector* vector;
	mpWindow* graph;
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
	Field *consumption, *efficiency, *weight, *resistance, *charge, *incline, *speed, *routeDistance, *routeSamples;
	wxSizer *i_sizer, *route_sizer;
	wxRadioBox *buttonGroup;	//Radio buttons for selecting run mode
	
	/*
		Output Variables
	*/
	Field *bestSpeed, *bestDistance, *tripTime, *chargeTime;
	wxSizer *o_sizer;

	/*
		Graph Variables:
	*/
	mpFXYVector *elevationLayer, *batteryLayer, *speedLayer;
	mpWindow *outputGraph, *outputGraph2, *elevationGraph;
	float samples, distance;
	std::vector<double> elevations;

	// ---------------------------------------------------
	// FUNCTIONS
	// ---------------------------------------------------

	/*
		Graph Functions:
	*/
	void	setGraph(mpWindow *graph, mpFXYVector *layer, std::vector<double> vectorX, std::vector<double> vectorY, wxColor color);
	void	prepareGraph(mpWindow *graph);

	std::vector<std::string> SaveInputForms();
	void	LoadInputForms(std::vector<std::string> loaded);
	void	SetElvFields(float distance, float samples);
	void    SetLoadedValues(std::vector<double> loadvec);

	void	HandleMainCalcs(double charge, double weight, double resistance);
	
	/*
		EVENT FUNCTIONS:
	*/
	void OnRun(wxCommandEvent& event);
	void OnMap(wxCommandEvent& event);
	void OnImport(wxCommandEvent& event);
	void OnClear(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnLoad(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnHelp(wxCommandEvent& event);
	void OnRadioBoxChange(wxCommandEvent& event);								//Radio Box Listener

	wxDECLARE_EVENT_TABLE();
};

/* wxIDs for menu items */
enum {
	Edit_Run,
	Edit_Launch_Map,
	Edit_Import,
	Edit_Clear,
	File_Save,
	File_Load,
	Help_Walkthrough
};

/* Event table */
wxBEGIN_EVENT_TABLE(MinFrame, wxFrame)
EVT_MENU(Edit_Run, MinFrame::OnRun)
EVT_MENU(Edit_Launch_Map, MinFrame::OnMap)
EVT_MENU(Edit_Import, MinFrame::OnImport)
EVT_MENU(File_Save, MinFrame::OnSave)
EVT_MENU(File_Load, MinFrame::OnLoad)
EVT_MENU(Edit_Clear, MinFrame::OnClear)
EVT_MENU(wxID_EXIT, MinFrame::OnQuit)
EVT_MENU(wxID_ABOUT, MinFrame::OnAbout)
EVT_MENU(Help_Walkthrough, MinFrame::OnHelp)
EVT_RADIOBOX(wxID_RADIOBOX, MinFrame::OnRadioBoxChange)
wxEND_EVENT_TABLE()