/////////////////////////////////////////////////////////////////////////////
// Name:        flash.cpp
// Purpose:     Sample showing integration of Flash ActiveX control
// Author:      Vadim Zeitlin
// Created:     2009-01-13
// RCS-ID:      $Id$
// Copyright:   (c) 2009 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
    Documentation for embedding Flash into anything other than a web browser is
    not easy to find, here is the tech note which provided most of the
    information used here: http://www.adobe.com/go/tn_12059
 */

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef __WXMSW__
    #error "ActiveX controls are MSW-only"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/cmdline.h"
#include "wx/filename.h"

#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

#include "wx/msw/ole/activex.h"

// we currently use VC-specific extensions in this sample, it could be
// rewritten to avoid them if there is real interest in doing it but compiler
// COM support in MSVC makes the code much simpler to understand
#ifndef __VISUALC__
    #error "This sample requires Microsoft Visual C++ compiler COM extensions"
#endif

// import Flash ActiveX control by using its (standard) type library UUID
//
// no_auto_exclude is needed to import IServiceProvider interface defined in
// this type library even though its name conflicts with a standard Windows
// interface with the same name
#import "libid:D27CDB6B-AE6D-11CF-96B8-444553540000" no_auto_exclude

using namespace ShockwaveFlashObjects;

const CLSID CLSID_ShockwaveFlash = __uuidof(ShockwaveFlash);
const IID IID_IShockwaveFlash = __uuidof(IShockwaveFlash);

inline wxString bstr2wx(const _bstr_t& bstr)
{
    return wxString(static_cast<const wchar_t *>(bstr));
}

inline _bstr_t wx2bstr(const wxString& str)
{
    return _bstr_t(str.wc_str());
}

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// taken from type library
namespace
{

const int FLASH_DISPID_ONREADYSTATECHANGE = -609; // DISPID_ONREADYSTATECHANGE
const int FLASH_DISPID_ONPROGRESS = 0x7a6;
const int FLASH_DISPID_FSCOMMAND = 0x96;
const int FLASH_DISPID_FLASHCALL = 0xc5;

enum FlashState
{
    FlashState_Unknown = -1,
    FlashState_Loading,
    FlashState_Uninitialized,
    FlashState_Loaded,
    FlashState_Interactive,
    FlashState_Complete,
    FlashState_Max
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class FlashApp : public wxApp
{
public:
    FlashApp() { }

    virtual bool OnInit();

    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

    virtual bool OnExceptionInMainLoop();

private:
    wxString m_swf;

    wxDECLARE_NO_COPY_CLASS(FlashApp);
};

// Define a new frame type: this is going to be our main frame
class FlashFrame : public wxFrame
{
public:
    // ctor takes ownership of the pointer which must be non-NULL and opens the
    // given SWF file if it's non-empty
    FlashFrame(IShockwaveFlash *flash, const wxString& swf);
    virtual ~FlashFrame();

    void SetMovie(const wxString& movie);

    void Play();
    void Stop();

private:
    enum
    {
        Flash_Play = 100,
        Flash_Get,
        Flash_Set,
        Flash_Call,
        Flash_CallWithArg
    };

    void OnOpen(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnPlay(wxCommandEvent&) { Play(); }
    void OnStop(wxCommandEvent&) { Stop(); }
    void OnBack(wxCommandEvent& event);
    void OnForward(wxCommandEvent& event);
    void OnInfo(wxCommandEvent& event);
    void OnVarGet(wxCommandEvent& event);
    void OnVarSet(wxCommandEvent& event);
    void OnCall(wxCommandEvent& event);
    void OnCallWithArg(wxCommandEvent& event);

    void OnActiveXEvent(wxActiveXEvent& event);

    // give an error message if hr is not S_OK
    void CheckFlashCall(HRESULT hr, const char *func);

    // return the name of the Flash control state
    wxString GetFlashStateString(int state);

    // call CallFunction() with a single argument of the type specified by
    // argtype or without any arguments if it is empty
    void CallFlashFunc(const wxString& argtype,
                       const wxString& func,
                       const wxString& arg = wxString());


    const IShockwaveFlashPtr m_flash;
    wxLog *m_oldLog;
    wxString m_swf;
    FlashState m_state;

    wxTextCtrl *m_varname,
               *m_varvalue,
               *m_funcname,
               *m_funcarg;

    DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(FlashFrame);
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FlashFrame, wxFrame)
    EVT_MENU(wxID_OPEN,  FlashFrame::OnOpen)
    EVT_MENU(wxID_EXIT,  FlashFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, FlashFrame::OnAbout)

