/////////////////////////////////////////////////////////////////////////////
// Name:        xh_bmp.cpp
// Purpose:     XML resource for wxBitmap and wxIcon
// Author:      Vaclav Slavik
// Created:     2000/09/09
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_bmp.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_bmp.h"
#include "wx/bitmap.h"


wxBitmapXmlHandler::wxBitmapXmlHandler() 
: wxXmlResourceHandler() 
{
}

wxObject *wxBitmapXmlHandler::DoCreateResource()
{ 
    return new wxBitmap(GetBitmap(_T("")));
}



bool wxBitmapXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, _T("wxBitmap"));
}


wxIconXmlHandler::wxIconXmlHandler() 
: wxXmlResourceHandler() 
{
}

wxObject *wxIconXmlHandler::DoCreateResource()
{ 
    return new wxIcon(GetIcon(_T("")));
}



bool wxIconXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, _T("wxIcon"));
}

