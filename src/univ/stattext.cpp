/////////////////////////////////////////////////////////////////////////////
// Name:        univ/stattext.cpp
// Purpose:     wxStaticText
// Author:      Vadim Zeitlin
// Modified by:
// Created:     14.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "stattext.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_STATTEXT

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/stattext.h"
    #include "wx/validate.h"
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

bool wxStaticText::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxString &label,
                          const wxPoint &pos,
                          const wxSize &size, 
                          long style,
                          const wxString &name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    SetLabel(label);

    if ( size.x == -1 || size.y == -1 )
    {
        wxSize sizeBest = DoGetBestSize();
        SetSize(size.x == -1 ? sizeBest.x : size.x,
                size.y == -1 ? sizeBest.y : size.y);
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// size management
// ----------------------------------------------------------------------------

wxSize wxStaticText::DoGetBestSize() const
{
    wxString text = GetLabel();

    int widthTextMax = 0, widthLine,
        heightTextTotal = 0, heightLineDefault = 0, heightLine = 0;

    wxString curLine;
    for ( const wxChar *pc = text; ; pc++ ) {
        if ( *pc == _T('\n') || *pc == _T('\0') ) {
            if ( !curLine ) {
                // we can't use GetTextExtent - it will return 0 for both width
                // and height and an empty line should count in height
                // calculation
                if ( !heightLineDefault )
                    heightLineDefault = heightLine;
                if ( !heightLineDefault )
                    GetTextExtent(_T("W"), NULL, &heightLineDefault);

                heightTextTotal += heightLineDefault;
            }
            else {
                GetTextExtent(curLine, &widthLine, &heightLine);
                if ( widthLine > widthTextMax )
                    widthTextMax = widthLine;
                heightTextTotal += heightLine;
            }

            if ( *pc == _T('\n') ) {
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

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

void wxStaticText::DoDraw(wxDC& dc, wxRenderer *renderer)
{
    // get the position of the label start
    const wxString text = GetLabel();

    wxCoord heightLine;
    dc.GetTextExtent(text[0u], NULL, &heightLine);

    wxCoord x = 0,
            y = heightLine;

    // split the string into lines and draw each of them separately
    wxString curLine;
    for ( const wxChar *pc = text; ; pc++ )
    {
        if ( *pc == _T('\n') || *pc == _T('\0') )
        {
            if ( !curLine.empty() )
            {
                dc.DrawText(curLine, x, y);
            }

            if ( *pc == _T('\0') )
                break;

            y += heightLine;
        }
        else
        {
            curLine += *pc;
        }
    }
}

#endif // wxUSE_STATTEXT
