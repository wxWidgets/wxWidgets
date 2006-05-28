/////////////////////////////////////////////////////////////////////////////
// Name:        combo.cpp
// Purpose:     wxComboCtrl sample
// Author:      Jaakko Salli
// Modified by:
// Created:     Apr-30-2006
// RCS-ID:      $Id$
// Copyright:   (c) Jaakko Salli
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

#if !wxUSE_COMBOCTRL
    #error "Please set wxUSE_COMBOCTRL to 1 and rebuild the library."
#endif

#include "wx/image.h"

#include "wx/combo.h"
#include "wx/odcombo.h"

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
    // ctor and dtor
    MyFrame(const wxString& title);
    ~MyFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    // log wxComboCtrl events
    void OnComboBoxUpdate( wxCommandEvent& event );

protected:
    wxTextCtrl*     m_logWin;
    wxLog*          m_logOld;

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
    ComboControl_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    ComboControl_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_TEXT(wxID_ANY,MyFrame::OnComboBoxUpdate)
    EVT_COMBOBOX(wxID_ANY,MyFrame::OnComboBoxUpdate)

    EVT_MENU(ComboControl_Quit,  MyFrame::OnQuit)
    EVT_MENU(ComboControl_About, MyFrame::OnAbout)
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
    // create the main application window
    MyFrame *frame = new MyFrame(_T("wxComboCtrl Sample"));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}


// ----------------------------------------------------------------------------
// wxListView Custom popup interface
// ----------------------------------------------------------------------------

#include <wx/listctrl.h>

class ListViewComboPopup : public wxListView, public wxComboPopup
{
public:

/*
    ListViewComboPopup(wxComboCtrlBase* combo)
        : wxListView(), wxComboPopup(combo)
    {
        m_value = -1;
        m_itemHere = -1; // hot item in list
    }
*/
    virtual void Init()
    {
        m_value = -1;
        m_itemHere = -1; // hot item in list
    }

    virtual bool Create( wxWindow* parent )
    {
        return wxListView::Create(parent,1,
                                  wxPoint(0,0),wxDefaultSize,
                                  wxLC_LIST|wxLC_SINGLE_SEL|
                                  wxLC_SORT_ASCENDING|wxSIMPLE_BORDER);
    }

    virtual wxWindow *GetControl() { return this; }

    virtual void SetStringValue( const wxString& s )
    {
        int n = wxListView::FindItem(-1,s);
        if ( n >= 0 && n < GetItemCount() )
            wxListView::Select(n);
    }

    virtual wxString GetStringValue() const
    {
        if ( m_value >= 0 )
            return wxListView::GetItemText(m_value);
        return wxEmptyString;
    }

    //
    // Popup event handlers
    //

    // Mouse hot-tracking
    void OnMouseMove(wxMouseEvent& event)
    {
        // Move selection to cursor if it is inside the popup

        int resFlags;
        int itemHere = HitTest(event.GetPosition(),resFlags);
        if ( itemHere >= 0 )
        {
            wxListView::Select(itemHere,true);
            m_itemHere = itemHere;
        }
        event.Skip();
    }

    // On mouse left, set the value and close the popup
    void OnMouseClick(wxMouseEvent& WXUNUSED(event))
    {
        m_value = m_itemHere;
        // TODO: Send event
        Dismiss();
    }

    //
    // Utilies for item manipulation
    //

    void AddSelection( const wxString& selstr )
    {
        wxListView::InsertItem(GetItemCount(),selstr);
    }

protected:

    int             m_value; // current item index
    int             m_itemHere; // hot item in popup

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(ListViewComboPopup, wxListView)
    EVT_MOTION(ListViewComboPopup::OnMouseMove)
    // NOTE: Left down event is used instead of left up right now
    //       since MSW wxListCtrl doesn't seem to emit left ups
    //       consistently.
    EVT_LEFT_DOWN(ListViewComboPopup::OnMouseClick)
END_EVENT_TABLE()


// ----------------------------------------------------------------------------
// wxTreeCtrl Custom popup interface
// ----------------------------------------------------------------------------

#include <wx/treectrl.h>

class TreeCtrlComboPopup : public wxTreeCtrl, public wxComboPopup
{
public:

/*
    TreeCtrlComboPopup(wxComboCtrlBase* combo)
        : wxTreeCtrl(), wxComboPopup(combo)
    {
    }
*/

