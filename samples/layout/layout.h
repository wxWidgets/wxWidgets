/////////////////////////////////////////////////////////////////////////////
// Name:        layout.h
// Purpose:     Layout sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// Define a new application
class MyApp: public wxApp
{
public:
    MyApp(){}
    bool OnInit() wxOVERRIDE;
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
    void TestNested(wxCommandEvent& event);
    void TestSetMinimal(wxCommandEvent& event);
    void TestWrap(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);

private:
    wxDECLARE_EVENT_TABLE();
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
    MyFlexSizerFrame(wxFrame* parent);

private:
    void InitFlexSizer(wxFlexGridSizer *sizer, wxWindow* parent);
};


// a dialog using notebook and sizers for layout
class MyNotebookWithSizerDialog : public wxDialog
{
public:
    MyNotebookWithSizerDialog(wxWindow *parent, const wxString &title );
};


// a frame using wxGridBagSizer for layout
class MyGridBagSizerFrame : public wxFrame
{
public:
    MyGridBagSizerFrame(wxFrame* parent);

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

    wxDECLARE_EVENT_TABLE();
};


// a frame for testing simple setting of "default size"
class MySimpleSizerFrame : public wxFrame
{
public:
    MySimpleSizerFrame(wxFrame* parent);

    void OnSetSmallSize( wxCommandEvent &event);
    void OnSetBigSize( wxCommandEvent &event);

private:
    wxTextCtrl  *m_target;

    wxDECLARE_EVENT_TABLE();
};


// a frame for testing simple setting of a frame containing
// a sizer containing a panel containing a sizer containing
// controls
class MyNestedSizerFrame : public wxFrame
{
public:
    MyNestedSizerFrame(wxFrame* parent);


private:
    wxTextCtrl  *m_target;
};

// a frame with several wrapping sizers

class MyWrapSizerFrame: public wxFrame
{
public:
    MyWrapSizerFrame(wxFrame* parent);

private:
    void OnAddCheckbox(wxCommandEvent& event);
    void OnRemoveCheckbox(wxCommandEvent& event);

    void DoAddCheckbox();

    wxWindow* m_checkboxParent;
    wxSizer* m_wrapSizer;

    wxDECLARE_EVENT_TABLE();
};

// controls and menu constants
enum
{
    LAYOUT_TEST_SIZER = 101,
    LAYOUT_TEST_NB_SIZER,
    LAYOUT_TEST_GB_SIZER,
    LAYOUT_TEST_PROPORTIONS,
    LAYOUT_TEST_SET_MINIMAL,
    LAYOUT_TEST_NESTED,
    LAYOUT_TEST_WRAP,
    LAYOUT_QUIT = wxID_EXIT,
    LAYOUT_ABOUT = wxID_ABOUT
};

