/////////////////////////////////////////////////////////////////////////////
// Name:        xti.cpp
// Purpose:     eXtended RTTI support sample
// Author:      Stefan Csomor, Francesco Montorsi
// Modified by:
// Created:     13/5/2007
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
#include "wx/spinbutt.h"
#include "wx/spinctrl.h"

#include "wx/xtistrm.h"
#include "wx/xtixml.h"
#include "wx/txtstrm.h"
#include "wx/wfstream.h"
#include "wx/sstream.h"
#include "wx/spinctrl.h"

#include "classlist.h"
#include "codereadercallback.h"

#if !wxUSE_EXTENDED_RTTI
    #error This sample requires XTI (eXtended RTTI) enabled
#endif


// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#ifndef wxHAS_IMAGES_IN_RESOURCES
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
    void OnGenerateCode(wxCommandEvent& event);
    void OnDumpClasses(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
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
    Minimal_GenerateCode,
    Minimal_DumpClasses,
    Minimal_Quit = wxID_EXIT,
    Minimal_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Persist, MyFrame::OnPersist)
    EVT_MENU(Minimal_Depersist, MyFrame::OnDepersist)
    EVT_MENU(Minimal_GenerateCode, MyFrame::OnGenerateCode)
    EVT_MENU(Minimal_DumpClasses, MyFrame::OnDumpClasses)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

void RegisterFrameRTTI();

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    RegisterFrameRTTI();

    // create the main application window
    MyFrame *frame = new MyFrame("Extended RTTI sample");

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

MyFrame::MyFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(300, 200))
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

    fileMenu->Append(Minimal_Persist, "Persist a wxFrame to XML...",
                    "Creates a wxFrame using wxXTI and saves its description as XML");
    fileMenu->Append(Minimal_Depersist, "Depersist XML file...",
                    "Loads the description of wxFrame from XML");
    fileMenu->Append(Minimal_GenerateCode, "Generate code for a wxFrame saved to XML...",
                    "Generates the C++ code which belong to a persisted wxFrame");
    fileMenu->AppendSeparator();
    fileMenu->Append(Minimal_DumpClasses, "Dump registered classes...",
                    "Dumps the description of all wxWidgets classes registered in XTI");
    fileMenu->AppendSeparator();
    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
#endif // wxUSE_STATUSBAR
}



// ----------------------------------------------------------------------------
// XTI sample code
// ----------------------------------------------------------------------------

// this is the kind of source code that would end up being generated by a
// designer corresponding to the information we are setting up via RTTI
// in the CreateFrameRTTI function:
//
//  class MyXTIFrame : public wxFrame
//  {
//  public:
//       //   construction
//      MyXTIFrame()
//      {
//          Init();
//          m_button = nullptr;
//      }
//
//      bool Create(wxWindow *parent,
//                  wxWindowID id,
//                  const wxString& title,
//                  const wxPoint& pos = wxDefaultPosition,
//                  const wxSize& size = wxDefaultSize,
//                  long style = wxDEFAULT_FRAME_STYLE,
//                  const wxString& name = wxFrameNameStr)
//      {
//          return wxFrame::Create( parent, id, title, pos, size, style, name );
//      }
//
//      void SetButton( wxButton * button ) { m_button = button; }
//      wxButton* GetButton() const { return m_button; }
//
//      void ButtonClickHandler( wxEvent & WXUNUSED(event) )
//      {
//          wxMessageBox( "Button Clicked ", "Hi!", wxOK );
//      }
//
//  protected:
//      wxButton* m_button;
//
//      wxDECLARE_EVENT_TABLE();
//      wxDECLARE_DYNAMIC_CLASS_NO_COPY(MyXTIFrame);
//  };
//
//  wxIMPLEMENT_DYNAMIC_CLASS_XTI(MyXTIFrame, MyXTIFrame, "x.h");
//
//  WX_BEGIN_PROPERTIES_TABLE(MyXTIFrame)
//      WX_PROPERTY( Button, wxButton*, SetButton, GetButton, )
//  WX_END_PROPERTIES_TABLE()
//
//  WX_BEGIN_HANDLERS_TABLE(MyXTIFrame)
//      WX_HANDLER( ButtonClickHandler, wxCommandEvent )
//  WX_END_HANDLERS_TABLE()
//
//  WX_CONSTRUCTOR_5( MyXTIFrame, wxWindow*, Parent, wxWindowID, Id,
//                    wxString, Title, wxPoint, Position, wxSize, Size )
//
//  wxBEGIN_EVENT_TABLE(MyXTIFrame, wxFrame)
//  wxEND_EVENT_TABLE()

