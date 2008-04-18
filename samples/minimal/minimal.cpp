#include "wx/wx.h"


class TestApp : public wxApp
{
public:
    virtual bool OnInit() {
    	wxFrame *frame = new wxFrame(NULL, -1, _T("Test"));
    	
    	wxFlexGridSizer *sizer = new wxFlexGridSizer(3, 1, 0, 0);
    	
    	sizer->Add(new wxListBox(frame, -1, wxDefaultPosition, wxSize(200,100)));
    	sizer->Add(new wxButton(frame, -1, _T("Hello")), 0, wxGROW | wxALIGN_RIGHT);
    	sizer->Add(new wxTextCtrl(frame, -1, _T("dummy"), wxDefaultPosition, wxSize(200, 100)) );
    	sizer->AddGrowableRow(0, 1);
    	sizer->AddGrowableRow(1, 1);
       	sizer->AddGrowableRow(2, 1);
        sizer->AddGrowableCol(0, 1);
	
    	frame->SetSizer(sizer);
    	sizer->SetSizeHints(frame);
    	
    	frame->Show();
    	SetTopWindow(frame);
    	
    	return true;
    }
};

IMPLEMENT_APP(TestApp)
