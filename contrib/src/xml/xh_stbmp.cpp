/////////////////////////////////////////////////////////////////////////////
// Name:        xh_stbmp.cpp
// Purpose:     XML resource for wxStaticBitmap
// Author:      Vaclav Slavik
// Created:     2000/04/22
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "xh_stbmp.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/xml/xh_stbmp.h"
#include "wx/statbmp.h"

wxStaticBitmapXmlHandler::wxStaticBitmapXmlHandler() 
: wxXmlResourceHandler() 
{
    AddWindowStyles();
}

wxObject *wxStaticBitmapXmlHandler::DoCreateResource()
{ 
    wxStaticBitmap *bmp = new wxStaticBitmap(m_ParentAsWindow,
                                    GetID(),
                                    GetBitmap(_T("bitmap"), GetSize()),
                                    GetPosition(), GetSize(),
                                    GetStyle(),
                                    GetName()
                                    );
    SetupWindow(bmp);
    
    return bmp;
}



bool wxStaticBitmapXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, _T("wxStaticBitmap"));
}


