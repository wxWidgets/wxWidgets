/////////////////////////////////////////////////////////////////////////////
// Name:        xh_treebk.h
// Purpose:     XML resource handler for wxTreebook
// Author:      Evgeniy Tarassov
// Created:     2005/09/28
// Copyright:   (c) 2005 TT-Solutions <vadim@tt-solutions.com>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_TREEBK_H_
#define _WX_XH_TREEBK_H_

#include "wx/xrc/xmlres.h"

#if wxUSE_TREEBOOK

#include "wx/treebook.h"

WX_DEFINE_ARRAY_INT(size_t, wxArrayTbkPageIndexes);

// ---------------------------------------------------------------------
// wxTreebookXmlHandler class
// ---------------------------------------------------------------------
// Resource xml structure have to be almost the "same" as for wxNotebook
// except the additional (size_t)depth parameter for treebookpage nodes
// which indicates the depth of the page in the tree.
// There is only one logical constraint on this parameter :
// it cannot be greater than the previous page depth plus one
class WXDLLIMPEXP_XRC wxTreebookXmlHandler : public wxXmlResourceHandler
{
DECLARE_DYNAMIC_CLASS(wxTreebookXmlHandler)
public:
    wxTreebookXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);

private:
    bool m_isInside;
    wxTreebook * m_tbk;
    wxArrayTbkPageIndexes m_treeContext;
};


// Example:
// -------
// Label
// \--First
// |  \--Second
// \--Third
//
//<resource>
//  ...
//  <object class="wxTreebook">
//    <object class="treebookpage">
//      <object class="wxWindow" />
//      <label>My first page</label>
//      <depth>0</depth>
//    </object>
//    <object class="treebookpage">
//      <object class="wxWindow" />
//      <label>First</label>
//      <depth>1</depth>
//    </object>
//    <object class="treebookpage">
//      <object class="wxWindow" />
//      <label>Second</label>
//      <depth>2</depth>
//    </object>
//    <object class="treebookpage">
//      <object class="wxWindow" />
//      <label>Third</label>
//      <depth>1</depth>
//    </object>
//  </object>
//  ...
//</resource>

#endif

#endif // _WX_XH_TREEBK_H_

