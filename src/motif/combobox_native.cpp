/////////////////////////////////////////////////////////////////////////////
// Name:        combobox_native.cpp
// Purpose:     wxComboBox class
// Author:      Julian Smart, Ian Brown
// Modified by:
// Created:     01/02/03
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/setup.h"

#if wxUSE_COMBOBOX

#include "wx/combobox.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

// use the new, shiny combobox for Motif 2.x
#if (XmVersion >= 2000)

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/ComboBox.h>
#include <Xm/Text.h>
#include <Xm/List.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

// utility
static Widget GetXmList( const wxComboBox* cb )
{
    Widget ret;
    XtVaGetValues( (Widget)cb->GetMainWidget(),
                   XmNlist, &ret,
                   NULL );

    return ret;
}

static Widget GetXmText( const wxComboBox* cb )
{
    Widget ret;
    XtVaGetValues( (Widget)cb->GetMainWidget(),
                   XmNtextField, &ret,
                   NULL );

    return ret;
}

void  wxComboBoxCallback (Widget w, XtPointer clientData,
                          XmComboBoxCallbackStruct * cbs);

IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxControl)

bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        int n, const wxString choices[],
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    if( !CreateControl( parent, id, pos, size, style, validator, name ) )
        return false;

    Widget parentWidget = (Widget) parent->GetClientWidget();

    int cb_type = ( style & wxCB_SIMPLE ) ? XmCOMBO_BOX :
                  ( style & wxCB_READONLY ) ? XmDROP_DOWN_LIST :
                  ( style & wxCB_DROPDOWN ) ? XmDROP_DOWN_COMBO_BOX :
    // default to wxCB_DROPDOWN
                                              XmDROP_DOWN_COMBO_BOX;

    Widget buttonWidget= XtVaCreateManagedWidget(name.c_str(),
        xmComboBoxWidgetClass, parentWidget,
	XmNcomboBoxType, cb_type,
        NULL);

    m_mainWidget = (Widget) buttonWidget;

    int i;
    for ( i = 0; i < n; ++i)
        Append( choices[i] );

    XtManageChild (buttonWidget);

    SetValue(value);

    ChangeFont(false);

    XtAddCallback (buttonWidget, XmNselectionCallback,
                   (XtCallbackProc) wxComboBoxCallback,
                   (XtPointer) this);
    XtAddCallback (GetXmText(this), XmNvalueChangedCallback,
                   (XtCallbackProc) wxComboBoxCallback,
                   (XtPointer) this);

    SetCanAddEventHandler(true);
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL,
                  pos.x, pos.y, size.x, size.y);

    XtVaSetValues (GetXmList(this),
                   XmNvisibleItemCount, 10,
                   NULL);

    ChangeBackgroundColour();

    return true;
}

wxComboBox::~wxComboBox()
{
    DetachWidget((Widget) m_mainWidget); // Removes event handlers
    XtDestroyWidget((Widget) m_mainWidget);
    m_mainWidget = (WXWidget) 0;
    if ( HasClientObjectData() )
        m_clientDataDict.DestroyData();
}

void wxComboBox::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    // Necessary so it doesn't call wxChoice::SetSize
    wxWindow::DoSetSize(x, y, width, DoGetBestSize().y, sizeFlags);
}

wxString wxComboBox::GetValue() const
{
    char* s = XmTextGetString (GetXmText (this));
    wxString str(s);
    if (s)
        XtFree (s);
    return str;
}

void wxComboBox::SetString(int n, const wxString& s)
{
    wxXmString text(s);
    Widget listBox = GetXmList(this);

    // delete the item and add it again.
    // FIXME isn't there a way to change it in place?
    XmListDeletePos (listBox, n+1);
    XmListAddItem (listBox, text(), n+1);
}

void wxComboBox::SetValue(const wxString& value)
{
    m_inSetValue = true;

    XtVaSetValues( GetXmText(this),
                   XmNvalue, (char *)value.c_str(),
                   NULL);

    m_inSetValue = false;
}

int wxComboBox::DoAppend(const wxString& item)
{
    wxXmString str( item.c_str() );
    XmComboBoxAddItem((Widget) m_mainWidget, str(), 0, False);
    m_stringList.Add(item);
    m_noStrings ++;

    return GetCount() - 1;
}

void wxComboBox::Delete(int n)
{
#ifdef LESSTIF_VERSION
    XmListDeletePos (GetXmList(this), n + 1);
#else
    XmComboBoxDeletePos((Widget) m_mainWidget, n+1);
#endif

    wxStringList::Node *node = m_stringList.Item(n);
    if (node)
    {
        delete[] node->GetData();
        delete node;
    }
    m_clientDataDict.Delete(n, HasClientObjectData());
    m_noStrings--;
}