    virtual void Init()
    {
    }

    virtual bool Create( wxWindow* parent )
    {
        return wxTreeCtrl::Create(parent,1,
                                  wxPoint(0,0),wxDefaultSize,
                                  wxTR_HIDE_ROOT|wxTR_HAS_BUTTONS|
                                  wxTR_SINGLE|wxTR_LINES_AT_ROOT|
                                  wxSIMPLE_BORDER);
    }

    virtual void OnShow()
    {
        // make sure selected item is visible
        if ( m_value.IsOk() )
            EnsureVisible(m_value);
    }

    virtual wxSize GetAdjustedSize( int minWidth,
                                    int WXUNUSED(prefHeight),
                                    int maxHeight )
    {
        return wxSize(wxMax(300,minWidth),wxMin(250,maxHeight));
    }

    virtual wxWindow *GetControl() { return this; }

    // Needed by SetStringValue
    wxTreeItemId FindItemByText( wxTreeItemId parent, const wxString& text )
    {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = GetFirstChild(parent,cookie);
        while ( child.IsOk() )
        {
            if ( GetItemText(child) == text )
            {
                return child;
            }
            if ( ItemHasChildren(child) )
            {
                wxTreeItemId found = FindItemByText(child,text);
                if ( found.IsOk() )
                    return found;
            }
            child = GetNextChild(parent,cookie);
        }
        return wxTreeItemId();
    }

    virtual void SetStringValue( const wxString& s )
    {
        wxTreeItemId root = GetRootItem();
        if ( !root.IsOk() )
            return;

        wxTreeItemId found = FindItemByText(root,s);
        if ( found.IsOk() )
        {
            m_value = m_itemHere = found;
            wxTreeCtrl::SelectItem(found);
        }
    }

    virtual wxString GetStringValue() const
    {
        if ( m_value.IsOk() )
            return wxTreeCtrl::GetItemText(m_value);
        return wxEmptyString;
    }

    //
    // Popup event handlers
    //

    // Mouse hot-tracking
    void OnMouseMove(wxMouseEvent& event)
    {
        int resFlags;
        wxTreeItemId itemHere = HitTest(event.GetPosition(),resFlags);
        if ( itemHere.IsOk() && (resFlags & wxTREE_HITTEST_ONITEMLABEL) )
        {
            wxTreeCtrl::SelectItem(itemHere,true);
            m_itemHere = itemHere;
        }
        event.Skip();
    }

    // On mouse left, set the value and close the popup
    void OnMouseClick(wxMouseEvent& event)
    {
        int resFlags;
        wxTreeItemId itemHere = HitTest(event.GetPosition(),resFlags);
        if ( itemHere.IsOk() && (resFlags & wxTREE_HITTEST_ONITEMLABEL) )
        {
            m_itemHere = itemHere;
            m_value = itemHere;
            Dismiss();
            // TODO: Send event
        }
        event.Skip();
    }

protected:

