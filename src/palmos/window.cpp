/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/windows.cpp
// Purpose:     wxWindow
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by: Wlodzimierz ABX Skiba - more than minimal functionality
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne, Wlodzimierz Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "window.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/accel.h"
    #include "wx/setup.h"
    #include "wx/menu.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/layout.h"
    #include "wx/dialog.h"
    #include "wx/frame.h"
    #include "wx/listbox.h"
    #include "wx/button.h"
    #include "wx/msgdlg.h"
    #include "wx/settings.h"
    #include "wx/statbox.h"
#endif

#if wxUSE_OWNER_DRAWN && !defined(__WXUNIVERSAL__)
    #include "wx/ownerdrw.h"
#endif

#include "wx/module.h"

#if wxUSE_DRAG_AND_DROP
    #include "wx/dnd.h"
#endif

#if wxUSE_ACCESSIBILITY
    #include "wx/access.h"
#endif

#include "wx/menuitem.h"
#include "wx/log.h"

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#if wxUSE_CARET
    #include "wx/caret.h"
#endif // wxUSE_CARET

#if wxUSE_SPINCTRL
    #include "wx/spinctrl.h"
#endif // wxUSE_SPINCTRL

#include "wx/intl.h"
#include "wx/log.h"

#include "wx/textctrl.h"
#include "wx/notebook.h"
#include "wx/listctrl.h"
#include "wx/window.h"

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------

#if wxUSE_MENUS_NATIVE
wxMenu *wxCurrentPopupMenu = NULL;
#endif // wxUSE_MENUS_NATIVE

// true if we had already created the std colour map, used by
// wxGetStdColourMap() and wxWindow::OnSysColourChanged()           (FIXME-MT)
static bool gs_hasStdCmap = false;

// ---------------------------------------------------------------------------
// private functions
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

// in wxUniv/Palm this class is abstract because it doesn't have DoPopupMenu()
// method
#ifdef __WXUNIVERSAL__
    IMPLEMENT_ABSTRACT_CLASS(wxWindowPalm, wxWindowBase)
#else // __WXPalm__
#if wxUSE_EXTENDED_RTTI

// windows that are created from a parent window during its Create method, eg. spin controls in a calendar controls
// must never been streamed out separately otherwise chaos occurs. Right now easiest is to test for negative ids, as
// windows with negative ids never can be recreated anyway

bool wxWindowStreamingCallback( const wxObject *object, wxWriter * , wxPersister * , wxxVariantArray & )
{
    return false;
}

IMPLEMENT_DYNAMIC_CLASS_XTI_CALLBACK(wxWindow, wxWindowBase,"wx/window.h", wxWindowStreamingCallback)

// make wxWindowList known before the property is used

wxCOLLECTION_TYPE_INFO( wxWindow* , wxWindowList ) ;

template<> void wxCollectionToVariantArray( wxWindowList const &theList, wxxVariantArray &value)
{
    wxListCollectionToVariantArray<wxWindowList::compatibility_iterator>( theList , value ) ;
}

WX_DEFINE_FLAGS( wxWindowStyle )

