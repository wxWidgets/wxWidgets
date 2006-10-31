/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/stattext.cpp
// Purpose:     wxStaticText
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/stattext.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
#endif

#include "wx/notebook.h"
#include "wx/tabctrl.h"

#include <stdio.h>

IMPLEMENT_DYNAMIC_CLASS(wxStaticText, wxControl)

#include "wx/mac/uma.h"

BEGIN_EVENT_TABLE(wxStaticText, wxStaticTextBase)
    EVT_PAINT(wxStaticText::OnPaint)
END_EVENT_TABLE()

bool wxStaticText::Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    m_label = wxStripMenuCodes(label) ;

    if ( !wxControl::Create( parent, id, pos, size, style,
                             wxDefaultValidator , name ) )
    {
        return false;
    }

    SetInitialSize( size ) ;

    return true;
}

const wxString punct = wxT(" ,.-;:!?");

void wxStaticText::DrawParagraph(wxDC &dc, wxString paragraph, int &y)
{
    long width, height ;

    if (paragraph.length() == 0)
    {
        // empty line
        dc.GetTextExtent( wxT("H"), &width, &height );
        y += height;

        return;
    }

    int x = 0 ;

    bool linedrawn = true;
    while( paragraph.length() > 0 )
    {
        dc.GetTextExtent( paragraph , &width , &height ) ;

        if ( width > m_width )
        {
            for ( size_t p = paragraph.length() - 1 ; p > 0 ; --p )
            {
                if ((punct.Find(paragraph[p]) != wxNOT_FOUND) || !linedrawn)
                {
                    int blank = (paragraph[p] == ' ') ? 0 : 1;

                    dc.GetTextExtent( paragraph.Left(p + blank) , &width , &height ) ;

                    if ( width <= m_width )
                    {
                        int pos = x ;
                        if ( HasFlag( wxALIGN_CENTER ) )
                        {
                            pos += ( m_width - width ) / 2 ;
                        }
                        else if ( HasFlag( wxALIGN_RIGHT ) )
                        {
                            pos += ( m_width - width ) ;
                        }

                        dc.DrawText( paragraph.Left(p + blank), pos , y) ;
                        y += height ;
                        paragraph = paragraph.Mid(p+1) ;
                        linedrawn = true;
                        break ;
                    }
                }
            }

            linedrawn = false;
        }
        else
        {
            int pos = x ;
            if ( HasFlag( wxALIGN_CENTER ) )
            {
                pos += ( m_width - width ) / 2 ;
            }
            else if ( HasFlag( wxALIGN_RIGHT ) )
            {
                pos += ( m_width - width ) ;
            }

            dc.DrawText( paragraph, pos , y) ;
            paragraph=wxEmptyString;
            y += height ;
        }
    }
}

void wxStaticText::OnDraw( wxDC &dc )
{
    if (m_width <= 0 || m_height <= 0)
        return;
    /*
        dc.Clear() ;
        wxRect rect(0,0,m_width,m_height) ;
        dc.SetFont(*wxSMALL_FONT) ;

          dc.DrawRectangle(rect) ;
    */
    if ( !IsWindowHilited( (WindowRef) MacGetRootWindow() ) &&
        ( GetBackgroundColour() == wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE )
        || GetBackgroundColour() == wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE) ) )
    {
        dc.SetTextForeground( wxColour( 0x80 , 0x80 , 0x80 ) ) ;
    }
    else
    {
        dc.SetTextForeground( GetForegroundColour() ) ;
    }

    wxString paragraph;
    size_t i = 0 ;
    wxString text = m_label;
    int y = 0 ;
    while (i < text.length())
    {

        if (text[i] == 13 || text[i] == 10)
        {
            DrawParagraph(dc, paragraph,y);
            paragraph = wxEmptyString ;
        }
        else
        {
            paragraph += text[i];
        }
        ++i;
    }
    if (paragraph.length() > 0)
        DrawParagraph(dc, paragraph,y);
}

void wxStaticText::OnPaint( wxPaintEvent & WXUNUSED(event) )
{
    wxPaintDC dc(this);
    OnDraw( dc ) ;
}

wxSize wxStaticText::DoGetBestSize() const
{
    int widthTextMax = 0, widthLine,
        heightTextTotal = 0, heightLineDefault = 0, heightLine = 0;

    wxString curLine;
    for ( const wxChar *pc = m_label; ; pc++ )
    {
        if ( *pc == wxT('\n') || *pc == wxT('\r') || *pc == wxT('\0') )
        {
            if ( !curLine )
            {
                // we can't use GetTextExtent - it will return 0 for both width
                // and height and an empty line should count in height
                // calculation
                if ( !heightLineDefault )
                    heightLineDefault = heightLine;
                if ( !heightLineDefault )
                    GetTextExtent(_T("W"), NULL, &heightLineDefault);

                heightTextTotal += heightLineDefault;

                heightTextTotal++;  // FIXME: why is this necessary?
            }
            else
            {
                GetTextExtent(curLine, &widthLine, &heightLine);
                if ( widthLine > widthTextMax )
                    widthTextMax = widthLine;
                heightTextTotal += heightLine;

                heightTextTotal++;  // FIXME: why is this necessary?
            }

            if ( *pc == wxT('\n') || *pc == wxT('\r')) {
               curLine.Empty();
            }
            else {
               // the end of string
               break;
            }
        }
        else {
            curLine += *pc;
        }
    }

    return wxSize(widthTextMax, heightTextTotal);
}

void wxStaticText::SetLabel(const wxString& st )
{
    wxStaticTextBase::SetLabel( st ) ;
    m_label = st ;
    if ( !(GetWindowStyle() & wxST_NO_AUTORESIZE) )
    {
        // temporary fix until layout measurement and drawing are in synch again
        Refresh() ;
        InvalidateBestSize();
        SetSize( GetBestSize() ) ;
    }
    Refresh() ;
    Update() ;
}

bool wxStaticText::SetFont(const wxFont& font)
{
    bool ret = wxControl::SetFont(font);

    if ( ret )
    {
        // adjust the size of the window to fit to the label unless autoresizing is
        // disabled
        if ( !(GetWindowStyle() & wxST_NO_AUTORESIZE) )
        {
            // temporary fix until layout measurement and drawing are in synch again
            Refresh() ;
            InvalidateBestSize();
            SetSize( GetBestSize() );
        }
    }

    return ret;
}
