///////////////////////////////////////////////////////////////////////////////
// Name:        statbar.cpp
// Purpose:     native implementation of wxStatusBar (optional)
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/setup.h"
  #include "wx/frame.h"
  #include "wx/settings.h"
  #include "wx/dcclient.h"
#endif

#include "wx/log.h"
#include "wx/generic/statusbr.h"
#include "wx/os2/statusbr.h"

#include "wx/os2/private.h"

#if     !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxStatusBarPM, wxStatusBar);

BEGIN_EVENT_TABLE(wxStatusBarPM, wxStatusBar)
    EVT_SIZE(wxStatusBarPM::OnSize)
END_EVENT_TABLE()
#endif  //USE_SHARED_LIBRARY


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStatusBarPM class
// ----------------------------------------------------------------------------

wxStatusBarPM::wxStatusBarPM()
{
    SetParent(NULL);
    m_hWnd = 0;
    m_windowId = 0;
}

wxStatusBarPM::wxStatusBarPM(wxWindow *parent, wxWindowID id, long style)
{
    Create(parent, id, style);
}

bool wxStatusBarPM::Create(wxWindow *parent, wxWindowID id, long style)
{
    SetParent(parent);

    if (id == -1)
        m_windowId = NewControlId();
    else
        m_windowId = id;

    // TODO: create status bar
    return FALSE;
}

void wxStatusBarPM::CopyFieldsWidth(const int widths[])
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

void wxStatusBarPM::SetFieldsCount(int nFields, const int widths[])
{
  wxASSERT( (nFields > 0) && (nFields < 255) );

  m_nFields = nFields;

  CopyFieldsWidth(widths);
  SetFieldsWidth();
}

void wxStatusBarPM::SetStatusWidths(int n, const int widths[])
{
  wxASSERT( n == m_nFields );

  CopyFieldsWidth(widths);
  SetFieldsWidth();
}

void wxStatusBarPM::SetFieldsWidth()
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

void wxStatusBarPM::SetStatusText(const wxString& strText, int nField)
{
    // TODO
}

wxString wxStatusBarPM::GetStatusText(int nField) const
{
    wxASSERT( (nField > -1) && (nField < m_nFields) );

    // TODO
    return wxString("");
}

void wxStatusBarPM::OnSize(wxSizeEvent& event)
{
    // adjust fields widths to the new size
    SetFieldsWidth();
}

