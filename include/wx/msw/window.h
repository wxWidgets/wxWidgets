/////////////////////////////////////////////////////////////////////////////
// Name:        window.h
// Purpose:     wxWindow class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINDOW_H_
#define _WX_WINDOW_H_

#ifdef __GNUG__
#pragma interface "window.h"
#endif

#include "wx/gdicmn.h"
#include "wx/icon.h"
#include "wx/cursor.h"
#include "wx/pen.h"
#include "wx/font.h"
#include "wx/validate.h"
#include "wx/event.h"
#include "wx/string.h"
#include "wx/list.h"
#include "wx/region.h"
#include "wx/msw/accel.h"

#define wxKEY_SHIFT     1
#define wxKEY_CTRL      2

/*
 * Base class for frame, panel, canvas, panel items, dialog box.
 *
 */

/*
 * Event handler: windows have themselves as their event handlers
 * by default, but their event handlers could be set to another
 * object entirely. This separation can reduce the amount of
 * derivation required, and allow alteration of a window's functionality
 * (e.g. by a resource editor that temporarily switches event handlers).
 */

class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxEvent;
class WXDLLEXPORT wxCommandEvent;
class WXDLLEXPORT wxKeyEvent;
class WXDLLEXPORT wxControl;
class WXDLLEXPORT wxCursor;
class WXDLLEXPORT wxColourMap;
class WXDLLEXPORT wxFont;
class WXDLLEXPORT wxMenu;
class WXDLLEXPORT wxRect;
class WXDLLEXPORT wxBitmap;
class WXDLLEXPORT wxSizer;
class WXDLLEXPORT wxList;
class WXDLLEXPORT wxLayoutConstraints;
class WXDLLEXPORT wxMouseEvent;
class WXDLLEXPORT wxButton;
class WXDLLEXPORT wxColour;
class WXDLLEXPORT wxBrush;
class WXDLLEXPORT wxPen;
class WXDLLEXPORT wxIcon;
class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxValidator;

#if wxUSE_DRAG_AND_DROP
class WXDLLEXPORT wxDropTarget;
#endif

#if wxUSE_WX_RESOURCES
class WXDLLEXPORT wxResourceTable;
class WXDLLEXPORT wxItemResource;
#endif

WXDLLEXPORT_DATA(extern const char*) wxPanelNameStr;

WXDLLEXPORT_DATA(extern const wxSize) wxDefaultSize;
WXDLLEXPORT_DATA(extern const wxPoint) wxDefaultPosition;

class WXDLLEXPORT wxWindow: public wxEvtHandler
{
  DECLARE_ABSTRACT_CLASS(wxWindow)

  friend class wxDC;
  friend class wxPaintDC;

public:
  wxWindow(void);
  inline wxWindow(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxPanelNameStr)
  {
  	  m_children = new wxList;
      Create(parent, id, pos, size, style, name);
  }

  virtual ~wxWindow(void);

