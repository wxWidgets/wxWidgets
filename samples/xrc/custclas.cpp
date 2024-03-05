//-----------------------------------------------------------------------------
// Name:        custclass.cpp
// Purpose:     XML resources sample: A custom class to insert into a XRC file
// Author:      Robert O'Connor (rob@medicalmnemonics.com), Vaclav Slavik
// Copyright:   (c) Robert O'Connor and Vaclav Slavik
// Licence:     wxWindows licence
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Standard wxWidgets headers
//-----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// For all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//-----------------------------------------------------------------------------
// Header of this .cpp file
//-----------------------------------------------------------------------------

#include "custclas.h"

//-----------------------------------------------------------------------------
// Internal constants
//-----------------------------------------------------------------------------

// Popup menu (PU) item control IDs. In this example, they aren't hooked up
// to any functions. Normally you would use these IDs in your event table, so
// that if one of these menu items is clicked, then a certain function is
// called.
enum {
    PU_ADD_RECORD        = wxID_HIGHEST,
    PU_EDIT_RECORD,
    PU_DELETE_RECORD
};

// Columns of the listctrl (the leftmost one starts at 0, and so on).
// Allows easier code maintenance if want to add/rearrangement of listctrl's
// columns.
enum {
    RECORD_COLUMN    = 0,
    ACTION_COLUMN,
    PRIORITY_COLUMN
};

//-----------------------------------------------------------------------------
// wxWidgets macro: implement dynamic class
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS( MyResizableListCtrl, wxListCtrl );

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE( MyResizableListCtrl, wxListCtrl )
    // Something to do when right mouse down
    EVT_RIGHT_DOWN( MyResizableListCtrl::ContextSensitiveMenu )
    // Something to do when resized
    EVT_SIZE( MyResizableListCtrl::OnSize )
wxEND_EVENT_TABLE()

//-----------------------------------------------------------------------------
// Public methods
//-----------------------------------------------------------------------------

// Constructor, including setting the dialog's m_configuration_section member
// to the incoming configuration_section string.
MyResizableListCtrl::MyResizableListCtrl( wxWindow *parent, wxWindowID id,
                                        const wxPoint& pos, const wxSize& size,
                                        long style, const wxValidator& validator,
                                        const wxString& name )
   : wxListCtrl( parent, id, pos, size, style, validator, name )
{

    // This listctrl needs to insert its columns in the constructor, since
    // as soon as the listctrl is built, it is resized and grafted onto an
    // "unknown" XRC placeholder. This induces an OnSize() event, calling the
    // overridden OnSize function for this class, which needs to have 3
    // columns to resize (else an assert on WXGTK debug build).
    InsertColumn( RECORD_COLUMN, _("Record"), wxLIST_FORMAT_LEFT, 140);
    InsertColumn( ACTION_COLUMN, _("Action"), wxLIST_FORMAT_LEFT, 70);
    InsertColumn( PRIORITY_COLUMN, _("Priority"), wxLIST_FORMAT_LEFT, 70 );
}


void MyResizableListCtrl::ContextSensitiveMenu( wxMouseEvent& event )
{
    // Make an instance of a menu.
    wxMenu      a_menu;

    a_menu.Append( PU_ADD_RECORD, _( "Add a new record...") );
    a_menu.Append( PU_EDIT_RECORD, _( "Edit selected record..." ) );
    a_menu.Append( PU_DELETE_RECORD, _( "Delete selected record" ) );

    // If no listctrl rows selected, then disable the menu items that
    // require selection
    if ( GetSelectedItemCount() == 0 ) {
        a_menu.Enable( PU_EDIT_RECORD, false );
        a_menu.Enable( PU_DELETE_RECORD, false );
    }

    // Show the popup menu (wxWindow::PopupMenu ), at the x,y position
    // of the click event
    PopupMenu( &a_menu, event.GetPosition() );
}


void MyResizableListCtrl::OnSize( wxSizeEvent &event )
{
    // Call our custom width setting function.
    SetColumnWidths();
    // REQURED event.Skip() call to allow this event to propagate
    // upwards so others can do what they need to do in response to
    // this size event.
    event.Skip();
}


void MyResizableListCtrl::SetColumnWidths()
{
    // Get width of entire listctrl
    int leftmostColumnWidth = GetSize().x;

    // Subtract width of other columns, scrollbar, and some padding
    leftmostColumnWidth -= GetColumnWidth( ACTION_COLUMN );
    leftmostColumnWidth -= GetColumnWidth( PRIORITY_COLUMN );
    leftmostColumnWidth -= wxSystemSettings::GetMetric( wxSYS_VSCROLL_X );
    leftmostColumnWidth -= 5;

    // Set the column width to the new value.
    SetColumnWidth( RECORD_COLUMN, leftmostColumnWidth );

    // This is just a debug message in case you want to watch the
    // events scroll by as you resize.
    wxLogDebug( "Successfully set column widths" );
}


