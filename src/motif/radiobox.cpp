/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.cpp
// Purpose:     wxRadioBox
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "radiobox.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __VMS
#define XtDisplay XTDISPLAY
#endif

#include "wx/defs.h"

#include "wx/radiobox.h"
#include "wx/utils.h"
#include "wx/arrstr.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

void wxRadioBoxCallback (Widget w, XtPointer clientData,
                    XmToggleButtonCallbackStruct * cbs);

IMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl)

// Radio box item
void wxRadioBox::Init()
{
    m_selectedButton = -1;
    m_noItems = 0;
    m_noRowsOrCols = 0;
    m_majorDim = 0 ;
}

bool wxRadioBox::Create(wxWindow *parent, wxWindowID id, const wxString& title,
             const wxPoint& pos, const wxSize& size,
             int n, const wxString choices[],
             int majorDim, long style,
             const wxValidator& val, const wxString& name)
{
    if( !CreateControl( parent, id, pos, size, style, val, name ) )
        return false;

    m_noItems = n;
    m_noRowsOrCols = majorDim;

    if (majorDim==0)
        m_majorDim = n ;
    else
        m_majorDim = majorDim ;

    Widget parentWidget = (Widget) parent->GetClientWidget();

    m_mainWidget = XtVaCreateWidget ("radioboxframe",
                                        xmFrameWidgetClass, parentWidget,
                                        XmNresizeHeight, True,
                                        XmNresizeWidth, True,
                                        NULL);

    wxString label1(wxStripMenuCodes(title));

    WXFontType fontType = m_font.GetFontType(XtDisplay(parentWidget));

    if (label1 != "")
    {
        wxXmString text(label1);
        m_labelWidget = (WXWidget)
            XtVaCreateManagedWidget( label1.c_str(),
#if wxUSE_GADGETS
                                       style & wxCOLOURED ? xmLabelWidgetClass
                                                          : xmLabelGadgetClass,
                                       (Widget)m_mainWidget,
#else
                                       xmLabelWidgetClass, 
                                       (Widget)m_mainWidget,
#endif
                                       wxFont::GetFontTag(), fontType,
                                       XmNlabelString, text(),
// XmNframeChildType is not in Motif 1.2, nor in Lesstif,
// if it was compiled with 1.2 compatibility
// TODO: check this still looks OK for Motif 1.2.
#if (XmVersion > 1200)
                                       XmNframeChildType, XmFRAME_TITLE_CHILD,
#else
                                       XmNchildType, XmFRAME_TITLE_CHILD,
#endif
                                       XmNchildVerticalAlignment, 
                                           XmALIGNMENT_CENTER,
                                       NULL);
    }

    Arg args[3];

    m_majorDim = (n + m_majorDim - 1) / m_majorDim;

    XtSetArg (args[0], XmNorientation, ((style & wxHORIZONTAL) == wxHORIZONTAL ?
                                          XmHORIZONTAL : XmVERTICAL));
    XtSetArg (args[1], XmNnumColumns, m_majorDim);
    XtSetArg (args[2], XmNadjustLast, False);

    Widget radioBoxWidget =
        XmCreateRadioBox ((Widget)m_mainWidget, "radioBoxWidget", args, 3);

    m_radioButtons.reserve(n);
    m_radioButtonLabels.reserve(n);

    int i;
    for (i = 0; i < n; i++)
    {
        wxString str(wxStripMenuCodes(choices[i]));
        m_radioButtonLabels.push_back(str);
        Widget radioItem =  XtVaCreateManagedWidget (wxConstCast(str.c_str(), char),
#if wxUSE_GADGETS
                            xmToggleButtonGadgetClass, radioBoxWidget,
#else
                            xmToggleButtonWidgetClass, radioBoxWidget,
#endif
                            wxFont::GetFontTag(), fontType,
                                                 NULL);
        m_radioButtons.push_back((WXWidget)radioItem);
        XtAddCallback (radioItem, XmNvalueChangedCallback,
                       (XtCallbackProc) wxRadioBoxCallback,
                       (XtPointer) this);
    }

    ChangeFont(FALSE);

    SetSelection (0);
 
    XtRealizeWidget((Widget)m_mainWidget);
    XtManageChild (radioBoxWidget);
    XtManageChild ((Widget)m_mainWidget);

    AttachWidget (parent, m_mainWidget, NULL, pos.x, pos.y, size.x, size.y);

    ChangeBackgroundColour();

    return TRUE;
}