  bool Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxPanelNameStr);

  // Fit the window around the items
  virtual void Fit(void);

  // Show or hide the window
  virtual bool Show(bool show);

  // Is the window shown?
  virtual bool IsShown(void) const;

  // Raise the window to the top of the Z order
  virtual void Raise(void);

  // Lower the window to the bottom of the Z order
  virtual void Lower(void);

  // Is the window enabled?
  virtual bool IsEnabled(void) const;

  // For compatibility
  inline bool Enabled(void) const { return IsEnabled(); }

  // Dialog support: override these and call
  // base class members to add functionality
  // that can't be done using validators.

  // Transfer values to controls. If returns FALSE,
  // it's an application error (pops up a dialog)
  virtual bool TransferDataToWindow(void);

  // Transfer values from controls. If returns FALSE,
  // transfer failed: don't quit
  virtual bool TransferDataFromWindow(void);

  // Validate controls. If returns FALSE,
  // validation failed: don't quit
  virtual bool Validate(void);

  // Return code for dialogs
  inline void SetReturnCode(int retCode);
  inline int GetReturnCode(void);

  // Set the cursor
  virtual void SetCursor(const wxCursor& cursor);
  inline virtual wxCursor *GetCursor(void) const { return (wxCursor *)& m_windowCursor; };

  // Get the window with the focus
  static wxWindow *FindFocus(void);

  // Get character size
  virtual int GetCharHeight(void) const;
  virtual int GetCharWidth(void) const;

  // Get overall window size
  virtual void GetSize(int *width, int *height) const;

  // Get window position, relative to parent (or screen if no parent)
  virtual void GetPosition(int *x, int *y) const;

  // Get client (application-useable) size
  virtual void GetClientSize(int *width, int *height) const;

  // Set overall size and position
  virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  inline virtual void SetSize(int width, int height) { SetSize(-1, -1, width, height, wxSIZE_USE_EXISTING); }
  inline virtual void Move(int x, int y) { SetSize(x, y, -1, -1, wxSIZE_USE_EXISTING); }

  // Set client size
  virtual void SetClientSize(int width, int size);

  // Convert client to screen coordinates
  virtual void ClientToScreen(int *x, int *y) const;

  // Convert screen to client coordinates
  virtual void ScreenToClient(int *x, int *y) const;

  // Set the focus to this window
  virtual void SetFocus(void);

  // Capture/release mouse
  virtual void CaptureMouse(void);
  virtual void ReleaseMouse(void);

  // Enable or disable the window
  virtual void Enable(bool enable);

#if wxUSE_DRAG_AND_DROP
  // Associate a drop target with this window (if the window already had a drop
  // target, it's deleted!) and return the current drop target (may be NULL).
  void          SetDropTarget(wxDropTarget *pDropTarget);
  wxDropTarget *GetDropTarget() const { return m_pDropTarget; }
#endif

  // Accept files for dragging
  virtual void DragAcceptFiles(bool accept);

  // Update region access
  virtual wxRegion GetUpdateRegion() const;
  virtual bool IsExposed(int x, int y, int w, int h) const;
  virtual bool IsExposed(const wxPoint& pt) const;
  virtual bool IsExposed(const wxRect& rect) const;

  // Set/get the window title
  virtual inline void SetTitle(const wxString& WXUNUSED(title)) {};
  inline virtual wxString GetTitle(void) const { return wxString(""); };
  // Most windows have the concept of a label; for frames, this is the
  // title; for items, this is the label or button text.
  inline virtual wxString GetLabel(void) const { return GetTitle(); }

  // Set/get the window name (used for resource setting in X)
  inline virtual wxString GetName(void) const;
  inline virtual void SetName(const wxString& name);

  // Centre the window
  virtual void Centre(int direction) ;
  inline void Center(int direction = wxHORIZONTAL) { Centre(direction); }

  // Popup a menu
  virtual bool PopupMenu(wxMenu *menu, int x, int y);

  // Send the window a refresh event
  virtual void Refresh(bool eraseBack = TRUE, const wxRect *rect = NULL);

#if WXWIN_COMPATIBILITY
  // Set/get scroll attributes
  virtual void SetScrollRange(int orient, int range, bool refresh = TRUE);
  virtual void SetScrollPage(int orient, int page, bool refresh = TRUE);
  virtual int OldGetScrollRange(int orient) const;
  virtual int GetScrollPage(int orient) const;
