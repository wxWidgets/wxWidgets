/////////////////////////////////////////////////////////////////////////////
// Name:        xh_gauge.cpp
// Purpose:     XML resource for wxGauge
// Author:      Bob Mitchell
// Created:     2000/03/21
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Bob Mitchell and Verant Interactive
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_gauge.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_gauge.h"
#include "wx/gauge.h"

#if wxUSE_GAUGE

wxGaugeXmlHandler::wxGaugeXmlHandler() 
: wxXmlResourceHandler() 
{
    ADD_STYLE( wxGA_HORIZONTAL );
    ADD_STYLE( wxGA_VERTICAL );
    ADD_STYLE( wxGA_PROGRESSBAR );
    ADD_STYLE( wxGA_SMOOTH );   // windows only
    AddWindowStyles();
}

wxObject *wxGaugeXmlHandler::DoCreateResource()
{ 
    wxGauge *control = new wxGauge(m_ParentAsWindow,
                                    GetID(),
                                    GetLong( wxT("range"), wxGAUGE_DEFAULT_RANGE), 
                                    GetPosition(), GetSize(),
                                    GetStyle(),
                                    wxDefaultValidator,
                                    GetName()
                                    );

    if( HasParam( wxT("value") ))
    {
        control->SetValue( GetLong( wxT("value") ));
    }
    if( HasParam( wxT("shadow") ))
    {
        control->SetShadowWidth( GetDimension( wxT("shadow") ));
    }
    if( HasParam( wxT("bezel") ))
    {
        control->SetBezelFace( GetDimension( wxT("bezel") ));
    }

    SetupWindow(control);
    
    return control;
}



bool wxGaugeXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxGauge"));
}


#endif // wxUSE_GAUGE