void wxComboBox::Clear()
{
#ifdef LESSTIF_VERSION
    XmListDeleteAllItems (GetXmList(this));
#else
    while(m_noStrings > 0)
    {
    	XmComboBoxDeletePos((Widget) m_mainWidget, m_noStrings--);
    }
 #endif

    m_stringList.Clear();

    if ( HasClientObjectData() )
        m_clientDataDict.DestroyData();
    m_noStrings = 0;
}

void wxComboBox::SetSelection (int n)
{
#ifdef LESSTIF_VERSION
    XmListSelectPos (GetXmList(this), n + 1, false);
    SetValue(GetString(n));
#else
    wxXmString str( GetString(n).c_str() );
    XmComboBoxSelectItem((Widget) m_mainWidget, str());
#if 0
    // does it work for Motif
    XtVaSetValues( (Widget)m_mainWidget,
                   XmNselectedPosition, n + 1,
                   NULL );
#endif
#endif
}

int wxComboBox::GetSelection (void) const
{
    return wxDoGetSelectionInList( GetXmList( this ) );
}

wxString wxComboBox::GetString(int n) const
{
    wxStringList::Node *node = m_stringList.Item(n);
    if (node)
        return wxString(node->GetData ());
    else
        return wxEmptyString;
}

int wxComboBox::FindString(const wxString& s) const
{
    return wxDoFindStringInList( GetXmList( this ), s );
}

// Clipboard operations
void wxComboBox::Copy()
{
    XmTextCopy( GetXmText(this), CurrentTime );
}

void wxComboBox::Cut()
{
    XmTextCut( GetXmText(this), CurrentTime );
}

void wxComboBox::Paste()
{
    XmTextPaste( GetXmText(this) );
}

void wxComboBox::SetEditable(bool WXUNUSED(editable))
{
    // TODO
}

void wxComboBox::SetInsertionPoint(long pos)
{
    XmTextSetInsertionPosition( GetXmText(this), (XmTextPosition)pos );
}

void wxComboBox::SetInsertionPointEnd()
{
    SetInsertionPoint( GetLastPosition() );
}

long wxComboBox::GetInsertionPoint() const
{
    return (long)XmTextGetInsertionPosition( GetXmText(this) );
}

long wxComboBox::GetLastPosition() const
{
    XmTextPosition pos = XmTextGetLastPosition( GetXmText(this) );
    return (long)pos;
}

void wxComboBox::Replace(long from, long to, const wxString& value)
{
    XmTextReplace( GetXmText(this), (XmTextPosition)from, (XmTextPosition)to,
                   (char*)value.c_str() );
}

void wxComboBox::Remove(long from, long to)
{
    SetSelection( from, to );
    XmTextRemove( GetXmText(this) );
}

void wxComboBox::SetSelection(long from, long to)
{
    if( to == -1 )
        to = GetLastPosition();

    XmTextSetSelection( GetXmText(this), (XmTextPosition)from,
                        (XmTextPosition)to, (Time)0 );
}

void  wxComboBoxCallback (Widget WXUNUSED(w), XtPointer clientData,
                          XmComboBoxCallbackStruct * cbs)
{
    wxComboBox *item = (wxComboBox *) clientData;

    switch (cbs->reason)
    {
    case XmCR_SELECT:
#if 0
    case XmCR_SINGLE_SELECT:
    case XmCR_BROWSE_SELECT:
#endif
        {
            wxCommandEvent event (wxEVT_COMMAND_COMBOBOX_SELECTED,
                                  item->GetId());
            int idx = cbs->item_position - 1;
            event.m_commandInt = idx;
            event.m_commandString = item->GetString (idx);
            if ( item->HasClientObjectData() )
                event.SetClientObject( item->GetClientObject(idx) );
            else if ( item->HasClientUntypedData() )
                event.SetClientData( item->GetClientData(idx) );
            event.m_extraLong = true;
            event.SetEventObject(item);
            item->ProcessCommand (event);
            break;
        }
    case XmCR_VALUE_CHANGED:
        {
            wxCommandEvent event (wxEVT_COMMAND_TEXT_UPDATED, item->GetId());
            event.m_commandInt = -1;
            event.m_commandString = item->GetValue();
            event.m_extraLong = true;
            event.SetEventObject(item);
            item->ProcessCommand (event);
            break;
        }
    default:
        break;
    }
}

void wxComboBox::ChangeFont(bool keepOriginalSize)
{
    // Don't use the base class wxChoice's ChangeFont
    wxWindow::ChangeFont(keepOriginalSize);
}

void wxComboBox::ChangeBackgroundColour()
{
    wxWindow::ChangeBackgroundColour();
}

void wxComboBox::ChangeForegroundColour()
{
    wxWindow::ChangeForegroundColour();
}

wxSize wxComboBox::DoGetBestSize() const
{
   return wxWindow::DoGetBestSize();
}

#endif // XmVersion >= 2000

#endif // wxUSE_COMBOBOX
