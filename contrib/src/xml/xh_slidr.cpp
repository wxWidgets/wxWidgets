/////////////////////////////////////////////////////////////////////////////
// Name:        xh_slidr.cpp
// Purpose:     XML resource for wxSlider
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_slidr.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_slidr.h"
#include "wx/slider.h"

#if wxUSE_SLIDER

wxSliderXmlHandler::wxSliderXmlHandler() 
: wxXmlResourceHandler() 
{
    ADD_STYLE( wxSL_HORIZONTAL );
    ADD_STYLE( wxSL_VERTICAL );
    ADD_STYLE( wxSL_AUTOTICKS );
    ADD_STYLE( wxSL_LABELS );
    ADD_STYLE( wxSL_LEFT );
    ADD_STYLE( wxSL_TOP );
    ADD_STYLE( wxSL_RIGHT );
    ADD_STYLE( wxSL_BOTTOM );
    ADD_STYLE( wxSL_BOTH );
    ADD_STYLE( wxSL_SELRANGE );
    AddWindowStyles();
}

wxObject *wxSliderXmlHandler::DoCreateResource()
{ 
    wxSlider *control = new wxSlider(m_ParentAsWindow,
                                    GetID(),
                                    GetLong( _T("value"), wxSL_DEFAULT_VALUE), 
                                    GetLong( _T("min"), wxSL_DEFAULT_MIN),
                                    GetLong( _T("max"), wxSL_DEFAULT_MAX),
                                    GetPosition(), GetSize(),
                                    GetStyle(),
                                    wxDefaultValidator,
                                    GetName()
                                    );

    if( HasParam( _T("tickfreq") ))
    {
        control->SetTickFreq( GetLong( _T("tickfreq") ), 0 );
    }
    if( HasParam( _T("pagesize") ))
    {
        control->SetPageSize( GetLong( _T("pagesize") ) );
    }
    if( HasParam( _T("linesize") ))
    {
        control->SetLineSize( GetLong( _T("linesize") ));
    }
    if( HasParam( _T("thumb") ))
    {
        control->SetThumbLength( GetLong( _T("thumb") ));
    }
    if( HasParam( _T("tick") ))
    {
        control->SetTick( GetLong( _T("tick") ));
    }
    if( HasParam( _T("selmin") ) && HasParam( _T("selmax")) )
    {
        control->SetSelection( GetLong( _T("selmin") ), GetLong( _T("selmax")) );
    }

    SetupWindow(control);
    
    return control;
}



bool wxSliderXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, _T("wxSlider"));
}


#endif
