/////////////////////////////////////////////////////////////////////////////
// Purpose:     XML resources editor
// Author:      Vaclav Slavik
// Created:     2000/05/05
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma interface "treedt.h"
#endif

#ifndef _TREEDT_H_
#define _TREEDT_H_


#include "wx/treectrl.h"

class WXDLLEXPORT wxXmlNode;

class XmlTreeData : public wxTreeItemData
{
    public:
        XmlTreeData(wxXmlNode *n) : Node(n) {}        
        wxXmlNode *Node;
};


#endif