    wxTreeItemId        m_value; // current item index
    wxTreeItemId        m_itemHere; // hot item in popup

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(TreeCtrlComboPopup, wxTreeCtrl)
    EVT_MOTION(TreeCtrlComboPopup::OnMouseMove)
    // NOTE: Left down event is used instead of left up right now
    //       since MSW wxTreeCtrl doesn't seem to emit left ups
    //       consistently.
    EVT_LEFT_DOWN(TreeCtrlComboPopup::OnMouseClick)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxOwnerDrawnComboBox with custom paint list items
// ----------------------------------------------------------------------------
/*
class wxPenStyleComboBox : public wxOwnerDrawnComboBox
{
public:
    virtual bool OnDrawListItem( wxDC& dc, const wxRect& rect, int item, int flags )
    {
        wxRect r(rect);
        r.Deflate(3);
        r.height -= 2;

        int pen_style = wxSOLID;
        if ( item == 1 )
            pen_style = wxTRANSPARENT;
        else if ( item == 2 )
            pen_style = wxDOT;
        else if ( item == 3 )
            pen_style = wxLONG_DASH;
        else if ( item == 4 )
            pen_style = wxSHORT_DASH;
        else if ( item == 5 )
            pen_style = wxDOT_DASH;
        else if ( item == 6 )
            pen_style = wxBDIAGONAL_HATCH;
        else if ( item == 7 )
            pen_style = wxCROSSDIAG_HATCH;
        else if ( item == 8 )
            pen_style = wxFDIAGONAL_HATCH;
        else if ( item == 9 )
            pen_style = wxCROSS_HATCH;
        else if ( item == 10 )
            pen_style = wxHORIZONTAL_HATCH;
        else if ( item == 11 )
            pen_style = wxVERTICAL_HATCH;

        wxPen pen( dc.GetTextForeground(), 3, pen_style );

        // Get text colour as pen colour
        dc.SetPen ( pen );

        if ( !(flags & wxCP_PAINTING_CONTROL) )
        {
            dc.DrawText(GetString( item ),
                        r.x + 3,
                        (r.y + 0) + ( (r.height/2) - dc.GetCharHeight() )/2
                       );

            dc.DrawLine( r.x+5, r.y+((r.height/4)*3), r.x+r.width - 5, r.y+((r.height/4)*3) );
        }
        else
        {
            dc.DrawLine( r.x+5, r.y+r.height/2, r.x+r.width - 5, r.y+r.height/2 );
        }

        return true;
    }

    virtual wxCoord OnMeasureListItem( int WXUNUSED(item) )
    {
        return 24;
    }

    virtual wxCoord OnMeasureListItemWidth( int WXUNUSED(item) )
    {
        return -1; // default - will be measured from text width
    }

};
*/

class wxPenStylePopup : public wxVListBoxComboPopup
{
public:
    virtual void OnDrawItem( wxDC& dc, const wxRect& rect, int item, int flags ) const
    {
        if ( item == wxNOT_FOUND )
            return;

        wxRect r(rect);
        r.Deflate(3);
        r.height -= 2;

        int pen_style = wxSOLID;
        if ( item == 1 )
            pen_style = wxTRANSPARENT;
        else if ( item == 2 )
            pen_style = wxDOT;
        else if ( item == 3 )
            pen_style = wxLONG_DASH;
        else if ( item == 4 )
            pen_style = wxSHORT_DASH;
        else if ( item == 5 )
            pen_style = wxDOT_DASH;
        else if ( item == 6 )
            pen_style = wxBDIAGONAL_HATCH;
        else if ( item == 7 )
            pen_style = wxCROSSDIAG_HATCH;
        else if ( item == 8 )
            pen_style = wxFDIAGONAL_HATCH;
        else if ( item == 9 )
            pen_style = wxCROSS_HATCH;
        else if ( item == 10 )
            pen_style = wxHORIZONTAL_HATCH;
        else if ( item == 11 )
            pen_style = wxVERTICAL_HATCH;

        wxPen pen( dc.GetTextForeground(), 3, pen_style );

        // Get text colour as pen colour
        dc.SetPen ( pen );

        if ( !(flags & wxCP_PAINTING_CONTROL) )
        {
            dc.DrawText(GetString( item ),
                        r.x + 3,
                        (r.y + 0) + ( (r.height/2) - dc.GetCharHeight() )/2
                       );

            dc.DrawLine( r.x+5, r.y+((r.height/4)*3), r.x+r.width - 5, r.y+((r.height/4)*3) );
        }
        else
        {
            dc.DrawLine( r.x+5, r.y+r.height/2, r.x+r.width - 5, r.y+r.height/2 );
        }
    }

    virtual wxCoord OnMeasureItem( size_t WXUNUSED(item) ) const
    {
        return 24;
    }

    virtual wxCoord OnMeasureItemWidth( size_t WXUNUSED(item) ) const
    {
        return -1; // default - will be measured from text width
    }

};

// ----------------------------------------------------------------------------
// wxComboCtrl with entirely custom button action (opens file dialog)
// ----------------------------------------------------------------------------

class wxFileSelectorCombo : public wxComboCtrl
{
public:
    wxFileSelectorCombo() : wxComboCtrl() { Init(); }