// the following class "persists" (i.e. saves) a wxFrame into a wxObjectWriter

class MyDesignerPersister : public wxObjectWriterCallback
{
public:
    MyDesignerPersister( wxDynamicObject * frame)
    {
        m_frame = frame;
    }

    virtual bool BeforeWriteDelegate( wxObjectWriter *WXUNUSED(writer),
                                    const wxObject *object,
                                    const wxClassInfo* WXUNUSED(classInfo),
                                    const wxPropertyInfo *propInfo,
                                    const wxObject *&eventSink,
                                    const wxHandlerInfo* &handlerInfo )
    {
        // this approach would be used if the handler would not
        // be connected really in the designer, so we have to supply
        // the information
        const wxObject* but = wxAnyGetAsObjectPtr( m_frame->GetProperty(wxT("Button")) );
        if ( object == but &&
             propInfo == wxCLASSINFO( wxButton )->FindPropertyInfo(wxT("OnClick")) )
        {
            eventSink = m_frame;
            handlerInfo = m_frame->GetClassInfo()->
                FindHandlerInfo(wxT("ButtonClickHandler"));
            return true;
        }
        return false;
    }

private:
    wxDynamicObject *m_frame;
};

// sometimes linkers (at least MSVC and GCC ones) optimize the final EXE
// even in debug mode pruning the object files which he "thinks" are useless;
// thus the classes defined in those files won't be available in the XTI
// table and the program will fail to allocate them.
// The following macro implements a simple hack to ensure that a given
// class is linked in.
//
// TODO: in wx/link.h there are already similar macros (also more "optimized":
//       don't need the creation of fake object) which however require to use
//       the wxFORCE_LINK_THIS_MODULE() macro inside the source files corresponding
//       to the class being discarded.
//
#define wxENSURE_CLASS_IS_LINKED(x)     { x test; }

void RegisterFrameRTTI()
{
    // set up the RTTI info for a class (MyXTIFrame) which
    // is not defined anywhere in this program
    wxDynamicClassInfo *dyninfo =
        wx_dynamic_cast( wxDynamicClassInfo *, wxClassInfo::FindClass("MyXTIFrame"));
    if ( dyninfo == nullptr )
    {
        dyninfo = new wxDynamicClassInfo(wxT("myxtiframe.h"),
                            wxT("MyXTIFrame"),
                            CLASSINFO(wxFrame) );

        // this class has a property named "Button" and the relative handler:
        dyninfo->AddProperty(wxT("Button"), wxGetTypeInfo((wxButton**) nullptr));
        dyninfo->AddHandler(wxT("ButtonClickHandler"),
            nullptr /* no instance of the handler method */, CLASSINFO( wxEvent ) );
    }
}