wxBEGIN_FLAGS( wxWindowStyle )
    // new style border flags, we put them first to
    // use them for streaming out

    wxFLAGS_MEMBER(wxBORDER_SIMPLE)
    wxFLAGS_MEMBER(wxBORDER_SUNKEN)
    wxFLAGS_MEMBER(wxBORDER_DOUBLE)
    wxFLAGS_MEMBER(wxBORDER_RAISED)
    wxFLAGS_MEMBER(wxBORDER_STATIC)
    wxFLAGS_MEMBER(wxBORDER_NONE)

    // old style border flags
    wxFLAGS_MEMBER(wxSIMPLE_BORDER)
    wxFLAGS_MEMBER(wxSUNKEN_BORDER)
    wxFLAGS_MEMBER(wxDOUBLE_BORDER)
    wxFLAGS_MEMBER(wxRAISED_BORDER)
    wxFLAGS_MEMBER(wxSTATIC_BORDER)
    wxFLAGS_MEMBER(wxBORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

wxEND_FLAGS( wxWindowStyle )

wxBEGIN_PROPERTIES_TABLE(wxWindow)
    wxEVENT_PROPERTY( Close , wxEVT_CLOSE_WINDOW , wxCloseEvent)
    wxEVENT_PROPERTY( Create , wxEVT_CREATE , wxWindowCreateEvent )
    wxEVENT_PROPERTY( Destroy , wxEVT_DESTROY , wxWindowDestroyEvent )
    // Always constructor Properties first

    wxREADONLY_PROPERTY( Parent,wxWindow*, GetParent, EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Id,wxWindowID, SetId, GetId, wxID_ANY, 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxPROPERTY( Position,wxPoint, SetPosition , GetPosition, wxDefaultPosition , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // pos
    wxPROPERTY( Size,wxSize, SetSize, GetSize, wxDefaultSize , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // size
    wxPROPERTY( WindowStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style

    // Then all relations of the object graph

    wxREADONLY_PROPERTY_COLLECTION( Children , wxWindowList , wxWindowBase* , GetWindowChildren , wxPROP_OBJECT_GRAPH /*flags*/ , wxT("Helpstring") , wxT("group"))

   // and finally all other properties

    wxPROPERTY( ExtraStyle , long , SetExtraStyle , GetExtraStyle , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // extstyle
    wxPROPERTY( BackgroundColour , wxColour , SetBackgroundColour , GetBackgroundColour , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // bg
    wxPROPERTY( ForegroundColour , wxColour , SetForegroundColour , GetForegroundColour , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // fg
    wxPROPERTY( Enabled , bool , Enable , IsEnabled , wxxVariant((bool)true) , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Shown , bool , Show , IsShown , wxxVariant((bool)true) , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
#if 0
    // possible property candidates (not in xrc) or not valid in all subclasses
    wxPROPERTY( Title,wxString, SetTitle, GetTitle, wxT("") )
    wxPROPERTY( Font , wxFont , SetFont , GetWindowFont  , )
    wxPROPERTY( Label,wxString, SetLabel, GetLabel, wxT("") )
    // MaxHeight, Width , MinHeight , Width
    // TODO switch label to control and title to toplevels

    wxPROPERTY( ThemeEnabled , bool , SetThemeEnabled , GetThemeEnabled , )
    //wxPROPERTY( Cursor , wxCursor , SetCursor , GetCursor , )
    // wxPROPERTY( ToolTip , wxString , SetToolTip , GetToolTipText , )
    wxPROPERTY( AutoLayout , bool , SetAutoLayout , GetAutoLayout , )



#endif
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxWindow)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_DUMMY(wxWindow)

#else
    IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxWindowBase)
#endif
#endif // __WXUNIVERSAL__/__WXPalm__

BEGIN_EVENT_TABLE(wxWindowPalm, wxWindowBase)
    EVT_ERASE_BACKGROUND(wxWindowPalm::OnEraseBackground)
    EVT_SYS_COLOUR_CHANGED(wxWindowPalm::OnSysColourChanged)
    EVT_INIT_DIALOG(wxWindowPalm::OnInitDialog)
END_EVENT_TABLE()

// ===========================================================================
// implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// wxWindow utility functions
// ---------------------------------------------------------------------------

// Find an item given the MS Windows id
wxWindow *wxWindowPalm::FindItem(long id) const
{
    return NULL;
}

// Find an item given the MS Windows handle
wxWindow *wxWindowPalm::FindItemByHWND(WXHWND hWnd, bool controlOnly) const
{
    return NULL;
}

// Default command handler
bool wxWindowPalm::PalmCommand(WXUINT WXUNUSED(param), WXWORD WXUNUSED(id))
{
    return false;
}

// ----------------------------------------------------------------------------
// constructors and such
// ----------------------------------------------------------------------------

void wxWindowPalm::Init()
{
}

// Destructor
wxWindowPalm::~wxWindowPalm()
{
}

// real construction (Init() must have been called before!)
bool wxWindowPalm::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    return false;
}

FormType *wxWindowPalm::GetFormPtr()
{
    return FrameForm;
}
void wxWindowPalm::SetFormPtr(FormType *FormPtr)
{
    FrameForm=FormPtr;
}

// ---------------------------------------------------------------------------
// basic operations
// ---------------------------------------------------------------------------

void wxWindowPalm::SetFocus()
{
}

void wxWindowPalm::SetFocusFromKbd()
{
}

// Get the window with the focus
wxWindow *wxWindowBase::DoFindFocus()
{
    return NULL;
}

bool wxWindowPalm::Enable(bool enable)
{
    return false;
}

bool wxWindowPalm::Show(bool show)
{
    return false;
}

// Raise the window to the top of the Z order
void wxWindowPalm::Raise()
{
}

// Lower the window to the bottom of the Z order
void wxWindowPalm::Lower()
{
}

void wxWindowPalm::SetTitle( const wxString& title)
{
}

wxString wxWindowPalm::GetTitle() const
{
    return wxEmptyString;
}

void wxWindowPalm::DoCaptureMouse()
{
}

void wxWindowPalm::DoReleaseMouse()
{
}

/* static */ wxWindow *wxWindowBase::GetCapture()
{
    return NULL;
}

bool wxWindowPalm::SetFont(const wxFont& font)
{
    return false;
}
bool wxWindowPalm::SetCursor(const wxCursor& cursor)
{
    return false;
}

void wxWindowPalm::WarpPointer (int x, int y)
{
}

// ---------------------------------------------------------------------------
// scrolling stuff
// ---------------------------------------------------------------------------

// convert wxHORIZONTAL/wxVERTICAL to SB_HORZ/SB_VERT
static inline int wxDirToWinStyle(int orient)
{
    return 0;
}

int wxWindowPalm::GetScrollPos(int orient) const
{
    return 0;
}

// This now returns the whole range, not just the number
// of positions that we can scroll.
int wxWindowPalm::GetScrollRange(int orient) const
{
    return 0;
}

int wxWindowPalm::GetScrollThumb(int orient) const
{
    return 0;
}

void wxWindowPalm::SetScrollPos(int orient, int pos, bool refresh)
{
}

// New function that will replace some of the above.
void wxWindowPalm::SetScrollbar(int orient,
                               int pos,
                               int pageSize,
                               int range,
                               bool refresh)
{
}

void wxWindowPalm::ScrollWindow(int dx, int dy, const wxRect *prect)
{
}

bool wxWindowPalm::ScrollLines(int lines)
{
    return false;
}

bool wxWindowPalm::ScrollPages(int pages)
{
    return false;
}

// ---------------------------------------------------------------------------
// subclassing
// ---------------------------------------------------------------------------

void wxWindowPalm::SubclassWin(WXHWND hWnd)
{
}

void wxWindowPalm::UnsubclassWin()
{
}

bool wxCheckWindowWndProc(WXHWND hWnd, WXFARPROC wndProc)
{
    return false;
}

// ----------------------------------------------------------------------------
// Style handling
// ----------------------------------------------------------------------------

void wxWindowPalm::SetWindowStyleFlag(long flags)
{
}

WXDWORD wxWindowPalm::PalmGetStyle(long flags, WXDWORD *exstyle) const
{
    return 0;
}

// Setup background and foreground colours correctly
void wxWindowPalm::SetupColours()
{
}

bool wxWindowPalm::IsMouseInWindow() const
{
    return false;
}

void wxWindowPalm::OnInternalIdle()
{
}

// Set this window to be the child of 'parent'.
bool wxWindowPalm::Reparent(wxWindowBase *parent)
{
    return false;
}

void wxWindowPalm::Freeze()
{
}

void wxWindowPalm::Thaw()
{
}

void wxWindowPalm::Refresh(bool eraseBack, const wxRect *rect)
{
}

void wxWindowPalm::Update()
{
}

// ---------------------------------------------------------------------------
// drag and drop
// ---------------------------------------------------------------------------


#if wxUSE_DRAG_AND_DROP
void wxWindowPalm::SetDropTarget(wxDropTarget *pDropTarget)
{
}
#endif // wxUSE_DRAG_AND_DROP

// old style file-manager drag&drop support: we retain the old-style
// DragAcceptFiles in parallel with SetDropTarget.
void wxWindowPalm::DragAcceptFiles(bool accept)
{
}

// ----------------------------------------------------------------------------
// tooltips
// ----------------------------------------------------------------------------

#if wxUSE_TOOLTIPS

void wxWindowPalm::DoSetToolTip(wxToolTip *tooltip)
{
}

#endif // wxUSE_TOOLTIPS

// ---------------------------------------------------------------------------
// moving and resizing
// ---------------------------------------------------------------------------

// Get total size
void wxWindowPalm::DoGetSize(int *x, int *y) const
{
}

// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindowPalm::DoGetClientSize(int *x, int *y) const
{
}

void wxWindowPalm::DoGetPosition(int *x, int *y) const
{
}

void wxWindowPalm::DoScreenToClient(int *x, int *y) const
{
}

void wxWindowPalm::DoClientToScreen(int *x, int *y) const
{
}

void wxWindowPalm::DoMoveWindow(int x, int y, int width, int height)
{
}

// set the size of the window: if the dimensions are positive, just use them,
// but if any of them is equal to -1, it means that we must find the value for
// it ourselves (unless sizeFlags contains wxSIZE_ALLOW_MINUS_ONE flag, in
// which case -1 is a valid value for x and y)
//
// If sizeFlags contains wxSIZE_AUTO_WIDTH/HEIGHT flags (default), we calculate
// the width/height to best suit our contents, otherwise we reuse the current
// width/height
void wxWindowPalm::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
}

void wxWindowPalm::DoSetClientSize(int width, int height)
{
}

// For implementation purposes - sometimes decorations make the client area
// smaller
wxPoint wxWindowPalm::GetClientAreaOrigin() const
{
    return wxPoint(0, 0);
}

// ---------------------------------------------------------------------------
// text metrics
// ---------------------------------------------------------------------------

int wxWindowPalm::GetCharHeight() const
{
    return 0;
}

int wxWindowPalm::GetCharWidth() const
{
    return 0;
}

void wxWindowPalm::GetTextExtent(const wxString& string,
                             int *x, int *y,
                             int *descent, int *externalLeading,
                             const wxFont *theFont) const
{
}

// ---------------------------------------------------------------------------
// popup menu
// ---------------------------------------------------------------------------

#if wxUSE_MENUS_NATIVE

// yield for WM_COMMAND events only, i.e. process all WM_COMMANDs in the queue
// immediately, without waiting for the next event loop iteration
//
// NB: this function should probably be made public later as it can almost
//     surely replace wxYield() elsewhere as well
static void wxYieldForCommandsOnly()
{
}

bool wxWindowPalm::DoPopupMenu(wxMenu *menu, int x, int y)
{
    return false;
}

#endif // wxUSE_MENUS_NATIVE

// ===========================================================================
// pre/post message processing
// ===========================================================================

WXLRESULT wxWindowPalm::PalmDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    return false;
}

bool wxWindowPalm::PalmProcessMessage(WXMSG* pMsg)
{
    return false;
}

bool wxWindowPalm::PalmTranslateMessage(WXMSG* pMsg)
{
    return false;
}

bool wxWindowPalm::PalmShouldPreProcessMessage(WXMSG* WXUNUSED(pMsg))
{
    return false;
}

// ---------------------------------------------------------------------------
// Main wxWidgets window proc and the window proc for wxWindow
// ---------------------------------------------------------------------------

// Hook for new window just as it's being created, when the window isn't yet
// associated with the handle

// implementation of wxWindowCreationHook class: it just sets gs_winBeingCreated to the
// window being created and insures that it's always unset back later
wxWindowCreationHook::wxWindowCreationHook(wxWindowPalm *winBeingCreated)
{
}

wxWindowCreationHook::~wxWindowCreationHook()
{
}

WXLRESULT wxWindowPalm::PalmWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
{
    return false;
}

// ----------------------------------------------------------------------------
// wxWindow <-> HWND map
// ----------------------------------------------------------------------------

wxWinHashTable *wxWinHandleHash = NULL;

wxWindow *wxFindWinFromHandle(WXHWND hWnd)
{
    return NULL;
}

void wxRemoveHandleAssociation(wxWindowPalm *win)
{
}

// ----------------------------------------------------------------------------
// various Palm speciic class dependent functions
// ----------------------------------------------------------------------------

// Default destroyer - override if you destroy it in some other way
// (e.g. with MDI child windows)
void wxWindowPalm::PalmDestroyWindow()
{
}

bool wxWindowPalm::PalmGetCreateWindowCoords(const wxPoint& pos,
                                           const wxSize& size,
                                           int& x, int& y,
                                           int& w, int& h) const
{
    return false;
}

WXHWND wxWindowPalm::PalmGetParent() const
{
    return NULL;
}

bool wxWindowPalm::PalmCreate(const wxChar *wclass,
                            const wxChar *title,
                            const wxPoint& pos,
                            const wxSize& size,
                            WXDWORD style,
                            WXDWORD extendedStyle)
{
    return false;
}

// ===========================================================================
// Palm message handlers
// ===========================================================================

// ---------------------------------------------------------------------------
// end session messages
// ---------------------------------------------------------------------------

bool wxWindowPalm::HandleQueryEndSession(long logOff, bool *mayEnd)
{
    return false;
}

bool wxWindowPalm::HandleEndSession(bool endSession, long logOff)
{
    return false;
}

// ---------------------------------------------------------------------------
// window creation/destruction
// ---------------------------------------------------------------------------

bool wxWindowPalm::HandleCreate(WXLPCREATESTRUCT cs, bool *mayCreate)
{
    return false;
}

bool wxWindowPalm::HandleDestroy()
{
    return false;
}

// ---------------------------------------------------------------------------
// activation/focus
// ---------------------------------------------------------------------------

bool wxWindowPalm::HandleActivate(int state,
                              bool WXUNUSED(minimized),
                              WXHWND WXUNUSED(activate))
{
    return false;
}

bool wxWindowPalm::HandleSetFocus(WXHWND hwnd)
{
    return false;
}

bool wxWindowPalm::HandleKillFocus(WXHWND hwnd)
{
    return false;
}

// ---------------------------------------------------------------------------
// miscellaneous
// ---------------------------------------------------------------------------

bool wxWindowPalm::HandleShow(bool show, int WXUNUSED(status))
{
    return false;
}

bool wxWindowPalm::HandleInitDialog(WXHWND WXUNUSED(hWndFocus))
{
    return false;
}

bool wxWindowPalm::HandleDropFiles(WXWPARAM wParam)
{
    return false;
}


bool wxWindowPalm::HandleSetCursor(WXHWND WXUNUSED(hWnd),
                                  short nHitTest,
                                  int WXUNUSED(mouseMsg))
{
    return false;
}

// ---------------------------------------------------------------------------
// owner drawn stuff
// ---------------------------------------------------------------------------

#if (wxUSE_OWNER_DRAWN && wxUSE_MENUS_NATIVE) || \
        (wxUSE_CONTROLS && !defined(__WXUNIVERSAL__))
    #define WXUNUSED_UNLESS_ODRAWN(param) param
#else
    #define WXUNUSED_UNLESS_ODRAWN(param)
#endif

bool
wxWindowPalm::PalmOnDrawItem(int WXUNUSED_UNLESS_ODRAWN(id),
                           WXDRAWITEMSTRUCT * WXUNUSED_UNLESS_ODRAWN(itemStruct))
{
    return false;
}

bool
wxWindowPalm::PalmOnMeasureItem(int WXUNUSED_UNLESS_ODRAWN(id),
                              WXMEASUREITEMSTRUCT *
                                  WXUNUSED_UNLESS_ODRAWN(itemStruct))
{
    return false;
}

// ---------------------------------------------------------------------------
// colours and palettes
// ---------------------------------------------------------------------------

bool wxWindowPalm::HandleSysColorChange()
{
    return false;
}

bool wxWindowPalm::HandleDisplayChange()
{
    return false;
}

bool wxWindowPalm::HandleCtlColor(WXHBRUSH *brush,
                              WXHDC pDC,
                              WXHWND pWnd,
                              WXUINT nCtlColor,
                              WXUINT message,
                              WXWPARAM wParam,
                              WXLPARAM lParam)
{
    return false;
}

// Define for each class of dialog and control
WXHBRUSH wxWindowPalm::OnCtlColor(WXHDC WXUNUSED(hDC),
                                 WXHWND WXUNUSED(hWnd),
                                 WXUINT WXUNUSED(nCtlColor),
                                 WXUINT WXUNUSED(message),
                                 WXWPARAM WXUNUSED(wParam),
                                 WXLPARAM WXUNUSED(lParam))
{
    return (WXHBRUSH)0;
}

bool wxWindowPalm::HandlePaletteChanged(WXHWND hWndPalChange)
{
    return false;
}

bool wxWindowPalm::HandleCaptureChanged(WXHWND hWndGainedCapture)
{
    return false;
}

bool wxWindowPalm::HandleQueryNewPalette()
{
    return false;
}

// Responds to colour changes: passes event on to children.
void wxWindowPalm::OnSysColourChanged(wxSysColourChangedEvent& WXUNUSED(event))
{
}

// ---------------------------------------------------------------------------
// painting
// ---------------------------------------------------------------------------

bool wxWindowPalm::HandlePaint()
{
    return false;
}

// Can be called from an application's OnPaint handler
void wxWindowPalm::OnPaint(wxPaintEvent& event)
{
}

bool wxWindowPalm::HandleEraseBkgnd(WXHDC hdc)
{
    return false;
}

void wxWindowPalm::OnEraseBackground(wxEraseEvent& event)
{
}

// ---------------------------------------------------------------------------
// moving and resizing
// ---------------------------------------------------------------------------

bool wxWindowPalm::HandleMinimize()
{
    return false;
}

bool wxWindowPalm::HandleMaximize()
{
    return false;
}

bool wxWindowPalm::HandleMove(int x, int y)
{
    return false;
}

bool wxWindowPalm::HandleMoving(wxRect& rect)
{
    return false;
}

bool wxWindowPalm::HandleSize(int WXUNUSED(w), int WXUNUSED(h),
                             WXUINT WXUNUSED(flag))
{
    return false;
}

bool wxWindowPalm::HandleSizing(wxRect& rect)
{
    return false;
}

bool wxWindowPalm::HandleGetMinMaxInfo(void *mmInfo)
{
    return false;
}

// ---------------------------------------------------------------------------
// command messages
// ---------------------------------------------------------------------------

bool wxWindowPalm::HandleCommand(WXWORD id, WXWORD cmd, WXHWND control)
{
    return false;
}

// ---------------------------------------------------------------------------
// mouse events
// ---------------------------------------------------------------------------

void wxWindowPalm::InitMouseEvent(wxMouseEvent& event,
                                 int x, int y,
                                 WXUINT flags)
{
}

// Windows doesn't send the mouse events to the static controls (which are
// transparent in the sense that their WM_NCHITTEST handler returns
// HTTRANSPARENT) at all but we want all controls to receive the mouse events
// and so we manually check if we don't have a child window under mouse and if
// we do, send the event to it instead of the window Windows had sent WM_XXX
// to.
//
// Notice that this is not done for the mouse move events because this could
// (would?) be too slow, but only for clicks which means that the static texts
// still don't get move, enter nor leave events.
static wxWindowPalm *FindWindowForMouseEvent(wxWindowPalm *win, int *x, int *y) //TW:REQ:Univ
{
    return NULL;
}

bool wxWindowPalm::HandleMouseEvent(WXUINT msg, int x, int y, WXUINT flags)
{
    return false;
}

bool wxWindowPalm::HandleMouseMove(int x, int y, WXUINT flags)
{
    return false;
}


bool wxWindowPalm::HandleMouseWheel(WXWPARAM wParam, WXLPARAM lParam)
{
    return false;
}


// ---------------------------------------------------------------------------
// keyboard handling
// ---------------------------------------------------------------------------

// create the key event of the given type for the given key - used by
// HandleChar and HandleKeyDown/Up
wxKeyEvent wxWindowPalm::CreateKeyEvent(wxEventType evType,
                                       int id,
                                       WXLPARAM lParam,
                                       WXWPARAM wParam) const
{
    wxKeyEvent event(evType);

    return event;
}

// isASCII is true only when we're called from WM_CHAR handler and not from
// WM_KEYDOWN one
bool wxWindowPalm::HandleChar(WXWPARAM wParam, WXLPARAM lParam, bool isASCII)
{
    return false;
}

bool wxWindowPalm::HandleKeyDown(WXWPARAM wParam, WXLPARAM lParam)
{
    return false;
}

bool wxWindowPalm::HandleKeyUp(WXWPARAM wParam, WXLPARAM lParam)
{
    return false;
}

// ---------------------------------------------------------------------------
// joystick
// ---------------------------------------------------------------------------

bool wxWindowPalm::HandleJoystickEvent(WXUINT msg, int x, int y, WXUINT flags)
{
    return false;
}

// ---------------------------------------------------------------------------
// scrolling
// ---------------------------------------------------------------------------

bool wxWindowPalm::PalmOnScroll(int orientation, WXWORD wParam,
                              WXWORD pos, WXHWND control)
{
    return false;
}

// ===========================================================================
// global functions
// ===========================================================================

void wxGetCharSize(WXHWND wnd, int *x, int *y, const wxFont *the_font)
{
}

// Returns 0 if was a normal ASCII value, not a special key. This indicates that
// the key should be ignored by WM_KEYDOWN and processed by WM_CHAR instead.
int wxCharCodePalmToWX(int keySym, WXLPARAM lParam)
{
    return 0;
}

int wxCharCodeWXToPalm(int id, bool *isVirtual)
{
    return 0;
}

bool wxGetKeyState(wxKeyCode key)
{
    return false;
}

wxWindow *wxGetActiveWindow()
{
    return NULL;
}

extern wxWindow *wxGetWindowFromHWND(WXHWND hWnd)
{
    return NULL;
}

// Find the wxWindow at the current mouse position, returning the mouse
// position.
wxWindow* wxFindWindowAtPointer(wxPoint& pt)
{
    return NULL;
}

wxWindow* wxFindWindowAtPoint(const wxPoint& pt)
{
    return NULL;
}

// Get the current mouse position.
wxPoint wxGetMousePosition()
{
    return wxPoint(0, 0);
}

#if wxUSE_HOTKEY

bool wxWindowPalm::RegisterHotKey(int hotkeyId, int modifiers, int keycode)
{
    return false;
}

bool wxWindowPalm::UnregisterHotKey(int hotkeyId)
{
    return false;
}

#if wxUSE_ACCEL

bool wxWindowPalm::HandleHotKey(WXWPARAM wParam, WXLPARAM lParam)
{
    return false;
}

#endif // wxUSE_ACCEL

#endif // wxUSE_HOTKEY