    EVT_BUTTON(Flash_Play, FlashFrame::OnPlay)
    EVT_BUTTON(wxID_STOP,  FlashFrame::OnStop)
    EVT_BUTTON(wxID_BACKWARD, FlashFrame::OnBack)
    EVT_BUTTON(wxID_FORWARD,  FlashFrame::OnForward)

    EVT_BUTTON(wxID_INFO, FlashFrame::OnInfo)
    EVT_BUTTON(Flash_Get, FlashFrame::OnVarGet)
    EVT_BUTTON(Flash_Set, FlashFrame::OnVarSet)
    EVT_BUTTON(Flash_Call, FlashFrame::OnCall)
    EVT_BUTTON(Flash_CallWithArg, FlashFrame::OnCallWithArg)

    EVT_ACTIVEX(wxID_ANY, FlashFrame::OnActiveXEvent)
END_EVENT_TABLE()

IMPLEMENT_APP(FlashApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

void FlashApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxApp::OnInitCmdLine(parser);

    parser.AddParam("SWF file to play",
                    wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
}

bool FlashApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    if ( parser.GetParamCount() )
        m_swf = parser.GetParam(0);

    return wxApp::OnCmdLineParsed(parser);
}

bool FlashApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    IShockwaveFlash *flash = NULL;
    HRESULT hr = ::CoCreateInstance
                   (
                    CLSID_ShockwaveFlash,
                    NULL,
                    CLSCTX_INPROC_SERVER,
                    IID_IShockwaveFlash,
                    (void **)&flash
                   );
    if ( FAILED(hr) )
    {
        wxLogSysError(hr, "Failed to create Flash ActiveX control");
        return false;
    }

    new FlashFrame(flash, m_swf);

    return true;
}

bool FlashApp::OnExceptionInMainLoop()
{
    try
    {
        throw;
    }
    catch ( _com_error& ce )
    {
        wxLogMessage("COM exception: %s", ce.ErrorMessage());

        return true;
    }
    catch ( ... )
    {
        throw;
    }
}

// ----------------------------------------------------------------------------
// main frame creation
// ----------------------------------------------------------------------------

// frame constructor
FlashFrame::FlashFrame(IShockwaveFlash *flash, const wxString& swf)
    : wxFrame(NULL, wxID_ANY, "wxWidgets Flash sample"),
      m_flash(flash, false /* take ownership */),
      m_swf(swf),
      m_state(FlashState_Unknown)
{
    // set the frame icon
    SetIcon(wxICON(sample));

    // create the new log target before doing anything with the Flash that
    // could result in log messages
    wxTextCtrl * const log = new wxTextCtrl(this, wxID_ANY, "",
                                            wxDefaultPosition, wxSize(-1, 100),
                                            wxTE_MULTILINE);
    m_oldLog = wxLog::SetActiveTarget(new wxLogTextCtrl(log));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_OPEN);
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT);

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT);

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets Flash embedding sample");
    SetStatusText("No loaded file", 1);
