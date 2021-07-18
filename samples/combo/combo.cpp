/////////////////////////////////////////////////////////////////////////////
// Name:        combo.cpp
// Purpose:     wxComboCtrl sample
// Author:      Jaakko Salli
// Modified by:
// Created:     Apr-30-2006
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

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
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
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit() wxOVERRIDE;
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

    void OnShowComparison( wxCommandEvent& event );

    // log wxComboCtrl events
    void OnComboBoxUpdate( wxCommandEvent& event );

    void OnIdle( wxIdleEvent& event );


protected:
    wxTextCtrl*     m_logWin;
    wxLog*          m_logOld;

    // Common list of items for all dialogs.
    wxArrayString   m_arrItems;

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
    ComboCtrl_Compare = wxID_HIGHEST,

    // menu items
    ComboCtrl_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    ComboCtrl_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_TEXT(wxID_ANY,MyFrame::OnComboBoxUpdate)
    EVT_TEXT_ENTER(wxID_ANY,MyFrame::OnComboBoxUpdate)
    EVT_COMBOBOX(wxID_ANY,MyFrame::OnComboBoxUpdate)

    EVT_MENU(ComboCtrl_Compare,  MyFrame::OnShowComparison)
    EVT_MENU(ComboCtrl_Quit,     MyFrame::OnQuit)
    EVT_MENU(ComboCtrl_About,    MyFrame::OnAbout)

    EVT_IDLE(MyFrame::OnIdle)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame("wxComboCtrl and wxOwnerDrawnComboBox Sample");

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}


// ----------------------------------------------------------------------------
// wxOwnerDrawnComboBox with custom paint list items
// ----------------------------------------------------------------------------

class wxPenStyleComboBox : public wxOwnerDrawnComboBox
{
public:
    virtual void OnDrawItem( wxDC& dc,
                             const wxRect& rect,
                             int item,
                             int flags ) const wxOVERRIDE
    {
        if ( item == wxNOT_FOUND )
            return;

        wxRect r(rect);
        r.Deflate(3);
        r.height -= 2;

        wxPenStyle penStyle = wxPENSTYLE_SOLID;
        if ( item == 1 )
            penStyle = wxPENSTYLE_TRANSPARENT;
        else if ( item == 2 )
            penStyle = wxPENSTYLE_DOT;
        else if ( item == 3 )
            penStyle = wxPENSTYLE_LONG_DASH;
        else if ( item == 4 )
            penStyle = wxPENSTYLE_SHORT_DASH;
        else if ( item == 5 )
            penStyle = wxPENSTYLE_DOT_DASH;
        else if ( item == 6 )
            penStyle = wxPENSTYLE_BDIAGONAL_HATCH;
        else if ( item == 7 )
            penStyle = wxPENSTYLE_CROSSDIAG_HATCH;
        else if ( item == 8 )
            penStyle = wxPENSTYLE_FDIAGONAL_HATCH;
        else if ( item == 9 )
            penStyle = wxPENSTYLE_CROSS_HATCH;
        else if ( item == 10 )
            penStyle = wxPENSTYLE_HORIZONTAL_HATCH;
        else if ( item == 11 )
            penStyle = wxPENSTYLE_VERTICAL_HATCH;

        wxPen pen( dc.GetTextForeground(), 3, penStyle );

        // Get text colour as pen colour
        dc.SetPen( pen );

        if ( !(flags & wxODCB_PAINTING_CONTROL) )
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

    virtual void OnDrawBackground( wxDC& dc, const wxRect& rect,
                                   int item, int flags ) const wxOVERRIDE
    {

        // If item is selected or even, or we are painting the
        // combo control itself, use the default rendering.
        if ( (flags & (wxODCB_PAINTING_CONTROL|wxODCB_PAINTING_SELECTED)) ||
             (item & 1) == 0 )
        {
            wxOwnerDrawnComboBox::OnDrawBackground(dc,rect,item,flags);
            return;
        }

        // Otherwise, draw every other background with different colour.
        wxColour bgCol(240,240,250);
        dc.SetBrush(wxBrush(bgCol));
        dc.SetPen(wxPen(bgCol));
        dc.DrawRectangle(rect);
    }

    virtual wxCoord OnMeasureItem( size_t item ) const wxOVERRIDE
    {
        // Simply demonstrate the ability to have variable-height items
        return FromDIP( item & 1 ? 36 : 24 );
    }

    virtual wxCoord OnMeasureItemWidth( size_t WXUNUSED(item) ) const wxOVERRIDE
    {
        return -1; // default - will be measured from text width
    }

};


// ----------------------------------------------------------------------------
// wxListView Custom popup interface
// ----------------------------------------------------------------------------

#include "wx/listctrl.h"

class ListViewComboPopup : public wxListView, public wxComboPopup
{
public:

