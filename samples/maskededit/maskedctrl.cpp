/////////////////////////////////////////////////////////////////////////////
// Name:        samples/maskededit/maskedctrl.cpp
// Purpose:     wxWidgets masked edit control sample
// Author:      Manuel Martin
// Modified by:
// Created:     2012-07-30
// RCS-ID:      $Id$
// Copyright:   (c) 2012 Manuel Martin
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

#include "wx/generic/maskededit.h"

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
    void OnBell(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnUpdatePlain(wxCommandEvent& event);

    //A container for some of our control's IDs
    wxVector<long> ctrIDs;

private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// Define a new panel type: this will fit inside our main frame
class MyPanel : public wxPanel
{
public:
    // ctor(s)
    MyPanel(MyFrame* parent);

    //Structs used in test functions in this sample
    wxRangeParams fparamsIP;
    wxRangeParams fparams23;
    wxRangeParams fparams59;
    wxDateParams  dparams1;
    wxDateParams  dparams2;
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    MaskCtrl_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    MaskCtrl_About = wxID_ABOUT,

    MaskCtrl_Bell = wxID_HIGHEST + 1000,
    MaskCtrl_First
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(MaskCtrl_Quit,  MyFrame::OnQuit)
    EVT_MENU(MaskCtrl_Bell,  MyFrame::OnBell)
    EVT_MENU(MaskCtrl_About, MyFrame::OnAbout)
    EVT_TEXT(wxID_ANY, MyFrame::OnUpdatePlain)
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
    MyFrame *frame = new MyFrame("Masked Edit Control wxWidgets App");

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
    wxMenuBar *menuBar = new wxMenuBar();

    // set menus
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(MaskCtrl_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenu *bellMenu = new wxMenu;
    bellMenu->AppendCheckItem(MaskCtrl_Bell, _("&Bell on error"),
                               _("Toggle bell on error"));
    bellMenu->Check(MaskCtrl_Bell, true);

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(MaskCtrl_About, "&About...\tF1", "Show about dialog");

    // now append the freshly created menu to the menu bar...
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(bellMenu, "&Bell");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
#endif // wxUSE_STATUSBAR

    //Add a panel to place our controls
    MyPanel* myPanel = new MyPanel(this);

    //The sizer is set for the panel. Because the panel is the only child
    //of the frame, it will fill all frame's client area.
    //We also want to set the minimum size of this frame.
    myPanel->GetSizer()->SetSizeHints(this);

    //Set the focus to our first control
    FindWindow(MaskCtrl_First)->SetFocus();
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close

    Close(true);
}

void MyFrame::OnBell(wxCommandEvent& WXUNUSED(event))
{
    //Use the ID of each wxMaskedEdit control
    wxVector<long>::const_iterator it;
    for (it = ctrIDs.begin(); it != ctrIDs.end(); it++)
    {
        wxWindow* win = FindWindow(*it);
        if ( wxMaskedEditText *etext = wxDynamicCast(win, wxMaskedEditText) )
            etext->SetBellOnError( GetMenuBar()->IsChecked(MaskCtrl_Bell) );
        else if ( wxMaskedEditCombo *ecombo = wxDynamicCast(win, wxMaskedEditCombo) )
            ecombo->SetBellOnError( GetMenuBar()->IsChecked(MaskCtrl_Bell) );
    }
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
                 (
                    "Welcome to %s!\n"
                    "\n"
                    "This is the Masked Edit Control wxWidgets sample\n"
                    "running under %s.",
                    wxVERSION_STRING,
                    wxGetOsDescription()
                 ),
                 "About wxWidgets Masked Control sample",
                 wxOK | wxICON_INFORMATION,
                 this);
}

void MyFrame::OnUpdatePlain(wxCommandEvent& event)
{
    //We want to reflect changes only for the wxMaskedEditXXXX samples
    //Use the ID of each wxMaskedEditXXXX control
    long plainTextID = -1;
    wxVector<long>::const_iterator it;
    for (it = ctrIDs.begin(); it != ctrIDs.end(); it++)
    {
        if ( *it == event.GetId() )
        {
            plainTextID = *it + 100;
            break;
        }
    }

    if (plainTextID == -1)
        return; //The sender for this event is not in our list

    //the wxTextCtrl where we show the plain value of its partner
    wxWindow *win = FindWindow(plainTextID);
    if ( !win )
        return;
    wxTextCtrl *ptc = wxDynamicCast(win, wxTextCtrl);

    win = FindWindow(plainTextID - 100);
    if ( wxMaskedEditText *etext = wxDynamicCast(win, wxMaskedEditText) )
        ptc->SetValue( etext->GetAllFieldsValue() );
    else if ( wxMaskedEditCombo *ecombo = wxDynamicCast(win, wxMaskedEditCombo) )
        ptc->SetValue( ecombo->GetAllFieldsValue() );
}

// ----------------------------------------------------------------------------
// An example function that operates with the whole control value.
// In this case, it does not need any extra data. So we don't need any special
//  struct for that data. 'params' is useless in this example.
// It's definition is fixed:
long MyFunction(const wxMaskedEdit* caller, void* WXUNUSED(params))
{
    //Get the whole control's value, without decorations
    wxString str = caller->GetAllFieldsValue();

    //Sum of distances to '0'
    long sum = 0;
    wxString::const_iterator it = str.begin();
    while ( it != str.end() )
    {
        sum += *it - wxChar('0');
        it++;
    }

    //We accept any sum > 0 and multiple of 5
    if ( sum > 0 && (sum % 5) == 0 )
        return -1; //means "OK"

    //Wrong value. We can't say where the error is, so return the beginning.
    return 0;
}

// ----------------------------------------------------------------------------
// The panel
// ----------------------------------------------------------------------------

MyPanel::MyPanel(MyFrame* parent)
             : wxPanel(parent,
                       wxID_ANY,
                       wxDefaultPosition,
                       wxDefaultSize,
                       wxTAB_TRAVERSAL)
{
    SetBackgroundColour(wxColour(235,235,255));

    //A grid sizer to layout formatting samples
    wxFlexGridSizer *mainSizer = new wxFlexGridSizer(4, 5, 5);

    //Reusable pointers
    wxStaticText *stDesText;
    wxStaticText *stMaskText;
    wxTextCtrl *tcPlainText;
    wxMaskedEditText *edTexCtrl;
    wxMaskedEditCombo *edComboCtrl;
    wxString edMask;
    wxString edPlain;

    //IDs for edit and 'PlainText' controls
    long idCtrl = MaskCtrl_First;

    //Sizer flags
    wxSizerFlags sizerFlags(0);
    sizerFlags.Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    sizerFlags.Border(wxLEFT | wxRIGHT | wxTOP, 10);

    //Colours
    wxMaskedEditColours mskColours;
    mskColours.colOKBackground.Set(160, 255, 100);
    mskColours.colOKForeground.Set(0, 0, 255);
    mskColours.colInvaldBackground.Set(255, 125, 100);

    //Headers
    stDesText = new wxStaticText(this, wxID_ANY, "Description");
    stDesText->SetFont( GetFont().Bold() );
    mainSizer->Add(stDesText, sizerFlags);
    stDesText = new wxStaticText(this, wxID_ANY, "Mask");
    stDesText->SetFont( GetFont().Bold() );
    mainSizer->Add(stDesText, sizerFlags);
    stDesText = new wxStaticText(this, wxID_ANY, "Edit control");
    stDesText->SetFont( GetFont().Bold() );
    mainSizer->Add(stDesText, sizerFlags);
    stDesText = new wxStaticText(this, wxID_ANY, "Plain Value");
    stDesText->SetFont( GetFont().Bold() );
    mainSizer->Add(stDesText, sizerFlags);

    sizerFlags.Border(wxLEFT | wxRIGHT, 10);

#if wxUSE_TOOLTIPS
    //The tooltip string
    wxString strTip;
#endif

    //Sample 1
    stDesText = new wxStaticText(this, wxID_ANY, "Age");
    mainSizer->Add(stDesText, sizerFlags);
    edMask = "999";
    stMaskText = new wxStaticText(this, wxID_ANY, edMask);
    mainSizer->Add(stMaskText, sizerFlags);

    edTexCtrl = new wxMaskedEditText(this, idCtrl);
    parent->ctrIDs.push_back(idCtrl);
    edTexCtrl->SetMask(edMask);
    edTexCtrl->SetFieldFlags(0, wxMaskedEditFieldFlags(wxALIGN_RIGHT));
    mainSizer->Add(edTexCtrl, sizerFlags);

    edPlain = edTexCtrl->GetAllFieldsValue();
    tcPlainText = new wxTextCtrl(this, idCtrl + 100, edPlain);
    tcPlainText->SetEditable(false);
    tcPlainText->SetMinSize( edTexCtrl->GetMinSize() );
    mainSizer->Add(tcPlainText, sizerFlags);

#if wxUSE_TOOLTIPS
    strTip = "Fields alignment: R\n";
    strTip += "Fields fill chars: ' '\n";
    strTip += "Fields padding chars: ''";
    edTexCtrl->SetToolTip(strTip);
#endif

    //Sample 2
    idCtrl++;
    stDesText = new wxStaticText(this, wxID_ANY, "Anger expression");
    mainSizer->Add(stDesText, sizerFlags);
    edMask = "c{20}";
    stMaskText = new wxStaticText(this, wxID_ANY, edMask);
    mainSizer->Add(stMaskText, sizerFlags);

    edTexCtrl = new wxMaskedEditText(this, idCtrl);
    parent->ctrIDs.push_back(idCtrl);
    edTexCtrl->SetMask(edMask);
    edTexCtrl->SetFieldFlags(0, wxMaskedEditFieldFlags().SetFillChar('_'));
    edTexCtrl->SetValue("GRRR ***@&#%!!!");
    mainSizer->Add(edTexCtrl, sizerFlags);

    edPlain = edTexCtrl->GetAllFieldsValue();
    tcPlainText = new wxTextCtrl(this, idCtrl + 100, edPlain);
    tcPlainText->SetEditable(false);
    tcPlainText->SetMinSize( edTexCtrl->GetMinSize() );
    mainSizer->Add(tcPlainText, sizerFlags);

#if wxUSE_TOOLTIPS
    strTip = "Fields alignment: L\n";
    strTip += "Fields fill chars: '_'\n";
    strTip += "Fields padding chars: ''";
    edTexCtrl->SetToolTip(strTip);
#endif

    //Sample 3
    idCtrl++;
    stDesText = new wxStaticText(this, wxID_ANY, "Phone No");
    mainSizer->Add(stDesText, sizerFlags);
    edMask = "(999) ###-#### E\\xt.999";
    stMaskText = new wxStaticText(this, wxID_ANY, edMask);
    mainSizer->Add(stMaskText, sizerFlags);

    edTexCtrl = new wxMaskedEditText(this, idCtrl);
    parent->ctrIDs.push_back(idCtrl);
    edTexCtrl->SetMask(edMask);
    wxMaskedEditFieldFlags fieldFlags;
    fieldFlags.SetAlignment(wxALIGN_RIGHT);
    fieldFlags.SetPaddingChar('0');
    edTexCtrl->SetAllFieldsFlags(fieldFlags);
    edTexCtrl->SetMaskedColours(mskColours);
    mainSizer->Add(edTexCtrl, sizerFlags);

    edPlain = edTexCtrl->GetAllFieldsValue();
    tcPlainText = new wxTextCtrl(this, idCtrl + 100, edPlain);
    tcPlainText->SetEditable(false);
    tcPlainText->SetMinSize( edTexCtrl->GetMinSize() );
    mainSizer->Add(tcPlainText, sizerFlags);

#if wxUSE_TOOLTIPS
    strTip = "Fields alignment: R,R,R,R\n";
    strTip += "Fields fill chars: ' ',' ',' ',' '\n";
    strTip += "Fields padding chars: '0','0','0','0'\n";
    strTip += "Coloured";
    edTexCtrl->SetToolTip(strTip);
#endif

    //Sample 4
    idCtrl++;
    stDesText = new wxStaticText(this, wxID_ANY, "ADDRESS:offset");
    mainSizer->Add(stDesText, sizerFlags);
    edMask = ">x{4}:<x{4}";
    stMaskText = new wxStaticText(this, wxID_ANY, edMask);
    mainSizer->Add(stMaskText, sizerFlags);

    edTexCtrl = new wxMaskedEditText(this, idCtrl);
    parent->ctrIDs.push_back(idCtrl);
    edTexCtrl->SetMask(edMask);
    edTexCtrl->SetAllFieldsFlags(fieldFlags); //same as sample 3
    mainSizer->Add(edTexCtrl, sizerFlags);

    edPlain = edTexCtrl->GetAllFieldsValue();
    tcPlainText = new wxTextCtrl(this, idCtrl + 100, edPlain);
    tcPlainText->SetEditable(false);
    tcPlainText->SetMinSize( edTexCtrl->GetMinSize() );
    mainSizer->Add(tcPlainText, sizerFlags);

#if wxUSE_TOOLTIPS
    strTip = "Fields alignment: R,R\n";
    strTip += "Fields fill chars: ' ',' '\n";
    strTip += "Fields padding chars: '0','0'\n";
    edTexCtrl->SetToolTip(strTip);
#endif

    //Sample 5
    idCtrl++;
    stDesText = new wxStaticText(this, wxID_ANY, "IP v4");
    mainSizer->Add(stDesText, sizerFlags);
    edMask = "99#.99#.99#.99#";
    stMaskText = new wxStaticText(this, wxID_ANY, edMask);
    mainSizer->Add(stMaskText, sizerFlags);

    edTexCtrl = new wxMaskedEditText(this, idCtrl);
    parent->ctrIDs.push_back(idCtrl);
    edTexCtrl->SetMask(edMask);

    edTexCtrl->SetAllFieldsFlags(fieldFlags); //same as sample 3
    edTexCtrl->SetMaskedColours(mskColours);
    mainSizer->Add(edTexCtrl, sizerFlags);
    edTexCtrl->SetPlainValue("1920");
    //use the predefined range function
    fparamsIP.rmin = 0;
    fparamsIP.rmax = 255;
    edTexCtrl->SetAllFieldsFunction(&wxMaskedRangeCheck, &fparamsIP);

    edPlain = edTexCtrl->GetAllFieldsValue();
    tcPlainText = new wxTextCtrl(this, idCtrl + 100, edPlain);
    tcPlainText->SetEditable(false);
    tcPlainText->SetMinSize( edTexCtrl->GetMinSize() );
    mainSizer->Add(tcPlainText, sizerFlags);

#if wxUSE_TOOLTIPS
    strTip = "Fields alignment: R,R,R,R\n";
    strTip += "Fields fill chars: ' ',' ',' ',' '\n";
    strTip += "Fields padding chars: '0','0','0','0'\n";
    strTip += "Coloured\n";
    strTip += "Same range check function in all fields";
    edTexCtrl->SetToolTip(strTip);
#endif

    //Sample 6
    idCtrl++;
    stDesText = new wxStaticText(this, wxID_ANY, "hh:mm:ss");
    mainSizer->Add(stDesText, sizerFlags);
    edMask = "99:99:99";
    stMaskText = new wxStaticText(this, wxID_ANY, edMask);
    mainSizer->Add(stMaskText, sizerFlags);

    edTexCtrl = new wxMaskedEditText(this, idCtrl);
    parent->ctrIDs.push_back(idCtrl);
    edTexCtrl->SetMask(edMask);

    edTexCtrl->SetAllFieldsFlags(fieldFlags); //same flags as sample 3
    edTexCtrl->SetMaskedColours(mskColours); //idem
    mainSizer->Add(edTexCtrl, sizerFlags);
    //use the predefined range function
    fparams23.rmin = 0;
    fparams23.rmax = 23;
    edTexCtrl->SetFieldFunction(0, &wxMaskedRangeCheck, &fparams23);
    fparams59.rmin = 0;
    fparams59.rmax = 59;
    edTexCtrl->SetFieldFunction(1, &wxMaskedRangeCheck, &fparams59);
    edTexCtrl->SetFieldFunction(2, &wxMaskedRangeCheck, &fparams59);

    edPlain = edTexCtrl->GetAllFieldsValue();
    tcPlainText = new wxTextCtrl(this, idCtrl + 100, edPlain);
    tcPlainText->SetEditable(false);
    tcPlainText->SetMinSize( edTexCtrl->GetMinSize() );
    mainSizer->Add(tcPlainText, sizerFlags);

#if wxUSE_TOOLTIPS
    strTip = "Fields alignment: R,R,R\n";
    strTip += "Fields fill chars: ' ',' ',' '\n";
    strTip += "Fields padding chars: '0','0','0'\n";
    strTip += "Coloured\n";
    strTip += "Different range check functions in fields";
    edTexCtrl->SetToolTip(strTip);
#endif

    //Sample 7
    idCtrl++;
    stDesText = new wxStaticText(this, wxID_ANY, "Date mm/dd/yyyy");
    mainSizer->Add(stDesText, sizerFlags);
    edMask = "9#/9#/####";
    stMaskText = new wxStaticText(this, wxID_ANY, edMask);
    mainSizer->Add(stMaskText, sizerFlags);

    edTexCtrl = new wxMaskedEditText(this, idCtrl);
    parent->ctrIDs.push_back(idCtrl);
    edTexCtrl->SetMask(edMask);

    edTexCtrl->SetAllFieldsFlags(fieldFlags); //same flags as sample 3
    edTexCtrl->SetFieldValue(2, "2012");
    edTexCtrl->SetMaskedColours(mskColours); //idem
    mainSizer->Add(edTexCtrl, sizerFlags);
    //use the predefined date function
    dparams1.monthField = 0;
    dparams1.dayField = 1;
    dparams1.yearField = 2;
    edTexCtrl->SetControlFunction( &wxMaskedDateShort, &dparams1 );

    edPlain = edTexCtrl->GetAllFieldsValue();
    tcPlainText = new wxTextCtrl(this, idCtrl + 100, edPlain);
    tcPlainText->SetEditable(false);
    tcPlainText->SetMinSize( edTexCtrl->GetMinSize() );
    mainSizer->Add(tcPlainText, sizerFlags);

#if wxUSE_TOOLTIPS
    strTip = "Fields alignment: R,R,R\n";
    strTip += "Fields fill chars: ' ',' ',' '\n";
    strTip += "Fields padding chars: '0','0','0'\n";
    strTip += "Coloured\n";
    strTip += "Short date check function";
    edTexCtrl->SetToolTip(strTip);
#endif

    //Sample 8
    idCtrl++;
    stDesText = new wxStaticText(this, wxID_ANY, "Date yyyy/mm/dd");
    mainSizer->Add(stDesText, sizerFlags);
    edMask = "####/9#/9#";
    stMaskText = new wxStaticText(this, wxID_ANY, edMask);
    mainSizer->Add(stMaskText, sizerFlags);

    edTexCtrl = new wxMaskedEditText(this, idCtrl);
    parent->ctrIDs.push_back(idCtrl);
    edTexCtrl->SetMask(edMask);

    edTexCtrl->SetAllFieldsFlags(fieldFlags); //same flags as sample 3
    edTexCtrl->SetMaskedColours(mskColours); //idem
    mainSizer->Add(edTexCtrl, sizerFlags);
    //use the predefined date function
    dparams2.monthField = 1;
    dparams2.dayField = 2;
    dparams2.yearField = 0;
    edTexCtrl->SetControlFunction( &wxMaskedDateShort, &dparams2 );

    edPlain = edTexCtrl->GetAllFieldsValue();
    tcPlainText = new wxTextCtrl(this, idCtrl + 100, edPlain);
    tcPlainText->SetEditable(false);
    tcPlainText->SetMinSize( edTexCtrl->GetMinSize() );
    mainSizer->Add(tcPlainText, sizerFlags);

#if wxUSE_TOOLTIPS
    strTip = "Fields alignment: R,R,R\n";
    strTip += "Fields fill chars: ' ',' ',' '\n";
    strTip += "Fields padding chars: '0','0','0'\n";
    strTip += "Coloured\n";
    strTip += "Short date check function";
    edTexCtrl->SetToolTip(strTip);
#endif

    //sample 9
    idCtrl++;
    stDesText = new wxStaticText(this, wxID_ANY, "Product key");
    mainSizer->Add(stDesText, sizerFlags);
    edMask = "AAA-NNNN-NN|##";
    stMaskText = new wxStaticText(this, wxID_ANY, edMask);
    mainSizer->Add(stMaskText, sizerFlags);

    edTexCtrl = new wxMaskedEditText(this, idCtrl);
    parent->ctrIDs.push_back(idCtrl);
    edTexCtrl->SetMask(edMask);

    edTexCtrl->SetMaskedColours(mskColours); //idem
    mainSizer->Add(edTexCtrl, sizerFlags);
    //use our user function. It does not need parameters, so pass NULL
    edTexCtrl->SetControlFunction( &MyFunction, NULL );

    edPlain = edTexCtrl->GetAllFieldsValue();
    tcPlainText = new wxTextCtrl(this, idCtrl + 100, edPlain);
    tcPlainText->SetEditable(false);
    tcPlainText->SetMinSize( edTexCtrl->GetMinSize() );
    mainSizer->Add(tcPlainText, sizerFlags);

#if wxUSE_TOOLTIPS
    strTip = "Fields alignment: L,L,L,L\n";
    strTip += "Fields fill chars: ' ',' ',' ',' '\n";
    strTip += "Fields padding chars: '','','',''\n";
    strTip += "Coloured\n";
    strTip += "'MyFunction' check function (1 every 5 is valid)";
    edTexCtrl->SetToolTip(strTip);
#endif

    //sample 10
    idCtrl++;
    stDesText = new wxStaticText(this, wxID_ANY, "Critical moments");
    mainSizer->Add(stDesText, sizerFlags);
    edMask = "9#/9#/#### \\at 9#:9#";
    stMaskText = new wxStaticText(this, wxID_ANY, edMask);
    mainSizer->Add(stMaskText, sizerFlags);

    wxString myChoices[] = {" 1/ 1/2000 at 00:00",
                            "12/31/1999 at 23:59",
                            "12/21/2012 at 12:00"};
    edComboCtrl = new wxMaskedEditCombo(this, idCtrl, "",
                                        wxDefaultPosition, wxDefaultSize,
                                        WXSIZEOF(myChoices), myChoices);
    parent->ctrIDs.push_back(idCtrl);
    edComboCtrl->SetMask(edMask);

    edComboCtrl->SetAllFieldsFlags(fieldFlags); //same flags as sample 3
    mskColours.colInvaldForeground.Set(0, 0, 0);
    edComboCtrl->SetMaskedColours(mskColours);
    mainSizer->Add(edComboCtrl, sizerFlags);
    //use the predefined range and date functions
    edComboCtrl->SetFieldFunction(3, &wxMaskedRangeCheck, &fparams23);
    edComboCtrl->SetFieldFunction(4, &wxMaskedRangeCheck, &fparams59);
    dparams1.monthField = 0;
    dparams1.dayField = 1;
    dparams1.yearField = 2;
    edComboCtrl->SetControlFunction( &wxMaskedDateShort, &dparams1 );

    edPlain = edComboCtrl->GetAllFieldsValue();
    tcPlainText = new wxTextCtrl(this, idCtrl + 100, edPlain);
    tcPlainText->SetEditable(false);
    tcPlainText->SetMinSize( edComboCtrl->GetMinSize() );
    mainSizer->Add(tcPlainText, sizerFlags);


    mainSizer->AddSpacer(5);

    //The layout of this panel is managed by this sizer
    SetSizer(mainSizer);
}
