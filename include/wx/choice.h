/////////////////////////////////////////////////////////////////////////////
// Name:        wx/choice.h
// Purpose:     wxChoice class interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     26.07.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHOICE_H_BASE_
#define _WX_CHOICE_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma interface "choicebase.h"
#endif

#include "wx/control.h"     // the base class

// ----------------------------------------------------------------------------
// global data
// ----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern const wxChar*) wxChoiceNameStr;

// ----------------------------------------------------------------------------
// wxChoice allows to select one of a non-modifiable list of strings
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxChoiceBase : public wxControl
{
public:
    // ctor
    wxChoiceBase() { m_clientDataItemsType = ClientData_None; }

    // add a new item to the list
        // no client data
    void Append(const wxString& item) { DoAppend(item); }
        // with client data which belongs to the caller
    void Append(const wxString &item, void* clientData)
        { int n = DoAppend(item); SetClientData(n, clientData); }
        // with client data which will be deleted by the control
    void Append(const wxString &item, wxClientData* clientData)
        { int n = DoAppend(item); SetClientObject(n, clientData); }

    // delete items from the list
        // one item
    virtual void Delete(int n) = 0;
        // all of them
    virtual void Clear() = 0;

    // selection (at most one item may be selected in wxChoice)
        // get the index of currently selected item or -1
    virtual int GetSelection() const = 0;
        // get the text of the currently selected item or empty string
    virtual wxString GetStringSelection() const;

        // set selectionto current item
    virtual void SetSelection(int n) = 0;
        // set selection to the current item, returns TRUE if ok
    virtual bool SetStringSelection(const wxString& sel);

    // accessors to the list of strings
        // get the number of items in the list of strings
    virtual int GetCount() const = 0;

        // find string in the list, return wxNOT_FOUND if not found
    virtual int FindString(const wxString& s) const = 0;
        // get the string with the specified index
    virtual wxString GetString(int n) const = 0;

    // set/get the number of columns in the control (as they're not supporte on
    // most platforms, they do nothing by default)
    virtual void SetColumns(int WXUNUSED(n) = 1 ) { }
    virtual int GetColumns() const { return 1 ; }

    // client data
        // untyped (isn't deleted by the control)
    void SetClientData( int n, void* clientData );
    void* GetClientData( int n ) const;
        // typed (is owned and deleted by the control)
    void SetClientObject( int n, wxClientData* clientData );
    wxClientData* GetClientObject( int n ) const;

    // emulate selecting the item event.GetInt() from the control
    virtual void Command(wxCommandEvent &event);

    // deprecated functions, heer for backwards compatibility only
    int Number() const { return GetCount(); }

private:
    // pure virtuals to implement in the derived classes
    virtual int DoAppend(const wxString& item) = 0;

    virtual void DoSetClientData( int n, void* clientData ) = 0;
    virtual void* DoGetClientData( int n ) const = 0;
    virtual void DoSetClientObject( int n, wxClientData* clientData ) = 0;
    virtual wxClientData* DoGetClientObject( int n ) const = 0;

    // the type of the client data for the items
    wxClientDataType m_clientDataItemsType;
    // the above pure virtuals hide these virtuals in wxWindowBase
    virtual void DoSetClientData(void* clientData ) { wxWindowBase::DoSetClientData(clientData); };
    virtual void* DoGetClientData() const { return(wxWindowBase::DoGetClientData()); };
    virtual void DoSetClientObject( wxClientData* clientData ) { wxWindowBase::DoSetClientObject(clientData); };
    virtual wxClientData* DoGetClientObject() const { return(wxWindowBase::DoGetClientObject()); };
};

// ----------------------------------------------------------------------------
// include the platform-dependent class definition
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/choice.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/choice.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/choice.h"
#elif defined(__WXQT__)
    #include "wx/qt/choice.h"
#elif defined(__WXMAC__)
    #include "wx/mac/choice.h"
#elif defined(__WXPM__)
    #include "wx/os2/choice.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/choice.h"
#endif

#endif
    // _WX_CHOICE_H_BASE_