#endif

  // New functions that will replace the above.
  virtual void SetScrollbar(int orient, int pos, int thumbVisible,
    int range, bool refresh = TRUE);

  virtual void SetScrollPos(int orient, int pos, bool refresh = TRUE);
  virtual int GetScrollPos(int orient) const;
  virtual int GetScrollRange(int orient) const;
  virtual int GetScrollThumb(int orient) const;

  virtual void ScrollWindow(int dx, int dy, const wxRect *rect = NULL);

  // Caret manipulation
  virtual void CreateCaret(int w, int h);
  virtual void CreateCaret(const wxBitmap *bitmap);
  virtual void DestroyCaret(void);
  virtual void ShowCaret(bool show);
  virtual void SetCaretPos(int x, int y);
  virtual void GetCaretPos(int *x, int *y) const;

  // Tell window how much it can be sized
  virtual void SetSizeHints(int minW = -1, int minH = -1, int maxW = -1, int maxH = -1, int incW = -1, int incH = -1);

  // Set/get the window's identifier
  inline int GetId() const;
  inline void SetId(int id);

  // Make the window modal (all other windows unresponsive)
  virtual void MakeModal(bool modal);

  // Get the private handle (platform-dependent)
  inline void *GetHandle(void) const;

  // Set/get the window's relatives
  inline wxWindow *GetParent(void) const;
  inline void SetParent(wxWindow *p) ;
  inline wxWindow *GetGrandParent(void) const;
  inline wxList *GetChildren() const;

  // Set/get the window's font
  virtual void SetFont(const wxFont& f);
  inline virtual wxFont *GetFont(void) const;

  // Set/get the window's validator
  void SetValidator(const wxValidator& validator);
  inline wxValidator *GetValidator(void) const;

  // Set/get the window's style
  inline void SetWindowStyleFlag(long flag);
  inline long GetWindowStyleFlag(void) const;

  // Set/get double-clickability
  // TODO: we probably wish to get rid of this, and
  // always allow double clicks.
  inline void SetDoubleClick(bool flag);
  inline bool GetDoubleClick(void) const;
  inline void AllowDoubleClick(bool value)  { SetDoubleClick(value); }

  // Handle a control command
  virtual void OnCommand(wxWindow& win, wxCommandEvent& event);

  // Set/get event handler
  inline void SetEventHandler(wxEvtHandler *handler);
  inline wxEvtHandler *GetEventHandler(void) const;

  // Push/pop event handler (i.e. allow a chain of event handlers
  // be searched)
  void PushEventHandler(wxEvtHandler *handler) ;
  wxEvtHandler *PopEventHandler(bool deleteHandler = FALSE) ;
  
  // Close the window by calling OnClose, posting a deletion
  virtual bool Close(bool force = FALSE);

  // Destroy the window (delayed, if a managed window)
  virtual bool Destroy(void) ;

  // Mode for telling default OnSize members to
  // call Layout(), if not using Sizers, just top-down constraints
  inline void SetAutoLayout(bool a);
  inline bool GetAutoLayout(void) const;

  // Set/get constraints
  inline wxLayoutConstraints *GetConstraints(void) const;
  void SetConstraints(wxLayoutConstraints *c);

  // Set/get window background colour
  inline virtual void SetBackgroundColour(const wxColour& col);
  inline virtual wxColour GetBackgroundColour(void) const;

  // Set/get window foreground colour
  inline virtual void SetForegroundColour(const wxColour& col);
  inline virtual wxColour GetForegroundColour(void) const;

  // For backward compatibility
  inline virtual void SetButtonFont(const wxFont& font) { SetFont(font); }
  inline virtual void SetLabelFont(const wxFont& font) { SetFont(font); }
  inline virtual wxFont  *GetLabelFont(void) const { return GetFont(); };
  inline virtual wxFont  *GetButtonFont(void) const { return GetFont(); };

  // Get the default button, if there is one
  inline virtual wxButton *GetDefaultItem(void) const;
  inline virtual void SetDefaultItem(wxButton *but);

  virtual void SetAcceleratorTable(const wxAcceleratorTable& accel);
  inline virtual wxAcceleratorTable& GetAcceleratorTable() const { return (wxAcceleratorTable&) m_acceleratorTable; }

  // Override to define new behaviour for default action (e.g. double clicking
  // on a listbox)
  virtual void OnDefaultAction(wxControl *initiatingItem);

  // Resource loading