wxDynamicObject* CreateFrameRTTI()
{
    int baseID = 100;
    wxAny Params[10];

    // the class is now part of XTI internal table so that we can
    // get a pointer to it just searching it like any other class:
    wxFrame* frame;
    wxClassInfo *info = wxClassInfo::FindClass("MyXTIFrame");
    wxASSERT( info );
    wxDynamicObject* frameWrapper =
        wx_dynamic_cast(wxDynamicObject*, info->CreateObject() );
    Params[0] = wxAny((wxWindow*)(nullptr));
    Params[1] = wxAny(wxWindowID(baseID++));
    Params[2] = wxAny(wxString("This is a frame created from XTI"));
    Params[3] = wxAny(wxPoint(-1,-1));
    Params[4] = wxAny(wxSize(400,300));
    Params[5] = wxAny((long)wxDEFAULT_FRAME_STYLE);
    wxASSERT( info->Create(frameWrapper, 6, Params ));
    frame = wx_dynamic_cast(wxFrame*, frameWrapper->GetSuperClassInstance());

#if 1
    // now build a notebook inside it:
    wxNotebook* notebook;
    info = wxClassInfo::FindClass("wxNotebook");
    wxASSERT( info );
    notebook = wxDynamicCast( info->CreateObject(), wxNotebook );
    Params[0] = wxAny((wxWindow*)frame);
    Params[1] = wxAny(wxWindowID(baseID++));
    Params[2] = wxAny(wxPoint( 10, 10 ));
    Params[3] = wxAny(wxDefaultSize);
    Params[4] = wxAny((long)0);
    wxASSERT( info->Create(notebook, 5, Params ));

    // button page

    wxPanel* panel;
    info = wxClassInfo::FindClass("wxPanel");
    wxASSERT( info );
    panel = wxDynamicCast( info->CreateObject(), wxPanel );
    Params[0] = wxAny((wxWindow*)(notebook));
    Params[1] = wxAny(wxWindowID(baseID++));
    Params[2] = wxAny(wxPoint(-1,-1));
    Params[3] = wxAny(wxSize(-1,-1));
    Params[4] = wxAny((long)0);
    Params[5] = wxAny(wxString("Hello"));
    wxASSERT( info->Create(panel, 6, Params ));
    notebook->AddPage( panel, "Buttons" );

    wxButton* button;
    info = wxClassInfo::FindClass("wxButton");
    wxASSERT( info );
    button = wxDynamicCast( info->CreateObject(), wxButton );
    Params[0] = wxAny((wxWindow*)(panel));
    Params[1] = wxAny(wxWindowID(baseID++));
    Params[2] = wxAny(wxString("Click Me!"));
    Params[3] = wxAny(wxPoint( 10, 10 ));
    Params[4] = wxAny(wxSize(-1,-1));
    Params[5] = wxAny((long)0);
    wxASSERT( info->Create(button, 6, Params ));
    frameWrapper->SetProperty( wxT("Button"), wxAny( button ) );

    // other controls page

    info = wxClassInfo::FindClass("wxPanel");
    wxASSERT( info );
    panel = wxDynamicCast( info->CreateObject(), wxPanel );
    Params[0] = wxAny((wxWindow*)(notebook));
    Params[1] = wxAny(wxWindowID(baseID++));
    Params[2] = wxAny(wxPoint(-1,-1));
    Params[3] = wxAny(wxSize(-1,-1));
    Params[4] = wxAny((long)0);
    Params[5] = wxAny(wxString("Hello"));
    wxASSERT( info->Create(panel, 6, Params ));
    notebook->AddPage( panel, "Other Standard controls" );

    wxControl* control;
    info = wxClassInfo::FindClass("wxCheckBox");
    wxASSERT( info );
    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[0] = wxAny((wxWindow*)(panel));
    Params[1] = wxAny(wxWindowID(baseID++));
    Params[2] = wxAny(wxString("A Checkbox"));
    Params[3] = wxAny(wxPoint( 10, 10 ));
    Params[4] = wxAny(wxSize(-1,-1));
    Params[5] = wxAny((long)0);
    wxASSERT( info->Create(control, 6, Params ));

    info = wxClassInfo::FindClass("wxRadioButton");
    wxASSERT( info );
    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[0] = wxAny((wxWindow*)(panel));
    Params[1] = wxAny(wxWindowID(baseID++));
    Params[2] = wxAny(wxString("A Radiobutton"));
    Params[3] = wxAny(wxPoint( 10, 30 ));
    Params[4] = wxAny(wxSize(-1,-1));
    Params[5] = wxAny((long)0);
    wxASSERT( info->Create(control, 6, Params ));

    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[1] = wxAny(wxWindowID(baseID++));
    Params[2] = wxAny(wxString("Another One"));
    Params[3] = wxAny(wxPoint( 10, 50 ));
    wxASSERT( info->Create(control, 6, Params ));

    info = wxClassInfo::FindClass("wxStaticText");
    wxASSERT( info );
    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[0] = wxAny((wxWindow*)(panel));
    Params[1] = wxAny(wxWindowID(baseID++));
    Params[2] = wxAny(wxString("A Static Text!"));
    Params[3] = wxAny(wxPoint( 10, 70 ));
    Params[4] = wxAny(wxSize(-1,-1));
    Params[5] = wxAny((long)0);
    wxASSERT( info->Create(control, 6, Params ));

    info = wxClassInfo::FindClass("wxStaticBox");
    wxASSERT( info );
    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[0] = wxAny((wxWindow*)(panel));
    Params[1] = wxAny(wxWindowID(baseID++));
    Params[2] = wxAny(wxString("A Static Box"));
    Params[3] = wxAny(wxPoint( 10, 90 ));
    Params[4] = wxAny(wxSize(100,80));
    Params[5] = wxAny((long)0);
    wxASSERT( info->Create(control, 6, Params ));

    info = wxClassInfo::FindClass("wxTextCtrl");
    wxASSERT( info );
    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[0] = wxAny((wxWindow*)(panel));
    Params[1] = wxAny(wxWindowID(baseID++));
    Params[2] = wxAny(wxString("A Text Control"));
    Params[3] = wxAny(wxPoint( 10, 200 ));
    Params[4] = wxAny(wxSize(-1,-1));
    Params[5] = wxAny((long)0);
    wxASSERT( info->Create(control, 6, Params ));

    // spins and gauges page

    info = wxClassInfo::FindClass("wxPanel");
    wxASSERT( info );
    panel = wxDynamicCast( info->CreateObject(), wxPanel );
    Params[0] = wxAny((wxWindow*)(notebook));
    Params[1] = wxAny(wxWindowID(baseID++));
    Params[2] = wxAny(wxPoint(-1,-1));
    Params[3] = wxAny(wxSize(-1,-1));
    Params[4] = wxAny((long)0);
    Params[5] = wxAny(wxString("Hello"));
    wxASSERT( info->Create(panel, 6, Params ));
    notebook->AddPage( panel, "Spins and Sliders" );

    wxENSURE_CLASS_IS_LINKED(wxSpinButton);

    info = wxClassInfo::FindClass("wxSpinButton");
    wxASSERT( info );
    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[0] = wxAny((wxWindow*)(panel));
    Params[1] = wxAny(wxWindowID(baseID++));
    Params[2] = wxAny(wxPoint( 10, 10 ));
    Params[3] = wxAny(wxSize(-1,-1));
    Params[4] = wxAny((long)wxSP_VERTICAL | wxSP_ARROW_KEYS);
    wxASSERT( info->Create(control, 5, Params ));

    wxENSURE_CLASS_IS_LINKED(wxSpinCtrl);

    info = wxClassInfo::FindClass("wxSpinCtrl");
    wxASSERT( info );
    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[0] = wxAny((wxWindow*)(panel));
    Params[1] = wxAny(wxWindowID(baseID++));
    Params[2] = wxAny(wxString("20"));
    Params[3] = wxAny(wxPoint( 40, 10 ));
    Params[4] = wxAny(wxSize(40,-1));
    Params[5] = wxAny((long) wxSP_ARROW_KEYS);
    wxASSERT( info->Create(control, 6, Params ));

    // MSVC likes to exclude from link wxGauge...
    wxENSURE_CLASS_IS_LINKED(wxGauge)
    wxENSURE_CLASS_IS_LINKED(wxCheckBox)
    wxENSURE_CLASS_IS_LINKED(wxSpinCtrl)

    info = wxClassInfo::FindClass("wxGauge");

    wxASSERT( info );
    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[0] = wxAny((wxWindow*)(panel));
    Params[1] = wxAny(wxWindowID(baseID++));
    Params[2] = wxAny((int) 100);
    Params[3] = wxAny(wxPoint( 10, 50 ));
    Params[4] = wxAny(wxSize(-1,-1));
    Params[5] = wxAny((long) wxGA_HORIZONTAL);
    wxASSERT( info->Create(control, 6, Params ));
    wx_dynamic_cast(wxGauge*, control)->SetValue(20);

#endif
    return frameWrapper;
}

