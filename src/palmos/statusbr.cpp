///////////////////////////////////////////////////////////////////////////////
// Name:        palmos/statusbr.cpp
// Purpose:     Implementation of wxStatusBar for PalmOS
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "statusbr.h"
#endif

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/setup.h"
  #include "wx/frame.h"
  #include "wx/settings.h"
  #include "wx/dcclient.h"
#endif

#if wxUSE_STATUSBAR && wxUSE_NATIVE_STATUSBAR

#include "wx/intl.h"
#include "wx/log.h"
#include "wx/statusbr.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStatusBarPalm class
// ----------------------------------------------------------------------------

wxStatusBarPalm::wxStatusBarPalm()
{
    SetParent(NULL);
    m_hWnd = 0;
    m_windowId = 0;
}

bool wxStatusBarPalm::Create(wxWindow *parent,
                           wxWindowID id,
                           long style,
                           const wxString& name)
{
    wxCHECK_MSG( parent, FALSE, wxT("status bar must have a parent") );

    StatusTextBuffer = NULL;

    SetName(name);
    SetParent(parent);

    parent->AddChild(this);

    m_windowId = id == -1 ? NewControlId() : id;

    SetFieldsCount(1);
    SubclassWin(m_hWnd);

    return TRUE;
}

wxStatusBarPalm::~wxStatusBarPalm()
{
    DeleteStatusBuffer();
}

void wxStatusBarPalm::SetFieldsCount(int nFields, const int *widths)
{
    // this is a Windows limitation
    wxASSERT_MSG( (nFields > 0) && (nFields < 255), _T("too many fields") );

    wxStatusBarBase::SetFieldsCount(nFields, widths);

    SetFieldsWidth();
}

void wxStatusBarPalm::SetStatusWidths(int n, const int widths[])
{
    wxStatusBarBase::SetStatusWidths(n, widths);

    SetFieldsWidth();
}

void wxStatusBarPalm::SetFieldsWidth()
{
    // clear the status bar
    DeleteStatusBuffer();
}

void wxStatusBarPalm::SetStatusText(const wxString& strText, int nField)
{
    wxCHECK_RET( (nField >= 0) && (nField < m_nFields),
                 _T("invalid statusbar field index") );

    SetStatusBufferText(strText,nField);
    DrawStatusBar();
}

wxString wxStatusBarPalm::GetStatusText(int nField) const
{
    wxCHECK_MSG( (nField >= 0) && (nField < m_nFields), wxEmptyString,
                 _T("invalid statusbar field index") );

    wxString text;
    return text;
}

void wxStatusBarPalm::DrawStatusBar()
{
    int i=0;
    int leftPos=0;
    wxArrayInt widthsAbs;
    wxString text;
    
    RectangleType EraseRect;
    EraseRect.topLeft.x=0;
    EraseRect.topLeft.y=160-FntCharHeight()-1;    
    EraseRect.extent.x=159;
    EraseRect.extent.y=159;    
    WinEraseRectangle(&EraseRect,0);

    if(m_nFields>0)
        widthsAbs=CalculateAbsWidths(160 - 2*(m_nFields - 1));
    
    for(i=0;i<m_nFields;i++)
    {
        text=GetStatusBufferText(i);
        WinDrawTruncChars(text,StrLen(text),leftPos,160-FntCharHeight(),widthsAbs[i]);
        leftPos+=widthsAbs[i]+2;
    }
    WinDrawLine(0,160-FntCharHeight()-1,159,160-FntCharHeight()-1);
}

void wxStatusBarPalm::SetStatusBufferText(const wxString& text, int number)
{
    wxListString* st = GetOrCreateStatusBuffer(number);

    wxString tmp1(text);
    wxString* tmp = new wxString(tmp1);
    st->Insert(tmp);
}

wxString wxStatusBarPalm::GetStatusBufferText(int number)
{
    wxListString *st = GetStatusBufferStack(number);
    if(st==0)
        return "";

    wxListString::compatibility_iterator top = st->GetFirst();
    return(*top->GetData());
}

wxListString *wxStatusBarPalm::GetOrCreateStatusBuffer(int i)
{
    if(!StatusTextBuffer)
    {
        StatusTextBuffer = new wxListString*[m_nFields];

        size_t j;
        for(j = 0; j < (size_t)m_nFields; ++j) StatusTextBuffer[j] = 0;
    }

    if(!StatusTextBuffer[i])
    {
        StatusTextBuffer[i] = new wxListString();
    }
    else
    {
        wxListString *st=StatusTextBuffer[i];
        wxListString::compatibility_iterator top = st->GetFirst();
        delete top->GetData();
        st->Erase(top);
        delete st;
          
        StatusTextBuffer[i] = new wxListString();
    }

    return StatusTextBuffer[i];
}

wxListString *wxStatusBarPalm::GetStatusBufferStack(int i) const
{
    if(!StatusTextBuffer)
        return 0;
    return StatusTextBuffer[i];
}

void wxStatusBarPalm::DeleteStatusBuffer()
{
    int i=0;
    
    if(!StatusTextBuffer)
    {
        return;
    }

    for(i=0;i<m_nFields;i++)
    {
        if(StatusTextBuffer[i])
        {
            wxListString *st=StatusTextBuffer[i];
            wxListString::compatibility_iterator top = st->GetFirst();
            delete top->GetData();
            st->Erase(top);
            delete st;
            StatusTextBuffer[i]=0;
        }
    }
    delete[] m_statusTextStacks;
}

int wxStatusBarPalm::GetBorderX() const
{
    return 0;
}

int wxStatusBarPalm::GetBorderY() const
{
    return 0;
}

void wxStatusBarPalm::SetMinHeight(int height)
{
}

bool wxStatusBarPalm::GetFieldRect(int i, wxRect& rect) const
{
}

void wxStatusBarPalm::DoMoveWindow(int x, int y, int width, int height)
{
}

#endif // wxUSE_NATIVE_STATUSBAR

