//wxConvertApp Programming Utility
/*This program currently offers 3 useful conversions
1.  Converts most of an .RC file into a wxXml file
2.  Converts most of an .wxr file into a wxXml file
3.  Converts portions of an .RC file into a wxr file
*/

#ifdef __GNUG__
#pragma implementation "convert.cpp"
#pragma interface "convert.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/image.h>
#include "wx/resource.h"

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
    SetAppName("wxConvertApp");

    if (HandleCommandLine())
        return TRUE;


// Create the main frame window
    m_pFrame = new wxMainFrame(NULL, -1, "wxConvertApp", wxPoint(0, 0), wxSize(500, 400),
        wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
   
    InitMenu();
    m_pFrame->Show(TRUE);
    SetTopWindow(m_pFrame);
    return TRUE;
}

void wxConvertApp::InitMenu()
{
    m_pMenuBar=new wxMenuBar;
    wxASSERT(m_pMenuBar);

    wxMenu *filemenu=new wxMenu;
    filemenu->Append(ID_RC2WXR,"Convert RC file to WXR file");
    filemenu->Append(ID_WXR2XML,"Convert WXR file to XML file");
    filemenu->Append(ID_RC2XML,"Convert RC file to XML file");

    filemenu->Append(ID_QUIT, "E&xit");
    m_pMenuBar->Append(filemenu,"&File");

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



void wxMainFrame::OnQuit()
{
    Close(TRUE);
}


void wxMainFrame::OnRc2Wxr()
{
    wxFileDialog filed(this);
    filed.SetWildcard("*.rc");
    filed.SetStyle(wxOPEN);

    if (filed.ShowModal()!=wxID_OK)
        return;

    wxFileDialog wxrfile(this,"Enter Desired WXR file name");
    wxrfile.SetWildcard("*.wxr");
    wxrfile.SetStyle(wxOPEN);
    wxrfile.SetFilename("resource.wxr");

    if (wxrfile.ShowModal()!=wxID_OK)
        return;

    rc2wxr convert;
    convert.Convert(wxrfile.GetPath(),filed.GetPath());
}

void wxMainFrame::OnWXR2XML()
{
    wxFileDialog f(this);
    f.SetWildcard("*.wxr");
    if (f.ShowModal()!=wxID_OK)
        return;


    wxFileDialog xmlfile(this,"Enter Desired XML file name");
    xmlfile.SetWildcard("*.xml");
    xmlfile.SetStyle(wxOPEN);
    xmlfile.SetFilename("resource.xml");

    if (xmlfile.ShowModal()!=wxID_OK)
        return;

    wxr2xml XMLCon;
    XMLCon.Convert(f.GetPath(),xmlfile.GetPath());
	
}

void wxMainFrame::OnRC2XML()
{
    wxFileDialog f(this);
    f.SetWildcard("*.rc");
    if (f.ShowModal()!=wxID_OK)
        return;

    wxFileDialog xmlfile(this,"Enter Desired XML file name");
    xmlfile.SetWildcard("*.xml");
    xmlfile.SetStyle(wxOPEN);
    xmlfile.SetFilename("resource.xml");

    if (xmlfile.ShowModal()!=wxID_OK)
        return;

    rc2xml XMLCon;
    XMLCon.Convert(f.GetPath(),xmlfile.GetPath());

}

bool wxConvertApp::HandleCommandLine()
{

   if (argc != 2) 
       return FALSE;

//Figure out kind of conversion
    wxString source,target;
    wxr2xml trans_wxr2xml;
    rc2xml trans_rc2xml;
    rc2wxr trans_rc2wxr;

    source=argv[1];
    target=argv[2];
    

    if ((source.Find(".wxr")>0)&&(target.Find(".xml")>0))
        {
        trans_wxr2xml.Convert(source,target);
        return TRUE;
        }
    else if ((source.Find(".rc")!=-1)&(target.Find(".wxr")!=-1))
        {
        trans_rc2wxr.Convert(source,target);
        return TRUE;
        }
    else if ((source.Find(".rc")!=-1)&(target.Find(".xml")!=-1))
        {
        trans_rc2xml.Convert(source,target);
        return TRUE;
        }
    
    return FALSE; 
}
