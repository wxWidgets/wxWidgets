/////////////////////////////////////////////////////////////////////////////
// Name:        xmlbinz.cpp
// Purpose:     wxXmlIOHandlerBinZ
// Author:      Vaclav Slavik
// Created:     2000/07/24
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
// nothing, already in xml.cpp
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/datstrm.h"
#include "wx/log.h"
#include "wx/zstream.h"

#include "wx/xml/xmlio.h"

#if wxUSE_ZLIB



bool wxXmlIOHandlerBinZ::CanLoad(wxInputStream& stream)
{
    bool canread;
    canread = (ReadHeader(stream) == wxT("XMLBINZ"));
    stream.SeekI(-9, wxFromCurrent);
    return canread;
}



bool wxXmlIOHandlerBinZ::Save(wxOutputStream& stream, const wxXmlDocument& doc)
{
    WriteHeader(stream, "XMLBINZ");
    wxZlibOutputStream costr(stream, 9);
    return wxXmlIOHandlerBin::Save(costr, doc);
}



bool wxXmlIOHandlerBinZ::Load(wxInputStream& stream, wxXmlDocument& doc)
{
    ReadHeader(stream);
    wxZlibInputStream costr(stream);
    return wxXmlIOHandlerBin::Load(costr, doc);
}


#endif
