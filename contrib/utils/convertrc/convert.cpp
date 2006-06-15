//wxConvertApp Programming Utility
/*This program currently offers 3 useful conversions
1.  Converts most of an .RC file into a wxXml file
2.  Converts most of an .wxr file into a wxXml file
3.  Converts portions of an .RC file into a wxr file
*/

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"

#include "wx/deprecated/setup.h"
#include "wx/deprecated/resource.h"

#include "convert.h"
#include "rc2wxr.h"
#include "wxr2xml.h"
#include "rc2xml.h"

IMPLEMENT_APP(wxConvertApp)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

wxConvertApp::wxConvertApp()
{
    m_pFrame=NULL;
    m_pMenuBar=NULL;

}

wxConvertApp::~wxConvertApp()
{

}


bool wxConvertApp::OnInit()
{
//Initialize all image loaders(JPEG,BMP,PNG,and etc)
    wxInitAllImageHandlers();
    SetAppName(_T("wxConvertApp"));

    if (HandleCommandLine())
        return true;


// Create the main frame window
    m_pFrame = new wxMainFrame(NULL, wxID_ANY, _T("wxConvertApp"), wxPoint(0, 0), wxSize(500, 400),
        wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);

    InitMenu();
    m_pFrame->Show(true);
    SetTopWindow(m_pFrame);
    return true;
}

void wxConvertApp::InitMenu()
{
    m_pMenuBar=new wxMenuBar;
    wxASSERT(m_pMenuBar);

    wxMenu *filemenu=new wxMenu;
    filemenu->Append(ID_RC2WXR,_T("Convert RC file to WXR file"));
    filemenu->Append(ID_WXR2XML,_T("Convert WXR file to XML file"));
    filemenu->Append(ID_RC2XML,_T("Convert RC file to XML file"));

    filemenu->Append(ID_QUIT, _T("E&xit"));
    m_pMenuBar->Append(filemenu,_T("&File"));

    m_pFrame->SetMenuBar(m_pMenuBar);
}



// MainFrame.cpp: implementation of the wxMainFrame class.
//
//////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxMainFrame, wxFrame)
    EVT_MENU(ID_QUIT,wxMainFrame::OnQuit)
    EVT_MENU(ID_RC2WXR,wxMainFrame::OnRc2Wxr)
    EVT_MENU(ID_WXR2XML,wxMainFrame::OnWXR2XML)
    EVT_MENU(ID_RC2XML,wxMainFrame::OnRC2XML)
END_EVENT_TABLE()



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

wxMainFrame::wxMainFrame(wxWindow* parent,wxWindowID id,
const wxString& title, const wxPoint& pos, const wxSize& size,
long style, const wxString& name)
:wxFrame(parent,id,title,pos,size,style,name)
{

}

wxMainFrame::~wxMainFrame()
{
}



void wxMainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}


void wxMainFrame::OnRc2Wxr(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_FILEDLG
    wxFileDialog filed(this, wxEmptyString,
                       wxEmptyString, wxEmptyString,
                       _T("*.rc"), wxFD_OPEN);

    if (filed.ShowModal()!=wxID_OK)
        return;

    wxFileDialog wxrfile(this, _T("Enter Desired WXR file name"),
                         wxEmptyString, _T("resource.wxr"),
                         _T("*.wxr"), wxFD_OPEN);

    if (wxrfile.ShowModal()!=wxID_OK)
        return;

    rc2wxr convert;
    convert.Convert(wxrfile.GetPath(),filed.GetPath());
#endif // wxUSE_FILEDLG
}

void wxMainFrame::OnWXR2XML(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_FILEDLG
    wxFileDialog f(this);
    f.SetWildcard(_T("*.wxr"));
    if (f.ShowModal()!=wxID_OK)
        return;


    wxFileDialog xmlfile(this,_T("Enter Desired XML file name"),
                         wxEmptyString, _T("resource.xml"),
                         _T("*.xml"), wxFD_OPEN);

    if (xmlfile.ShowModal()!=wxID_OK)
        return;

    wxr2xml XMLCon;
    XMLCon.Convert(f.GetPath(),xmlfile.GetPath());
#endif // wxUSE_FILEDLG
}

void wxMainFrame::OnRC2XML(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_FILEDLG
    wxFileDialog f(this);
    f.SetWildcard(_T("*.rc"));
    if (f.ShowModal()!=wxID_OK)
        return;

    wxFileDialog xmlfile(this,_T("Enter Desired XML file name"),
                         wxEmptyString, _T("resource.xml"),
                         _T("*.xml"), wxFD_OPEN);

    if (xmlfile.ShowModal()!=wxID_OK)
        return;

    rc2xml XMLCon;
    XMLCon.Convert(f.GetPath(),xmlfile.GetPath());
#endif // wxUSE_FILEDLG
}

bool wxConvertApp::HandleCommandLine()
{

   if (argc != 2)
       return false;

//Figure out kind of conversion
    wxString source,target;

    wxr2xml trans_wxr2xml;
    rc2xml trans_rc2xml;
    rc2wxr trans_rc2wxr;

    source=argv[1];
    target=argv[2];


    if ((source.Find(_T(".wxr"))>0)&&(target.Find(_T(".xml"))>0))
        {
        trans_wxr2xml.Convert(source,target);
        return true;
        }
    else if ((source.Find(_T(".rc"))!=wxNOT_FOUND)&(target.Find(_T(".wxr"))!=wxNOT_FOUND))
        {
        trans_rc2wxr.Convert(source,target);
        return true;
        }
    else if ((source.Find(_T(".rc"))!=wxNOT_FOUND)&(target.Find(_T(".xml"))!=wxNOT_FOUND))
        {
        trans_rc2xml.Convert(source,target);
        return true;
        }

    return false;
}
