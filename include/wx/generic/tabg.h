/////////////////////////////////////////////////////////////////////////////
// Name:        tabg.h
// Purpose:     Generic tabbed dialogs
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __TABGH_G__
#define __TABGH_G__

#ifdef __GNUG__
#pragma interface "tabg.h"
#endif

#define WXTAB_VERSION   1.1

#include "wx/hash.h"
#include "wx/string.h"

class wxTabView;

/*
 * A wxTabControl is the internal and visual representation
 * of the tab.
 */
 
class wxTabControl: public wxObject
{
DECLARE_DYNAMIC_CLASS(wxTabControl)
public:
    wxTabControl(wxTabView *v = (wxTabView *) NULL);
    ~wxTabControl(void);
    
    virtual void OnDraw(wxDC& dc, bool lastInRow);
    inline void SetLabel(const wxString& str) { m_controlLabel = str; }
    inline wxString GetLabel(void) const { return m_controlLabel; }

    inline void SetFont(wxFont *f) { m_labelFont = f; }
    inline wxFont *GetFont(void) const { return m_labelFont; }

    inline void SetSelected(bool sel) { m_isSelected = sel; }
    inline bool IsSelected(void) const { return m_isSelected; }

    inline void SetPosition(int x, int y) { m_offsetX = x; m_offsetY = y; }
    inline void SetSize(int x, int y) { m_width = x; m_height = y; }
    
    inline void SetRowPosition(int r) { m_rowPosition = r; }
    inline int GetRowPosition() const { return m_rowPosition; }
    inline void SetColPosition(int c) { m_colPosition = c; }
    inline int GetColPosition() const { return m_colPosition; }
    
    inline int GetX(void) const { return m_offsetX; }
    inline int GetY(void) const { return m_offsetY; }
    inline int GetWidth(void) const { return m_width; }
    inline int GetHeight(void) const { return m_height; }
    
    inline int GetId(void) const { return m_id; }
    inline void SetId(int i) { m_id = i; }
    
    virtual bool HitTest(int x, int y) const ;

protected:
    wxTabView*      m_view;
    wxString        m_controlLabel;
    bool            m_isSelected;
    wxFont*         m_labelFont;
    int             m_offsetX; // Offsets from top-left of tab view area (the area below the tabs)
    int             m_offsetY;
    int             m_width;
    int             m_height;
    int             m_id;
    int             m_rowPosition; // Position in row from 0
    int             m_colPosition; // Position in col from 0
};
 
/*
 * Each wxTabLayer is a list of tabs. E.g. there
 * are 3 layers in the MS Word Options dialog.
 */
 
class wxTabLayer: public wxList
{
  DECLARE_DYNAMIC_CLASS(wxTabLayer)
  public:
    wxTabLayer(void)
    {
    }
};

/*
 * The wxTabView controls and draws the tabbed object
 */

#define wxTAB_STYLE_DRAW_BOX         1   // Draws 3D boxes round tab layers
#define wxTAB_STYLE_COLOUR_INTERIOR  2   // Colours interior of tabs, otherwise draws outline

class wxTabView: public wxObject
{
DECLARE_DYNAMIC_CLASS(wxTabView)
public:
  wxTabView(long style = wxTAB_STYLE_DRAW_BOX | wxTAB_STYLE_COLOUR_INTERIOR);
  ~wxTabView();

  inline int GetNumberOfLayers() const { return m_layers.Number(); }
  inline wxList& GetLayers() { return m_layers; }

  inline void SetWindow(wxWindow* wnd) { m_window = wnd; }
  inline wxWindow* GetWindow(void) const { return m_window; }
  
  // Automatically positions tabs
  wxTabControl *AddTab(int id, const wxString& label, wxTabControl *existingTab = (wxTabControl *) NULL);
  
  void ClearTabs(bool deleteTabs = TRUE);

  // Layout tabs (optional, e.g. if resizing window)
  void Layout(void);

  // Draw all tabs
  virtual void Draw(wxDC& dc);
  
  // Process mouse event, return FALSE if we didn't process it
  virtual bool OnEvent(wxMouseEvent& event);

