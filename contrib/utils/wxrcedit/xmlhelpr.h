/////////////////////////////////////////////////////////////////////////////
// Purpose:     XML resources editor
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma interface "xmlhelpr.h"
#endif

#ifndef _XMLHELPR_H_
#define _XMLHELPR_H_

// some helper functions:

void XmlWriteValue(wxXmlNode *parent, const wxString& name, const wxString& value);
wxString XmlReadValue(wxXmlNode *parent, const wxString& name);
wxXmlNode *XmlFindNode(wxXmlNode *parent, const wxString& name);
wxString XmlGetClass(wxXmlNode *parent);
void XmlSetClass(wxXmlNode *parent, const wxString& classname);

#endif 