bool SaveFrameRTTI(const wxString &testFileName, wxDynamicObject *frame)
{
    // setup the XML document
    wxXmlDocument xml;
    wxXmlNode *root = new wxXmlNode(wxXML_ELEMENT_NODE,
                                    "TestXTI", "This is the content");
    xml.SetRoot(root);

    // setup the XTI writer and persister
    wxObjectXmlWriter writer(root);
    MyDesignerPersister persister(frame);

    // write the given wxObject into the XML document
    wxStringToAnyHashMap empty;
    writer.WriteObject( frame, frame->GetClassInfo(), &persister,
                        wxString("myTestFrame"), empty );

    return xml.Save(testFileName);
}

wxDynamicObject* LoadFrameRTTI(const wxString &fileName)
{
    // load the XML document
    wxXmlDocument xml;
    if (!xml.Load(fileName))
        return nullptr;

    wxXmlNode *root = xml.GetRoot();
    if (root->GetName() != "TestXTI")
        return nullptr;

    // now depersist the wxFrame we saved into it using wxObjectRuntimeReaderCallback
    wxObjectRuntimeReaderCallback Callbacks;
    wxObjectXmlReader Reader( root );
    int obj = Reader.ReadObject( wxString("myTestFrame"), &Callbacks );
    return (wxDynamicObject*)Callbacks.GetObject( obj );
}

