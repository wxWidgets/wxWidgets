///////////////////////////////////////////////////////////////////////////////
// Name:        treelay.h
// Purpose:     wxTreeLayout class
// Author:      Julian Smart
// Modified by:
// Created:     7/4/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TREELAY_H_
#define _WX_TREELAY_H_

#ifndef WX_PRECOMP
#include "wx/object.h"
class wxList;
class wxDC;
class wxMouseEvent;
#endif

#include "wx/deprecated/setup.h"
#include "wx/string.h"

#if wxUSE_TREELAYOUT

class WXDLLIMPEXP_DEPRECATED wxTreeLayout: public wxObject
{
public:
    wxTreeLayout();
    virtual ~wxTreeLayout() { }

    // Redefine these
    virtual void GetChildren(long id, wxList& list) = 0;
    virtual long GetNextNode(long id) = 0;
    virtual long GetNodeParent(long id) = 0;
    virtual long GetNodeX(long id) = 0;
    virtual long GetNodeY(long id) = 0;
    virtual void SetNodeX(long id, long x) = 0;
    virtual void SetNodeY(long id, long y) = 0;
    virtual void ActivateNode(long id, bool active) = 0;
    virtual bool NodeActive(long id) = 0;

    // Optional redefinition
    void Initialize(void);
    inline virtual void SetNodeName(long WXUNUSED(id), const wxString& WXUNUSED(name)) {}
    inline virtual wxString GetNodeName(long WXUNUSED(id)) { return wxString(wxT("")); }
    virtual void GetNodeSize(long id, long *x, long *y, wxDC& dc);
    virtual void Draw(wxDC& dc);
    virtual void DrawNodes(wxDC& dc);
    virtual void DrawBranches(wxDC& dc);
    virtual void DrawNode(long id, wxDC& dc);
    virtual void DrawBranch(long from, long to, wxDC& dc);

    // Don't redefine
    virtual void DoLayout(wxDC& dc, long topNode = -1);

    // Accessors -- don't redefine
    inline void SetTopNode(long id) { m_parentNode = id; }
    inline long GetTopNode(void) const { return m_parentNode; }
    inline void SetSpacing(long x, long y) { m_xSpacing = x; m_ySpacing = y; }
    inline long GetXSpacing(void) const { return m_xSpacing; }
    inline long GetYSpacing(void) const { return m_ySpacing; }
    inline void SetMargins(long x, long y) { m_leftMargin = x; m_topMargin = y; }
    inline long GetTopMargin(void) const { return m_topMargin; }
    inline long GetLeftMargin(void) const { return m_leftMargin; }

    inline bool GetOrientation(void) const { return m_orientation; }
    inline void SetOrientation(bool orient) { m_orientation = orient; }

private:
    void CalcLayout(long node_id, int level, wxDC& dc);

protected:
    long          m_parentNode;
    long          m_lastY;
    long          m_lastX;
    long          m_xSpacing;
    long          m_ySpacing;
    long          m_topMargin;
    long          m_leftMargin;
    bool          m_orientation; // true for top-to-bottom, false for left-to-right

private:
    DECLARE_ABSTRACT_CLASS(wxTreeLayout)
};

class WXDLLIMPEXP_DEPRECATED wxStoredNode
{
public:
    wxString      m_name;
    long          m_x, m_y;
    long          m_parentId;
    bool          m_active;
    long          m_clientData;
};

/*
 * A version of wxTreeLayout with storage for nodes
 */

class WXDLLIMPEXP_DEPRECATED wxTreeLayoutStored: public wxTreeLayout
{
public:
    wxTreeLayoutStored(int noNodes = 200);
    virtual ~wxTreeLayoutStored(void);
    void Initialize(int n);

    wxString HitTest(wxMouseEvent& event, wxDC& dc);
    wxStoredNode* GetNode(long id) const;
    inline int GetNumNodes() const { return m_maxNodes; };
    inline int GetNodeCount() const { return m_num; };

    virtual void GetChildren(long id, wxList& list);
    virtual long GetNextNode(long id);
    virtual long GetNodeParent(long id);
    virtual long GetNodeX(long id);
    virtual long GetNodeY(long id);
    virtual void SetNodeX(long id, long x);
    virtual void SetNodeY(long id, long y);
    virtual void SetNodeName(long id, const wxString& name);
    virtual wxString GetNodeName(long id);
    virtual void ActivateNode(long id, bool active);
    virtual bool NodeActive(long id);
    virtual void SetClientData(long id, long clientData);
    virtual long GetClientData(long id) const;

    virtual long AddChild(const wxString& name, const wxString& parent = wxT(""));
    virtual long AddChild(const wxString& name, long parent);
    virtual long NameToId(const wxString& name);

    // Data members
private:
    wxStoredNode*     m_nodes;
    int               m_num;
    int               m_maxNodes;

private:
    DECLARE_DYNAMIC_CLASS(wxTreeLayoutStored)
    DECLARE_NO_COPY_CLASS(wxTreeLayoutStored)
};

// For backward compatibility
#define wxStoredTree wxTreeLayoutStored

#endif
    // wxUSE_TREELAYOUT

#endif
 // _WX_TREELAY_H_