    virtual void Init() wxOVERRIDE
    {
        m_value = -1;
        m_itemHere = -1; // hot item in list
    }

    virtual bool Create( wxWindow* parent ) wxOVERRIDE
    {
        return wxListView::Create(parent,1,
                                  wxPoint(0,0),wxDefaultSize,
                                  wxLC_LIST|wxLC_SINGLE_SEL|
                                  wxLC_SORT_ASCENDING|wxSIMPLE_BORDER);
    }

    virtual wxWindow *GetControl() wxOVERRIDE { return this; }

    virtual void SetStringValue( const wxString& s ) wxOVERRIDE
    {
        int n = wxListView::FindItem(-1,s);
        if ( n >= 0 && n < GetItemCount() )
            wxListView::Select(n);
    }

    virtual wxString GetStringValue() const wxOVERRIDE
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
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(ListViewComboPopup, wxListView)
    EVT_MOTION(ListViewComboPopup::OnMouseMove)
    // NOTE: Left down event is used instead of left up right now
    //       since MSW wxListCtrl doesn't seem to emit left ups
    //       consistently.
    EVT_LEFT_DOWN(ListViewComboPopup::OnMouseClick)
wxEND_EVENT_TABLE()


// ----------------------------------------------------------------------------
// wxTreeCtrl Custom popup interface
// ----------------------------------------------------------------------------

#include "wx/treectrl.h"

class TreeCtrlComboPopup : public wxTreeCtrl, public wxComboPopup
{
public:

    virtual void Init() wxOVERRIDE
    {
    }
    virtual ~TreeCtrlComboPopup()
    {
        if (!m_isBeingDeleted)
        {
            wxMessageBox("error wxTreeCtrl::Destroy() was not called");
        }
        SendDestroyEvent();
    }

    virtual bool Create( wxWindow* parent ) wxOVERRIDE
    {
        return wxTreeCtrl::Create(parent,1,
                                  wxPoint(0,0),wxDefaultSize,
                                  wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxSIMPLE_BORDER );
    }

    virtual void OnShow()
    {
        // make sure selected item is visible
        if ( m_value.IsOk() )
            EnsureVisible(m_value);
    }

    virtual wxSize GetAdjustedSize( int minWidth,
                                    int WXUNUSED(prefHeight),
                                    int maxHeight ) wxOVERRIDE
    {
        return wxSize(wxMax(300,minWidth),wxMin(250,maxHeight));
    }

    virtual wxWindow *GetControl() wxOVERRIDE { return this; }

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

    virtual void SetStringValue( const wxString& s ) wxOVERRIDE
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