  // Called when a tab is activated
  virtual void OnTabActivate(int activateId, int deactivateId);
  // Allows vetoing
  virtual bool OnTabPreActivate(int WXUNUSED(activateId), int WXUNUSED(deactivateId) ) { return TRUE; };

  // Allows use of application-supplied wxTabControl classes.
  virtual wxTabControl *OnCreateTabControl(void) { return new wxTabControl(this); }

  void SetHighlightColour(const wxColour& col);
  void SetShadowColour(const wxColour& col);
  void SetBackgroundColour(const wxColour& col);
  inline void SetTextColour(const wxColour& col) { m_textColour = col; }
  
  inline wxColour GetHighlightColour(void) const { return m_highlightColour; }
  inline wxColour GetShadowColour(void) const { return m_shadowColour; }
  inline wxColour GetBackgroundColour(void) const { return m_backgroundColour; }
  inline wxColour GetTextColour(void) const { return m_textColour; }
  inline wxPen *GetHighlightPen(void) const { return m_highlightPen; }
  inline wxPen *GetShadowPen(void) const { return m_shadowPen; }
  inline wxPen *GetBackgroundPen(void) const { return m_backgroundPen; }
  inline wxBrush *GetBackgroundBrush(void) const { return m_backgroundBrush; }
  
  inline void SetViewRect(const wxRectangle& rect) { m_tabViewRect = rect; }
  inline wxRect GetViewRect(void) const { return m_tabViewRect; }
  
  // Calculate tab width to fit to view, and optionally adjust the view
  // to fit the tabs exactly.
  int CalculateTabWidth(int noTabs, bool adjustView = FALSE);

  inline void SetTabStyle(long style) { m_tabStyle = style; }
  inline long GetTabStyle(void) const { return m_tabStyle; }
  
  inline void SetTabSize(int w, int h) { m_tabWidth = w; m_tabHeight = h; }
  inline int GetTabWidth(void) const { return m_tabWidth; }
  inline int GetTabHeight(void) const { return m_tabHeight; }
  inline void SetTabSelectionHeight(int h) { m_tabSelectionHeight = h; }
  inline int GetTabSelectionHeight(void) const { return m_tabSelectionHeight; }
  
  inline int GetTopMargin(void) const { return m_topMargin; }
  inline void SetTopMargin(int margin) { m_topMargin = margin; }
  
  void SetTabSelection(int sel, bool activateTool = TRUE);
  inline int GetTabSelection() const { return m_tabSelection; }
  
  // Find tab control for id
  wxTabControl *FindTabControlForId(int id) const ;

  // Find tab control for layer, position (starting from zero)
  wxTabControl *FindTabControlForPosition(int layer, int position) const ;
  
  inline int GetHorizontalTabOffset() const { return m_tabHorizontalOffset; }
  inline int GetHorizontalTabSpacing() const { return m_tabHorizontalSpacing; }
  inline void SetHorizontalTabOffset(int sp) { m_tabHorizontalOffset = sp; }
  inline void SetHorizontalTabSpacing(int sp) { m_tabHorizontalSpacing = sp; }
  
  inline void SetVerticalTabTextSpacing(int s) { m_tabVerticalTextSpacing = s; }
  inline int GetVerticalTabTextSpacing() const { return m_tabVerticalTextSpacing; }
  
  inline wxFont *GetTabFont() const { return m_tabFont; }
  inline void SetTabFont(wxFont *f) { m_tabFont = f; }

  inline wxFont *GetSelectedTabFont() const { return m_tabSelectedFont; }
  inline void SetSelectedTabFont(wxFont *f) { m_tabSelectedFont = f; }
  // Find the node and the column at which this control is positioned.
  wxNode *FindTabNodeAndColumn(wxTabControl *control, int *col) const ;
  
  // Do the necessary to change to this tab
  virtual bool ChangeTab(wxTabControl *control);

  // Move the selected tab to the bottom layer, if necessary,
  // without calling app activation code
  bool MoveSelectionTab(wxTabControl *control);

