/////////////////////////////////////////////////////////////////////////////
// Name:        _sashwin.i
// Purpose:     SWIG interface defs for wxSashWindow and wxSashLayoutWindow
//
// Author:      Robin Dunn
//
// Created:     22-Dec-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING2(SashNameStr, wxT("sashWindow"));
MAKE_CONST_WXSTRING2(SashLayoutNameStr, wxT("layoutWindow"));

//---------------------------------------------------------------------------
%newgroup;


enum {
    wxSASH_DRAG_NONE,
    wxSASH_DRAG_DRAGGING,
    wxSASH_DRAG_LEFT_DOWN,

    wxSW_NOBORDER,
    wxSW_BORDER,
    wxSW_3DSASH,
    wxSW_3DBORDER,
    wxSW_3D,
};


enum wxSashEdgePosition {
    wxSASH_TOP = 0,
    wxSASH_RIGHT,
    wxSASH_BOTTOM,
    wxSASH_LEFT,
    wxSASH_NONE = 100
};



// wxSashWindow allows any of its edges to have a sash which can be dragged
// to resize the window. The actual content window will be created as a child
// of wxSashWindow.
MustHaveApp(wxSashWindow);

class wxSashWindow: public wxWindow
{
public:
    %pythonAppend wxSashWindow         "self._setOORInfo(self)"
    %pythonAppend wxSashWindow()       ""

    wxSashWindow(wxWindow* parent, wxWindowID id=-1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxCLIP_CHILDREN | wxSW_3D,
                 const wxString& name = wxPySashNameStr);
    %RenameCtor(PreSashWindow, wxSashWindow());

    bool Create(wxWindow* parent, wxWindowID id=-1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxCLIP_CHILDREN | wxSW_3D,
                 const wxString& name = wxPySashNameStr);


    // Set whether there's a sash in this position
    void SetSashVisible(wxSashEdgePosition edge, bool sash);

    // Get whether there's a sash in this position
    bool GetSashVisible(wxSashEdgePosition edge) const;

//     // Set whether there's a border in this position
//     void SetSashBorder(wxSashEdgePosition edge, bool border);

//     // Get whether there's a border in this position
//     bool HasBorder(wxSashEdgePosition edge) const;

    // Get border size
    int GetEdgeMargin(wxSashEdgePosition edge) const;

    // Sets the default sash border size
    void SetDefaultBorderSize(int width);

    // Gets the default sash border size
    int GetDefaultBorderSize() const;

    // Sets the addition border size between child and sash window
    void SetExtraBorderSize(int width);

    // Gets the addition border size between child and sash window
    int GetExtraBorderSize() const;

    virtual void SetMinimumSizeX(int min);
    virtual void SetMinimumSizeY(int min);
    virtual int GetMinimumSizeX() const;
    virtual int GetMinimumSizeY() const;

    virtual void SetMaximumSizeX(int max);
    virtual void SetMaximumSizeY(int max);
    virtual int GetMaximumSizeX() const;
    virtual int GetMaximumSizeY() const;

    // Tests for x, y over sash
    wxSashEdgePosition SashHitTest(int x, int y, int tolerance = 2);

    // Resizes subwindows
    void SizeWindows();
    
    %property(DefaultBorderSize, GetDefaultBorderSize, SetDefaultBorderSize, doc="See `GetDefaultBorderSize` and `SetDefaultBorderSize`");
    %property(ExtraBorderSize, GetExtraBorderSize, SetExtraBorderSize, doc="See `GetExtraBorderSize` and `SetExtraBorderSize`");
    %property(MaximumSizeX, GetMaximumSizeX, SetMaximumSizeX, doc="See `GetMaximumSizeX` and `SetMaximumSizeX`");
    %property(MaximumSizeY, GetMaximumSizeY, SetMaximumSizeY, doc="See `GetMaximumSizeY` and `SetMaximumSizeY`");
    %property(MinimumSizeX, GetMinimumSizeX, SetMinimumSizeX, doc="See `GetMinimumSizeX` and `SetMinimumSizeX`");
    %property(MinimumSizeY, GetMinimumSizeY, SetMinimumSizeY, doc="See `GetMinimumSizeY` and `SetMinimumSizeY`");
};



enum wxSashDragStatus
{
    wxSASH_STATUS_OK,
    wxSASH_STATUS_OUT_OF_RANGE
};

class wxSashEvent: public wxCommandEvent
{
public:
    wxSashEvent(int id = 0, wxSashEdgePosition edge = wxSASH_NONE);

    void SetEdge(wxSashEdgePosition edge);
    wxSashEdgePosition GetEdge() const;

    //// The rectangle formed by the drag operation
    void SetDragRect(const wxRect& rect);
    wxRect GetDragRect() const;

    //// Whether the drag caused the rectangle to be reversed (e.g.
    //// dragging the top below the bottom)
    void SetDragStatus(wxSashDragStatus status);
    wxSashDragStatus GetDragStatus() const;

    %property(DragRect, GetDragRect, SetDragRect, doc="See `GetDragRect` and `SetDragRect`");
    %property(DragStatus, GetDragStatus, SetDragStatus, doc="See `GetDragStatus` and `SetDragStatus`");
    %property(Edge, GetEdge, SetEdge, doc="See `GetEdge` and `SetEdge`");
};



