/////////////////////////////////////////////////////////////////////////////
// Name:        src/motif/combobox_native.cpp
// Purpose:     wxComboBox class
// Author:      Julian Smart, Ian Brown
// Modified by:
// Created:     01/02/03
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_COMBOBOX

#include "wx/combobox.h"

#ifndef WX_PRECOMP
    #include "wx/arrstr.h"
#endif

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
    if( cb_type == XmDROP_DOWN_COMBO_BOX )
        SetWindowStyle( style | wxCB_DROPDOWN );

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

    wxSize best = GetBestSize();
    if( size.x != wxDefaultCoord ) best.x = size.x;
    if( size.y != wxDefaultCoord ) best.y = size.y;

    AttachWidget (parent, m_mainWidget, (WXWidget) NULL,
                  pos.x, pos.y, best.x, best.y);

    ChangeBackgroundColour();

    return true;
}

bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
                        const wxString& value,
                        const wxPoint& pos,
                        const wxSize& size,
                        const wxArrayString& choices,
                        long style,
                        const wxValidator& validator,
                        const wxString& name)
{
    wxCArrayString chs(choices);
    return Create(parent, id, value, pos, size, chs.GetCount(),
                  chs.GetStrings(), style, validator, name);
}

void wxComboBox::AdjustDropDownListSize()
{
    int newListCount = -1, itemCount = GetCount();
    const int MAX = 12;

    if( !itemCount )
        newListCount = 1;
    else if( itemCount < MAX )
        newListCount = itemCount;
    else
        newListCount = MAX;

    XtVaSetValues( GetXmList(this),
                   XmNvisibleItemCount, newListCount,
                   NULL );
}

wxComboBox::~wxComboBox()
{
    DetachWidget((Widget) m_mainWidget); // Removes event handlers
    XtDestroyWidget((Widget) m_mainWidget);
    m_mainWidget = (WXWidget) 0;
    if ( HasClientObjectData() )
        m_clientDataDict.DestroyData();
}

void wxComboBox::DoSetSize(int x, int y, int width, int WXUNUSED(height), int sizeFlags)
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

void wxComboBox::SetString(unsigned int n, const wxString& s)
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
                   XmNvalue, value.mb_str(),
                   NULL);

    m_inSetValue = false;
}

int wxComboBox::DoAppend(const wxString& item)
{
    wxXmString str( item.c_str() );
    XmComboBoxAddItem((Widget) m_mainWidget, str(), 0, False);
    m_noStrings ++;
    AdjustDropDownListSize();

    return GetCount() - 1;
}

int wxComboBox::DoInsert(const wxString& item, unsigned int pos)
{
    wxCHECK_MSG(!(GetWindowStyle() & wxCB_SORT), -1, wxT("can't insert into sorted list"));
    wxCHECK_MSG(IsValidInsert(pos), -1, wxT("invalid index"));

    if (pos == GetCount())
        return DoAppend(item);

    wxXmString str( item.c_str() );
    XmComboBoxAddItem((Widget) m_mainWidget, str(), pos+1, False);
    m_noStrings ++;
    AdjustDropDownListSize();

    return GetCount() - 1;
}

void wxComboBox::Delete(unsigned int n)
{
#ifdef LESSTIF_VERSION
    XmListDeletePos (GetXmList(this), n + 1);
#else
    XmComboBoxDeletePos((Widget) m_mainWidget, n+1);
#endif

    m_clientDataDict.Delete(n, HasClientObjectData());
    m_noStrings--;

    AdjustDropDownListSize();
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

    if ( HasClientObjectData() )
        m_clientDataDict.DestroyData();
    m_noStrings = 0;
    AdjustDropDownListSize();
}

void wxComboBox::SetSelection (int n)
{
    m_inSetSelection = true;

#if wxCHECK_LESSTIF()
    XmListSelectPos (GetXmList(this), n + 1, false);
    SetValue(GetString(n));
#else
#if 0
    wxXmString str(GetString(n).c_str());
    XmComboBoxSelectItem((Widget) m_mainWidget, str());
#endif
    XtVaSetValues( (Widget)m_mainWidget,
                   XmNselectedPosition, n,
                   NULL );
#endif

    m_inSetSelection = false;
}

int wxComboBox::GetSelection (void) const
{
    return wxDoGetSelectionInList( GetXmList( this ) );
}

wxString wxComboBox::GetString(unsigned int n) const
{
    return wxDoGetStringInList( GetXmList(this), n );
}

int wxComboBox::FindString(const wxString& s, bool WXUNUSED(bCase)) const
{
    // FIXME: back to base class for not supported value of bCase

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

wxTextPos wxComboBox::GetLastPosition() const
{
    XmTextPosition pos = XmTextGetLastPosition( GetXmText(this) );
    return (long)pos;
}

void wxComboBox::Replace(long from, long to, const wxString& value)
{
    XmTextReplace( GetXmText(this), (XmTextPosition)from, (XmTextPosition)to,
                   wxConstCast(value.mb_str(), char) );
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

    if( item->m_inSetSelection ) return;

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
            int idx = cbs->item_position;
            event.SetInt(idx);
            event.SetString( item->GetString (idx) );
            if ( item->HasClientObjectData() )
                event.SetClientObject( item->GetClientObject(idx) );
            else if ( item->HasClientUntypedData() )
                event.SetClientData( item->GetClientData(idx) );
            event.SetExtraLong(true);
            event.SetEventObject(item);
            item->GetEventHandler()->ProcessEvent(event);
            break;
        }
    case XmCR_VALUE_CHANGED:
        {
            wxCommandEvent event (wxEVT_COMMAND_TEXT_UPDATED, item->GetId());
            event.SetInt(-1);
            event.SetString( item->GetValue() );
            event.SetExtraLong(true);
            event.SetEventObject(item);
            item->GetEventHandler()->ProcessEvent(event);
            break;
        }
    default:
        break;
    }
}

void wxComboBox::ChangeFont(bool keepOriginalSize)
{
    if( m_font.Ok() )
    {
        wxDoChangeFont( GetXmText(this), m_font );
        wxDoChangeFont( GetXmList(this), m_font );
    }

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
    if( (GetWindowStyle() & wxCB_DROPDOWN) == wxCB_DROPDOWN ||
        (GetWindowStyle() & wxCB_READONLY) == wxCB_READONLY )
    {
        Dimension arrowW, arrowS, highlight, xmargin, ymargin, shadow;

        XtVaGetValues( (Widget)m_mainWidget,
                       XmNarrowSize, &arrowW,
                       XmNarrowSpacing, &arrowS,
                       XmNhighlightThickness, &highlight,
                       XmNmarginWidth, &xmargin,
                       XmNmarginHeight, &ymargin,
                       XmNshadowThickness, &shadow,
                       NULL );

        wxSize listSize = wxDoGetListBoxBestSize( GetXmList(this), this );
        wxSize textSize = wxDoGetSingleTextCtrlBestSize( GetXmText(this),
                                                         this );

        // FIXME arbitrary constants
        return wxSize( listSize.x + arrowW + arrowS + 2 * highlight
                                  + 2 * shadow + 2 * xmargin ,
                       textSize.y + 2 * highlight + 2 * ymargin + 2 * shadow );
    }
    else
        return wxWindow::DoGetBestSize();
}

#endif // XmVersion >= 2000

#endif // wxUSE_COMBOBOX
