// MainFrame.h: interface for the wxMainFrame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(MAINFRAME_H)
#define MAINFRAME_H

#include "wx/wxprec.h"


#define ID_QUIT 1002
#define ID_ABOUT 1003
#define ID_RC2WXR 1005
#define ID_WXR2XML 1006
#define ID_RC2XML 1007

class wxMainFrame:public wxFrame
{
public:
    void OnRC2XML(wxCommandEvent& event);
    void OnWXR2XML(wxCommandEvent& event);
    void OnRc2Wxr(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    wxMainFrame(wxWindow* parent, wxWindowID id,
        const wxString& title, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE,
        const wxString& name = _T("frame"));
    virtual ~wxMainFrame();

protected:
    DECLARE_EVENT_TABLE()
};

class wxConvertApp : public wxApp
{
public:
    bool HandleCommandLine();
    void InitMenu();
    bool OnInit(void);
    wxConvertApp();
    virtual ~wxConvertApp();

protected:
    wxMenuBar * m_pMenuBar;
    wxMainFrame *m_pFrame;
};



#endif // !defined(MAINFRAME_H)