#if wxUSE_WX_RESOURCES
  virtual bool LoadFromResource(wxWindow *parent, const wxString& resourceName, const wxResourceTable *table = NULL);
  virtual wxControl *CreateItem(const wxItemResource* childResource, const wxItemResource* parentResource, const wxResourceTable *table = NULL);
#endif

  // Native resource loading
  virtual bool LoadNativeDialog(wxWindow* parent, wxWindowID& id);
  virtual bool LoadNativeDialog(wxWindow* parent, const wxString& name);
  virtual wxWindow* GetWindowChild1(wxWindowID& id);
  virtual wxWindow* GetWindowChild(wxWindowID& id);

  virtual void GetTextExtent(const wxString& string, int *x, int *y,
                             int *descent = NULL,
                             int *externalLeading = NULL,
                             const wxFont *theFont = NULL, bool use16 = FALSE) const;

  // Is the window retained?
  inline bool IsRetained(void) const;

  // Warp the pointer the given position
  virtual void WarpPointer(int x_pos, int y_pos) ;

  // Clear the window
  virtual void Clear(void);

  // Find a window by id or name
  virtual wxWindow *FindWindow(long id);
  virtual wxWindow *FindWindow(const wxString& name);

  // Constraint operations
  bool Layout(void);
  void SetSizer(wxSizer *sizer);    // Adds sizer child to this window
  inline wxSizer *GetSizer(void) const ;
  inline wxWindow *GetSizerParent(void) const ;
  inline void SetSizerParent(wxWindow *win);

  // Do Update UI processing for controls
  void UpdateWindowUI(void);

  void OnEraseBackground(wxEraseEvent& event);
  void OnChar(wxKeyEvent& event);
  void OnPaint(wxPaintEvent& event);
  void OnIdle(wxIdleEvent& event);

  // Does this window want to accept keyboard focus?
  virtual bool AcceptsFocus() const;

  virtual void PrepareDC( wxDC &dc ) {};