%constant wxEventType wxEVT_SASH_DRAGGED;

%pythoncode {
    EVT_SASH_DRAGGED = wx.PyEventBinder( wxEVT_SASH_DRAGGED, 1 )
    EVT_SASH_DRAGGED_RANGE = wx.PyEventBinder( wxEVT_SASH_DRAGGED, 2 )
};




//---------------------------------------------------------------------------
%newgroup;



enum wxLayoutOrientation
{
    wxLAYOUT_HORIZONTAL,
    wxLAYOUT_VERTICAL
};

enum wxLayoutAlignment
{
    wxLAYOUT_NONE,
    wxLAYOUT_TOP,
    wxLAYOUT_LEFT,
    wxLAYOUT_RIGHT,
    wxLAYOUT_BOTTOM
};

enum {
    wxLAYOUT_LENGTH_Y,
    wxLAYOUT_LENGTH_X,
    wxLAYOUT_MRU_LENGTH,
    wxLAYOUT_QUERY,
};


%constant wxEventType wxEVT_QUERY_LAYOUT_INFO;
%constant wxEventType wxEVT_CALCULATE_LAYOUT;


// This event is used to get information about window alignment,
// orientation and size.
class wxQueryLayoutInfoEvent: public wxEvent
{
public:
    wxQueryLayoutInfoEvent(wxWindowID id = 0);

    // Read by the app
    void SetRequestedLength(int length);
    int GetRequestedLength() const;

    void SetFlags(int flags);
    int GetFlags() const;

    // Set by the app
    void SetSize(const wxSize& size);
    wxSize GetSize() const;

    void SetOrientation(wxLayoutOrientation orient);
    wxLayoutOrientation GetOrientation() const;

    void SetAlignment(wxLayoutAlignment align);
    wxLayoutAlignment GetAlignment() const;

    %property(Alignment, GetAlignment, SetAlignment, doc="See `GetAlignment` and `SetAlignment`");
    %property(Flags, GetFlags, SetFlags, doc="See `GetFlags` and `SetFlags`");
    %property(Orientation, GetOrientation, SetOrientation, doc="See `GetOrientation` and `SetOrientation`");
    %property(RequestedLength, GetRequestedLength, SetRequestedLength, doc="See `GetRequestedLength` and `SetRequestedLength`");
    %property(Size, GetSize, SetSize, doc="See `GetSize` and `SetSize`");
};


// This event is used to take a bite out of the available client area.
class wxCalculateLayoutEvent: public wxEvent
{
public:
    wxCalculateLayoutEvent(wxWindowID id = 0);

    // Read by the app
    void SetFlags(int flags);
    int GetFlags() const;

    // Set by the app
    void SetRect(const wxRect& rect);
    wxRect GetRect() const;

    %property(Flags, GetFlags, SetFlags, doc="See `GetFlags` and `SetFlags`");
    %property(Rect, GetRect, SetRect, doc="See `GetRect` and `SetRect`");
};


%pythoncode {
    EVT_QUERY_LAYOUT_INFO = wx.PyEventBinder( wxEVT_QUERY_LAYOUT_INFO )
    EVT_CALCULATE_LAYOUT = wx.PyEventBinder( wxEVT_CALCULATE_LAYOUT )
};



// This is window that can remember alignment/orientation, does its own layout,
// and can provide sashes too. Useful for implementing docked windows with sashes in
// an IDE-style interface.
MustHaveApp(wxSashLayoutWindow);

class wxSashLayoutWindow: public wxSashWindow
{
public:
    %pythonAppend wxSashLayoutWindow         "self._setOORInfo(self)"
    %pythonAppend wxSashLayoutWindow()       ""
    
    wxSashLayoutWindow(wxWindow* parent, wxWindowID id=-1,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxCLIP_CHILDREN | wxSW_3D,
                       const wxString& name = wxPySashLayoutNameStr);
    %RenameCtor(PreSashLayoutWindow, wxSashLayoutWindow());

    bool Create(wxWindow* parent, wxWindowID id=-1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCLIP_CHILDREN | wxSW_3D,
                const wxString& name = wxPySashLayoutNameStr);

    wxLayoutAlignment GetAlignment();
    wxLayoutOrientation GetOrientation();
    void SetAlignment(wxLayoutAlignment alignment);
    void SetDefaultSize(const wxSize& size);
    void SetOrientation(wxLayoutOrientation orientation);
    
    %property(Alignment, GetAlignment, SetAlignment, doc="See `GetAlignment` and `SetAlignment`");
    %property(Orientation, GetOrientation, SetOrientation, doc="See `GetOrientation` and `SetOrientation`");
};



class wxLayoutAlgorithm : public wxObject {
public:
    wxLayoutAlgorithm();
    ~wxLayoutAlgorithm();

    bool LayoutMDIFrame(wxMDIParentFrame* frame, wxRect* rect = NULL);
    bool LayoutFrame(wxFrame* frame, wxWindow* mainWindow = NULL);
    bool LayoutWindow(wxWindow* parent, wxWindow* mainWindow = NULL);
};


//---------------------------------------------------------------------------
