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
                                    GetLong( wxT("value"), wxSL_DEFAULT_VALUE), 
                                    GetLong( wxT("min"), wxSL_DEFAULT_MIN),
                                    GetLong( wxT("max"), wxSL_DEFAULT_MAX),
                                    GetPosition(), GetSize(),
                                    GetStyle(),
                                    wxDefaultValidator,
                                    GetName()
                                    );

    if( HasParam( wxT("tickfreq") ))
    {
        control->SetTickFreq( GetLong( wxT("tickfreq") ), 0 );
    }
    if( HasParam( wxT("pagesize") ))
    {
        control->SetPageSize( GetLong( wxT("pagesize") ) );
    }
    if( HasParam( wxT("linesize") ))
    {
        control->SetLineSize( GetLong( wxT("linesize") ));
    }
    if( HasParam( wxT("thumb") ))
    {
        control->SetThumbLength( GetLong( wxT("thumb") ));
    }
    if( HasParam( wxT("tick") ))
    {
        control->SetTick( GetLong( wxT("tick") ));
    }
    if( HasParam( wxT("selmin") ) && HasParam( wxT("selmax")) )
    {
        control->SetSelection( GetLong( wxT("selmin") ), GetLong( wxT("selmax")) );
    }

    SetupWindow(control);
    
    return control;
}



bool wxSliderXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxSlider"));
}


#endif
