/////////////////////////////////////////////////////////////////////////////
// Name:        xti.cpp
// Purpose:     eXtended RTTI support sample
// Author:      Stefan Csomor, Francesco Montorsi
// Modified by:
// Created:     13/5/2007
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor, Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
 
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
 
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wx.h"
#include "wx/variant.h"
#include "wx/xml/xml.h"
#include "wx/frame.h"
#include "wx/notebook.h"
#include "wx/event.h"

#include "wx/xtistrm.h"
#include "wx/xtixml.h"
#include "wx/txtstrm.h"
#include "wx/wfstream.h"

#include "classlist.h"

#if !wxUSE_EXTENDED_RTTI
    #error This sample requires XTI (eXtended RTTI) enabled
#endif


// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);

    void OnPersist(wxCommandEvent& event);
    void OnDepersist(wxCommandEvent& event);
    void OnDumpClasses(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Persist = wxID_HIGHEST,
    Minimal_Depersist,
    Minimal_DumpClasses,
    Minimal_Quit = wxID_EXIT,
    Minimal_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Persist, MyFrame::OnPersist)
    EVT_MENU(Minimal_Depersist, MyFrame::OnDepersist)
    EVT_MENU(Minimal_DumpClasses, MyFrame::OnDumpClasses)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

wxObject* CreateFrameRTTI();
wxObject* TryLoad();
static void TryCode();

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

#if 1
    // create the main application window
    MyFrame *frame = new MyFrame(_T("Extended RTTI sample"));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

#else

	wxObject* frame ;

	wxxVariant v(wxPoint(100, 100));
	wxPoint pt = v.Get<wxPoint>();

#if 1
	 frame = CreateFrameRTTI() ;
     delete frame ;
#endif

	frame = TryLoad() ;
	TryCode() ;

    wxFrame *trueFrame = dynamic_cast< wxFrame * >( frame ) ;
    if ( !trueFrame )
    {
        wxDynamicObject* dyno = dynamic_cast< wxDynamicObject* >( frame ) ;
        if ( dyno )
            trueFrame = dynamic_cast< wxFrame *>( dyno->GetSuperClassInstance() ) ;
    }
    if ( trueFrame )
        trueFrame->Show() ;
    else
        wxMessageBox(wxT("We were not getting a real window") , wxT("Error") , wxOK ) ;
	return true ;

#endif

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(300, 200))
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tF1"), _T("Show about dialog"));

    fileMenu->Append(Minimal_Persist, _T("Persist a wxFrame to XML..."), 
                     _T("Creates a wxFrame using wxXTI and saves its description as XML"));
    fileMenu->Append(Minimal_Depersist, _T("Depersist XML file..."), 
                     _T("Loads the description of wxFrame from XML"));
    fileMenu->AppendSeparator();
    fileMenu->Append(Minimal_DumpClasses, _T("Dump registered classes..."), 
                     _T("Dumps the description of all wxWidgets classes registered in XTI"));
    fileMenu->AppendSeparator();
    fileMenu->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR
}


// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void MyFrame::OnPersist(wxCommandEvent& WXUNUSED(event))
{
}

void MyFrame::OnDepersist(wxCommandEvent& WXUNUSED(event))
{
}

void MyFrame::OnDumpClasses(wxCommandEvent& WXUNUSED(event))
{
    wxClassListDialog dlg(this);
    dlg.ShowModal();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format(
                    _T("Welcome to %s!\n")
                    _T("\n")
                    _T("This sample demonstrates wxWidgets eXtended RTTI (XTI)")
                    wxVERSION_STRING
                 ),
                 _T("About wxWidgets XTI sample"),
                 wxOK | wxICON_INFORMATION,
                 this);
}











#if 0

// this is the kind of source code that would end up being generated by a designer
// corresponding to the information we are setting up via RTTI in the CreateFrameRTTI