bool wxRadioBox::Create(wxWindow *parent, wxWindowID id, const wxString& title,
             const wxPoint& pos, const wxSize& size,
             const wxArrayString& choices,
             int majorDim, long style,
             const wxValidator& val, const wxString& name)
{
    wxCArrayString chs(choices);
    return Create(parent, id, title, pos, size, chs.GetCount(),
                  chs.GetStrings(), majorDim, style, val, name);
}

wxRadioBox::~wxRadioBox()
{
    DetachWidget(m_mainWidget);
    XtDestroyWidget((Widget) m_mainWidget);

    m_mainWidget = (WXWidget) 0;
}

void wxRadioBox::SetString(int item, const wxString& label)
{
    if (item < 0 || item >= m_noItems)
        return;

    Widget widget = (Widget) m_radioButtons[item];
    if (label != "")
    {
        wxString label1(wxStripMenuCodes(label));
        wxXmString text( label1 );
        m_radioButtonLabels[item] = label1;
        XtVaSetValues (widget,
                        XmNlabelString, text(),
                        XmNlabelType, XmSTRING,
                        NULL);
    }
}

int wxRadioBox::FindString(const wxString& s) const
{
    int i;
    for (i = 0; i < m_noItems; i++)
        if (s == m_radioButtonLabels[i])
            return i;
    return -1;
}

void wxRadioBox::SetSelection(int n)
{
    if ((n < 0) || (n >= m_noItems))
        return;

    m_selectedButton = n;

    m_inSetValue = TRUE;

    XmToggleButtonSetState ((Widget) m_radioButtons[n], TRUE, FALSE);

    int i;
    for (i = 0; i < m_noItems; i++)
        if (i != n)
            XmToggleButtonSetState ((Widget) m_radioButtons[i], FALSE, FALSE);

    m_inSetValue = FALSE;
}

// Get single selection, for single choice list items
int wxRadioBox::GetSelection() const
{
    return m_selectedButton;
}

// Find string for position
wxString wxRadioBox::GetString(int n) const
{
    if ((n < 0) || (n >= m_noItems))
        return wxEmptyString;
    return m_radioButtonLabels[n];
}

void wxRadioBox::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    bool managed = XtIsManaged((Widget) m_mainWidget);

    if (managed)
        XtUnmanageChild ((Widget) m_mainWidget);

    int xx = x; int yy = y;
    AdjustForParentClientOrigin(xx, yy, sizeFlags);

    if (x > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        XtVaSetValues ((Widget) m_mainWidget, XmNx, xx, NULL);
    if (y > -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        XtVaSetValues ((Widget) m_mainWidget, XmNy, yy, NULL);    

    if (width > 0)
        XtVaSetValues ((Widget) m_mainWidget, XmNwidth, width, NULL);
    if (height > 0)
        XtVaSetValues ((Widget) m_mainWidget, XmNheight, height, NULL);

    if (managed)
        XtManageChild ((Widget) m_mainWidget);
}

// Enable a specific button
void wxRadioBox::Enable(int n, bool enable)
{
    if ((n < 0) || (n >= m_noItems))
        return;

    XtSetSensitive ((Widget) m_radioButtons[n], (Boolean) enable);
}

// Enable all controls
bool wxRadioBox::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
        return FALSE;

    int i;
    for (i = 0; i < m_noItems; i++)
        XtSetSensitive ((Widget) m_radioButtons[i], (Boolean) enable);

    return TRUE;
}

bool wxRadioBox::Show(bool show)
{
    // TODO: show/hide all children
    return wxControl::Show(show);
}

