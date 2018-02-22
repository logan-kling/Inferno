#include <string>
#pragma warning(disable:4996)

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

#include "Sim.h"
#include "mathplot.h"

class MinApp : public wxApp {
public:
	virtual bool OnInit();
};

class InputPanel : public wxPanel {
public:
	InputPanel(wxWindow * parent);
	float GetV1() const { return (float)atof(i_v1->GetValue()); };
	float GetV2() const { return (float)atof(i_v2->GetValue()); };
	float GetV3() const { return (float)atof(i_v3->GetValue()); };
private:
	wxStaticText *i_test;
	wxTextCtrl *i_v1, *i_v2, *i_v3, *i_v4;
	wxSizer *i_sizer;
};

class OutputPanel : public wxPanel {
public:
	OutputPanel(wxWindow * parent);
	void SetOutputField(float out);
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
	
	i_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "Input");
	i_v1 = new wxTextCtrl(this, -1, "Voltage", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	i_v2 = new wxTextCtrl(this, -1, "Availble Kwh", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	i_v3 = new wxTextCtrl(this, -1, "259 wh/m", wxDefaultPosition, wxDefaultSize, wxTE_LEFT);
	i_v3->SetEditable(0);
	
	i_sizer->Add(new wxStaticText(this, wxID_ANY, "Voltage:"));
	i_sizer->Add(
		i_v1,
		0,            // not vertically stretchable
		wxALL,        // and make border all around
		10);          // set border width to 10

	i_sizer->Add(new wxStaticText(this, wxID_ANY, "Availble Kwh:"));
	i_sizer->Add(i_v2, 0, wxALL, 10);

	i_sizer->Add(new wxStaticText(this, wxID_ANY, "Watt-Hours per Mile:"));
	i_sizer->Add(i_v3, 0, wxALL, 10);

	SetSizer(i_sizer);
	i_sizer->SetSizeHints(this);
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
	o_sizer->Add(
		outputGraph,
		1,				// Yes stretch
		wxALL,
		10);
	
	SetSizer(o_sizer);
	o_sizer->SetSizeHints(this);
}

void OutputPanel::SetOutputField(float out)
{
	o_v1->SetValue(std::to_string(out));
}

void MinFrame::OnRun(wxCommandEvent & event)
{
	out_p->SetOutputField(testMiles(in_p->GetV1(), in_p->GetV2()));
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

	wxMessageBox(str);

	/*processLine(str); // placeholder, do whatever you want with the string

					  // read all lines one by one
					  // until the end of the file
	while (!tfile.Eof())
	{
		str = tfile.GetNextLine();
		processLine(str); // placeholder, do whatever you want with the string
	}*/
}

void MinFrame::OnSave(wxCommandEvent & event)
{
	wxFileDialog* SaveDialog = new wxFileDialog(this, _("Save to a file"), wxEmptyString, wxEmptyString, _("Text files (*.txt)"), wxFD_SAVE, wxDefaultPosition);

	if (SaveDialog->ShowModal() == wxID_OK) {
		CurrentDocPath = SaveDialog->GetPath();
		MainEditBox->SaveFile(CurrentDocPath);
		SetTitle(wxString("Saving - ") << SaveDialog->GetFilename());
	}
}

void MinFrame::OnLoad(wxCommandEvent & event) 
{
	wxFileDialog* OpenDialog = new wxFileDialog(this, _("Load a file"), wxEmptyString, wxEmptyString, _("Text files (*.txt)"), wxFD_OPEN, wxDefaultPosition);

	if (OpenDialog->ShowModal() == wxID_OK) {
		CurrentDocPath = OpenDialog->GetPath();
		MainEditBox->LoadFile(CurrentDocPath);
		SetTitle(wxString("Edit - ") << OpenDialog->GetFilename());
	}
}