class MyXTIFrame : public wxFrame
{
public :
     // construction
    MyXTIFrame()
	{
		Init();
		m_button = NULL ;
	}

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr)
	{
		return wxFrame::Create( parent , id , title , pos , size, style , name ) ;
	}

	wxButton*	m_button ;
	void		SetButton( wxButton * button ) { m_button = button ; }
	wxButton*	GetButton() const { return m_button ; }

	void ButtonClickHandler( wxEvent & WXUNUSED(event) )
	{
		wxMessageBox( "Button Clicked " , "Hi!" , wxOK ) ;
	}

	DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS_NO_COPY(MyXTIFrame)
} ;

IMPLEMENT_DYNAMIC_CLASS_XTI(MyXTIFrame, MyXTIFrame,"x.h")

WX_BEGIN_PROPERTIES_TABLE(MyXTIFrame)
	WX_PROPERTY( Button , wxButton* , SetButton , GetButton , )
WX_END_PROPERTIES_TABLE()

WX_BEGIN_HANDLERS_TABLE(MyXTIFrame)
	WX_HANDLER( ButtonClickHandler , wxCommandEvent )
WX_END_HANDLERS_TABLE()

WX_CONSTRUCTOR_5( MyXTIFrame , wxWindow* , Parent , wxWindowID , Id , wxString , Title , wxPoint , Position , wxSize , Size )

BEGIN_EVENT_TABLE(MyXTIFrame, wxFrame)
END_EVENT_TABLE()

#endif

class MyDesignerPersister : public wxPersister
{
public :
       MyDesignerPersister( wxDynamicObject * frame)
       {
        m_frame = frame ;
       }
        virtual bool BeforeWriteDelegate( wxWriter *WXUNUSED(writer) , const wxObject *object,  const wxClassInfo* WXUNUSED(classInfo) , const wxPropertyInfo *propInfo , 
        const wxObject *&eventSink , const wxHandlerInfo* &handlerInfo )  
        {      
            // this approach would be used it the handler would not be connected really in the designer, so we have to supply the information
            if ( object == m_frame->GetProperty(wxT("Button")).GetAsObject() && propInfo == CLASSINFO( wxButton )->FindPropertyInfo("OnClick") )
            {
                eventSink = m_frame ;
                handlerInfo = m_frame->GetClassInfo()->FindHandlerInfo("ButtonClickHandler") ;
                return true ;
            }
            return false ;
        }
private :
    wxDynamicObject *m_frame ;
} ;

