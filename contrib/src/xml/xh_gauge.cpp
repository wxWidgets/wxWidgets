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
}

wxObject *wxGaugeXmlHandler::DoCreateResource()
{ 
    wxGauge *control = new wxGauge(m_ParentAsWindow,
                                    GetID(),
                                    GetLong( _T("range"), wxGAUGE_DEFAULT_RANGE), 
                                    GetPosition(), GetSize(),
                                    GetStyle(),
                                    wxDefaultValidator,
                                    GetName()
                                    );

    if( HasParam( _T("value") ))
    {
        control->SetValue( GetLong( _T("value") ));
    }
    if( HasParam( _T("shadow") ))
    {
        control->SetShadowWidth( GetDimension( _T("shadow") ));
    }
    if( HasParam( _T("bezel") ))
    {
        control->SetBezelFace( GetDimension( _T("bezel") ));
    }

    SetupWindow(control);
    
    return control;
}



bool wxGaugeXmlHandler::CanHandle(wxXmlNode *node)
{
    return node->GetName() == _T("gauge");
}


#endif // wxUSE_GAUGE