  inline int GetNumberOfTabs() const { return m_noTabs; }

protected:
   // List of layers, from front to back.
   wxList           m_layers;
   
   // Selected tab
   int              m_tabSelection;

   // Usual tab height
   int              m_tabHeight;

   // The height of the selected tab
   int              m_tabSelectionHeight;

   // Usual tab width
   int              m_tabWidth;
   
   // Space between tabs
   int              m_tabHorizontalSpacing;
   
   // Space between top of normal tab and text
   int              m_tabVerticalTextSpacing;
   
   // Horizontal offset of each tab row above the first
   int              m_tabHorizontalOffset;

   // The distance between the bottom of the first tab row
   // and the top of the client area (i.e. the margin)
   int              m_topMargin;

   // The position and size of the view above which the tabs are placed.
   // I.e., the internal client area of the sheet.
   wxRect           m_tabViewRect;
   
   // Bitlist of styles
   long             m_tabStyle;

   // Colours
   wxColour         m_highlightColour;
   wxColour         m_shadowColour;
   wxColour         m_backgroundColour;
   wxColour         m_textColour;
   
   // Pen and brush cache
   wxPen*           m_highlightPen;
   wxPen*           m_shadowPen;
   wxPen*           m_backgroundPen;
   wxBrush*         m_backgroundBrush;
   
   wxFont*          m_tabFont;
   wxFont*          m_tabSelectedFont;
   
   int              m_noTabs;

   wxWindow*        m_window;
};

/*
 * A dialog box class that is tab-friendly
 */
 
class wxTabbedDialog: public wxDialog
{
DECLARE_DYNAMIC_CLASS(wxTabbedDialog)
 
public:

   wxTabbedDialog(wxWindow *parent, wxWindowID id, const wxString& title,
    const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize,
     long windowStyle = wxDEFAULT_DIALOG_STYLE, const wxString& name = wxDialogNameStr);
   ~wxTabbedDialog(void);
 
   inline wxTabView *GetTabView() const { return m_tabView; }
   inline void SetTabView(wxTabView *v) { m_tabView = v; }

   void OnCloseWindow(wxCloseEvent& event);
   void OnMouseEvent(wxMouseEvent& event);
   void OnPaint(wxPaintEvent& event);

protected:
   wxTabView*   m_tabView;
   
DECLARE_EVENT_TABLE()
};

/*
 * A panel class that is tab-friendly
 */

class wxTabbedPanel: public wxPanel
{
DECLARE_DYNAMIC_CLASS(wxTabbedPanel)
 
public:

   wxTabbedPanel(wxWindow *parent, wxWindowID id,
    const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize,
    long windowStyle = 0, const wxString& name = wxPanelNameStr);
   ~wxTabbedPanel(void);
 
   inline wxTabView *GetTabView() const { return m_tabView; }
   inline void SetTabView(wxTabView *v) { m_tabView = v; }
  
   void OnMouseEvent(wxMouseEvent& event);
   void OnPaint(wxPaintEvent& event);

protected:
   wxTabView*   m_tabView;
   
DECLARE_EVENT_TABLE()
};

class wxPanelTabView: public wxTabView
{
DECLARE_DYNAMIC_CLASS(wxPanelTabView)
public:
  wxPanelTabView(wxPanel *pan, long style = wxTAB_STYLE_DRAW_BOX | wxTAB_STYLE_COLOUR_INTERIOR);
  ~wxPanelTabView(void);

  // Called when a tab is activated
  virtual void OnTabActivate(int activateId, int deactivateId);

  // Specific to this class
   void AddTabWindow(int id, wxWindow *window);
   wxWindow *GetTabWindow(int id) const ;
   void ClearWindows(bool deleteWindows = TRUE);
   inline wxWindow *GetCurrentWindow() const { return m_currentWindow; }
   
   void ShowWindowForTab(int id);

protected:
   // List of panels, one for each tab. Indexed
   // by tab ID.
   wxList           m_tabWindows;
   wxWindow*        m_currentWindow;
   wxPanel*         m_panel;
};

#endif

