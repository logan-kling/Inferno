#include <wx/app.h>
#include <wx/frame.h>
#include <wx/menu.h>

class MinApp : public wxApp {
public:
	virtual bool OnInit();
};

class MinFrame : public wxFrame {
public:
	MinFrame(const wxString& title);
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
}

bool MinApp::OnInit() {
	MinFrame* frame = new MinFrame("Basic");
	frame->Show(true);
	return true;
}