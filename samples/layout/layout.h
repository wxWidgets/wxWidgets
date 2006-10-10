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
    MyApp(){};
    bool OnInit();
};

// the main frame class
class MyFrame : public wxFrame
{
public:
    MyFrame();

    void TestProportions(wxCommandEvent& event);
    void TestFlexSizers(wxCommandEvent& event);
    void TestNotebookSizers(wxCommandEvent& event);
    void TestGridBagSizer(wxCommandEvent& event);
    void TestSetMinimal(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);

private:
    DECLARE_EVENT_TABLE()
};

// a frame showing the box sizer proportions
class MyProportionsFrame : public wxFrame
{
public:
    MyProportionsFrame(wxFrame *parent);

protected:
    void UpdateProportions();

    void OnProportionChanged(wxSpinEvent& event);
    void OnProportionUpdated(wxCommandEvent& event);

    wxSpinCtrl *m_spins[3]; // size can be changed without changing anything else
    wxSizer *m_sizer;
};

// a frame using flex sizers for layout
class MyFlexSizerFrame : public wxFrame
{
public:
    MyFlexSizerFrame(const wxChar *title, int x, int y );

private:
    void InitFlexSizer(wxFlexGridSizer *sizer, wxWindow* parent);
};


// a dialog using notebook sizer for layout
class MySizerDialog : public wxDialog
{
public:
    MySizerDialog(wxWindow *parent, const wxChar *title);
};


// a frame using wxGridBagSizer for layout
class MyGridBagSizerFrame : public wxFrame
{
public:
    MyGridBagSizerFrame(const wxChar *title, int x, int y );

    void OnHideBtn(wxCommandEvent&);
    void OnShowBtn(wxCommandEvent&);
    void OnMoveBtn(wxCommandEvent&);

private:
    wxGridBagSizer*     m_gbs;
    wxPanel*            m_panel;
    wxButton*           m_hideBtn;
    wxButton*           m_showBtn;
    wxTextCtrl*         m_hideTxt;

    wxButton*           m_moveBtn1;
    wxButton*           m_moveBtn2;
    wxGBPosition        m_lastPos;

    DECLARE_EVENT_TABLE()
};


// a frame for testing simple setting of "default size"
class MySimpleSizerFrame : public wxFrame
{
public:
    MySimpleSizerFrame(const wxChar *title, int x, int y );
    
    void OnSetSmallSize( wxCommandEvent &event);
    void OnSetBigSize( wxCommandEvent &event);
    
private:
    wxTextCtrl  *m_target;

    DECLARE_EVENT_TABLE()
};


// controls and menu constants
enum
{
    LAYOUT_TEST_SIZER = 101,
    LAYOUT_TEST_NB_SIZER,
    LAYOUT_TEST_GB_SIZER,
    LAYOUT_TEST_PROPORTIONS,
    LAYOUT_TEST_SET_MINIMAL,
    LAYOUT_QUIT = wxID_EXIT,
    LAYOUT_ABOUT = wxID_ABOUT
};