bool GenerateFrameRTTICode(const wxString &inFileName, const wxString &outFileName)
{
    // is loading the streamed out component from xml and writing code that
    // will create the same component

    wxFFileOutputStream fos( outFileName );
    wxTextOutputStream tos( fos );
    if (!fos.IsOk())
        return false;

    wxXmlDocument xml;
    if (!xml.Load(inFileName))
        return false;

    wxXmlNode *root = xml.GetRoot();
    if (root->GetName() != "TestXTI")
        return false;

    // read the XML file using the wxObjectCodeReaderCallback

    wxString headerincludes;
    wxString sourcecode;
    wxObjectCodeReaderCallback Callbacks(headerincludes,sourcecode);
    wxObjectXmlReader Reader(root);

    // ReadObject will return the ID of the object read??
    Reader.ReadObject( wxString("myTestFrame"), &Callbacks );

    // header preamble
    tos <<
        "#include \"wx/wxprec.h\" \n#ifndef WX_PRECOMP\n#include \"wx/wx.h\" \n#endif\n\n";
    // add object includes
    tos.WriteString( headerincludes );

    tos << "\n\nvoid test()\n{";
    tos.WriteString( sourcecode );
    tos << "}";
    return true;
}




// ----------------------------------------------------------------------------
// MyFrame event handlers
// ----------------------------------------------------------------------------

