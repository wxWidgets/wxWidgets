///////////////////////////////////////////////////////////////////////////////
// Name:        statbar.cpp
// Purpose:     native implementation of wxStatusBar (optional)
// Author:      AUTHOR
// Modified by: 
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 AUTHOR
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "statusbr.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/mac/statusbr.h"

IMPLEMENT_DYNAMIC_CLASS(wxStatusBarXX, wxStatusBar);

BEGIN_EVENT_TABLE(wxStatusBarXX, wxStatusBar)
    EVT_SIZE(wxStatusBarXX::OnSize)
END_EVENT_TABLE()


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStatusBarXX class
// ----------------------------------------------------------------------------

wxStatusBarXX::wxStatusBarXX()
{
    SetParent(NULL);
}

wxStatusBarXX::wxStatusBarXX(wxWindow *parent, wxWindowID id, long style)
{
    Create(parent, id, style);
}

bool wxStatusBarXX::Create(wxWindow *parent, wxWindowID id, long style)
{
    SetParent(parent);

    if (id == -1)
        m_windowId = NewControlId();
    else
        m_windowId = id;

    // TODO: create status bar
    return FALSE;
}

void wxStatusBarXX::SetFieldsCount(int nFields, const int widths[])
{
  wxASSERT( (nFields > 0) && (nFields < 255) );

  m_nFields = nFields;

  CopyFieldsWidth(widths);
  SetFieldsWidth();
}

void wxStatusBarXX::SetStatusWidths(int n, const int widths[])
{
  wxASSERT( n == m_nFields );

  CopyFieldsWidth(widths);
  SetFieldsWidth();
}

void wxStatusBarXX::CopyFieldsWidth(const int widths[])
{
  if (widths && !m_statusWidths)
    m_statusWidths = new int[m_nFields];

  if ( widths != NULL ) {
    for ( int i = 0; i < m_nFields; i++ )
      m_statusWidths[i] = widths[i];
  }
  else {
    delete [] m_statusWidths;
    m_statusWidths = NULL;
  }
}

void wxStatusBarXX::SetFieldsWidth()
{
  int *pWidths = new int[m_nFields];

  int nWindowWidth, y;
  GetClientSize(&nWindowWidth, &y);

  if ( m_statusWidths == NULL ) {
    // default: all fields have the same width
    int nWidth = nWindowWidth / m_nFields;
    for ( int i = 0; i < m_nFields; i++ )
      pWidths[i] = (i + 1) * nWidth;
  }
  else {
    // -1 doesn't mean the same thing for wxWindows and Win32, recalc
    int nTotalWidth = 0, 
        nVarCount = 0, 
        i;
    for ( i = 0; i < m_nFields; i++ ) {
      if ( m_statusWidths[i] == -1 )
        nVarCount++;
      else
        nTotalWidth += m_statusWidths[i];
    }

    if ( nVarCount == 0 ) {
      // wrong! at least one field must be of variable width
      wxFAIL;

      nVarCount++;
    }

    int nVarWidth = (nWindowWidth - nTotalWidth) / nVarCount;

    // do fill the array
    int nCurPos = 0;
    for ( i = 0; i < m_nFields; i++ ) {
      if ( m_statusWidths[i] == -1 )
        nCurPos += nVarWidth;
      else
        nCurPos += m_statusWidths[i];
      pWidths[i] = nCurPos;
    }
  }

  // TODO: set widths

  delete [] pWidths;
}

void wxStatusBarXX::SetStatusText(const wxString& strText, int nField)
{
    // TODO
}

wxString wxStatusBarXX::GetStatusText(int nField) const
{
    wxASSERT( (nField > -1) && (nField < m_nFields) );

    // TODO
    return wxString("");
}

void wxStatusBarXX::OnSize(wxSizeEvent& event)
{
    // adjust fields widths to the new size
    SetFieldsWidth();
}