    virtual wxString GetStringValue() const wxOVERRIDE
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
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(TreeCtrlComboPopup, wxTreeCtrl)
    EVT_MOTION(TreeCtrlComboPopup::OnMouseMove)
    // NOTE: Left down event is used instead of left up right now
    //       since MSW wxTreeCtrl doesn't seem to emit left ups
    //       consistently.
    EVT_LEFT_DOWN(TreeCtrlComboPopup::OnMouseClick)
wxEND_EVENT_TABLE()

// ----------------------------------------------------------------------------
// wxComboCtrl with custom popup animation, using wxWindow::ShowWithEffect().
// ----------------------------------------------------------------------------

class wxComboCtrlWithCustomPopupAnim : public wxComboCtrl
{
protected:
    virtual bool AnimateShow( const wxRect& rect, int WXUNUSED(flags) ) wxOVERRIDE
    {
        wxWindow* win = GetPopupWindow();
        win->SetSize(rect);
        win->Raise();  // This is needed
        win->ShowWithEffect(wxSHOW_EFFECT_BLEND);
        return true;
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
        wxString str = "...";
        int w,h;
        dc.GetTextExtent(str, &w, &h, 0, 0);
        dc.DrawText(str, (bmp.GetWidth()-w)/2, (bmp.GetHeight()-h)/2);

        dc.SelectObject( wxNullBitmap );

        // Finalize transparency with a mask
        wxMask *mask = new wxMask( bmp, magic );
        bmp.SetMask( mask );

        SetButtonBitmaps(bmp,true);
    }

    virtual void OnButtonClick() wxOVERRIDE
    {
        // Show standard wxFileDialog on button click

        wxFileDialog dlg(this,
                         "Choose File",
                         wxEmptyString,
                         GetValue(),
                         "All files (*.*)|*.*",
                         wxFD_OPEN);

        if ( dlg.ShowModal() == wxID_OK )
        {
            SetValue(dlg.GetPath());
        }
    }