void MyFrame::OnPersist(wxCommandEvent& WXUNUSED(event))
{
    // first create a frame using XTI calls
    wxDynamicObject *frame = CreateFrameRTTI();
    if (!frame)
    {
        wxLogError("Cannot create the XTI frame!");
        return;
    }

    // show the frame we're going to save to the user
    wxFrame *trueFrame = wx_dynamic_cast(wxFrame *, frame->GetSuperClassInstance() );
    trueFrame->Show();

    // ask the user where to save it
    wxFileDialog dlg(this, "Where should the frame be saved?",
                     wxEmptyString, "test.xml", "XML files (*.xml)|*.xml",
                     wxFD_SAVE);
    if (dlg.ShowModal() == wxID_CANCEL)
        return;

    // then save it to a test XML file
    if (!SaveFrameRTTI(dlg.GetPath(), frame))
    {
        wxLogError("Cannot save the XTI frame into '%s'", dlg.GetPath());
        return;
    }

    // now simply delete it
    delete frame;
}

void MyFrame::OnDepersist(wxCommandEvent& WXUNUSED(event))
{
    // ask the user which file to load
    wxFileDialog dlg(this, "Which file contains the frame to depersist?",
                     wxEmptyString, "test.xml", "XML files (*.xml)|*.xml",
                     wxFD_OPEN);
    if (dlg.ShowModal() == wxID_CANCEL)
        return;

    wxObject *frame = LoadFrameRTTI(dlg.GetPath());
    if (!frame)
    {
        wxLogError("Could not depersist the wxFrame from '%s'", dlg.GetPath());
        return;
    }

    wxFrame *trueFrame = wx_dynamic_cast(wxFrame*, frame );
    if ( !trueFrame )
    {
        wxDynamicObject* dyno = wx_dynamic_cast(wxDynamicObject*, frame );
        if ( dyno )
            trueFrame = wx_dynamic_cast(wxFrame *, dyno->GetSuperClassInstance() );
    }

    if ( trueFrame )
        trueFrame->Show();
    else
        wxLogError("Could not show the frame");
}

void MyFrame::OnGenerateCode(wxCommandEvent& WXUNUSED(event))
{
    // ask the user which file to load
    wxFileDialog dlg(this, "Which file contains the frame to work on?",
                     wxEmptyString, "test.xml", "XML files (*.xml)|*.xml",
                     wxFD_OPEN);
    if (dlg.ShowModal() == wxID_CANCEL)
        return;

    // ask the user which file to load
    wxFileDialog dlg2(this, "Where should the C++ code be saved?",
                      wxEmptyString, "test.cpp", "Source files (*.cpp)|*.cpp",
                      wxFD_SAVE);
    if (dlg2.ShowModal() == wxID_CANCEL)
        return;

    // do generate code
    if (!GenerateFrameRTTICode(dlg.GetPath(), dlg2.GetPath()))
    {
        wxLogError("Could not generate the code for the frame!");
        return;
    }

    // show the generated code
    {
        wxFileInputStream f(dlg2.GetPath());
        wxStringOutputStream str;
        f.Read(str);

        wxDialog dlg3(this, wxID_ANY, "Generated code",
                     wxDefaultPosition, wxDefaultSize,
                     wxRESIZE_BORDER|wxDEFAULT_DIALOG_STYLE);
        wxPanel *panel = new wxPanel(&dlg3);
        wxSizer *sz = new wxBoxSizer(wxVERTICAL);
        sz->Add(new wxTextCtrl(panel, wxID_ANY, str.GetString(),
                               wxDefaultPosition, wxDefaultSize,
                               wxTE_MULTILINE|wxTE_READONLY|wxTE_DONTWRAP),
                1, wxGROW|wxALL, 5);
        sz->Add(new wxButton(panel, wxID_OK), 0, wxALIGN_RIGHT|wxALL, 5);
        panel->SetSizerAndFit(sz);
        dlg3.ShowModal();
    }
}

void MyFrame::OnDumpClasses(wxCommandEvent& WXUNUSED(event))
{
    ClassListDialog dlg(this);
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
                    "Welcome to %s!\n"
                    "\n"
                    "This sample demonstrates wxWidgets eXtended RTTI (XTI) system.",
                    wxVERSION_STRING
                ),
                "About wxWidgets XTI sample",
                wxOK | wxICON_INFORMATION,
                this);
}
