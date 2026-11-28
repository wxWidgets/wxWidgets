///////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xmlrespriv.h
// Purpose:     Private XRC resource-creation transaction state
// Author:      wxWidgets development team
// Created:     2026-08-02
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XRC_XMLRESPRIV_H_
#define _WX_XRC_XMLRESPRIV_H_

class wxXmlResourceHandler;

// Return true only while handler is creating the current m_instance supplied
// by an XRC subclass factory. The state is scoped to the innermost nested XRC
// transaction and does not change the public handler object's layout or ABI.
bool wxXRCIsCurrentInstanceFactoryOwned(
    const wxXmlResourceHandler* handler);

#endif // _WX_XRC_XMLRESPRIV_H_