    // Implement empty DoSetPopupControl to prevent assertion failure.
    virtual void DoSetPopupControl(wxComboPopup* WXUNUSED(popup)) wxOVERRIDE
    {
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

    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(ComboCtrl_About, "&About\tF1", "Show about dialog");

    fileMenu->Append(ComboCtrl_Compare, "&Compare against wxComboBox...",
        "Show some wxOwnerDrawnComboBoxes side-by-side with native wxComboBoxes.");
    fileMenu->AppendSeparator();
    fileMenu->Append(ComboCtrl_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

    wxPanel* panel = new wxPanel(this);

    // Prepare log window right away since it shows EVT_TEXTs
    m_logWin = new wxTextCtrl(panel, 105, wxEmptyString,
                              wxDefaultPosition,
                              wxSize(-1, 125),
                              wxTE_MULTILINE);
    wxLogTextCtrl* logger = new wxLogTextCtrl(m_logWin);
    m_logOld = logger->SetActiveTarget(logger);
    logger->DisableTimestamp();


    topSizer = new wxBoxSizer( wxVERTICAL );

    topRowSizer = new wxBoxSizer( wxHORIZONTAL );

    colSizer = new wxBoxSizer( wxVERTICAL );


    wxComboCtrl* cc;
    wxGenericComboCtrl* gcc;
    wxOwnerDrawnComboBox* odc;

    // Create common strings array
    m_arrItems.Add( "Solid" );
    m_arrItems.Add( "Transparent" );
    m_arrItems.Add( "Dot" );
    m_arrItems.Add( "Long Dash" );
    m_arrItems.Add( "Short Dash" );
    m_arrItems.Add( "Dot Dash" );
    m_arrItems.Add( "Backward Diagonal Hatch" );
    m_arrItems.Add( "Cross-diagonal Hatch" );
    m_arrItems.Add( "Forward Diagonal Hatch" );
    m_arrItems.Add( "Cross Hatch" );
    m_arrItems.Add( "Horizontal Hatch" );
    m_arrItems.Add( "Vertical Hatch" );


    //
    // Create pen selector ODComboBox with owner-drawn items
    //
    rowSizer = new wxBoxSizer( wxHORIZONTAL );
    rowSizer->Add( new wxStaticText(panel,wxID_ANY,
                   "OwnerDrawnComboBox with owner-drawn items:"), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    rowSizer = new wxBoxSizer( wxHORIZONTAL );


    // When defining derivative class for callbacks, we need
    // to use two-stage creation (or redefine the common wx
    // constructor).
    odc = new wxPenStyleComboBox();
    odc->Create(panel,wxID_ANY,wxEmptyString,
                wxDefaultPosition, wxDefaultSize,
                m_arrItems,
                wxCB_READONLY //wxNO_BORDER | wxCB_READONLY
               );


    odc->SetSelection(0);

    rowSizer->Add( odc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );
    rowSizer->AddStretchSpacer(1);
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );



    //
    // Same but with changed button position
    //
    rowSizer = new wxBoxSizer( wxHORIZONTAL );
    rowSizer->Add( new wxStaticText(panel,wxID_ANY,
                   "OwnerDrawnComboBox with owner-drawn items and button on the left:"), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    rowSizer = new wxBoxSizer( wxHORIZONTAL );


    // When defining derivative class for callbacks, we need
    // to use two-stage creation (or redefine the common wx
    // constructor).
    odc = new wxPenStyleComboBox();
    odc->Create(panel,wxID_ANY,wxEmptyString,
                wxDefaultPosition, wxDefaultSize,
                m_arrItems,
                wxCB_READONLY //wxNO_BORDER | wxCB_READONLY
               );


    odc->SetSelection(0);

    // Use button size that is slightly smaller than the default.
    wxSize butSize = odc->GetButtonSize();
    odc->SetButtonPosition(butSize.x - 2, // button width
                           butSize.y - 6, // button height
                           wxLEFT, // side
                           2 // horizontal spacing
                          );

    rowSizer->Add( odc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );
    rowSizer->AddStretchSpacer(1);
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );


    //
    // List View wxComboCtrl
    //

    rowSizer = new wxBoxSizer( wxHORIZONTAL );
    rowSizer->Add( new wxStaticText(panel,
                        wxID_ANY,
                        "List View wxComboCtrl (custom animation):"),
                   1, wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );
    rowSizer->Add( new wxStaticText(panel,wxID_ANY,"Tree Ctrl wxComboCtrl:"), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    rowSizer = new wxBoxSizer( wxHORIZONTAL );
    cc = new wxComboCtrlWithCustomPopupAnim();

    // Let's set a custom style for the contained wxTextCtrl. We need to
    // use two-step creation for it to work properly.
    cc->SetTextCtrlStyle(wxTE_RIGHT);

    cc->Create(panel, wxID_ANY, wxEmptyString);

    // Make sure we use popup that allows focusing the listview.
    cc->UseAltPopupWindow();

    cc->SetPopupMinWidth(300);

    ListViewComboPopup* iface = new ListViewComboPopup();
    cc->SetPopupControl(iface);

    int i;
    for ( i=0; i<100; i++ )
        iface->AddSelection( wxString::Format("Item %02i",i));

    rowSizer->Add( cc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


    //
    // Tree Ctrl wxComboCtrl
    //

    // Note that we test that wxGenericComboCtrl works
    gcc = new wxGenericComboCtrl(panel,wxID_ANY,wxEmptyString,
                                 wxDefaultPosition, wxDefaultSize);

    // Make sure we use popup that allows focusing the treectrl.
    gcc->UseAltPopupWindow();

    // Set popup interface right away, otherwise some of the calls
    // below may fail
    TreeCtrlComboPopup* tcPopup = new TreeCtrlComboPopup();
    gcc->SetPopupControl(tcPopup);

    // Add items using wxTreeCtrl methods directly
    wxTreeItemId rootId = tcPopup->AddRoot("<hidden_root>");

    wxTreeItemId groupId;

    for ( i=0; i<4; i++ )
    {
        groupId = tcPopup->AppendItem(rootId,
            wxString::Format("Branch %02i",i));

        int n;
        for ( n=0; n<25; n++ )
            tcPopup->AppendItem(groupId,
                wxString::Format("Subitem %02i",(i*25)+n));
    }

    gcc->SetValue("Subitem 05");

    // Move button to left - it makes more sense for a tree ctrl
    gcc->SetButtonPosition(-1, // button width
                           -1, // button height
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
    rowSizer = new wxBoxSizer( wxHORIZONTAL );
    rowSizer->Add( new wxStaticText(panel,wxID_ANY,
                   "OwnerDrawnComboBox with simple dropbutton graphics:"), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );

    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    rowSizer = new wxBoxSizer( wxHORIZONTAL );

    odc = new wxOwnerDrawnComboBox(panel,wxID_ANY,wxEmptyString,
                                   wxDefaultPosition, wxDefaultSize,
                                   m_arrItems,
                                   (long)0 // wxCB_SORT // wxNO_BORDER | wxCB_READONLY
                                  );

    wxOwnerDrawnComboBox* odc2;
    odc2 = new wxOwnerDrawnComboBox(panel,wxID_ANY,wxEmptyString,
                                    wxDefaultPosition, wxDefaultSize,
                                    m_arrItems,
                                    (long)0 // wxCB_SORT // wxNO_BORDER | wxCB_READONLY
                                   );

    // Load images from disk
    wxImage imgNormal("dropbutn.png");
    wxImage imgPressed("dropbutp.png");
    wxImage imgHover("dropbuth.png");

    if ( imgNormal.IsOk() && imgPressed.IsOk() && imgHover.IsOk() )
    {
        wxBitmap bmpNormal(imgNormal);
        wxBitmap bmpPressed(imgPressed);
        wxBitmap bmpHover(imgHover);
        odc->SetButtonBitmaps(bmpNormal,false,bmpPressed,bmpHover);
        odc2->SetButtonBitmaps(bmpNormal,true,bmpPressed,bmpHover);
    }
    else
        wxLogError("Dropbutton images not found");

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
    rowSizer = new wxBoxSizer( wxHORIZONTAL );
    rowSizer->Add( new wxStaticText(panel,wxID_ANY,
                        "wxComboCtrl with custom button action:"), 1,
                   wxALIGN_CENTER_VERTICAL|wxRIGHT, 4 );


    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );

    rowSizer = new wxBoxSizer( wxHORIZONTAL );
    wxFileSelectorCombo* fsc;

    fsc = new wxFileSelectorCombo(panel,wxID_ANY,wxEmptyString,
                                  wxDefaultPosition, wxDefaultSize,
                                  (long)0
                                 );

    rowSizer->Add( fsc, 1, wxALIGN_CENTER_VERTICAL|wxALL, 4 );
    colSizer->Add( rowSizer, 0, wxEXPAND|wxALL, 5 );


    // Make sure GetFeatures is implemented
    wxComboCtrl::GetFeatures();


    topRowSizer->Add( colSizer, 1, wxALL, 2 );

    colSizer = new wxBoxSizer( wxVERTICAL );

    colSizer->AddSpacer(8);
    colSizer->Add( new wxStaticText(panel, wxID_ANY, "Log Messages:"), 0, wxTOP|wxLEFT, 3 );
    colSizer->Add( m_logWin, 1, wxEXPAND|wxALL, 3 );

    topRowSizer->Add( colSizer, 1, wxEXPAND|wxALL, 2 );
    topSizer->Add( topRowSizer, 1, wxEXPAND );

    panel->SetSizer( topSizer );
    topSizer->SetSizeHints( panel );

    Fit();
    Centre();
}

// event handlers

void MyFrame::OnComboBoxUpdate( wxCommandEvent& event )
{
    // Don't show messages for the log output window (it'll crash)
    if ( !event.GetEventObject()->IsKindOf(CLASSINFO(wxComboCtrl)) )
        return;

    if ( event.GetEventType() == wxEVT_COMBOBOX )
    {
        wxLogDebug("EVT_COMBOBOX(id=%i,selection=%i)",event.GetId(),event.GetSelection());
    }
    else if ( event.GetEventType() == wxEVT_TEXT )
    {
        wxLogDebug("EVT_TEXT(id=%i,string=\"%s\")",event.GetId(),event.GetString());
    }
    else if ( event.GetEventType() == wxEVT_TEXT_ENTER )
    {
        wxLogDebug("EVT_TEXT_ENTER(id=%i,string=\"%s\")",
                   event.GetId(), event.GetString());
    }
}

void MyFrame::OnShowComparison( wxCommandEvent& WXUNUSED(event) )
{
    //
    // Show some wxOwnerDrawComboBoxes for comparison
    //

    wxBoxSizer* colSizer;
    wxBoxSizer* rowSizer;
    wxStaticBoxSizer* groupSizer;

    wxComboBox* cb;
    wxOwnerDrawnComboBox* odc;

    const int border = 4;

    wxDialog* dlg = new wxDialog(this,wxID_ANY,
                                 "Compare against wxComboBox",
                                 wxDefaultPosition,wxDefaultSize,
                                 wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

    colSizer = new wxBoxSizer( wxVERTICAL );

    rowSizer = new wxBoxSizer(wxHORIZONTAL);

    groupSizer = new wxStaticBoxSizer(new wxStaticBox(dlg,wxID_ANY," wxOwnerDrawnComboBox "),
                                      wxVERTICAL);

    groupSizer->Add( new wxStaticText(dlg, wxID_ANY,
                     "Writable, with margins, sorted:"),
                     wxSizerFlags().Expand().Border(wxRIGHT, border) );

    odc = new wxOwnerDrawnComboBox(dlg,wxID_ANY,wxEmptyString,
                                   wxDefaultPosition, wxDefaultSize,
                                   m_arrItems,
                                   wxCB_SORT // wxNO_BORDER|wxCB_READONLY
                                  );

    odc->Append("H - Appended Item"); // test sorting in append

    odc->SetValue("Dot Dash");
    odc->SetMargins(15, 10);
    groupSizer->Add( odc, wxSizerFlags().Border(wxALL, border) );
    groupSizer->AddStretchSpacer();

    //
    // Readonly ODComboBox
    groupSizer->Add( new wxStaticText(dlg, wxID_ANY,
                     "Read-only, big font:"),
                     wxSizerFlags().Border(wxRIGHT, border) );

    odc = new wxOwnerDrawnComboBox(dlg,wxID_ANY,wxEmptyString,
                                   wxDefaultPosition, wxDefaultSize,
                                   m_arrItems,
                                   wxCB_SORT|wxCB_READONLY // wxNO_BORDER|wxCB_READONLY
                                  );

    odc->SetFont(odc->GetFont().Scale(1.5));
    odc->SetValue("Dot Dash");
    odc->SetText("Dot Dash (Testing SetText)");

    groupSizer->Add( odc, 0, wxALL, border );
    groupSizer->AddStretchSpacer();

    //
    // Disabled read-only ODComboBox
    groupSizer->Add( new wxStaticText(dlg,wxID_ANY,"Read-only disabled:"),
                   wxSizerFlags().Border(wxRIGHT, border) );

    odc = new wxOwnerDrawnComboBox(dlg,wxID_ANY,wxEmptyString,
                                     wxDefaultPosition, wxDefaultSize,
                                     m_arrItems,
                                     wxCB_READONLY // wxNO_BORDER|wxCB_READONLY
                                     );

    odc->SetValue("Dot Dash");
    odc->Enable(false);

    groupSizer->Add( odc, wxSizerFlags(3).Expand().Border(wxALL, border) );

    // Disabled ODComboBox
    groupSizer->Add(new wxStaticText(dlg, wxID_ANY, "Disabled:"),
        wxSizerFlags().Border(wxRIGHT, border));

    odc = new wxOwnerDrawnComboBox(dlg, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize, m_arrItems);

    odc->SetValue("Dot Dash");
    odc->Enable(false);

    groupSizer->Add(odc, wxSizerFlags(3).Expand().Border(wxALL, border));

    rowSizer->Add( groupSizer, 1, wxEXPAND|wxALL, border );


    groupSizer = new wxStaticBoxSizer(new wxStaticBox(dlg,wxID_ANY," wxComboBox "),
                                      wxVERTICAL);

    //
    // wxComboBox
    //
    groupSizer->Add( new wxStaticText(dlg,wxID_ANY,
                     "Writable, with margins, sorted:"),
                     wxSizerFlags().Expand().Border(wxRIGHT, border) );

    cb = new wxComboBox(dlg,wxID_ANY,wxEmptyString,
                        wxDefaultPosition, wxDefaultSize,
                        m_arrItems,
                        wxCB_SORT // wxNO_BORDER|wxCB_READONLY
                       );

    cb->Append("H - Appended Item"); // test sorting in append

    cb->SetValue("Dot Dash");
    cb->SetMargins(15, 10);
    groupSizer->Add( cb, wxSizerFlags().Border(wxALL, border) );
    groupSizer->AddStretchSpacer();

    //
    // Readonly wxComboBox
    groupSizer->Add( new wxStaticText(dlg, wxID_ANY,
                     "Read-only, big font:"),
                     wxSizerFlags().Border(wxRIGHT, border) );

    cb = new wxComboBox(dlg,wxID_ANY,wxEmptyString,
                        wxDefaultPosition, wxDefaultSize,
                        m_arrItems,
                        wxCB_SORT|wxCB_READONLY // wxNO_BORDER|wxCB_READONLY
                       );

    cb->SetFont(cb->GetFont().Scale(1.5));
    cb->SetValue("Dot Dash");

    groupSizer->Add( cb, 0, wxALL, border );
    groupSizer->AddStretchSpacer();

    //
    // Disabled read-only wxComboBox
    groupSizer->Add( new wxStaticText(dlg,wxID_ANY,"Read-only disabled:"),
                   wxSizerFlags().Border(wxRIGHT, border) );

    cb = new wxComboBox(dlg,wxID_ANY,wxEmptyString,
                        wxDefaultPosition, wxDefaultSize,
                        m_arrItems,
                        wxCB_READONLY // wxNO_BORDER|wxCB_READONLY
                       );

    cb->SetValue("Dot Dash");
    cb->Enable(false);

    groupSizer->Add( cb, wxSizerFlags(3).Expand().Border(wxALL, border) );

    //
    // Disabled wxComboBox
    groupSizer->Add(new wxStaticText(dlg, wxID_ANY, "Disabled:"),
        wxSizerFlags().Border(wxRIGHT, border));

    cb = new wxComboBox(dlg, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize, m_arrItems);

    cb->SetValue("Dot Dash");
    cb->Enable(false);

    groupSizer->Add(cb, wxSizerFlags(3).Expand().Border(wxALL, border));

    rowSizer->Add( groupSizer, 1, wxEXPAND|wxALL, border );

    colSizer->Add( rowSizer, 1, wxEXPAND|wxALL, border );

    dlg->SetSizer( colSizer );
    colSizer->SetSizeHints( dlg );

    dlg->SetSize(60,240);
    dlg->Centre();
    dlg->ShowModal();
}

MyFrame::~MyFrame()
{
    delete wxLog::SetActiveTarget(m_logOld);
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
                    "This is the wxWidgets wxComboCtrl and wxOwnerDrawnComboBox sample\n"
                    "running under %s.",
                    wxVERSION_STRING,
                    wxGetOsDescription()
                 ),
                 "About wxComboCtrl sample",
                 wxOK | wxICON_INFORMATION,
                 this);
}

void MyFrame::OnIdle(wxIdleEvent& event)
{
    // This code is useful for debugging focus problems
    // (which are plentiful when dealing with popup windows).
#if 0
    static wxWindow* lastFocus = (wxWindow*) NULL;

    wxWindow* curFocus = ::wxWindow::FindFocus();

    if ( curFocus != lastFocus )
    {
        const wxChar* className = "<none>";
        if ( curFocus )
            className = curFocus->GetClassInfo()->GetClassName();
        lastFocus = curFocus;
        wxLogDebug( "FOCUSED: %s %X",
            className,
            (unsigned int)curFocus);
    }
#endif

    event.Skip();
}