wxObject* CreateFrameRTTI()
{
    // Setting up the RTTI info for a not-yet-compiled class MyXTIFrame
    wxDynamicClassInfo *dyninfo = new wxDynamicClassInfo(wxT("myxtiframe.h") ,
            wxT("MyXTIFrame") , CLASSINFO( wxFrame ) ) ;

    dyninfo->AddProperty("Button",wxGetTypeInfo((wxButton**) NULL)) ;
    dyninfo->AddHandler("ButtonClickHandler", 
        NULL /* no instance of the handler method */ , CLASSINFO( wxEvent ) ) ;

    wxClassInfo *info;
	info = wxClassInfo::FindClass(wxT("MyXTIFrame"));

    int ID = 100 ;
    wxDynamicObject* frame = dynamic_cast<wxDynamicObject*> ( info->CreateObject() );
    wxxVariant Params[10];
    Params[0] = wxxVariant((wxWindow*)(NULL));
    Params[1] = wxxVariant(wxWindowID(ID++));
    Params[2] = wxxVariant(wxString(wxT("Hello")));
    Params[3] = wxxVariant(wxPoint(-1,-1));
    Params[4] = wxxVariant(wxSize(200,400));
    Params[5] = wxxVariant((long)wxDEFAULT_FRAME_STYLE);
    info->Create(frame, 6, Params );

	wxNotebook* notebook ;
	info = wxClassInfo::FindClass("wxNotebook");
    notebook = wxDynamicCast( info->CreateObject() , wxNotebook );
    Params[0] = wxxVariant((wxWindow*)(frame->GetSuperClassInstance()));
    Params[1] = wxxVariant(wxWindowID(ID++));
    Params[2] = wxxVariant(wxPoint( 10, 10 ));
    Params[3] = wxxVariant(wxDefaultSize);
    info->Create(notebook, 4, Params );

    // button

    wxPanel* panel ;
	info = wxClassInfo::FindClass("wxPanel");
    panel = wxDynamicCast( info->CreateObject() , wxPanel );
    Params[0] = wxxVariant((wxWindow*)(notebook));
    Params[1] = wxxVariant(wxWindowID(ID++));
    Params[2] = wxxVariant(wxPoint(-1,-1));
    Params[3] = wxxVariant(wxSize(-1,-1));
    info->Create(panel, 4, Params );
    notebook->AddPage( panel , "Buttons" ) ;

	wxButton* button ;
	info = wxClassInfo::FindClass("wxButton");
    button = wxDynamicCast( info->CreateObject() , wxButton );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(ID++));
    Params[2] = wxxVariant(wxString(wxT("Click Me!")));
    Params[3] = wxxVariant(wxPoint( 10, 10 ));
    Params[4] = wxxVariant(wxSize(-1,-1));
    Params[5] = wxxVariant((long)0);
    info->Create(button, 6, Params );
    frame->SetProperty( "Button" , wxxVariant( button ) ) ;

    // other controls

	info = wxClassInfo::FindClass("wxPanel");
    panel = wxDynamicCast( info->CreateObject() , wxPanel );
    Params[0] = wxxVariant((wxWindow*)(notebook));
    Params[1] = wxxVariant(wxWindowID(ID++));
    Params[2] = wxxVariant(wxPoint(-1,-1));
    Params[3] = wxxVariant(wxSize(-1,-1));
    info->Create(panel, 4, Params );
    notebook->AddPage( panel , "Other Standard controls" ) ;

	wxControl* control ;
	info = wxClassInfo::FindClass("wxCheckBox");
    control = wxDynamicCast( info->CreateObject() , wxControl );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(ID++));
    Params[2] = wxxVariant(wxString(wxT("A Checkbox")));
    Params[3] = wxxVariant(wxPoint( 10, 10 ));
    Params[4] = wxxVariant(wxSize(-1,-1));
    Params[5] = wxxVariant((long)0);
    info->Create(control, 6, Params );

    info = wxClassInfo::FindClass("wxRadioButton");
    control = wxDynamicCast( info->CreateObject() , wxControl );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(ID++));
    Params[2] = wxxVariant(wxString(wxT("A Radiobutton")));
    Params[3] = wxxVariant(wxPoint( 10, 30 ));
    Params[4] = wxxVariant(wxSize(-1,-1));
    Params[5] = wxxVariant((long)0);
    info->Create(control, 6, Params );

    control = wxDynamicCast( info->CreateObject() , wxControl );
    Params[1] = wxxVariant(wxWindowID(ID++));
    Params[2] = wxxVariant(wxString(wxT("Another One")));
    Params[3] = wxxVariant(wxPoint( 10, 50 ));
    info->Create(control, 6, Params );

    info = wxClassInfo::FindClass("wxStaticText");
    control = wxDynamicCast( info->CreateObject() , wxControl );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(ID++));
    Params[2] = wxxVariant(wxString(wxT("A Static Text!")));
    Params[3] = wxxVariant(wxPoint( 10, 70 ));
    Params[4] = wxxVariant(wxSize(-1,-1));
    Params[5] = wxxVariant((long)0);
    info->Create(control, 6, Params );

    info = wxClassInfo::FindClass("wxStaticBox");
    control = wxDynamicCast( info->CreateObject() , wxControl );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(ID++));
    Params[2] = wxxVariant(wxString(wxT("A Static Box")));
    Params[3] = wxxVariant(wxPoint( 10, 90 ));
    Params[4] = wxxVariant(wxSize(100,80));
    Params[5] = wxxVariant((long)0);
    info->Create(control, 6, Params );

    info = wxClassInfo::FindClass("wxTextCtrl");
    control = wxDynamicCast( info->CreateObject() , wxControl );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(ID++));
    Params[2] = wxxVariant(wxString(wxT("A Text Control")));
    Params[3] = wxxVariant(wxPoint( 10, 200 ));
    Params[4] = wxxVariant(wxSize(-1,-1));
    Params[5] = wxxVariant((long)0);
    info->Create(control, 6, Params );

    // spins and gauges

	info = wxClassInfo::FindClass("wxPanel");
    panel = wxDynamicCast( info->CreateObject() , wxPanel );
    Params[0] = wxxVariant((wxWindow*)(notebook));
    Params[1] = wxxVariant(wxWindowID(ID++));
    Params[2] = wxxVariant(wxPoint(-1,-1));
    Params[3] = wxxVariant(wxSize(-1,-1));
    info->Create(panel, 4, Params );
    notebook->AddPage( panel , "Spins and Sliders" ) ;

    info = wxClassInfo::FindClass("wxSpinButton");
    control = wxDynamicCast( info->CreateObject() , wxControl );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(ID++));
    Params[2] = wxxVariant(wxPoint( 10, 10 ));
    Params[3] = wxxVariant(wxSize(-1,-1));
    Params[4] = wxxVariant((long)wxSP_VERTICAL | wxSP_ARROW_KEYS);
    info->Create(control, 5, Params );

    /*
    info = wxClassInfo::FindClass("wxSpinCtrl");
    control = wxDynamicCast( info->CreateObject() , wxControl );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(ID++));
    Params[2] = wxxVariant(wxPoint( 10, 30 ));
    Params[3] = wxxVariant(wxSize(-1,-1));
    Params[4] = wxxVariant((long) wxSP_ARROW_KEYS);
    info->Create(control, 5, Params );
    */
    /*
    info = wxClassInfo::FindClass("wxGauge95");
    control = wxDynamicCast( info->CreateObject() , wxControl );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(ID++));
    Params[2] = wxxVariant((int) 100);
    Params[3] = wxxVariant(wxPoint( 10, 50 ));
    Params[4] = wxxVariant(wxSize(-1,-1));
    Params[5] = wxxVariant((long) wxGA_HORIZONTAL);
    info->Create(control, 6, Params );
    */

    /*
	Save it out to XML
    */
    wxXmlDocument xml;
    wxXmlNode *root;
    root = new wxXmlNode(wxXML_ELEMENT_NODE, "TestXTI", "This is the content");
    xml.SetRoot(root);
    wxXmlWriter writer(root) ;
    MyDesignerPersister persister(frame) ;
