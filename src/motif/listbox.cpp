///////////////////////////////////////////////////////////////////////////////
// Name:        listbox.cpp
// Purpose:     wxListBox
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "listbox.h"
#endif

#include "wx/listbox.h"
#include "wx/settings.h"
#include "wx/dynarray.h"
#include "wx/log.h"

#if !USE_SHARED_LIBRARY
  IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)
#endif

// ============================================================================
// list box control implementation
// ============================================================================

// Listbox item
wxListBox::wxListBox()
{
  m_noItems = 0;
  m_selected = 0;
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       int n, const wxString choices[],
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
  m_noItems = n;
  m_selected = 0;

  SetName(name);
  SetValidator(validator);

  if (parent) parent->AddChild(this);

  wxSystemSettings settings;
  SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW));

  m_windowId = ( id == -1 ) ? (int)NewControlId() : id;

  // TODO create listbox

  return FALSE;
}

wxListBox::~wxListBox()
{
}

void wxListBox::SetFirstItem(int N)
{
    // TODO
}

void wxListBox::SetFirstItem(const wxString& s)
{
    // TODO
}

void wxListBox::Delete(int N)
{
  m_noItems --;
    // TODO
}

void wxListBox::Append(const wxString& item)
{
  m_noItems ++;

  // TODO
}

void wxListBox::Append(const wxString& item, char *Client_data)
{
  m_noItems ++;

  // TODO
}

void wxListBox::Set(int n, const wxString *choices, char** clientData)
{
  m_noItems = n;

  // TODO
}

int wxListBox::FindString(const wxString& s) const
{
    // TODO
    return -1;
}

void wxListBox::Clear()
{
  m_noItems = 0;
  // TODO
}

void wxListBox::SetSelection(int N, bool select)
{
    // TODO
}

bool wxListBox::Selected(int N) const
{
    // TODO
    return FALSE;
}

void wxListBox::Deselect(int N)
{
    // TODO
}

char *wxListBox::GetClientData(int N) const
{
    // TODO
    return (char *)NULL;
}

void wxListBox::SetClientData(int N, char *Client_data)
{
    // TODO
}

// Return number of selections and an array of selected integers
int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    aSelections.Empty();

/* TODO
    if ((m_windowStyle & wxLB_MULTIPLE) || (m_windowStyle & wxLB_EXTENDED))
    {
        int no_sel = ??
        for ( int n = 0; n < no_sel; n++ )
            aSelections.Add(??);

        return no_sel;
    }
    else  // single-selection listbox
    {
        aSelections.Add(??);

        return 1;
    }
*/
    return 0;
}

// Get single selection, for single choice list items
int wxListBox::GetSelection() const
{
    // TODO
    return -1;
}

// Find string for position
wxString wxListBox::GetString(int N) const
{
    // TODO
    return wxString("");
}

void wxListBox::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    // TODO
}

void wxListBox::InsertItems(int nItems, const wxString items[], int pos)
{
    m_noItems += nItems;

    // TODO
}

void wxListBox::SetString(int N, const wxString& s)
{
    // TODO
}

int wxListBox::Number () const
{
  return m_noItems;
}

// For single selection items only
wxString wxListBox::GetStringSelection () const
{
  int sel = GetSelection ();
  if (sel > -1)
    return this->GetString (sel);
  else
    return wxString("");
}

bool wxListBox::SetStringSelection (const wxString& s, bool flag)
{
  int sel = FindString (s);
  if (sel > -1)
    {
      SetSelection (sel, flag);
      return TRUE;
    }
  else
    return FALSE;
}

void wxListBox::Command (wxCommandEvent & event)
{
  if (event.m_extraLong)
    SetSelection (event.m_commandInt);
  else
    {
      Deselect (event.m_commandInt);
      return;
    }
  ProcessCommand (event);
}