public:
  ////////////////////////////////////////////////////////////////////////
  //// IMPLEMENTATION
  
  // For implementation purposes - sometimes decorations make the client area
  // smaller
  virtual wxPoint GetClientAreaOrigin() const;

  // Makes an adjustment to the window position (for example, a frame that has
  // a toolbar that it manages itself).
  virtual void AdjustForParentClientOrigin(int& x, int& y, int sizeFlags);

  // Windows subclassing
  void SubclassWin(WXHWND hWnd);
  void UnsubclassWin(void);
  virtual long Default(void);
  virtual bool MSWCommand(WXUINT param, WXWORD id);
  virtual bool MSWNotify(WXWPARAM wParam, WXLPARAM lParam);
  virtual wxWindow *FindItem(int id) const;
  virtual wxWindow *FindItemByHWND(WXHWND hWnd, bool controlOnly = FALSE) const ;
  virtual void PreDelete(WXHDC dc);              // Allows system cleanup
  // TO DO: how many of these need to be virtual?
  virtual WXHWND GetHWND(void) const ;
  virtual void SetHWND(WXHWND hWnd);

  // Make a Windows extended style from the given wxWindows window style
  virtual WXDWORD MakeExtendedStyle(long style, bool eliminateBorders = TRUE);
  // Determine whether 3D effects are wanted
  virtual WXDWORD Determine3DEffects(WXDWORD defaultBorderStyle, bool *want3D);

  virtual void AddChild(wxWindow *child);         // Adds reference to the child object
  virtual void RemoveChild(wxWindow *child);   // Removes reference to child
                                       // (but doesn't delete the child object)
  virtual void DestroyChildren(void);  // Removes and destroys all children

  inline bool IsBeingDeleted(void);

  // MSW only: TRUE if this control is part of the main control
  virtual bool ContainsHWND(WXHWND WXUNUSED(hWnd)) const { return FALSE; };

  // Constraint implementation
  void UnsetConstraints(wxLayoutConstraints *c);
  inline wxList *GetConstraintsInvolvedIn(void) const ;
  // Back-pointer to other windows we're involved with, so if we delete
  // this window, we must delete any constraints we're involved with.
  void AddConstraintReference(wxWindow *otherWin);
  void RemoveConstraintReference(wxWindow *otherWin);
  void DeleteRelatedConstraints(void);

  virtual void ResetConstraints(void);
  virtual void SetConstraintSizes(bool recurse = TRUE);
  virtual bool LayoutPhase1(int *noChanges);
  virtual bool LayoutPhase2(int *noChanges);
  virtual bool DoPhase(int);
  // Transforms from sizer coordinate space to actual
  // parent coordinate space
  virtual void TransformSizerToActual(int *x, int *y) const ;

  // Set size with transformation to actual coordinates if nec.
  virtual void SizerSetSize(int x, int y, int w, int h);
  virtual void SizerMove(int x, int y);

  // Only set/get the size/position of the constraint (if any)
  virtual void SetSizeConstraint(int x, int y, int w, int h);
  virtual void MoveConstraint(int x, int y);
  virtual void GetSizeConstraint(int *w, int *h) const ;
  virtual void GetClientSizeConstraint(int *w, int *h) const ;
  virtual void GetPositionConstraint(int *x, int *y) const ;

  // Dialog units translations. Implemented in wincmn.cpp.
  wxPoint ConvertPixelsToDialog(const wxPoint& pt) ;
  wxPoint ConvertDialogToPixels(const wxPoint& pt) ;
  inline wxSize ConvertPixelsToDialog(const wxSize& sz)
  { wxPoint pt(ConvertPixelsToDialog(wxPoint(sz.x, sz.y))); return wxSize(pt.x, pt.y); }
  inline wxSize ConvertDialogToPixels(const wxSize& sz)
  { wxPoint pt(ConvertDialogToPixels(wxPoint(sz.x, sz.y))); return wxSize(pt.x, pt.y); }

  wxObject *GetChild(int number) const ;

  void MSWCreate(int id, wxWindow *parent, const char *wclass, wxWindow *wx_win, const char *title,
               int x, int y, int width, int height,
               WXDWORD style, const char *dialog_template = NULL,
               WXDWORD exendedStyle = 0);

  // Actually defined in wx_canvs.cc since requires wxCanvas declaration
  virtual void MSWDeviceToLogical(float *x, float *y) const ;

  // Create an appropriate wxWindow from a HWND
  virtual wxWindow* CreateWindowFromHWND(wxWindow* parent, WXHWND hWnd);

  // Make sure the window style reflects the HWND style (roughly)
  virtual void AdoptAttributesFromHWND(void);

  // Setup background and foreground colours correctly
  virtual void SetupColours(void);

  // Saves the last message information before calling base version
  virtual bool ProcessEvent(wxEvent& event);

  // Handlers
  virtual void MSWOnCreate(WXLPCREATESTRUCT cs);
  virtual bool MSWOnPaint(void);
  virtual WXHICON MSWOnQueryDragIcon(void) { return 0; }
  virtual void MSWOnSize(int x, int y, WXUINT flag);
  virtual void MSWOnWindowPosChanging(void *lpPos);
  virtual void MSWOnHScroll(WXWORD nSBCode, WXWORD pos, WXHWND control);
  virtual void MSWOnVScroll(WXWORD nSBCode, WXWORD pos, WXHWND control);
  virtual bool MSWOnCommand(WXWORD id, WXWORD cmd, WXHWND control);
  virtual long MSWOnSysCommand(WXWPARAM wParam, WXLPARAM lParam);
  virtual bool MSWOnNotify(WXWPARAM wParam, WXLPARAM lParam);
  virtual WXHBRUSH MSWOnCtlColor(WXHDC dc, WXHWND pWnd, WXUINT nCtlColor,
                              WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
  virtual bool MSWOnColorChange(WXHWND hWnd, WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
  virtual long MSWOnPaletteChanged(WXHWND hWndPalChange);
  virtual long MSWOnQueryNewPalette();
  virtual bool MSWOnEraseBkgnd(WXHDC pDC);
  virtual void MSWOnMenuHighlight(WXWORD item, WXWORD flags, WXHMENU sysmenu);
  virtual void MSWOnInitMenuPopup(WXHMENU menu, int pos, bool isSystem);
  virtual bool MSWOnClose(void);
  virtual bool MSWOnDestroy(void);
  virtual bool MSWOnSetFocus(WXHWND wnd);
  virtual bool MSWOnKillFocus(WXHWND wnd);
  virtual void MSWOnDropFiles(WXWPARAM wParam);
  virtual bool MSWOnInitDialog(WXHWND hWndFocus);
  virtual void MSWOnShow(bool show, int status);

  // TODO: rationalise these functions into 1 or 2 which take the
  // event type as argument.
  virtual void MSWOnLButtonDown(int x, int y, WXUINT flags);
  virtual void MSWOnLButtonUp(int x, int y, WXUINT flags);
  virtual void MSWOnLButtonDClick(int x, int y, WXUINT flags);

  virtual void MSWOnMButtonDown(int x, int y, WXUINT flags);
  virtual void MSWOnMButtonUp(int x, int y, WXUINT flags);
  virtual void MSWOnMButtonDClick(int x, int y, WXUINT flags);

  virtual void MSWOnRButtonDown(int x, int y, WXUINT flags);
  virtual void MSWOnRButtonUp(int x, int y, WXUINT flags);
  virtual void MSWOnRButtonDClick(int x, int y, WXUINT flags);

  virtual void MSWOnMouseMove(int x, int y, WXUINT flags);
  virtual void MSWOnMouseEnter(int x, int y, WXUINT flags);
  virtual void MSWOnMouseLeave(int x, int y, WXUINT flags);

  virtual void MSWOnChar(WXWORD wParam, WXLPARAM lParam, bool isASCII = FALSE);

  virtual bool MSWOnActivate(int flag, bool minimized, WXHWND activate);
  virtual long MSWOnMDIActivate(long flag, WXHWND activate, WXHWND deactivate);

  virtual bool MSWOnDrawItem(int id, WXDRAWITEMSTRUCT *item);
  virtual bool MSWOnMeasureItem(int id, WXMEASUREITEMSTRUCT *item);

  virtual void MSWOnJoyDown(int joystick, int x, int y, WXUINT flags);
  virtual void MSWOnJoyUp(int joystick, int x, int y, WXUINT flags);
  virtual void MSWOnJoyMove(int joystick, int x, int y, WXUINT flags);
  virtual void MSWOnJoyZMove(int joystick, int z, WXUINT flags);

  virtual long MSWGetDlgCode();

  // Window procedure
  virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

  // Calls an appropriate default window procedure
  virtual long MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
  virtual bool MSWProcessMessage(WXMSG* pMsg);
  virtual bool MSWTranslateMessage(WXMSG* pMsg);
  virtual void MSWDestroyWindow(void);

  // Detach "Window" menu from menu bar so it doesn't get deleted
  void MSWDetachWindowMenu(void);
  
  inline WXFARPROC MSWGetOldWndProc() const;
  inline void MSWSetOldWndProc(WXFARPROC proc);

  // Define for each class of dialog and control
  virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

  inline void SetShowing(bool show);
  inline bool IsUserEnabled(void) const;
  inline bool GetUseCtl3D(void) const ;
  inline bool GetTransparentBackground(void) const ;

  // Responds to colour changes: passes event on to children.
  void OnSysColourChanged(wxSysColourChangedEvent& event);

  // Transfers data to any child controls
  void OnInitDialog(wxInitDialogEvent& event);

  // Sends an OnInitDialog event, which in turns transfers data to
  // to the window via validators.
  virtual void InitDialog(void);

  ////////////////////////////////////////////////////////////////////////
  //// PROTECTED DATA
protected:
  wxAcceleratorTable    m_acceleratorTable;
  int                   m_windowId;
  long                  m_windowStyle; 			// Store the window's style
  wxEvtHandler *        m_windowEventHandler; 	// Usually is 'this'
  wxLayoutConstraints * m_constraints;           // Constraints for this window
  wxList *              m_constraintsInvolvedIn; // List of constraints we're involved in
  wxSizer *             m_windowSizer;                       // Window's top-level sizer (if any)
  wxWindow *            m_sizerParent;                      // Window's parent sizer (if any)
  bool                  m_autoLayout;                            // Whether to call Layout() in OnSize
  wxWindow *            m_windowParent;                     // Each window always knows its parent
  wxValidator *         m_windowValidator;
  // Old window proc, for subclassed controls
  WXFARPROC             m_oldWndProc;
  bool                  m_useCtl3D;             // Using CTL3D for this control

  bool                  m_inOnSize; 			// Protection against OnSize reentry
#ifndef _WX_WIN32__
  // Pointer to global memory, for EDIT controls that need
  // special treatment to reduce USER area consumption.
  WXHGLOBAL             m_globalHandle;
#endif

  bool                  m_winEnabled;
  int                   m_minSizeX;
  int                   m_minSizeY;
  int                   m_maxSizeX;
  int                   m_maxSizeY;

  // Caret data
  int                   m_caretWidth;
  int                   m_caretHeight;
  bool                  m_caretEnabled;
  bool                  m_caretShown;
  wxFont                m_windowFont;                               // Window's font
  bool                  m_isShown;
  bool                  m_doubleClickAllowed ;
  wxCursor              m_windowCursor;                        // Window's cursor
  bool                  m_winCaptured;
  wxString              m_windowName;                            // Window name

#if wxUSE_EXTENDED_STATICS
  wxList                m_staticItems;
#endif

  wxButton *            m_defaultItem;
  wxColour              m_backgroundColour ;
  wxColour              m_foregroundColour ;
  bool                  m_backgroundTransparent;

  int                   m_xThumbSize;
  int                   m_yThumbSize;

  float                 m_lastXPos;
  float                 m_lastYPos;
  int                   m_lastEvent;

  bool                  m_mouseInWindow;

#if wxUSE_DRAG_AND_DROP
  wxDropTarget         *m_pDropTarget;    // the current drop target or NULL
#endif  //USE_DRAG_AND_DROP

public:
  WXHWND                m_hWnd; 			// MS Windows window handle
  WXUINT                m_lastMsg;
  WXWPARAM              m_lastWParam;
  WXLPARAM              m_lastLParam;

  wxRegion              m_updateRegion;
/*
  wxRect           m_updateRect;             // Bounding box for screen damage area
#ifdef __WIN32__
  WXHRGN                m_updateRgn;                  // NT allows access to the rectangle list
#endif
*/

//  WXHANDLE              m_acceleratorTable;
  WXHMENU               m_hMenu; // Menu, if any
  wxList *              m_children;                           // Window's children
  int                   m_returnCode;
  bool                  m_isBeingDeleted; // Fudge because can't access parent
                        // when being deleted

DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////
//// INLINES

inline void *wxWindow::GetHandle(void) const { return (void *)GetHWND(); }
inline int wxWindow::GetId() const { return m_windowId; }
inline void wxWindow::SetId(int id) { m_windowId = id; }
inline wxWindow *wxWindow::GetParent(void) const { return m_windowParent; }
inline void wxWindow::SetParent(wxWindow *p) { m_windowParent = p; }
inline wxWindow *wxWindow::GetGrandParent(void) const { return (m_windowParent ? m_windowParent->m_windowParent : NULL); }
inline wxList *wxWindow::GetChildren() const { return m_children; }
inline wxFont *wxWindow::GetFont(void) const { return (wxFont *) & m_windowFont; }
inline wxString wxWindow::GetName(void) const { return m_windowName; }
inline void wxWindow::SetName(const wxString& name) { m_windowName = name; }
inline long wxWindow::GetWindowStyleFlag(void) const { return m_windowStyle; }
inline void wxWindow::SetWindowStyleFlag(long flag) { m_windowStyle = flag; }
inline void wxWindow::SetDoubleClick(bool flag) { m_doubleClickAllowed = flag; }
inline bool wxWindow::GetDoubleClick(void) const { return m_doubleClickAllowed; }
inline void wxWindow::SetEventHandler(wxEvtHandler *handler) { m_windowEventHandler = handler; }
inline wxEvtHandler *wxWindow::GetEventHandler(void) const { return m_windowEventHandler; }
inline void wxWindow::SetAutoLayout(bool a) { m_autoLayout = a; }
inline bool wxWindow::GetAutoLayout(void) const { return m_autoLayout; }
inline wxLayoutConstraints *wxWindow::GetConstraints(void) const { return m_constraints; }
inline void wxWindow::SetBackgroundColour(const wxColour& col) { m_backgroundColour = col; };
inline wxColour wxWindow::GetBackgroundColour(void) const { return m_backgroundColour; };
inline void wxWindow::SetForegroundColour(const wxColour& col) { m_foregroundColour = col; };
inline wxColour wxWindow::GetForegroundColour(void) const { return m_foregroundColour; };

inline wxButton *wxWindow::GetDefaultItem(void) const { return m_defaultItem; }
inline void wxWindow::SetDefaultItem(wxButton *but) { m_defaultItem = but; }
inline bool wxWindow::IsRetained(void) const { return ((m_windowStyle & wxRETAINED) == wxRETAINED); }

inline void wxWindow::SetShowing(bool show) { m_isShown = show; }
inline wxList *wxWindow::GetConstraintsInvolvedIn(void) const { return m_constraintsInvolvedIn; }
inline wxSizer *wxWindow::GetSizer(void) const { return m_windowSizer; }
inline wxWindow *wxWindow::GetSizerParent(void) const { return m_sizerParent; }
inline void wxWindow::SetSizerParent(wxWindow *win) { m_sizerParent = win; }
inline WXFARPROC wxWindow::MSWGetOldWndProc() const { return m_oldWndProc; }
inline void wxWindow::MSWSetOldWndProc(WXFARPROC proc) { m_oldWndProc = proc; }
inline wxValidator *wxWindow::GetValidator(void) const { return m_windowValidator; }
inline bool wxWindow::IsUserEnabled(void) const { return m_winEnabled; }
inline bool wxWindow::GetUseCtl3D(void) const { return m_useCtl3D; }
inline bool wxWindow::GetTransparentBackground(void) const { return m_backgroundTransparent; }
inline void wxWindow::SetReturnCode(int retCode) { m_returnCode = retCode; }
inline int wxWindow::GetReturnCode(void) { return m_returnCode; }
inline bool wxWindow::IsBeingDeleted(void) { return m_isBeingDeleted; }

// Window specific (so far)
wxWindow* WXDLLEXPORT wxGetActiveWindow(void);

WXDLLEXPORT_DATA(extern wxList) wxTopLevelWindows;

int WXDLLEXPORT wxCharCodeMSWToWX(int keySym);
int WXDLLEXPORT wxCharCodeWXToMSW(int id, bool *IsVirtual);

// Allocates control ids
int WXDLLEXPORT NewControlId(void);

#endif
    // _WX_WINDOW_H_
