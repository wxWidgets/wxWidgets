/////////////////////////////////////////////////////////////////////////////
// Name:        layout.h
// Purpose:     Layout sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Define a new application
class MyApp: public wxApp
{
public:
    MyApp();
    bool OnInit();
};

// the main frame class
class MyFrame : public wxFrame
{
public:
    MyFrame();

    void TestConstraints(wxCommandEvent& event);
    void TestFlexSizers(wxCommandEvent& event);
    void TestNotebookSizers(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

// a frame using constraints for layout
class MyConstraintsFrame : public wxFrame
{
public:
    MyConstraintsFrame(const wxChar *title, int x, int y );
};

// a frame using flex sizers for layout
class MyFlexSizerFrame : public wxFrame
{
public:
    MyFlexSizerFrame(const wxChar *title, int x, int y );

private:
    void InitFlexSizer(wxFlexGridSizer *sizer);
};

// a dialog using notebook sizer for layout
class MySizerDialog : public wxDialog
{
public:
    MySizerDialog(wxWindow *parent, const wxChar *title);
};

// controls an menu constants
enum
{
    LAYOUT_QUIT = 100,
    LAYOUT_ABOUT,
    LAYOUT_TEST_CONSTRAINTS,
    LAYOUT_TEST_SIZER,
    LAYOUT_TEST_NB_SIZER
};