#endif // wxUSE_STATUSBAR

    wxPanel * const panel = new wxPanel(this);
    wxSizer * const sizerPanel = new wxBoxSizer(wxVERTICAL);
    wxWindow * const activeXParent = new wxWindow(panel, wxID_ANY,
                                                  wxDefaultPosition,
                                                  wxSize(300, 200));
    new wxActiveXContainer(activeXParent, IID_IShockwaveFlash, flash);
    if ( !swf.empty() )
        SetMovie(swf);

    sizerPanel->Add(activeXParent,
                    wxSizerFlags(1).Expand().Border());

    const wxSizerFlags flagsHorz(wxSizerFlags().Centre().HorzBorder());

    wxSizer * const sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    sizerBtns->Add(new wxButton(panel, wxID_BACKWARD), flagsHorz);
    sizerBtns->Add(new wxButton(panel, Flash_Play, "&Play"), flagsHorz);
    sizerBtns->Add(new wxButton(panel, wxID_STOP), flagsHorz);
    sizerBtns->Add(new wxButton(panel, wxID_FORWARD), flagsHorz);
    sizerBtns->AddSpacer(20);
    sizerBtns->Add(new wxButton(panel, wxID_INFO), flagsHorz);
    sizerPanel->Add(sizerBtns, wxSizerFlags().Center().Border());

    wxSizer * const sizerVar = new wxBoxSizer(wxHORIZONTAL);
    sizerVar->Add(new wxStaticText(panel, wxID_ANY, "Variable &name:"),
                  flagsHorz);
    m_varname = new wxTextCtrl(panel, wxID_ANY);
    sizerVar->Add(m_varname, flagsHorz);
    sizerVar->Add(new wxStaticText(panel, wxID_ANY, "&value:"),
                  flagsHorz);
    m_varvalue = new wxTextCtrl(panel, wxID_ANY);
    sizerVar->Add(m_varvalue, flagsHorz);
    sizerVar->AddSpacer(10);
    sizerVar->Add(new wxButton(panel, Flash_Get, "&Get"), flagsHorz);
    sizerVar->Add(new wxButton(panel, Flash_Set, "&Set"), flagsHorz);
    sizerPanel->Add(sizerVar, wxSizerFlags().Center().Border());

    wxSizer * const sizerCall = new wxBoxSizer(wxHORIZONTAL);
    sizerCall->Add(new wxStaticText(panel, wxID_ANY, "&Function name:"),
                   flagsHorz);
    m_funcname = new wxTextCtrl(panel, wxID_ANY);
    sizerCall->Add(m_funcname, flagsHorz);
    sizerCall->Add(new wxButton(panel, Flash_Call, "&Call"), flagsHorz);
    sizerCall->Add(new wxStaticText(panel, wxID_ANY, "&argument:"),
                   flagsHorz);
    m_funcarg = new wxTextCtrl(panel, wxID_ANY);
    sizerCall->Add(m_funcarg, flagsHorz);
    sizerCall->Add(new wxButton(panel, Flash_CallWithArg, "Call &with arg"),
                   flagsHorz);
    sizerPanel->Add(sizerCall, wxSizerFlags().Center().Border());

    panel->SetSizer(sizerPanel);

    wxSizer * const sizerFrame = new wxBoxSizer(wxVERTICAL);
    sizerFrame->Add(panel, wxSizerFlags(2).Expand());
    sizerFrame->Add(log, wxSizerFlags(1).Expand());
    SetSizerAndFit(sizerFrame);

    Show();

    m_flash->PutAllowScriptAccess(L"always");
    wxLogMessage("Script access changed to \"%s\"",
                 bstr2wx(m_flash->GetAllowScriptAccess()));
}

FlashFrame::~FlashFrame()
{
    delete wxLog::SetActiveTarget(m_oldLog);
}

// ----------------------------------------------------------------------------
// Flash API wrappers
// ----------------------------------------------------------------------------

void FlashFrame::CheckFlashCall(HRESULT hr, const char *func)
{
    if ( FAILED(hr) )
    {
        wxLogSysError(hr, "Call to IShockwaveFlash::%s() failed", func);
    }
}

void FlashFrame::CallFlashFunc(const wxString& argtype,
                               const wxString& func,
                               const wxString& arg)
{
    wxString args;
    if ( !argtype.empty() )
    {
        args = wxString::Format("<%s>%s</%s>", argtype, arg, argtype);
    }

    // take care with XML formatting: there should be no spaces in it or the
    // call would fail!
    wxString request = wxString::Format
                       (
                            "<invoke name=\"%s\" returntype=\"xml\">"
                                "<arguments>"
                                "%s"
                                "</arguments>"
                            "</invoke>",
                            func,
                            args
                       );

    wxLogMessage("%s(%s) returned \"%s\"",
                 func, args,
                 bstr2wx(m_flash->CallFunction(wx2bstr(request))));
}

