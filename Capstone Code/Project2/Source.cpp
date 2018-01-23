#include <wx/wx.h>
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

class MinApp : public wxApp {
public:
	virtual bool OnInit();
};

class InputPanel : public wxPanel {
public:
	InputPanel(wxWindow * parent);
private:
	wxStaticText *i_test;
	wxTextCtrl *i_v1, *i_v2, *i_v3, *i_v4;
	wxSizer *i_sizer;
};

class OutputPanel : public wxPanel {
public:
	OutputPanel(wxWindow * parent);
private:
	wxStaticText *o_test;
	wxSizer *o_sizer;
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
};

wxIMPLEMENT_APP(MinApp);

MinFrame::MinFrame(const wxString& title) 
	: wxFrame(NULL, wxID_ANY, title)
{
	menuBar = new wxMenuBar();
	fileMenu = new wxMenu();
	editMenu = new wxMenu();
	helpMenu = new wxMenu();

	// File Menu
	fileMenu->Append(wxID_OPEN, "&Load_Run", "Load a previous run");
	fileMenu->Append(wxID_SAVE, "&Save_Run", "Save the current run");
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_ANY, "&Import_Settings", "Import variables");
	fileMenu->Append(wxID_ANY, "&Export_Settings", "Export variables");
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, "&Exit", "Quit the program");
	menuBar->Append(fileMenu, _T("&File"));

	// Edit Menu
	editMenu->Append(wxID_ANY, "&Clear_Forms", "Empty all the form fields");
	editMenu->Append(wxID_ANY, "&Run/Cancel", "Begin or end the simulation");
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
	
	i_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Input");
	i_sizer->Add(
		new wxTextCtrl(this, -1, "Distance", wxDefaultPosition, wxDefaultSize, wxTE_LEFT),
		0,            // not vertically stretchable
		wxALL,        // and make border all around
		10);          // set border width to 10
	i_sizer->Add(
	new wxTextCtrl(this, -1, "Charge", wxDefaultPosition, wxDefaultSize, wxTE_LEFT),
		0,
		wxALL,
		10);
	i_sizer->Add(
		new wxTextCtrl(this, -1, "Motor Consumption", wxDefaultPosition, wxDefaultSize, wxTE_LEFT),
		0,
		wxALL,
		10);

	SetSizer(i_sizer);
	i_sizer->SetSizeHints(this);
}

OutputPanel::OutputPanel(wxWindow * parent)
	: wxPanel(parent, wxID_ANY) {

	o_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Output");
	o_sizer->Add(
		new wxTextCtrl(this, -1, "Output", wxDefaultPosition, wxDefaultSize, wxTE_LEFT),
		0,
		wxALL,
		10);
	o_sizer->Add(
		new wxTextCtrl(this, -1, "Graph will go here", wxDefaultPosition, wxDefaultSize, wxTE_LEFT),
		1,				// Yes stretch
		wxALL,
		10);
	
	SetSizer(o_sizer);
	o_sizer->SetSizeHints(this);
}