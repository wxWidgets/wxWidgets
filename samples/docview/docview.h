/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/docview.h
// Purpose:     Document/view demo
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: merge with the MDI version and general cleanup
// Created:     04/01/98
// Copyright:   (c) 1998 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SAMPLES_DOCVIEW_DOCVIEW_H_
#define _WX_SAMPLES_DOCVIEW_DOCVIEW_H_

#include "wx/docview.h"
#include "wx/vector.h"

class MyCanvas;

// Define a new application
class MyApp : public wxApp
{
public:
    // this sample can be launched in several different ways:
    enum Mode
    {
#if wxUSE_MDI_ARCHITECTURE
        Mode_MDI,   // MDI mode: multiple documents, single top level window
#endif // wxUSE_MDI_ARCHITECTURE
#if wxUSE_AUI
        Mode_AUI,   // MDI AUI mode
#endif // wxUSE_AUI
        Mode_SDI,   // SDI mode: multiple documents, multiple top level windows
        Mode_Single // single document mode (and hence single top level window)
    };

    MyApp();
    MyApp(const MyApp&) = delete;
    MyApp& operator=(const MyApp&) = delete;

    // override some wxApp virtual methods
    virtual bool OnInit() override;
    virtual int OnExit() override;

    virtual void OnInitCmdLine(wxCmdLineParser& parser) override;
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser) override;

#ifdef __WXMAC__
    virtual void MacNewFile() override;
#endif // __WXMAC__

    // our specific methods
    Mode GetMode() const { return m_mode; }
    wxFrame *CreateChildFrame(wxView *view, bool isCanvas);

    // these accessors should only be called in single document mode, otherwise
    // the pointers are null and an assert is triggered
    MyCanvas *GetMainWindowCanvas() const
        { wxASSERT(m_canvas); return m_canvas; }
    wxMenu *GetMainWindowEditMenu() const
        { wxASSERT(m_menuEdit); return m_menuEdit; }

private:
    // append the standard document-oriented menu commands to this menu
    void AppendDocumentFileCommands(wxMenu *menu, bool supportsPrinting);

    // create the edit menu for drawing documents
    wxMenu *CreateDrawingEditMenu();

    // create and associate with the given frame the menu bar containing the
    // given file and edit (possibly null) menus as well as the standard help
    // one
    void CreateMenuBarForFrame(wxFrame *frame, wxMenu *file, wxMenu *edit);


    // force close all windows
    void OnForceCloseAll(wxCommandEvent& event);

    // show the about box: as we can have different frames it's more
    // convenient, even if somewhat less usual, to handle this in the
    // application object itself
    void OnAbout(wxCommandEvent& event);

    // contains the file names given on the command line, possibly empty
    wxVector<wxString> m_filesFromCmdLine;

    // the currently used mode
    Mode m_mode;

    // only used if m_mode == Mode_Single
    MyCanvas *m_canvas;
    wxMenu *m_menuEdit;

    wxDECLARE_EVENT_TABLE();
};

wxDECLARE_APP(MyApp);

#endif // _WX_SAMPLES_DOCVIEW_DOCVIEW_H_