wxString FlashFrame::GetFlashStateString(int state)
{
    static const char *knownStates[] =
    {
        "Loading", "Uninitialized", "Loaded", "Interactive", "Complete",
    };

    if ( state >= 0 && state < WXSIZEOF(knownStates) )
        return knownStates[state];

    return wxString::Format("unknown state (%d)", state);
}

void FlashFrame::SetMovie(const wxString& movie)
{
    // Flash doesn't like relative file names
    wxFileName fn(movie);
    fn.MakeAbsolute();
    const wxString swf = fn.GetFullPath();
    if ( swf == m_swf )
        m_flash->PutMovie(L"");
    else
        m_swf = swf;

    m_flash->PutMovie(m_swf.wc_str());

    SetStatusText("Loaded \"" + m_swf + '"', 1);
}

void FlashFrame::Play()
{
    CheckFlashCall(m_flash->Play(), "Play");
}

void FlashFrame::Stop()
{
    CheckFlashCall(m_flash->Stop(), "Stop");
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void FlashFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
    wxString swf = wxLoadFileSelector("Flash movie", ".swf", m_swf, this);
    if ( swf.empty() )
        return;

    SetMovie(swf);
}

void FlashFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void FlashFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("Flash ActiveX control embedding sample\n"
                 "\n"
                 "(c) 2009 Vadim Zeitlin",
                 "About " + GetTitle(),
                 wxOK | wxICON_INFORMATION,
                 this);
}

void FlashFrame::OnActiveXEvent(wxActiveXEvent& event)
{
    switch ( event.GetDispatchId() )
    {
        case FLASH_DISPID_ONREADYSTATECHANGE:
            {
                const int state = event[0].GetInteger();
                if ( state != m_state )
                {
                    wxLogMessage("State changed to %s",
                                 GetFlashStateString(state));

                    if ( state >= 0 && state < FlashState_Max )
                        m_state = static_cast<FlashState>(state);
                    else
                        m_state = FlashState_Unknown;
                }
            }
            break;

        case FLASH_DISPID_ONPROGRESS:
            wxLogMessage("Progress: %d%%", event[0].GetInteger());
            break;

        case FLASH_DISPID_FSCOMMAND:
            wxLogMessage("Flash command %s(%s)",
                         event[0].GetString(), event[1].GetString());
            break;

        case FLASH_DISPID_FLASHCALL:
            wxLogMessage("Flash request \"%s\"", event[0].GetString());
            break;

        default:
            wxLogMessage("Unknown event %ld", event.GetDispatchId());
    }

    event.Skip();
}

void FlashFrame::OnBack(wxCommandEvent& WXUNUSED(event))
{
    CheckFlashCall(m_flash->Back(), "Back");
}

void FlashFrame::OnForward(wxCommandEvent& WXUNUSED(event))
{
    CheckFlashCall(m_flash->Forward(), "Forward");
}

void FlashFrame::OnInfo(wxCommandEvent& WXUNUSED(event))
{
    const int state = m_flash->GetReadyState();
    wxString msg = "State: " + GetFlashStateString(state);

    if ( state == FlashState_Complete )
    {
        msg += wxString::Format(", frame: %ld/%ld",
                                m_flash->GetFrameNum() + 1,
                                m_flash->GetTotalFrames());
    }

    if ( m_flash->IsPlaying() )
        msg += ", playing";

    wxLogMessage("%s", msg);
}

void FlashFrame::OnVarGet(wxCommandEvent& WXUNUSED(event))
{
    m_varvalue->SetValue(bstr2wx(
        m_flash->GetVariable(wx2bstr(m_varname->GetValue()))));
}

void FlashFrame::OnVarSet(wxCommandEvent& WXUNUSED(event))
{
    m_flash->SetVariable(wx2bstr(m_varname->GetValue()),
                         wx2bstr(m_varvalue->GetValue()));
}

void FlashFrame::OnCall(wxCommandEvent& WXUNUSED(event))
{
    CallFlashFunc("", m_funcname->GetValue());
}

void FlashFrame::OnCallWithArg(wxCommandEvent& WXUNUSED(event))
{
    CallFlashFunc("string", m_funcname->GetValue(), m_funcarg->GetValue());
}



