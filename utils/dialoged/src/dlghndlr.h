/////////////////////////////////////////////////////////////////////////////
// Name:        dlghndlr.h
// Purpose:     Dialog handler
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _DLGHNDLR_H_
#define _DLGHNDLR_H_

#define wxDRAG_MODE_NONE            0
#define wxDRAG_MODE_START_LEFT      1
#define wxDRAG_MODE_CONTINUE_LEFT   2
#define wxDRAG_MODE_START_RIGHT     3
#define wxDRAG_MODE_CONTINUE_RIGHT  4
#define wxDRAG_TYPE_NONE            0
#define wxDRAG_TYPE_ITEM            100
#define wxDRAG_TYPE_BOUNDING_BOX    200

#define wxKEY_SHIFT     1
#define wxKEY_CTRL      2

class wxResourceEditorDialogHandler: public wxEvtHandler
{
    DECLARE_CLASS(wxResourceEditorDialogHandler)
public:
    wxResourceManager *resourceManager;
    wxPanel *handlerDialog;
    wxItemResource *handlerResource;
    wxEvtHandler *handlerOldHandler;
    
    wxControl *dragItem;
    int dragMode;
    int dragType;
    int dragTolerance;
    bool checkTolerance;
    int firstDragX;
    int firstDragY;
    int oldDragX;
    int oldDragY;
    bool                      m_mouseCaptured;
    //  long                      m_treeItem;
    
    wxResourceEditorDialogHandler(wxPanel *dialog, wxItemResource *resource, wxEvtHandler *oldHandler,
        wxResourceManager *manager);
    ~wxResourceEditorDialogHandler(void);
    
    void OnPaint(wxPaintEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    
    virtual void OnItemEvent(wxControl *win, wxMouseEvent& event);
    virtual void OnLeftClick(int x, int y, int keys);
    virtual void OnRightClick(int x, int y, int keys);
    virtual void OnItemLeftClick(wxControl *item, int x, int y, int keys);
    virtual void OnItemRightClick(wxControl *item, int x, int y, int keys);
    virtual void OnItemSelect(wxControl *item, bool select);
    virtual void OnItemMove(
        wxControl *WXUNUSED(item), int WXUNUSED(x), int WXUNUSED(y) ) {};
    virtual void OnItemSize(
        wxControl *WXUNUSED(item), int WXUNUSED(w), int WXUNUSED(h) ) {};
    
    void AddChildHandlers(void);
    void PaintSelectionHandles(wxDC& dc);
    void ProcessItemEvent(wxControl *item, wxMouseEvent& event, int selectionHandle);

    virtual void DrawBoundingBox(wxDC& dc, int x, int y, int w, int h);
    virtual void OnDragBegin(int x, int y, int keys, wxDC& dc, int selectionHandle);
    virtual void OnDragContinue(bool paintIt, int x, int y, int keys, wxDC& dc, int selectionHandle);
    virtual void OnDragEnd(int x, int y, int keys, wxDC& dc, int selectionHandle);

    // Accessors
    /*
    inline long GetTreeItem() const { return m_treeItem; }
    inline void SetTreeItem(long item) { m_treeItem = item; }
    */
    
    DECLARE_EVENT_TABLE()
};

class wxResourceEditorControlHandler: public wxEvtHandler
{
    DECLARE_CLASS(wxResourceEditorControlHandler)
public:
    //  wxResourceManager *resourceManager;
    wxControl *handlerControl;
    //  wxItemResource *handlerResource;
    wxEvtHandler *handlerOldHandler;
    
    bool isSelected;
    int handleSize;   // selection handle size
    int handleMargin; // Distance between item edge and handle edge
    long                  m_treeItem;
    static int dragOffsetX;  // Distance between pointer at start of drag and
    static int dragOffsetY;  // top-left of item
    
    wxResourceEditorControlHandler(wxControl *control, wxEvtHandler *oldHandler);
    ~wxResourceEditorControlHandler(void);
    
    void OnMouseEvent(wxMouseEvent& event);
    
    // Manipulation and drawing of items in Edit Mode
    
    // Calculate position of the 8 handles
    virtual void CalcSelectionHandles(int *hx, int *hy);
    virtual void DrawSelectionHandles(wxDC& dc, bool erase = FALSE);
    virtual void DrawBoundingBox(wxDC& dc, int x, int y, int w, int h);
    virtual void SelectItem(bool select);
    virtual inline bool IsSelected(void) { return isSelected; }
    
    // Returns TRUE or FALSE
    virtual bool HitTest(int x, int y);
    
    // Returns 0 (no hit), 1 - 8 for which selection handle
    // (clockwise from top middle)
    virtual int SelectionHandleHitTest(int x, int y);
    
    // If selectionHandle is zero, not dragging the selection handle.
    virtual void OnDragBegin(int x, int y, int keys, wxDC& dc, int selectionHandle);
    virtual void OnDragContinue(bool paintIt, int x, int y, int keys, wxDC& dc, int selectionHandle);
    virtual void OnDragEnd(int x, int y, int keys, wxDC& dc, int selectionHandle);
    
    // These functions call panel functions
    // by default.
    virtual void OldOnMove(int x, int y);
    virtual void OldOnSize(int w, int h);
    virtual void OnLeftClick(int x, int y, int keys);
    virtual void OnRightClick(int x, int y, int keys);
    virtual void OnSelect(bool select);
    
    // Accessors
    /*
    inline long GetTreeItem() const { return m_treeItem; }
    inline void SetTreeItem(long item) { m_treeItem = item; }
    */
    
    DECLARE_EVENT_TABLE()
};

// This dialog, for testing dialogs, has to intercept commands before
// they go up the hierarchy and accidentally set off arbitrary
// Dialog Editor functionality
class ResourceEditorDialogTester: public wxDialog
{
public:
    ResourceEditorDialogTester() {}
    
    bool ProcessEvent(wxEvent& event);
};

#endif