//    writer.WriteObject( frame , frame->GetClassInfo() , &persister , wxString("myTestFrame") ) ;
    xml.Save("testxti.xml");
	return frame ;

}

// is loading the streamed out component from xml and instantiating it using the
// wxRuntimeDepersister

wxObject* TryLoad()
{
    wxXmlDocument xml;
    wxXmlNode *root;
    wxRuntimeDepersister Callbacks;
    xml.Load("testxti.xml");
    root = xml.GetRoot();
    if (root->GetName() != "TestXTI")
    {
		assert(!"Bad XML file");
    }
    wxXmlReader Reader( root ) ;
	int obj = Reader.ReadObject( wxString("myTestFrame") , &Callbacks ) ;
    return Callbacks.GetObject( obj ) ;
 }

// is loading the streamed out component from xml and writing code that  
// will create the same component 

static void TryCode()
{
    wxXmlDocument xml;
    wxXmlNode *root;

	wxFFileOutputStream fos( "testxti.cpp" ) ;
	wxTextOutputStream tos( fos ) ;
    wxCodeDepersister Callbacks(&tos);

    xml.Load("testxti.xml");
    root = xml.GetRoot();
    if (root->GetName() != "TestXTI")
    {
		assert(!"Bad XML file");
    }
	wxXmlReader Reader(root);
	Reader.ReadObject( wxString("myTestFrame") , &Callbacks ) ;
}
/*
void test()
{
}*/



