/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.cpp
// Purpose:     wxStaticText
// Author:      AUTHOR
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "stattext.h"
#endif

#include "wx/app.h"
#include "wx/stattext.h"

#include <stdio.h>

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxStaticText, wxControl)
#endif

#include <wx/mac/uma.h>

BEGIN_EVENT_TABLE(wxStaticText, wxControl)
    EVT_PAINT(wxStaticText::OnPaint)
END_EVENT_TABLE()

bool wxStaticText::Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    SetName(name);
    m_backgroundColour = parent->GetBackgroundColour() ;
    m_foregroundColour = parent->GetForegroundColour() ;

    if ( id == -1 )
  	    m_windowId = (int)NewControlId();
    else
	    m_windowId = id;

    m_windowStyle = style;
    m_label = label ;

	bool ret = wxControl::Create( parent, id, pos, size, style , wxDefaultValidator , name );
	SetSizeOrDefault( size ) ;
    
    return ret;
}

void wxStaticText::OnDraw( wxDC &dc )
{
    if (m_width <= 0 || m_height <= 0)
        return;

    PrepareDC(dc);
    dc.Clear() ;
    
    int x = 0 ;
    int y = 0 ;
    wxString text = m_label ;
    wxString paragraph ;
   	int i = 0 ;
   	int laststop = 0 ;
   	long width, height ;

   	while( i < text.Length() )
   	{
		if( text[i] == 13 || text[i] == 10)
		{
   			paragraph = text.Mid( laststop , i - laststop ) ;
   			while( paragraph.Length() > 0 )
   			{
   				dc.GetTextExtent( paragraph , &width , &height ) ;
   				if ( width > m_width )
   				{
	   				for ( int p = paragraph.Length() -1 ; p > 0 ; --p )
	   				{
	   					if ( paragraph[p]=='.' )
	   					{
	   						dc.GetTextExtent( paragraph.Left(p+1) , &width , &height ) ;
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
	     						dc.DrawText( paragraph.Left(p+1), pos , y) ;
	     						y += height ;
	   							paragraph = paragraph.Mid(p+1) ;
	   							break ;
	   						}
	   					}
	   					if ( paragraph[p]==' ' )
	   					{
	   						dc.GetTextExtent( paragraph.Left(p) , &width , &height ) ;
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
	     						dc.DrawText( paragraph.Left(p), pos , y) ;
	     						y += height ;
	   							paragraph = paragraph.Mid(p+1) ;
	   							break ;
	   						}
	   					}
	   				}
	   			}
	   			else
	   			{
	     			dc.DrawText( paragraph, x , y) ;
	     			paragraph="";
	     			y += height ;
	   			}
	   		}
   			laststop = i+1 ;
		}
   		++i ;
   	}
   	paragraph = text.Mid( laststop , text.Length() - laststop ) ;
	while( paragraph.Length() > 0 )
	{
		dc.GetTextExtent( paragraph , &width , &height ) ;
		if ( width > m_width )
		{
			for ( int p = paragraph.Length() -1 ; p > 0 ; --p )
			{
				if ( paragraph[p]=='.' )
				{
					dc.GetTextExtent( paragraph.Left(p+1) , &width , &height ) ;
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
 						dc.DrawText( paragraph.Left(p+1), pos , y) ;
 						y += height ;
						paragraph = paragraph.Mid(p+1) ;
						break ;
					}
				}
				if ( paragraph[p]==' ' )
				{
					dc.GetTextExtent( paragraph.Left(p) , &width , &height ) ;
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
 						dc.DrawText( paragraph.Left(p), pos , y) ;
 						y += height ;
						paragraph = paragraph.Mid(p+1) ;
						break ;
					}
				}
			}
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
 			paragraph="";
 			y += height ;
		}
 	}
}

void wxStaticText::OnPaint( wxPaintEvent &event ) 
{
    wxPaintDC dc(this);
    OnDraw( dc ) ;
}

wxSize wxStaticText::DoGetBestSize() const
{
	int x , y  ;
   int widthTextMax = 0, widthLine,
        heightTextTotal = 0, heightLineDefault = 0, heightLine = 0;

    wxString curLine;
    for ( const wxChar *pc = m_label; ; pc++ ) {
        if ( *pc == wxT('\n') || *pc == wxT('\0') ) {
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

            if ( *pc == wxT('\n') ) {
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
	SetTitle( st ) ;
	m_label = st ;
	if ( !(GetWindowStyle() & wxST_NO_AUTORESIZE) )
		SetSizeOrDefault() ;
	
    wxClientDC dc(this);
    OnDraw( dc ) ;
}
