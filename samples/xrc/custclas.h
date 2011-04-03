//-----------------------------------------------------------------------------
// Name:        custclas.h
// Purpose:     XML resources sample: A custom class to insert into a XRC file
// Author:      Robert O'Connor (rob@medicalmnemonics.com), Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) Robert O'Connor and Vaclav Slavik
// Licence:     wxWindows licence
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#ifndef _CUSTCLAS_H_
#define _CUSTCLAS_H_

//----------------------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------------------

#include "wx/listctrl.h"

//----------------------------------------------------------------------------------------
// Class definition: MyResizableListCtrl
//----------------------------------------------------------------------------------------

//! A custom listctrl that resizes itself and pops up a context-sensitive menu.
class MyResizableListCtrl : public wxListCtrl
{
    // Very helpful wxWidgets macro required for wxWidgets-RTTI tracing: By using this
    // you will see "Leaked one object of type myResizableListCtrl" in the debug log,
    // along with which line you if was created, but you forget to free the memory.
    // NOTE: Using this REQUIRES a default constructor: that means either: giving a
    // default value for all parameters in your constructor, or else having a dummy
    // MyResizableListCtrl(){} constructor in addition to your regular one.
    DECLARE_DYNAMIC_CLASS( MyResizableListCtrl )

public:

    // Constructor.
    /*
        These parameters are the same as a wxWidgets constructor.
        \param parent The parent window.
        \param id The id of the progress_listbox. Will usually be -1 unless multiple
            of them on the same dialog.
        \param pos The pixel position of the listctrl on its parent window
        \param size The pixel size of the listctrl
        \param style Style of the listbox. See wxWidgets wxListBox docs for details.
        \param validator Window validator. See wxWidgets docs for details.
        \param name Windows name (rarely used).
        \param exclusion_column_caption The label of header of listctrl's exclusion
            column.
     */
    MyResizableListCtrl( wxWindow *parent = NULL,
                         wxWindowID id = wxID_ANY,
                         const wxPoint &pos = wxDefaultPosition,
                         const wxSize &size = wxDefaultSize,
                         long style = wxLC_REPORT,
                         const wxValidator& validator = wxDefaultValidator,
                         const wxString &name = wxT("myResizableListCtrl")
                       );

    // Destructor.
    ~MyResizableListCtrl(){};

protected:

    // A custom function for a context sensitive menu.
    void ContextSensitiveMenu( wxMouseEvent& event );

    // This is a wxWidgets function that we are going to override with our own behaviour.
    void OnSize( wxSizeEvent &event );

    // A custom function. What is called in the constructor, and in an OnSize()
    void SetColumnWidths();

private:

    // wxWidgets macro, required to be able to use Event tables in the .cpp file.
    DECLARE_EVENT_TABLE()

};

//----------------------------------------------------------------------------------------
// End single inclusion of this .h file condition
//----------------------------------------------------------------------------------------

#endif  //_CUSTCLAS_H_

