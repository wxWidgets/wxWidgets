/////////////////////////////////////////////////////////////////////////////
// Name:        minimal.cpp
// Purpose:     Minimal wxWidgets sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
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
#include "wx/txtstrm.h"
#include "wx/wfstream.h"




// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)
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
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);

    // event handlers (these functions should _not_ be virtual)
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
    Minimal_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame(_T("Minimal wxWidgets App"));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tF1"), _T("Show about dialog"));

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


// event handlers

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
                    _T("This is the minimal wxWidgets sample\n")
                    _T("running under %s."),
                    wxVERSION_STRING,
                    wxGetOsDescription().c_str()
                 ),
                 _T("About wxWidgets minimal sample"),
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
    dyninfo->AddHandler("ButtonClickHandler", NULL /* no instance of the handler method */ , CLASSINFO( wxEvent ) ) ;

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
    writer.WriteObject( frame , frame->GetClassInfo() , &persister , wxString("myTestFrame") ) ;
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

bool MyApp::OnInit()
{
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
}

void test()
{
}