// Show a specific button
void wxRadioBox::Show(int n, bool show)
{
  // This method isn't complete, and we try do do our best...
  // It's main purpose isn't for allowing Show/Unshow dynamically,
  // but rather to provide a way to design wxRadioBox such:
  //
  //        o Val1  o Val2   o Val3 
  //        o Val4           o Val6 
  //        o Val7  o Val8   o Val9 
  //
  // In my case, this is a 'direction' box, and the Show(5,False) is
  // coupled with an Enable(5,False)
  //
    if ((n < 0) || (n >= m_noItems))
        return;

    XtVaSetValues ((Widget) m_radioButtons[n],
        XmNindicatorOn, (unsigned char) show,
        NULL);

    // Please note that this is all we can do: removing the label
    // if switching to unshow state. However, when switching
    // to the on state, it's the prog. resp. to call SetString(item,...)
    // after this call!!
    if (!show)
        wxRadioBox::SetString (n, " ");
}

// For single selection items only
wxString wxRadioBox::GetStringSelection () const
{
    int sel = GetSelection ();
    if (sel > -1)
        return this->GetString (sel);
    else
        return wxString("");
}

bool wxRadioBox::SetStringSelection (const wxString& s)
{
    int sel = FindString (s);
    if (sel > -1)
    {
        SetSelection (sel);
        return TRUE;
    }
    else
        return FALSE;
}

void wxRadioBox::Command (wxCommandEvent & event)
{
    SetSelection (event.GetInt());
    ProcessCommand (event);
}

void wxRadioBox::ChangeFont(bool keepOriginalSize)
{
    wxWindow::ChangeFont(keepOriginalSize);

    WXFontType fontType =
        m_font.GetFontType(XtDisplay((Widget) GetTopWidget()));

    int i;
    for (i = 0; i < m_noItems; i++)
    {
        WXWidget radioButton = m_radioButtons[i];

        XtVaSetValues ((Widget) radioButton,
                       wxFont::GetFontTag(), fontType,
                       NULL);
    }
}

void wxRadioBox::ChangeBackgroundColour()
{
    wxWindow::ChangeBackgroundColour();

    int selectPixel = wxBLACK->AllocColour(XtDisplay((Widget)m_mainWidget));

    int i;
    for (i = 0; i < m_noItems; i++)
    {
        WXWidget radioButton = m_radioButtons[i];

        wxDoChangeBackgroundColour(radioButton, m_backgroundColour, TRUE);

        XtVaSetValues ((Widget) radioButton,
          XmNselectColor, selectPixel,
          NULL);
    }
}

void wxRadioBox::ChangeForegroundColour()
{
    wxWindow::ChangeForegroundColour();

    int i;
    for (i = 0; i < m_noItems; i++)
    {
        WXWidget radioButton = m_radioButtons[i];

        wxDoChangeForegroundColour(radioButton, m_foregroundColour);
    }
}

static int CalcOtherDim( int items, int dim )
{
    return items / dim + ( items % dim ? 1 : 0 );
}

int wxRadioBox::GetRowCount() const
{
    return m_windowStyle & wxRA_SPECIFY_ROWS ? m_noRowsOrCols
        : CalcOtherDim( GetCount(), m_noRowsOrCols );
}

int wxRadioBox::GetColumnCount() const
{
    return m_windowStyle & wxRA_SPECIFY_COLS ? m_noRowsOrCols
        : CalcOtherDim( GetCount(), m_noRowsOrCols );
}

void wxRadioBoxCallback (Widget w, XtPointer clientData,
                    XmToggleButtonCallbackStruct * cbs)
{
  if (!cbs->set)
    return;

  wxRadioBox *item = (wxRadioBox *) clientData;
  int sel = -1;
  int i;
  const wxWidgetArray& buttons = item->GetRadioButtons();
  for (i = 0; i < item->GetCount(); i++)
    if (((Widget)buttons[i]) == w)
      sel = i;
  item->SetSel(sel);

  if (item->InSetValue())
    return;

  wxCommandEvent event (wxEVT_COMMAND_RADIOBOX_SELECTED, item->GetId());
  event.SetInt(sel);
  event.SetString(item->GetStringSelection());
  event.SetEventObject(item);
  item->ProcessCommand (event);
}