    wxFileSelectorCombo(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxString& value = wxEmptyString,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxComboBoxNameStr)
        : wxComboCtrl()
    {
        Init();
        Create(parent,id,value,
               pos,size,
               // Style flag wxCC_STD_BUTTON makes the button
               // behave more like a standard push button.
               style | wxCC_STD_BUTTON,
               validator,name);

        //
        // Prepare custom button bitmap (just '...' text)
        wxMemoryDC dc;
        wxBitmap bmp(12,16);
        dc.SelectObject(bmp);

        // Draw transparent background
        wxColour magic(255,0,255);
        wxBrush magicBrush(magic);
        dc.SetBrush( magicBrush );
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.DrawRectangle(0,0,bmp.GetWidth(),bmp.GetHeight());

        // Draw text
        wxString str = wxT("...");
        int w,h;
        dc.GetTextExtent(str, &w, &h, 0, 0);
        dc.DrawText(str, (bmp.GetWidth()-w)/2, (bmp.GetHeight()-h)/2);

        dc.SelectObject( wxNullBitmap );

        // Finalize transparency with a mask
        wxMask *mask = new wxMask( bmp, magic );
        bmp.SetMask( mask );

        SetButtonBitmaps(bmp,true);
    }

    virtual void OnButtonClick()
    {
        // Show standard wxFileDialog on button click

        wxFileDialog dlg(this,
                         wxT("Choose File"),
                         wxEmptyString,
                         GetValue(),
                         wxT("All files (*.*)|*.*"),
                         wxFD_OPEN);

        if ( dlg.ShowModal() == wxID_OK )
        {
            SetValue(dlg.GetPath());
        }
    }

private:
    void Init()
    {
        // Initialize member variables here
    }
};

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    wxBoxSizer* topSizer;
    wxBoxSizer* topRowSizer;
    wxBoxSizer* colSizer;
    wxBoxSizer* rowSizer;
    wxStaticBoxSizer* groupSizer;

    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(ComboControl_About, _T("&About...\tF1"), _T("Show about dialog"));

