/////////////////////////////////////////////////////////////////////////////
// Name:        xmlio.h
// Purpose:     wxXmlIOHandler - XML I/O classes
// Author:      Vaclav Slavik
// Created:     2000/07/24
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XMLIO_H_
#define _WX_XMLIO_H_

#ifdef __GNUG__
#pragma interface "xmlio.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/xml/xml.h"


class WXDLLEXPORT wxXmlIOHandlerExpat : public wxXmlIOHandler
{
public:
    virtual wxXmlIOType GetType() { return wxXML_IO_EXPAT; }
    virtual bool CanLoad(wxInputStream& stream);
    virtual bool CanSave() { return FALSE; }

    virtual bool Load(wxInputStream& stream, wxXmlDocument& doc);
    virtual bool Save(wxOutputStream& stream, const wxXmlDocument& doc) { return FALSE; }
};


class WXDLLEXPORT wxXmlIOHandlerWriter : public wxXmlIOHandler
{
public:
    virtual wxXmlIOType GetType() { return wxXML_IO_TEXT_OUTPUT; }
    virtual bool CanLoad(wxInputStream& stream) { return FALSE; }
    virtual bool CanSave() { return TRUE; }

    virtual bool Load(wxInputStream& stream, wxXmlDocument& doc) { return FALSE; }
    virtual bool Save(wxOutputStream& stream, const wxXmlDocument& doc);
};


class WXDLLEXPORT wxXmlIOHandlerBin : public wxXmlIOHandler
{
public:
    wxXmlIOHandlerBin() {}

    virtual wxXmlIOType GetType() { return wxXML_IO_BIN; }
    virtual bool CanLoad(wxInputStream& stream);
    virtual bool CanSave() { return TRUE; }

    virtual bool Load(wxInputStream& stream, wxXmlDocument& doc);
    virtual bool Save(wxOutputStream& stream, const wxXmlDocument& doc);

protected: 
    wxString ReadHeader(wxInputStream& stream);
    void WriteHeader(wxOutputStream& stream, const wxString& header);
};



#if wxUSE_ZLIB

class WXDLLEXPORT wxXmlIOHandlerBinZ : public wxXmlIOHandlerBin
{
public:
    wxXmlIOHandlerBinZ() {}

    virtual wxXmlIOType GetType() { return wxXML_IO_BINZ; }
    virtual bool CanLoad(wxInputStream& stream);

    virtual bool Load(wxInputStream& stream, wxXmlDocument& doc);
    virtual bool Save(wxOutputStream& stream, const wxXmlDocument& doc);
};

#endif


#endif // _WX_XMLIO_H_