    fileMenu->Append(ComboControl_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

    wxPanel* panel = new wxPanel(this);

    // Prepare log window right away since it shows EVT_TEXTs
    m_logWin = new wxTextCtrl( panel, 105, wxEmptyString, wxDefaultPosition,
                               wxSize(-1,125), wxTE_MULTILINE|wxFULL_REPAINT_ON_RESIZE );
    m_logWin->SetEditable(false);
    wxLogTextCtrl* logger = new wxLogTextCtrl( m_logWin );
    m_logOld = logger->SetActiveTarget( logger );
    logger->SetTimestamp( NULL );


    topSizer = new wxBoxSizer( wxVERTICAL );

    topRowSizer = new wxBoxSizer( wxHORIZONTAL );

    colSizer = new wxBoxSizer( wxVERTICAL );


    // Make sure GetFeatures is implemented
    int features = wxComboCtrl::GetFeatures();
    wxLogDebug(wxT("wxComboCtrl features: 0x%X (all features: 0x%X)"),
               features,wxComboCtrlFeatures::All);


    wxComboBox* cb;
    wxComboCtrl* cc;
    wxGenericComboControl* gcc;
    wxOwnerDrawnComboBox* odc;

    // Create common strings array
    wxArrayString arrItems;
    arrItems.Add( wxT("Solid") );
    arrItems.Add( wxT("Transparent") );
    arrItems.Add( wxT("Dot") );
    arrItems.Add( wxT("Long Dash") );
    arrItems.Add( wxT("Short Dash") );
    arrItems.Add( wxT("Dot Dash") );
    arrItems.Add( wxT("Backward Diagonal Hatch") );
    arrItems.Add( wxT("Cross-diagonal Hatch") );
    arrItems.Add( wxT("Forward Diagonal Hatch") );
    arrItems.Add( wxT("Cross Hatch") );
    arrItems.Add( wxT("Horizontal Hatch") );
    arrItems.Add( wxT("Vertical Hatch") );

    int border = 4;

    //
    // Show some wxOwnerDrawComboBoxes for comparison
    //
    rowSizer = new wxBoxSizer(wxHORIZONTAL);

    groupSizer = new wxStaticBoxSizer(new wxStaticBox(panel,wxID_ANY,wxT(" wxOwnerDrawnComboBox ")),
                                      wxVERTICAL);

    groupSizer->Add( new wxStaticText(panel,wxID_ANY,wxT("Writable, sorted:")), 0,
                     wxALIGN_CENTER_VERTICAL|wxRIGHT|wxEXPAND, border );

    odc = new wxOwnerDrawnComboBox(panel,wxID_ANY,wxEmptyString,
                                   wxDefaultPosition, wxDefaultSize,
                                   arrItems,
                                   wxCB_SORT // wxNO_BORDER|wxCB_READONLY
                                  );

    odc->Append(wxT("H - Appended Item")); // test sorting in append

    odc->SetValue(wxT("Dot Dash"));

    groupSizer->Add( odc, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, border );

    //
    // Readonly ODComboBox
    groupSizer->Add( new wxStaticText(panel,wxID_ANY,wxT("Read-only:")), 0,
                     wxALIGN_CENTER_VERTICAL|wxRIGHT, border );

    odc = new wxOwnerDrawnComboBox(panel,wxID_ANY,wxEmptyString,
                                   wxDefaultPosition, wxDefaultSize,
                                   arrItems,
                                   wxCB_SORT|wxCB_READONLY // wxNO_BORDER|wxCB_READONLY
                                  );

    odc->SetValue(wxT("Dot Dash"));
    odc->SetText(wxT("Dot Dash (Testing SetText)"));

    groupSizer->Add( odc, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, border );

    //
    // Disabled ODComboBox
    groupSizer->Add( new wxStaticText(panel,wxID_ANY,wxT("Disabled:")), 0,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, border );

    odc = new wxOwnerDrawnComboBox(panel,wxID_ANY,wxEmptyString,
                                     wxDefaultPosition, wxDefaultSize,
                                     arrItems,
                                     wxCB_SORT|wxCB_READONLY // wxNO_BORDER|wxCB_READONLY
                                     );

    odc->SetValue(wxT("Dot Dash"));
    odc->Enable(false);

    groupSizer->Add( odc, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, border );

    rowSizer->Add( groupSizer, 1, wxEXPAND|wxALL, border );


    groupSizer = new wxStaticBoxSizer(new wxStaticBox(panel,wxID_ANY,wxT(" wxComboBox ")),
                                      wxVERTICAL);

    //
    // wxComboBox
    //
    groupSizer->Add( new wxStaticText(panel,wxID_ANY,wxT("Writable, sorted:")), 0,
                     wxALIGN_CENTER_VERTICAL|wxRIGHT|wxEXPAND, border );

    cb = new wxComboBox(panel,wxID_ANY,wxEmptyString,
                        wxDefaultPosition, wxDefaultSize,
                        arrItems,
                        wxCB_SORT // wxNO_BORDER|wxCB_READONLY
                       );

    cb->Append(wxT("H - Appended Item")); // test sorting in append

    cb->SetValue(wxT("Dot Dash"));

    groupSizer->Add( cb, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, border );

    //
    // Readonly wxComboBox
    groupSizer->Add( new wxStaticText(panel,wxID_ANY,wxT("Read-only:")), 0,
                     wxALIGN_CENTER_VERTICAL|wxRIGHT, border );

    cb = new wxComboBox(panel,wxID_ANY,wxEmptyString,
                        wxDefaultPosition, wxDefaultSize,
                        arrItems,
                        wxCB_SORT|wxCB_READONLY // wxNO_BORDER|wxCB_READONLY
                       );

    cb->SetValue(wxT("Dot Dash"));

    groupSizer->Add( cb, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, border );

    //
    // Disabled wxComboBox
    groupSizer->Add( new wxStaticText(panel,wxID_ANY,wxT("Disabled:")), 0,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, border );

    cb = new wxComboBox(panel,wxID_ANY,wxEmptyString,
                        wxDefaultPosition, wxDefaultSize,
                        arrItems,
                        wxCB_SORT|wxCB_READONLY // wxNO_BORDER|wxCB_READONLY
                       );

    cb->SetValue(wxT("Dot Dash"));
    cb->Enable(false);

    groupSizer->Add( cb, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, border );

    rowSizer->Add( groupSizer, 1, wxEXPAND|wxALL, border );


    colSizer->Add( rowSizer, 1, wxEXPAND|wxALL, border );


    //
    // Pen selector ODComboBox with application painted items
    //
    rowSizer = new wxBoxSizer ( wxHORIZONTAL );
    rowSizer->Add( new wxStaticText(panel,wxID_ANY,
                   wxT("OwnerDrawnComboBox with Custom Paint Items and Button Placing:")), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    rowSizer = new wxBoxSizer ( wxHORIZONTAL );

    // When defining derivative class for callbacks, we need
    // to use two-stage creation (or redefine the common wx
    // constructor).
    odc = new wxOwnerDrawnComboBox(panel,wxID_ANY,wxEmptyString,
                                   wxDefaultPosition, wxDefaultSize,
                                   arrItems,
                                   wxCB_READONLY //wxNO_BORDER | wxCB_READONLY
                                  );

    odc->SetPopupControl( new wxPenStylePopup() );

    //m_odc->SetCustomPaintWidth( 60 );
    odc->SetSelection(0);
    odc->SetButtonPosition(-2, // width adjustment
                           -6, // height adjustment
                           wxLEFT, // side
                           2 // horizontal spacing
                          );

    rowSizer->Add( odc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );
    rowSizer->AddStretchSpacer(1);
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );


    //
    // List View wxComboCtrl
    //

    rowSizer = new wxBoxSizer ( wxHORIZONTAL );
    rowSizer->Add( new wxStaticText(panel,wxID_ANY,wxT("List View wxComboCtrl:")), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );
    rowSizer->Add( new wxStaticText(panel,wxID_ANY,wxT("Tree Ctrl wxGenericComboControl:")), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    rowSizer = new wxBoxSizer ( wxHORIZONTAL );
    cc = new wxComboCtrl(panel,2,wxEmptyString,
                            wxDefaultPosition, wxDefaultSize);

    cc->SetPopupMinWidth(300);

    ListViewComboPopup* iface = new ListViewComboPopup();
    cc->SetPopupControl(iface);

    iface->AddSelection( wxT("Cabbage") );
    iface->AddSelection( wxT("Potato") );
    iface->AddSelection( wxT("Onion") );
    iface->AddSelection( wxT("Carrot") );
    iface->AddSelection( wxT("Cauliflower") );
    iface->AddSelection( wxT("Bean") );
    iface->AddSelection( wxT("Raddish") );
    iface->AddSelection( wxT("Banana") );
    iface->AddSelection( wxT("Apple") );
    iface->AddSelection( wxT("Orange") );
    iface->AddSelection( wxT("Kiwi") );
    iface->AddSelection( wxT("Strawberry") );
    iface->AddSelection( wxT("Cucumber") );
    iface->AddSelection( wxT("Blackberry") );
    iface->AddSelection( wxT("Melon") );
    iface->AddSelection( wxT("Cherry") );
    iface->AddSelection( wxT("Pea") );
    iface->AddSelection( wxT("Pear") );

    rowSizer->Add( cc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


    //
    // Tree Ctrl wxComboCtrl
    //

    // Note that we test that wxGenericComboControl works
    gcc = new wxGenericComboControl(panel,wxID_ANY,wxEmptyString,
                                    wxDefaultPosition, wxDefaultSize);

    // Set popup interface right away, otherwise some of the calls
    // below may fail
    TreeCtrlComboPopup* tcPopup = new TreeCtrlComboPopup();
    gcc->SetPopupControl(tcPopup);

    // Add items using wxTreeCtrl methods directly
    wxTreeItemId rootId = tcPopup->AddRoot(wxT("<hidden_root>"));

    wxTreeItemId groupId;

    groupId = tcPopup->AppendItem(rootId,wxT("Controls"));
    tcPopup->AppendItem(groupId,wxT("wxButton"));
    tcPopup->AppendItem(groupId,wxT("wxCheckBox"));
    tcPopup->AppendItem(groupId,wxT("wxListCtrl"));
    tcPopup->AppendItem(groupId,wxT("wxStaticBox"));
    tcPopup->AppendItem(groupId,wxT("wxStaticText"));
    tcPopup->AppendItem(groupId,wxT("wxTextCtrl"));
    tcPopup->AppendItem(groupId,wxT("wxTreeCtrl"));
    groupId = tcPopup->AppendItem(rootId,wxT("Dialogs"));
    tcPopup->AppendItem(groupId,wxT("wxDirDialog"));
    tcPopup->AppendItem(groupId,wxT("wxFileDialog"));
    tcPopup->AppendItem(groupId,wxT("wxWizard"));

    gcc->SetValue(wxT("wxStaticBox"));

    // Move button to left - it makes more sense for a tree ctrl
    gcc->SetButtonPosition(0, // width adjustment
                           0, // height adjustment
                           wxLEFT, // side
                           0 // horizontal spacing
                          );

    rowSizer->Add( gcc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

#if wxUSE_IMAGE
    wxInitAllImageHandlers();

    //
    // Custom Dropbutton Bitmaps
    // (second one uses blank button background)
    //
    rowSizer = new wxBoxSizer ( wxHORIZONTAL );
    rowSizer->Add( new wxStaticText(panel,wxID_ANY,
                   wxT("OwnerDrawnComboBox with simple dropbutton graphics:")), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );

    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    rowSizer = new wxBoxSizer ( wxHORIZONTAL );

    odc = new wxOwnerDrawnComboBox(panel,wxID_ANY,wxEmptyString,
                                   wxDefaultPosition, wxDefaultSize,
                                   arrItems,
                                   (long)0 // wxCB_SORT // wxNO_BORDER | wxCB_READONLY
                                  );

    wxOwnerDrawnComboBox* odc2;
    odc2 = new wxOwnerDrawnComboBox(panel,wxID_ANY,wxEmptyString,
                                    wxDefaultPosition, wxDefaultSize,
                                    arrItems,
                                    (long)0 // wxCB_SORT // wxNO_BORDER | wxCB_READONLY
                                   );

    // Load images from disk
    wxImage imgNormal(wxT("dropbutn.png"));
    wxImage imgPressed(wxT("dropbutp.png"));
    wxImage imgHover(wxT("dropbuth.png"));

    if ( imgNormal.Ok() && imgPressed.Ok() && imgHover.Ok() )
    {
        wxBitmap bmpNormal(imgNormal);
        wxBitmap bmpPressed(imgPressed);
        wxBitmap bmpHover(imgHover);
        odc->SetButtonBitmaps(bmpNormal,false,bmpPressed,bmpHover);
        odc2->SetButtonBitmaps(bmpNormal,true,bmpPressed,bmpHover);
    }
    else
        wxLogError(wxT("Dropbutton images not found"));

    //odc2->SetButtonPosition(0, // width adjustment
    //                        0, // height adjustment
    //                        wxLEFT, // side
    //                        0 // horizontal spacing
    //                       );

    rowSizer->Add( odc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );
    rowSizer->Add( odc2, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );
#endif


    //
    // wxComboCtrl with totally custom button action (open file dialog)
    //
    rowSizer = new wxBoxSizer ( wxHORIZONTAL );
    rowSizer->Add( new wxStaticText(panel,wxID_ANY,
                        wxT("wxComboCtrl with custom button action:")), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );


    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    rowSizer = new wxBoxSizer ( wxHORIZONTAL );
    wxFileSelectorCombo* fsc;

    fsc = new wxFileSelectorCombo(panel,wxID_ANY,wxEmptyString,
                                  wxDefaultPosition, wxDefaultSize,
                                  (long)0
                                 );

    rowSizer->Add( fsc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );


    topRowSizer->Add( colSizer, 1, wxALL, 2 );

    topRowSizer->Add( m_logWin, 1, wxEXPAND|wxALL, 5 );
    topSizer->Add( topRowSizer, 1, wxEXPAND );

    panel->SetSizer( topSizer );
    topSizer->SetSizeHints( panel );

    SetSize(740,480);
    Centre();
}

MyFrame::~MyFrame()
{
    delete wxLog::SetActiveTarget(m_logOld);
}

// event handlers

void MyFrame::OnComboBoxUpdate( wxCommandEvent& event )
{
    // Don't show messages for the log output window (it'll crash)
    if ( event.GetId() == 105 )
        return;

    if ( event.GetEventType() == wxEVT_COMMAND_COMBOBOX_SELECTED )
        wxLogDebug(wxT("EVT_COMBOBOX(id=%i,selection=%i)"),event.GetId(),event.GetSelection());
    else if ( event.GetEventType() == wxEVT_COMMAND_TEXT_UPDATED )
        wxLogDebug(wxT("EVT_TEXT(id=%i,string=\"%s\")"),event.GetId(),event.GetString().c_str());
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
                    _T("This is the wxWidgets wxComboCtrl sample\n")
                    _T("running under %s."),
                    wxVERSION_STRING,
                    wxGetOsDescription().c_str()
                 ),
                 _T("About wxComboCtrl sample"),
                 wxOK | wxICON_INFORMATION,
                 this);
}
