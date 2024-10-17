/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/combocmn.cpp
// Purpose:     wxComboCtrlBase
// Author:      Jaakko Salli
// Created:     Apr-30-2006
// Copyright:   (c) 2005 Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#include "wx/combo.h"
#include "wx/display.h"

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#if wxUSE_COMBOBOX
#include "wx/combobox.h"
extern WXDLLEXPORT_DATA(const char) wxComboBoxNameStr[] = "comboBox";

// ----------------------------------------------------------------------------
// XTI
// ----------------------------------------------------------------------------

wxDEFINE_FLAGS( wxComboBoxStyle )
wxBEGIN_FLAGS( wxComboBoxStyle )
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
wxFLAGS_MEMBER(wxWANTS_CHARS)
wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
wxFLAGS_MEMBER(wxVSCROLL)
wxFLAGS_MEMBER(wxHSCROLL)

wxFLAGS_MEMBER(wxCB_SIMPLE)
wxFLAGS_MEMBER(wxCB_SORT)
wxFLAGS_MEMBER(wxCB_READONLY)
wxFLAGS_MEMBER(wxCB_DROPDOWN)

wxEND_FLAGS( wxComboBoxStyle )

wxIMPLEMENT_DYNAMIC_CLASS_XTI(wxComboBox, wxControl, "wx/combobox.h");

wxBEGIN_PROPERTIES_TABLE(wxComboBox)
wxEVENT_PROPERTY( Select, wxEVT_COMBOBOX, wxCommandEvent )
wxEVENT_PROPERTY( TextEnter, wxEVT_TEXT_ENTER, wxCommandEvent )

// TODO DELEGATES
wxPROPERTY( Font, wxFont, SetFont, GetFont, wxEMPTY_PARAMETER_VALUE, \
           0 /*flags*/, wxT("Helpstring"), wxT("group"))
wxPROPERTY_COLLECTION( Choices, wxArrayString, wxString, AppendString, \
                      GetStrings, 0 /*flags*/, wxT("Helpstring"), wxT("group"))
wxPROPERTY( Value,wxString, SetValue, GetValue, wxEMPTY_PARAMETER_VALUE, \
           0 /*flags*/, wxT("Helpstring"), wxT("group"))
wxPROPERTY( Selection,int, SetSelection, GetSelection, wxEMPTY_PARAMETER_VALUE, \
           0 /*flags*/, wxT("Helpstring"), wxT("group"))

wxPROPERTY_FLAGS( WindowStyle, wxComboBoxStyle, long, SetWindowStyleFlag, \
                 GetWindowStyleFlag, wxEMPTY_PARAMETER_VALUE, 0 /*flags*/, \
                 wxT("Helpstring"), wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxEMPTY_HANDLERS_TABLE(wxComboBox)

wxCONSTRUCTOR_5( wxComboBox, wxWindow*, Parent, wxWindowID, Id, \
                wxString, Value, wxPoint, Position, wxSize, Size )

#endif // wxUSE_COMBOBOX

#if wxUSE_COMBOCTRL

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
    #include "wx/timer.h"
    #include "wx/textctrl.h"
#endif

#include "wx/tooltip.h"

// constants
// ----------------------------------------------------------------------------

#define DEFAULT_DROPBUTTON_WIDTH                19

#define BMP_BUTTON_MARGIN                       4

#define DEFAULT_POPUP_HEIGHT                    400

#define DEFAULT_TEXT_INDENT                     3

#define COMBO_MARGIN                            2 // spacing right of wxTextCtrl


#if defined(__WXMSW__)

// Let's use wxFrame as a fall-back solution until wxMSW gets wxNonOwnedWindow
#include "wx/frame.h"
#define wxCC_GENERIC_TLW_IS_FRAME
#define wxComboCtrlGenericTLW   wxFrame

#elif defined(__WXGTK__)

// NB: It is not recommended to use wxDialog as popup on wxGTK, because of
//     this bug: If wxDialog is hidden, its position becomes corrupt
//     between hide and next show, but without internal coordinates being
//     reflected (or something like that - atleast commenting out ->Hide()
//     seemed to eliminate the position change).

#include "wx/dialog.h"
#define wxComboCtrlGenericTLW   wxDialog

#include "wx/gtk/private.h"

#elif defined(__WXMAC__)

#include "wx/nonownedwnd.h"
#define wxCC_GENERIC_TLW_IS_NONOWNEDWINDOW
#define wxComboCtrlGenericTLW   wxNonOwnedWindow

#define FOCUS_RING                    3 // Reserve room for the textctrl's focus ring to display

#undef DEFAULT_DROPBUTTON_WIDTH
#define DEFAULT_DROPBUTTON_WIDTH      22
#undef COMBO_MARGIN
#define COMBO_MARGIN                  FOCUS_RING

#elif defined(__WXQT__)

#include "wx/nonownedwnd.h"
#define wxComboCtrlGenericTLW   wxNonOwnedWindow

#else

#include "wx/dialog.h"
#define wxComboCtrlGenericTLW   wxDialog

// Assume we can't use wxPopupTransientWindow on the other platforms.
#undef wxUSE_POPUPWIN
#define wxUSE_POPUPWIN 0

#endif

// No focus ring by default.
#ifndef FOCUS_RING
    #define FOCUS_RING 0
#endif

#if wxUSE_POPUPWIN
    #include "wx/popupwin.h"

    #define wxComboPopupWindowBase  wxPopupTransientWindow
#else
    #define wxComboPopupWindowBase      wxComboCtrlGenericTLW
#endif

//
// ** TODO **
// * wxComboPopupWindow for external use (ie. replace old wxUniv wxPopupComboWindow)
//


// ----------------------------------------------------------------------------
// wxComboFrameEventHandler takes care of hiding the popup when events happen
// in its top level parent.
// ----------------------------------------------------------------------------

#if !wxUSE_POPUPWIN

//
// This will no longer be necessary after wxTransientPopupWindow
// works well on all platforms.
//

class wxComboFrameEventHandler : public wxEvtHandler
{
public:
    wxComboFrameEventHandler( wxComboCtrlBase* pCb );
    virtual ~wxComboFrameEventHandler();

    void OnPopup();

    void OnIdle( wxIdleEvent& event );
    void OnMouseEvent( wxMouseEvent& event );
    void OnActivate( wxActivateEvent& event );
    void OnResize( wxSizeEvent& event );
    void OnMove( wxMoveEvent& event );
    void OnMenuEvent( wxMenuEvent& event );
    void OnClose( wxCloseEvent& event );

protected:
    wxWindow*                       m_focusStart;
    wxComboCtrlBase*     m_combo;

private:
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxComboFrameEventHandler, wxEvtHandler)
    EVT_IDLE(wxComboFrameEventHandler::OnIdle)
    EVT_LEFT_DOWN(wxComboFrameEventHandler::OnMouseEvent)
    EVT_RIGHT_DOWN(wxComboFrameEventHandler::OnMouseEvent)
    EVT_SIZE(wxComboFrameEventHandler::OnResize)
    EVT_MOVE(wxComboFrameEventHandler::OnMove)
    EVT_MENU_HIGHLIGHT(wxID_ANY,wxComboFrameEventHandler::OnMenuEvent)
    EVT_MENU_OPEN(wxComboFrameEventHandler::OnMenuEvent)
    EVT_ACTIVATE(wxComboFrameEventHandler::OnActivate)
    EVT_CLOSE(wxComboFrameEventHandler::OnClose)
wxEND_EVENT_TABLE()

wxComboFrameEventHandler::wxComboFrameEventHandler( wxComboCtrlBase* combo )
    : wxEvtHandler()
{
    m_combo = combo;
}

wxComboFrameEventHandler::~wxComboFrameEventHandler()
{
}

void wxComboFrameEventHandler::OnPopup()
{
    m_focusStart = ::wxWindow::FindFocus();
}

void wxComboFrameEventHandler::OnIdle( wxIdleEvent& event )
{
    wxWindow* winFocused = ::wxWindow::FindFocus();

    wxWindow* popup = m_combo->GetPopupControl()->GetControl();
    wxWindow* winpopup = m_combo->GetPopupWindow();

    if ( !winFocused || (
         winFocused != m_focusStart &&
         winFocused != popup &&
         winFocused->GetParent() != popup &&
         winFocused != winpopup &&
         winFocused->GetParent() != winpopup &&
         winFocused != m_combo &&
         winFocused != m_combo->GetButton() // GTK (atleast) requires this
        )
       )
    {
        m_combo->HidePopup(true);
    }

    event.Skip();
}

void wxComboFrameEventHandler::OnMenuEvent( wxMenuEvent& event )
{
    m_combo->HidePopup(true);
    event.Skip();
}

void wxComboFrameEventHandler::OnMouseEvent( wxMouseEvent& event )
{
    m_combo->HidePopup(true);
    event.Skip();
}

void wxComboFrameEventHandler::OnClose( wxCloseEvent& event )
{
    m_combo->HidePopup(true);
    event.Skip();
}

void wxComboFrameEventHandler::OnActivate( wxActivateEvent& event )
{
    m_combo->HidePopup(true);
    event.Skip();
}

void wxComboFrameEventHandler::OnResize( wxSizeEvent& event )
{
    m_combo->HidePopup(true);
    event.Skip();
}

void wxComboFrameEventHandler::OnMove( wxMoveEvent& event )
{
    m_combo->HidePopup(true);
    event.Skip();
}

#endif // !wxUSE_POPUPWIN

// ----------------------------------------------------------------------------
// wxComboPopupWindow is, in essence, wxPopupWindow customized for
// wxComboCtrl.
// ----------------------------------------------------------------------------

class wxComboPopupWindow : public wxComboPopupWindowBase
{
public:

    wxComboPopupWindow( wxComboCtrlBase *parent,
                        int style )
    #if wxUSE_POPUPWIN
                       : wxComboPopupWindowBase(parent,
                                                style | wxPU_CONTAINS_CONTROLS)
    #else
                       : wxComboPopupWindowBase(parent,
                                                wxID_ANY,
                                                wxEmptyString,
                                                wxPoint(-21,-21),
                                                wxSize(20,20),
                                                style)
    #endif
                       , m_combo(parent)
    {
        m_inShow = 0;
    }

#if wxUSE_POPUPWIN
    virtual bool Show( bool show ) override;
    virtual bool ProcessLeftDown(wxMouseEvent& event) override;
protected:
    virtual void OnDismiss() override;
#endif

#ifdef __WXMSW__
    virtual bool MSWHandleMessage(WXLRESULT *result,
                                  WXUINT message,
                                  WXWPARAM wParam,
                                  WXLPARAM lParam) override
    {
        // This is a workaround for a MSW-specific problem: popup windows are
        // created with the TLW and not the combobox itself as their parent and
        // so if the TLW gets destroyed, they're destroyed together with it,
        // but the associated wxComboCtrl may survive if it gets reparented to
        // something else, which is exactly what happens when it's used inside
        // a wxAUI pane. And if this wxComboCtrl is opened again later, it
        // tries to use the existing popup whose HWND had been destroyed, which
        // doesn't work at all.
        //
        // To prevent this from happening, we delete the popup if this happens
        // to force recreating it later.
        if ( message == WM_DESTROY && !m_isBeingDeleted )
        {
            m_combo->OnPopupDestroy();

            // We can't delete it immediately because this object is used after
            // this function returns, so do it slightly later.
            wxWindow* const self = this;
            m_combo->CallAfter([self]()
            {
                // Before really deleting it, reset the HWND which had been
                // already destroyed, to prevent us from trying to destroy it
                // again (which would just fail with an error).
                self->DissociateHandle();

                delete self;
            });
        }

        return wxComboPopupWindowBase::MSWHandleMessage(result, message,
                                                        wParam, lParam);
    }
#endif // __WXMSW__

private:
    // This is the same as our parent, but has the right type, so that we can
    // avoid using casts later.
    wxComboCtrlBase* const m_combo;

    wxByte      m_inShow;

    wxDECLARE_NO_COPY_CLASS(wxComboPopupWindow);
};


#if wxUSE_POPUPWIN
bool wxComboPopupWindow::Show( bool show )
{
    // Guard against recursion
    if ( m_inShow )
        return wxComboPopupWindowBase::Show(show);

    m_inShow++;

    wxPopupTransientWindow* const
        ptw = static_cast<wxPopupTransientWindow*>(this);

    if ( show != ptw->IsShown() )
    {
        if ( show )
            // We used to do wxPopupTransientWindow::Popup here,
            // but this would hide normal Show, which we are
            // also going to need.
            ptw->Show();
        else
            ptw->Dismiss();
    }

    m_inShow--;

    return true;
}

bool wxComboPopupWindow::ProcessLeftDown(wxMouseEvent& event)
{
    return wxPopupTransientWindow::ProcessLeftDown(event);
}

// First thing that happens when a transient popup closes is that this method gets called.
void wxComboPopupWindow::OnDismiss()
{
    m_combo->OnPopupDismiss(true);
}
#endif // wxUSE_POPUPWIN


// ----------------------------------------------------------------------------
// custom event handling for popup window is done in wxComboCtrlBase so we can
// have different types of popup windows.
// ----------------------------------------------------------------------------

void wxComboCtrlBase::OnPopupSize( wxSizeEvent& WXUNUSED(event) )
{
    // Block the event so that the popup control does not get auto-resized.
}

void wxComboCtrlBase::OnPopupKey( wxKeyEvent& event )
{
    // Relay keyboard event to the main child controls
    wxWindowList children = GetPopupWindow()->GetChildren();
    wxWindowList::iterator node = children.begin();
    wxWindow* child = (wxWindow*)*node;
    child->GetEventHandler()->ProcessEvent(event);
}

#if !wxUSE_POPUPWIN
void wxComboCtrlBase::OnPopupActivate( wxActivateEvent& event )
{
    if ( !event.GetActive() )
    {
        // Tell combo control that we are dismissed.
#ifdef __WXMSW__
        // We need to hide the popup but calling ::ShowWindow() directly from WM_ACTIVATE
        // event handler causes some side effects like calling this handler again (Win 7)
        // or setting the focus improperly (Win 10), so postpone it slightly.
        // See wxPopupTransientWindow::MSWHandleMessage().
        CallAfter(&wxComboCtrlBase::Dismiss);
#else // !__WXMSW__
        Dismiss();
#endif // __WXMSW__ / !__WXMSW__
        event.Skip();
    }
}
#endif


// ----------------------------------------------------------------------------
// wxComboPopup
//
// ----------------------------------------------------------------------------

wxComboPopup::~wxComboPopup()
{
}

void wxComboPopup::OnPopup()
{
}

void wxComboPopup::OnDismiss()
{
}

wxComboCtrl* wxComboPopup::GetComboCtrl() const
{
    return wxStaticCast(m_combo, wxComboCtrl);
}

wxSize wxComboPopup::GetAdjustedSize( int minWidth,
                                      int prefHeight,
                                      int WXUNUSED(maxHeight) )
{
    return wxSize(minWidth,prefHeight);
}

void wxComboPopup::DefaultPaintComboControl( wxComboCtrlBase* combo,
                                             wxDC& dc, const wxRect& rect )
{
    if ( combo->GetWindowStyle() & wxCB_READONLY ) // ie. no textctrl
    {
        combo->PrepareBackground(dc,rect,0);

        dc.DrawText( combo->GetValue(),
                     rect.x + combo->m_marginLeft,
                     (rect.height-dc.GetCharHeight())/2 + rect.y );
    }
}

void wxComboPopup::PaintComboControl( wxDC& dc, const wxRect& rect )
{
    DefaultPaintComboControl(m_combo,dc,rect);
}

void wxComboPopup::OnComboKeyEvent( wxKeyEvent& event )
{
    event.Skip();
}

void wxComboPopup::OnComboCharEvent( wxKeyEvent& event )
{
    event.Skip();
}

void wxComboPopup::OnComboDoubleClick()
{
}

void wxComboPopup::SetStringValue( const wxString& WXUNUSED(value) )
{
}

bool wxComboPopup::FindItem(const wxString& WXUNUSED(item),
                            wxString* WXUNUSED(trueItem))
{
    return true;
}

bool wxComboPopup::LazyCreate()
{
    return false;
}

void wxComboPopup::Dismiss()
{
    m_combo->HidePopup(true);
}

void wxComboPopup::DestroyPopup()
{
    // Here we make sure that the popup control's Destroy() gets called.
    // This is necessary for the wxPersistentWindow to work properly.
    wxWindow* popupCtrl = GetControl();
    if ( popupCtrl )
    {
        // While all wxComboCtrl examples have m_popupInterface and
        // popupCtrl as the same class (that will be deleted via the
        // Destroy() call below), it is technically still possible to
        // have implementations where they are in fact not same
        // multiple-inherited class. Here we use C++ RTTI to check for
        // this rare case.
      #ifndef wxNO_RTTI
        // It is probably better to delete m_popupInterface first, so
        // that it retains access to its popup control window.
        if ( dynamic_cast<void*>(this) !=
             dynamic_cast<void*>(popupCtrl) )
            delete this;
      #endif
        popupCtrl->Destroy();
    }
    else
    {
        delete this;
    }
}

// ----------------------------------------------------------------------------
// input handling
// ----------------------------------------------------------------------------

void wxComboCtrlBase::OnTextKey(wxKeyEvent& event)
{
    // Let the wxComboCtrl event handler have a go first.
    wxKeyEvent redirectedEvent(event);
    redirectedEvent.SetId(GetId());
    redirectedEvent.SetEventObject(this);

    if ( !GetEventHandler()->ProcessEvent(redirectedEvent) )
    {
        event.Skip();
    }
}

void wxComboCtrlBase::OnTextFocus(wxFocusEvent& event)
{
    // FIXME: This code does run when control is clicked,
    //        yet on Windows it doesn't select all the text.
    if ( event.GetEventType() == wxEVT_SET_FOCUS &&
        !(GetInternalFlags() & wxCC_NO_TEXT_AUTO_SELECT) )
    {
        if ( GetTextCtrl() )
            GetTextCtrl()->SelectAll();
        else
            SelectAll();
    }

    // Send focus indication to parent.
    // NB: This is needed for cases where the textctrl gets focus
    //     instead of its parent. While this may trigger multiple
    //     wxEVT_SET_FOCUSes (since m_text->SetFocus is called
    //     from combo's focus event handler), they should be quite
    //     harmless.
    wxFocusEvent evt2(event);
    evt2.SetId(GetId());
    evt2.SetEventObject(this);
    GetEventHandler()->ProcessEvent(evt2);

    event.Skip();
}


void wxComboCtrlBase::OnPopupMouseEvent( wxMouseEvent& event )
{
    wxPoint pt = event.GetPosition();
    wxSize sz = GetPopupControl()->GetControl()->GetClientSize();
    int evtType = event.GetEventType();
    bool isInside = pt.x >= 0 && pt.y >= 0 && pt.x < sz.x && pt.y < sz.y;
    bool relayToButton = false;

    event.Skip();

    if ( !isInside || !IsPopupShown() )
    {
        // Mouse is outside the popup or popup is not actually shown (yet)

        if ( evtType == wxEVT_MOTION ||
             evtType == wxEVT_LEFT_DOWN ||
             evtType == wxEVT_LEFT_UP ||
             evtType == wxEVT_RIGHT_DOWN )
        {
            // Block motion and click events outside the popup
            event.Skip(false);
        }
    }
    else
    {
        // Mouse is inside the popup, which is fully shown

        m_beenInsidePopup = true;

        // Do not let the popup control respond to mouse events until
        // mouse press used to display the popup has been lifted. This
        // is important for users with slower mouse fingers or mouse
        // drivers. Note that we have some redundancy here, just in
        // case the popup is some native control that does not emit all
        // mouse event types.
        if ( evtType == wxEVT_MOTION )
        {
            if ( m_blockEventsToPopup )
            {
                if ( event.LeftIsDown() )
                    event.Skip(false);
                else
                    m_blockEventsToPopup = false;
            }
        }
        else if ( evtType == wxEVT_LEFT_DOWN )
        {
            if ( m_blockEventsToPopup )
                m_blockEventsToPopup = false;
        }
        else if ( evtType == wxEVT_LEFT_UP )
        {
            if ( m_blockEventsToPopup )
            {
                // On first left up, stop blocking mouse events (but still
                // block this one)
                m_blockEventsToPopup = false;
                event.Skip(false);

                // Also, this button press was (probably) used to display
                // the popup, so relay it back to the drop-down button
                // (which supposedly originated it). This is necessary to
                // refresh it properly.
                relayToButton = true;
            }
        }
        else if ( m_blockEventsToPopup )
        {
            event.Skip(false);
        }
    }

    //
    // Some mouse events to popup that happen outside it, before cursor
    // has been inside the popup, need to be ignored by it but relayed to
    // the dropbutton.
    //
    if ( evtType == wxEVT_LEFT_UP )
    {
        if ( !IsPopupShown() )
        {
            event.Skip(false);
            relayToButton = true;
        }
        else if ( !isInside && !m_beenInsidePopup )
        {
            // Popup is shown but the cursor is not inside, nor it has been
            relayToButton = true;
        }
    }

    if ( relayToButton )
    {
        wxWindow* btn = GetButton();
        if ( btn )
            btn->GetEventHandler()->ProcessEvent(event);
        else
            // Bypass the event handling mechanism. Using it would be
            // confusing for the platform-specific wxComboCtrl
            // implementations.
            HandleButtonMouseEvent(event, 0);
    }
}

void wxComboCtrlBase::WXHandleDPIChanged(wxDPIChangedEvent& event)
{
    // Ensure it is really recalculated.
    m_btnSize = wxDefaultSize;
    // And calculate it again
    m_btnSize = GetButtonSize();

    event.Skip();
}

// ----------------------------------------------------------------------------
// wxComboCtrlTextCtrl
// ----------------------------------------------------------------------------

class wxComboCtrlTextCtrl : public wxTextCtrl
{
public:
    wxComboCtrlTextCtrl() : wxTextCtrl() { }
    virtual ~wxComboCtrlTextCtrl() { }

    virtual wxWindow *GetMainWindowOfCompositeControl() override
    {
        wxComboCtrl* combo = (wxComboCtrl*) GetParent();

        // Returning this instead of just 'parent' lets FindFocus work
        // correctly even when parent control is a child of a composite
        // generic control (as is case with wxGenericDatePickerCtrl).
        return combo->GetMainWindowOfCompositeControl();
    }
};

// ----------------------------------------------------------------------------
// wxComboCtrlBase
// ----------------------------------------------------------------------------


wxBEGIN_EVENT_TABLE(wxComboCtrlBase, wxControl)
    EVT_SIZE(wxComboCtrlBase::OnSizeEvent)
    EVT_SET_FOCUS(wxComboCtrlBase::OnFocusEvent)
    EVT_KILL_FOCUS(wxComboCtrlBase::OnFocusEvent)
    EVT_IDLE(wxComboCtrlBase::OnIdleEvent)
    //EVT_BUTTON(wxID_ANY,wxComboCtrlBase::OnButtonClickEvent)
    EVT_KEY_DOWN(wxComboCtrlBase::OnKeyEvent)
    EVT_CHAR(wxComboCtrlBase::OnCharEvent)
    EVT_SYS_COLOUR_CHANGED(wxComboCtrlBase::OnSysColourChanged)
wxEND_EVENT_TABLE()


wxIMPLEMENT_ABSTRACT_CLASS(wxComboCtrlBase, wxControl);

void wxComboCtrlBase::Init()
{
    m_winPopup = nullptr;
    m_popup = nullptr;
    m_popupWinState = Hidden;
    m_btn = nullptr;
    m_text = nullptr;
    m_mainWindow = nullptr;
    m_popupInterface = nullptr;

#if !wxUSE_POPUPWIN
    m_toplevEvtHandler = nullptr;
#endif

    m_mainCtrlWnd = this;

    m_heightPopup = -1;
    m_widthMinPopup = -1;
    m_anchorSide = 0;
    m_widthCustomPaint = 0;
    m_widthCustomBorder = 0;

    m_btnState = 0;
    m_btnWidDefault = 0;
    m_blankButtonBg = false;
    m_ignoreEvtText = 0;
    m_btnWid = m_btnHei = -1;
    m_btnSide = wxRIGHT;
    m_btnSpacingX = 0;

    m_extLeft = 0;
    m_extRight = 0;
    m_marginLeft = -1;
    m_iFlags = 0;
    m_textCtrlStyle = 0;
    m_timeCanAcceptClick = 0;

    m_resetFocus = false;
    m_hasTcBgCol = false;

    m_beenInsidePopup = false;

    // Let's make it so that the popup control will not receive mouse
    // events until mouse left button has been up.
    m_blockEventsToPopup = true;

    Bind(wxEVT_DPI_CHANGED, &wxComboCtrlBase::WXHandleDPIChanged, this);
}

bool wxComboCtrlBase::Create(wxWindow *parent,
                             wxWindowID id,
                             const wxString& value,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxValidator& validator,
                             const wxString& name)
{
    if ( !wxControl::Create(parent,
                            id,
                            pos,
                            size,
                            style | wxWANTS_CHARS,
                            validator,
                            name) )
        return false;

    m_valueString = value;

    // Get colours
    OnThemeChange();
    m_marginLeft = GetNativeTextIndent();

    m_iFlags |= wxCC_IFLAG_CREATED;

    // If x and y indicate valid size, wxSizeEvent won't be
    // emitted automatically, so we need to add artificial one.
    if ( size.x > 0 && size.y > 0 )
    {
        wxSizeEvent evt(size,GetId());
        evt.SetEventObject(this);
        GetEventHandler()->AddPendingEvent(evt);
    }

    return true;
}

void
wxComboCtrlBase::SetMainControl(wxWindow* win)
{
    // We can't have both a custom window and a text control, so get rid of the
    // latter if we have it.
    if ( m_text )
    {
        m_text->Destroy();

        // Note that we currently always set it to nullptr, even if the custom
        // window is a (subclass of) wxTextCtrl because our m_text must be a
        // wxComboCtrlTextCtrl for things to work correctly.
        m_text = nullptr;
    }

    // We don't do anything with the previous main window, if any, it's the
    // caller's responsibility to delete or hide it, as needed.
    m_mainWindow = win;
}

void
wxComboCtrlBase::CreateTextCtrl(int style)
{
    // If we're using a custom main window explicitly set using
    // SetMainControl(), don't recreate it and just keep using it.
    if ( m_mainWindow && !m_text )
        return;

    if ( !(m_windowStyle & wxCB_READONLY) )
    {
        if ( m_text )
            m_text->Destroy();

        // wxTE_PROCESS_TAB is needed because on Windows, wxTAB_TRAVERSAL is
        // not used by the wxPropertyGrid and therefore the tab is processed by
        // looking at ancestors to see if they have wxTAB_TRAVERSAL. The
        // navigation event is then sent to the wrong window.
        style |= wxTE_PROCESS_TAB | m_textCtrlStyle;

        if ( HasFlag(wxTE_PROCESS_ENTER) )
            style |= wxTE_PROCESS_ENTER;

        m_text = new wxComboCtrlTextCtrl();
        m_mainWindow = m_text;

        m_text->Create(this, wxID_ANY, m_valueString,
                       wxDefaultPosition, wxSize(10,-1),
                       style);

        // Connecting the events is currently the most reliable way
        m_text->Bind(wxEVT_TEXT, &wxComboCtrlBase::OnTextCtrlEvent, this);
        if ( style & wxTE_PROCESS_ENTER )
        {
            m_text->Bind(wxEVT_TEXT_ENTER, &wxComboCtrlBase::OnTextCtrlEvent, this);
        }

        m_text->Bind(wxEVT_SET_FOCUS, &wxComboCtrlBase::OnTextFocus, this);
        m_text->Bind(wxEVT_KILL_FOCUS, &wxComboCtrlBase::OnTextFocus, this);

        m_text->Bind(wxEVT_KEY_DOWN, &wxComboCtrlBase::OnTextKey, this);
        m_text->Bind(wxEVT_CHAR, &wxComboCtrlBase::OnTextKey, this);
        m_text->Bind(wxEVT_KEY_UP, &wxComboCtrlBase::OnTextKey, this);

        m_text->SetHint(m_hintText);
    }
}

void wxComboCtrlBase::OnThemeChange()
{
    // Because wxComboCtrl has transparent parts on most platforms, we
    // don't want to touch the actual background colour. Instead, we just
    // usually re-obtain m_tcBgCol here.

#if defined(__WXMSW__) || defined(__WXGTK__)
    wxVisualAttributes vattrs = wxComboBox::GetClassDefaultAttributes();
#else
    wxVisualAttributes vattrs;
    vattrs.colFg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    vattrs.colBg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
#endif

    if ( !m_hasTcBgCol )
        m_tcBgCol = vattrs.colBg;

#ifndef __WXMAC__
    // Only change the colours if application has not specified
    // custom ones.
    if ( !m_hasFgCol )
    {
        SetOwnForegroundColour(vattrs.colFg);
    }
    if ( !HasTransparentBackground() )
    {
        SetOwnBackgroundColour(GetParent()->GetBackgroundColour());
    }
#endif // !__WXMAC__
}

wxComboCtrlBase::~wxComboCtrlBase()
{
    if ( HasCapture() )
        ReleaseMouse();

#if !wxUSE_POPUPWIN
    delete ((wxComboFrameEventHandler*)m_toplevEvtHandler);
    m_toplevEvtHandler = nullptr;
#endif

    DestroyPopup();
}


// ----------------------------------------------------------------------------
// geometry stuff
// ----------------------------------------------------------------------------

// Recalculates button and textctrl areas
void wxComboCtrlBase::CalculateAreas( int btnWidth )
{
    wxSize sz = GetClientSize();
    int btnBorder; // border for button only

    // check if button should really be outside the border: we'll do it if
    // its platform default or bitmap+pushbutton background is used, but not if
    // there is vertical size adjustment or horizontal spacing.
    if ( ( (m_iFlags & wxCC_BUTTON_OUTSIDE_BORDER) ||
                (m_bmpNormal.IsOk() && m_blankButtonBg) ) &&
         m_btnSpacingX == 0 &&
         m_btnHei <= 0 )
    {
        m_iFlags |= wxCC_IFLAG_BUTTON_OUTSIDE;
        btnBorder = 0;
    }
    else if ( (m_iFlags & wxCC_BUTTON_COVERS_BORDER) &&
              m_btnSpacingX == 0 && !m_bmpNormal.IsOk() )
    {
        m_iFlags &= ~(wxCC_IFLAG_BUTTON_OUTSIDE);
        btnBorder = 0;
    }
    else
    {
        m_iFlags &= ~(wxCC_IFLAG_BUTTON_OUTSIDE);
        btnBorder = m_widthCustomBorder;
    }

    // Defaul indentation
    if ( m_marginLeft < 0 )
        m_marginLeft = GetNativeTextIndent();

    int butWidth = btnWidth;

    if ( butWidth <= 0 )
        butWidth = m_btnWidDefault;
    else
        m_btnWidDefault = butWidth;

    if ( butWidth <= 0 )
        return;

    int butHeight = sz.y - btnBorder*2;

    // Adjust button width
    if ( m_btnWid > 0 )
        butWidth = m_btnWid;
    else
    {
        // Adjust button width to match aspect ratio
        // (but only if control is smaller than best size).
        int bestHeight = GetBestSize().y;
        int height = GetSize().y;

        if ( height < bestHeight )
        {
            // Make very small buttons square, as it makes
            // them accommodate arrow image better and still
            // looks decent.
            if ( height > FromDIP(18) )
                butWidth = (height*butWidth)/bestHeight;
            else
                butWidth = butHeight;
        }
    }

    // Adjust button height
    if ( m_btnHei > 0 )
        butHeight = m_btnHei;

    // Use size of normal bitmap if...
    //   It is larger
    //   OR
    //   button width is set to default and blank button bg is not drawn
    if ( m_bmpNormal.IsOk() )
    {
        wxSize bmpReqSize = m_bmpNormal.GetPreferredLogicalSizeFor(this);
        int bmpReqWidth = bmpReqSize.GetWidth();
        int bmpReqHeight = bmpReqSize.GetHeight();

        // If drawing blank button background, we need to add some margin.
        if ( m_blankButtonBg )
        {
            bmpReqWidth += BMP_BUTTON_MARGIN*2;
            bmpReqHeight += BMP_BUTTON_MARGIN*2;
        }

        if ( butWidth < bmpReqWidth || ( m_btnWid == 0 && !m_blankButtonBg ) )
            butWidth = bmpReqWidth;
        if ( butHeight < bmpReqHeight || ( m_btnHei == 0 && !m_blankButtonBg ) )
            butHeight = bmpReqHeight;

        // Need to fix height?
        if ( (sz.y-(m_widthCustomBorder*2)) < butHeight && btnWidth == 0 )
        {
            int newY = butHeight+(m_widthCustomBorder*2);
            SetClientSize(wxDefaultCoord,newY);
            if ( m_bmpNormal.IsOk() || m_btnArea.width != butWidth || m_btnArea.height != butHeight )
                m_iFlags |= wxCC_IFLAG_HAS_NONSTANDARD_BUTTON;
            else
                m_iFlags &= ~wxCC_IFLAG_HAS_NONSTANDARD_BUTTON;

            sz.y = newY;
        }
    }

    int butAreaWid = butWidth + (m_btnSpacingX*2);

    m_btnSize.x = butWidth;
    m_btnSize.y = butHeight;

    m_btnArea.x = ( m_btnSide==wxRIGHT ? sz.x - butAreaWid - btnBorder : btnBorder );
    m_btnArea.y = btnBorder + FOCUS_RING;
    m_btnArea.width = butAreaWid;
    m_btnArea.height = sz.y - ((btnBorder+FOCUS_RING)*2);

    m_tcArea.x = ( m_btnSide==wxRIGHT ? 0 : butAreaWid ) + m_widthCustomBorder;
    m_tcArea.y = m_widthCustomBorder + FOCUS_RING;
    m_tcArea.width = sz.x - butAreaWid - (m_widthCustomBorder*2) - FOCUS_RING;
    m_tcArea.height = sz.y - ((m_widthCustomBorder+FOCUS_RING)*2);

/*
    if ( m_mainWindow )
    {
        ::wxMessageBox(wxString::Format(wxT("ButtonArea (%i,%i,%i,%i)\n"),m_btnArea.x,m_btnArea.y,m_btnArea.width,m_btnArea.height) +
                       wxString::Format(wxT("TextCtrlArea (%i,%i,%i,%i)"),m_tcArea.x,m_tcArea.y,m_tcArea.width,m_tcArea.height));
    }
*/
}

void wxComboCtrlBase::PositionTextCtrl( int textCtrlXAdjust, int textCtrlYAdjust )
{
    if ( !m_mainWindow || !m_mainWindow->GetHandle() )
        return;

    wxSize sz = GetClientSize();

    // This function actually positions any main window, not just a text
    // control, but it only does any special adjustments for m_text.
    if ( m_text && (m_text->GetWindowStyleFlag() & wxBORDER_MASK) == wxNO_BORDER )
    {
        int x;

        if ( !m_widthCustomPaint )
        {
            // No special custom paint area - we can use 0 left margin
            // with wxTextCtrl.
            if ( m_text->SetMargins(0) )
                textCtrlXAdjust = 0;
            x = m_tcArea.x + m_marginLeft + textCtrlXAdjust;
        }
        else
        {
            // There is special custom paint area - it is better to
            // use some margin with the wxTextCtrl.
            m_text->SetMargins(m_marginLeft);
            x = m_tcArea.x + m_widthCustomPaint +
                m_marginLeft + textCtrlXAdjust;
        }

        // Centre textctrl vertically
        int tcSizeY = m_text->GetBestSize().y;
        int diff0 = sz.y - tcSizeY;
        int y = textCtrlYAdjust + (diff0/2);

        if ( y < m_widthCustomBorder )
            y = m_widthCustomBorder;

        m_text->SetSize(x,
                        y,
                        m_tcArea.width - m_tcArea.x - x,
                        -1 );

        // Make sure textctrl doesn't exceed the bottom custom border
        wxSize tsz = m_text->GetSize();
        int diff1 = (y + tsz.y) - (sz.y - m_widthCustomBorder);
        if ( diff1 >= 0 )
        {
            tsz.y = tsz.y - diff1 - 1;
            m_text->SetSize(tsz);
        }
    }
    else
    {
        // If the main window has border or is not a text control at all, have
        // it fill the entire available space.
        int w = m_tcArea.width - m_widthCustomPaint;
        if (w < 0) w = 0;
        m_mainWindow->SetSize( m_tcArea.x + m_widthCustomPaint,
                         m_tcArea.y,
                         w,
                         m_tcArea.height );
    }
}

wxSize wxComboCtrlBase::DoGetBestSize() const
{
    int width = m_mainWindow && m_mainWindow->GetHandle()
                    ? m_mainWindow->GetBestSize().x : FromDIP(80);

    return GetSizeFromTextSize(width);
}

wxSize wxComboCtrlBase::DoGetSizeFromTextSize(int xlen, int ylen) const
{
    // Calculate close-to-native control height

    int fhei;

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    fhei = EDIT_HEIGHT_FROM_CHAR_HEIGHT(GetCharHeight());
#elif defined(__WXGTK__) && !defined(__WXUNIVERSAL__)
    // Control creation is not entirely cheap, so cache the heights to
    // avoid repeatedly creating dummy controls:
    static wxString s_last_font;
    static int s_last_fhei = -1;
    wxString fontdesc;
    if ( m_font.IsOk() )
        fontdesc = m_font.GetNativeFontInfoDesc();
    if ( s_last_fhei != -1 && fontdesc == s_last_font )
    {
        fhei = s_last_fhei;
    }
    else
    {
        wxComboBox* cb = new wxComboBox;
#ifndef __WXGTK3__
        // GTK3 returns zero for the preferred size of a hidden widget
        cb->Hide();
#endif
        cb->Create(const_cast<wxComboCtrlBase*>(this), wxID_ANY);
        if ( m_font.IsOk() )
            cb->SetFont(m_font);
        s_last_font = fontdesc;
        s_last_fhei = fhei = cb->GetBestSize().y;
        cb->Destroy();
    }
#else
    if ( m_font.IsOk() )
        fhei = (m_font.GetPointSize()*2) + 5;
    else if ( wxNORMAL_FONT->IsOk() )
        fhei = (wxNORMAL_FONT->GetPointSize()*2) + 5;
    else
        fhei = 22;
#endif // only for wxComboBox on MSW or GTK

    // Need to force height to accommodate bitmap?
    int btnSizeY = m_btnSize.y;
    if ( m_bmpNormal.IsOk() && fhei < btnSizeY )
        fhei = btnSizeY;

    // Control height doesn't depend on border
/*
    // Add border
    int border = m_windowStyle & wxBORDER_MASK;
    if ( border == wxSIMPLE_BORDER )
        fhei += 2;
    else if ( border == wxNO_BORDER )
        fhei += (m_widthCustomBorder*2);
    else
        // Sunken etc.
        fhei += 4;
*/

#ifdef __WXMAC__
    // these are the numbers from the HIG:
    switch ( m_windowVariant )
    {
        case wxWINDOW_VARIANT_NORMAL:
        default :
            fhei = 22;
            break;
        case wxWINDOW_VARIANT_SMALL:
            fhei = 19;
            break;
        case wxWINDOW_VARIANT_MINI:
            fhei = 15;
            break;
    }
#endif

    fhei += 2 * FOCUS_RING;

    // Calculate width
    int fwid = GetNativeTextIndent() + xlen + FOCUS_RING + COMBO_MARGIN + m_btnArea.width;

    // Add the margins we have previously set
    wxPoint marg( GetMargins() );
    fwid += wxMax(0, marg.x);
    fhei += wxMax(0, marg.y);

    if ( ylen > 0 )
        fhei += ylen - GetCharHeight();

    return wxSize(fwid, fhei);
}

void wxComboCtrlBase::OnSizeEvent( wxSizeEvent& event )
{
    if ( !IsCreated() )
        return;

    // defined by actual wxComboCtrls
    OnResize();

    event.Skip();
}

// ----------------------------------------------------------------------------
// standard operations
// ----------------------------------------------------------------------------

bool wxComboCtrlBase::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
        return false;

    if ( m_btn )
        m_btn->Enable(enable);
    if ( m_mainWindow )
        m_mainWindow->Enable(enable);

    Refresh();

    return true;
}

bool wxComboCtrlBase::Show(bool show)
{
    if ( !wxControl::Show(show) )
        return false;

    if (m_btn)
        m_btn->Show(show);

    if (m_mainWindow)
        m_mainWindow->Show(show);

    return true;
}

bool wxComboCtrlBase::SetFont ( const wxFont& font )
{
    if ( !wxControl::SetFont(font) )
        return false;

    if ( m_mainWindow )
    {
        // Without hiding the wxTextCtrl there would be some
        // visible 'flicker' (at least on Windows XP).
        m_mainWindow->Hide();
        m_mainWindow->SetFont(font);
        OnResize();
        m_mainWindow->Show();
    }

    return true;
}

#if wxUSE_TOOLTIPS
void wxComboCtrlBase::DoSetToolTip(wxToolTip *tooltip)
{
    wxControl::DoSetToolTip(tooltip);

    // Set tool tip for button and text box
    if ( tooltip )
    {
        const wxString &tip = tooltip->GetTip();
        if ( m_mainWindow ) m_mainWindow->SetToolTip(tip);
        if ( m_btn ) m_btn->SetToolTip(tip);
    }
    else
    {
        if ( m_mainWindow ) m_mainWindow->SetToolTip( nullptr );
        if ( m_btn ) m_btn->SetToolTip( nullptr );
    }
}
#endif // wxUSE_TOOLTIPS

bool wxComboCtrlBase::SetForegroundColour(const wxColour& colour)
{
    if ( wxControl::SetForegroundColour(colour) )
    {
        if ( m_mainWindow )
            m_mainWindow->SetForegroundColour(colour);
        return true;
    }
    return false;
}

bool wxComboCtrlBase::SetBackgroundColour(const wxColour& colour)
{
    if ( m_mainWindow )
        m_mainWindow->SetBackgroundColour(colour);
    m_tcBgCol = colour;
    m_hasTcBgCol = true;
    return true;
}

wxColour wxComboCtrlBase::GetBackgroundColour() const
{
    if ( m_mainWindow )
        return m_mainWindow->GetBackgroundColour();
    return m_tcBgCol;
}

// ----------------------------------------------------------------------------
// painting
// ----------------------------------------------------------------------------

#if (!defined(__WXMSW__)) || defined(__WXUNIVERSAL__)
// prepare combo box background on area in a way typical on platform
void wxComboCtrlBase::PrepareBackground( wxDC& dc, const wxRect& rect, int flags ) const
{
    wxSize sz = GetClientSize();
    bool isEnabled;
    bool doDrawFocusRect; // also selected

    // For smaller size control (and for disabled background) use less spacing
    int focusSpacingX;
    int focusSpacingY;

    if ( !(flags & wxCONTROL_ISSUBMENU) )
    {
        // Drawing control
        isEnabled = IsEnabled();
        doDrawFocusRect = ShouldDrawFocus() && !(m_iFlags & wxCC_FULL_BUTTON);

        // Windows-style: for smaller size control (and for disabled background) use less spacing
        focusSpacingX = isEnabled ? 2 : 1;
        focusSpacingY = sz.y > (GetCharHeight()+2) && isEnabled ? 2 : 1;
    }
    else
    {
        // Drawing a list item
        isEnabled = true; // they are never disabled
        doDrawFocusRect = (flags & wxCONTROL_SELECTED) != 0;

        focusSpacingX = 0;
        focusSpacingY = 0;
    }

    // Set the background sub-rectangle for selection, disabled etc
    wxRect selRect(rect);
    selRect.y += focusSpacingY;
    selRect.height -= (focusSpacingY*2);

    int wcp = 0;

    if ( !(flags & wxCONTROL_ISSUBMENU) )
        wcp += m_widthCustomPaint;

    selRect.x += wcp + focusSpacingX;
    selRect.width -= wcp + (focusSpacingX*2);

    wxColour bgCol;
    wxColour fgCol;

    bool doDrawSelRect = true;

    // Determine foreground colour
    if ( isEnabled )
    {
        if ( doDrawFocusRect )
        {
            fgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
        }
        else if ( m_hasFgCol )
        {
            // Honour the custom foreground colour
            fgCol = GetForegroundColour();
        }
        else
        {
            fgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
        }
    }
    else
    {
        fgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
    }

    // Determine background colour
    if ( isEnabled )
    {
        if ( doDrawFocusRect )
        {
            bgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        }
        else if ( m_hasTcBgCol )
        {
            // Honour the custom background colour
            bgCol = m_tcBgCol;
        }
        else
        {
#ifndef __WXMAC__  // see note in OnThemeChange
            doDrawSelRect = false;
            bgCol = GetBackgroundColour();
#else
            bgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
#endif
        }
    }
    else
    {
#ifndef __WXMAC__  // see note in OnThemeChange
        bgCol = GetBackgroundColour();
#else
        bgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
#endif
    }

    dc.SetTextForeground( fgCol );
    dc.SetBrush( bgCol );
    if ( doDrawSelRect )
    {
        dc.SetPen( bgCol );
        dc.DrawRectangle( selRect );
    }

    // Don't clip exactly to the selection rectangle so we can draw
    // to the non-selected area in front of it.
    wxRect clipRect(rect.x,rect.y,
                    (selRect.x+selRect.width)-rect.x,rect.height);
    dc.SetClippingRegion(clipRect);
}
#else
// Save the library size a bit for platforms that re-implement this.
void wxComboCtrlBase::PrepareBackground( wxDC&, const wxRect&, int ) const
{
}
#endif

void wxComboCtrlBase::DrawButton( wxDC& dc, const wxRect& rect, int flags )
{
    int drawState = m_btnState;

    if ( (m_iFlags & wxCC_BUTTON_STAYS_DOWN) &&
         GetPopupWindowState() >= Animating )
        drawState |= wxCONTROL_PRESSED;

    wxRect drawRect(rect.x+m_btnSpacingX,
                    rect.y+((rect.height-m_btnSize.y)/2),
                    m_btnSize.x,
                    m_btnSize.y);

    // Make sure area is not larger than the control
    if ( drawRect.y < rect.y )
        drawRect.y = rect.y;
    if ( drawRect.height > rect.height )
        drawRect.height = rect.height;

    bool enabled = IsEnabled();

    if ( !enabled )
        drawState |= wxCONTROL_DISABLED;

    // Need to clear button background even if m_btn is present
    // and also when using custom bitmap for the button
    if ( (flags & Button_PaintBackground) &&
            (!HasTransparentBackground() ||
             !(m_iFlags & wxCC_IFLAG_BUTTON_OUTSIDE)) )
    {
        wxColour bgCol;

        if ( m_iFlags & wxCC_IFLAG_BUTTON_OUTSIDE )
            bgCol = GetParent()->GetBackgroundColour();
        else
            bgCol = GetBackgroundColour();

        dc.SetBrush(bgCol);
        dc.SetPen(bgCol);
        dc.DrawRectangle(rect);
    }

    if ( !m_bmpNormal.IsOk() )
    {
        if ( flags & Button_BitmapOnly )
            return;

        // Draw standard button
        wxRendererNative::Get().DrawComboBoxDropButton(this,
                                                       dc,
                                                       drawRect,
                                                       drawState);
    }
    else
    {
        // Draw bitmap

        wxBitmapBundle* pBmp;

        if ( !enabled )
            pBmp = &m_bmpDisabled;
        else if ( m_btnState & wxCONTROL_PRESSED )
            pBmp = &m_bmpPressed;
        else if ( m_btnState & wxCONTROL_CURRENT )
            pBmp = &m_bmpHover;
        else
            pBmp = &m_bmpNormal;

        if ( m_blankButtonBg )
        {
            if ( !(flags & Button_BitmapOnly) )
            {
                wxRendererNative::Get().DrawPushButton(this,
                                                       dc,
                                                       drawRect,
                                                       drawState);
            }
        }

        // Draw bitmap centered in drawRect
        wxBitmap currentBmp = pBmp->GetBitmapFor(this);
        dc.DrawBitmap(currentBmp,
                      drawRect.x + (drawRect.width-currentBmp.GetLogicalWidth())/2,
                      drawRect.y + (drawRect.height-currentBmp.GetLogicalHeight())/2,
                      true);
    }
}

void wxComboCtrlBase::RecalcAndRefresh()
{
    if ( IsCreated() )
    {
        wxSizeEvent evt(GetSize(),GetId());
        evt.SetEventObject(this);
        GetEventHandler()->ProcessEvent(evt);
        Refresh();
    }
}

// ----------------------------------------------------------------------------
// miscellaneous event handlers
// ----------------------------------------------------------------------------

void wxComboCtrlBase::OnTextCtrlEvent(wxCommandEvent& event)
{
    // Avoid infinite recursion
    if ( event.GetEventObject() == this )
    {
        event.Skip();
        return;
    }

    if ( event.GetEventType() == wxEVT_TEXT )
    {
        if ( m_ignoreEvtText > 0 )
        {
            m_ignoreEvtText--;
            return;
        }
    }

    // For safety, completely re-create a new wxCommandEvent
    wxCommandEvent evt2(event);
    evt2.SetId(GetId());
    evt2.SetEventObject(this);
    HandleWindowEvent(evt2);

    event.StopPropagation();
}

// call if cursor is on button area or mouse is captured for the button
bool wxComboCtrlBase::HandleButtonMouseEvent( wxMouseEvent& event,
                                              int flags )
{
    int type = event.GetEventType();

    if ( type == wxEVT_MOTION )
    {
        if ( (flags & wxCC_MF_ON_BUTTON) &&
             IsPopupWindowState(Hidden) )
        {
            if ( !(m_btnState & wxCONTROL_CURRENT) )
            {
                // Mouse hover begins
                m_btnState |= wxCONTROL_CURRENT;
                if ( HasCapture() ) // Retain pressed state.
                    m_btnState |= wxCONTROL_PRESSED;
                Refresh();
            }
        }
        else if ( (m_btnState & wxCONTROL_CURRENT) )
        {
            // Mouse hover ends
            m_btnState &= ~(wxCONTROL_CURRENT|wxCONTROL_PRESSED);
            Refresh();
        }
    }
    else if ( type == wxEVT_LEFT_DOWN || type == wxEVT_LEFT_DCLICK )
    {
        if ( flags & (wxCC_MF_ON_CLICK_AREA|wxCC_MF_ON_BUTTON) )
        {
            m_btnState |= wxCONTROL_PRESSED;
            Refresh();

            if ( !(m_iFlags & wxCC_POPUP_ON_MOUSE_UP) )
                OnButtonClick();
            else
                // If showing popup now, do not capture mouse or there will be interference
                CaptureMouse();
        }
    }
    else if ( type == wxEVT_LEFT_UP )
    {

        // Only accept event if mouse was left-press was previously accepted
        if ( HasCapture() )
            ReleaseMouse();

        if ( m_btnState & wxCONTROL_PRESSED )
        {
            // If mouse was inside, fire the click event.
            if ( m_iFlags & wxCC_POPUP_ON_MOUSE_UP )
            {
                if ( flags & (wxCC_MF_ON_CLICK_AREA|wxCC_MF_ON_BUTTON) )
                    OnButtonClick();
            }

            m_btnState &= ~(wxCONTROL_PRESSED);
            Refresh();
        }
    }
    else if ( type == wxEVT_LEAVE_WINDOW )
    {
        if ( m_btnState & (wxCONTROL_CURRENT|wxCONTROL_PRESSED) )
        {
            m_btnState &= ~(wxCONTROL_CURRENT);

            // Mouse hover ends
            if ( IsPopupWindowState(Hidden) )
            {
                m_btnState &= ~(wxCONTROL_PRESSED);
                Refresh();
            }
        }
    }
    else
        return false;

    // Never have 'hot' state when popup is being shown
    // (this is mostly needed because of the animation).
    if ( !IsPopupWindowState(Hidden) )
        m_btnState &= ~wxCONTROL_CURRENT;

    return true;
}

// returns true if event was consumed or filtered
bool wxComboCtrlBase::PreprocessMouseEvent( wxMouseEvent& event,
                                            int WXUNUSED(flags) )
{
    wxMilliClock_t t = ::wxGetLocalTimeMillis();
    int evtType = event.GetEventType();

#if !wxUSE_POPUPWIN
    if ( IsPopupWindowState(Visible) &&
         ( evtType == wxEVT_LEFT_DOWN || evtType == wxEVT_RIGHT_DOWN ) )
    {
        HidePopup(true);
        return true;
    }
#endif

    // Filter out clicks on button immediately after popup dismiss
    if ( evtType == wxEVT_LEFT_DOWN && t < m_timeCanAcceptClick )
    {
        event.SetEventType(0);
        return true;
    }

    return false;
}

void wxComboCtrlBase::HandleNormalMouseEvent( wxMouseEvent& event )
{
    int evtType = event.GetEventType();

    if ( (evtType == wxEVT_LEFT_DOWN || evtType == wxEVT_LEFT_DCLICK) &&
         (m_windowStyle & wxCB_READONLY) )
    {
        if ( GetPopupWindowState() >= Animating )
        {
    #if !wxUSE_POPUPWIN
            // Click here always hides the popup.
            HidePopup(true);
    #endif
        }
        else
        {
            if ( !(m_windowStyle & wxCC_SPECIAL_DCLICK) )
            {
                // In read-only mode, clicking the text is the
                // same as clicking the button.
                OnButtonClick();
            }
            else if ( /*evtType == wxEVT_LEFT_UP || */evtType == wxEVT_LEFT_DCLICK )
            {
                //if ( m_popupInterface->CycleValue() )
                //    Refresh();
                if ( m_popupInterface )
                    m_popupInterface->OnComboDoubleClick();
            }
        }
    }
    else if ( evtType == wxEVT_MOUSEWHEEL )
    {
        if ( IsPopupShown() )
        {
            // relay (some) mouse events to the popup
            m_popup->GetEventHandler()->ProcessEvent(event);
        }
        else if ( event.GetWheelAxis() == 0 &&
                  event.GetWheelRotation() != 0 &&
                  event.GetModifiers() == 0 )
        {
            // Translate mousewheel actions into key up/down. This is
            // the simplest way of getting native behaviour: scrolling the
            // wheel moves selection up/down by one item.
            wxKeyEvent kevent(wxEVT_KEY_DOWN);
            kevent.m_keyCode = event.GetWheelRotation() > 0
                               ? WXK_UP
                               : WXK_DOWN;

            if (!GetEventHandler()->ProcessEvent(kevent))
                event.Skip();
        }
        else
        {
            event.Skip();
        }
    }
    else if ( evtType )
    {
        event.Skip();
    }
}

void wxComboCtrlBase::OnKeyEvent(wxKeyEvent& event)
{
    if ( IsPopupShown() )
    {
        // pass it to the popped up control
        GetPopupControl()->GetControl()->GetEventHandler()->ProcessEvent(event);
    }
    else // no popup
    {
        wxWindow* mainCtrl = GetMainWindowOfCompositeControl();

        if ( mainCtrl->GetParent()->HasFlag(wxTAB_TRAVERSAL) )
        {
            if ( mainCtrl->HandleAsNavigationKey(event) )
                return;
        }

        if ( IsKeyPopupToggle(event) )
        {
            OnButtonClick();
            return;
        }

        int comboStyle = GetWindowStyle();
        wxComboPopup* popupInterface = GetPopupControl();

        if ( !popupInterface )
        {
            event.Skip();
            return;
        }

        int keycode = event.GetKeyCode();

        if ( (comboStyle & wxCB_READONLY) ||
             (keycode != WXK_RIGHT && keycode != WXK_LEFT) )
        {
            popupInterface->OnComboKeyEvent(event);
        }
        else
            event.Skip();
    }
}

void wxComboCtrlBase::OnCharEvent(wxKeyEvent& event)
{
    if ( IsPopupShown() )
    {
        // pass it to the popped up control
        GetPopupControl()->GetControl()->GetEventHandler()->ProcessEvent(event);
    }
    else // no popup
    {
        wxComboPopup* popupInterface = GetPopupControl();
        if ( popupInterface )
        {
            popupInterface->OnComboCharEvent(event);
        }
        else
        {
            event.Skip();
        }
    }
}

void wxComboCtrlBase::OnFocusEvent( wxFocusEvent& event )
{
    // Always let default handling of focus events to take place.
    event.Skip();

    // On Mac, setting focus here led to infinite recursion so
    // m_resetFocus is used as a guard

    if ( event.GetEventType() == wxEVT_SET_FOCUS )
    {
        if ( !m_resetFocus && m_mainWindow && !m_mainWindow->HasFocus() )
        {
            m_resetFocus = true;
            m_mainWindow->SetFocus();
            m_resetFocus = false;
        }
    }

    Refresh();
}

void wxComboCtrlBase::OnIdleEvent( wxIdleEvent& WXUNUSED(event) )
{
    if ( m_resetFocus )
    {
        m_resetFocus = false;
        if ( m_mainWindow )
            m_mainWindow->SetFocus();
    }
}

void wxComboCtrlBase::OnSysColourChanged(wxSysColourChangedEvent& WXUNUSED(event))
{
    OnThemeChange();
    // left margin may also have changed
    if ( !(m_iFlags & wxCC_IFLAG_LEFT_MARGIN_SET) )
        m_marginLeft = GetNativeTextIndent();
    RecalcAndRefresh();
}

// ----------------------------------------------------------------------------
// popup handling
// ----------------------------------------------------------------------------

// Create popup window and the child control
void wxComboCtrlBase::CreatePopup()
{
    wxComboPopup* popupInterface = m_popupInterface;

    if ( !m_winPopup )
    {
        m_winPopup = new wxComboPopupWindow( this, wxNO_BORDER );

        m_winPopup->Bind(wxEVT_KEY_DOWN, &wxComboCtrlBase::OnPopupKey, this);
        m_winPopup->Bind(wxEVT_CHAR, &wxComboCtrlBase::OnPopupKey, this);
        m_winPopup->Bind(wxEVT_KEY_UP, &wxComboCtrlBase::OnPopupKey, this);
#if !wxUSE_POPUPWIN
        m_winPopup->Bind(wxEVT_ACTIVATE, &wxComboCtrlBase::OnPopupActivate, this);
#endif
        m_winPopup->Bind(wxEVT_SIZE, &wxComboCtrlBase::OnPopupSize, this);
    }

    popupInterface->Create(m_winPopup);
    m_popup = popupInterface->GetControl();

    // Bind all mouse events, as used to be done by EVT_MOUSE_EVENTS() event
    // table macro, to this handler.
    const wxEventTypeTag<wxMouseEvent> allMouseEventTypes[] =
    {
        wxEVT_LEFT_DOWN,
        wxEVT_LEFT_UP,
        wxEVT_LEFT_DCLICK,
        wxEVT_MIDDLE_DOWN,
        wxEVT_MIDDLE_UP,
        wxEVT_MIDDLE_DCLICK,
        wxEVT_RIGHT_DOWN,
        wxEVT_RIGHT_UP,
        wxEVT_RIGHT_DCLICK,
        wxEVT_AUX1_DOWN,
        wxEVT_AUX1_UP,
        wxEVT_AUX1_DCLICK,
        wxEVT_AUX2_DOWN,
        wxEVT_AUX2_UP,
        wxEVT_AUX2_DCLICK,
        wxEVT_MOTION,
        wxEVT_LEAVE_WINDOW,
        wxEVT_ENTER_WINDOW,
        wxEVT_MOUSEWHEEL,
        wxEVT_MAGNIFY,
    };

    for ( size_t n = 0; n < WXSIZEOF(allMouseEventTypes); ++n )
    {
        m_popup->Bind(allMouseEventTypes[n],
                      &wxComboCtrlBase::OnPopupMouseEvent, this);
    }

    // This may be helpful on some platforms
    //   (eg. it bypasses a wxGTK popupwindow bug where
    //    window is not initially hidden when it should be)
    m_winPopup->Hide();

    popupInterface->m_iFlags |= wxCP_IFLAG_CREATED;
}

// Destroy popup window and the child control
void wxComboCtrlBase::DestroyPopup()
{
    HidePopup(true);

    if ( m_popupInterface )
    {
        // NB: DestroyPopup() performs 'delete this'.
        m_popupInterface->DestroyPopup();
        m_popupInterface = nullptr;
    }

    if ( m_winPopup )
    {
        m_winPopup->Destroy();
        m_winPopup = nullptr;
    }

    m_popup = nullptr;
}

void wxComboCtrlBase::DoSetPopupControl(wxComboPopup* iface)
{
    wxCHECK_RET( iface, wxT("no popup interface set for wxComboCtrl") );

    DestroyPopup();

    iface->InitBase(this);
    iface->Init();

    m_popupInterface = iface;

    if ( !iface->LazyCreate() )
    {
        CreatePopup();
    }
    else
    {
        m_popup = nullptr;
    }

    // This must be done after creation
    if ( !m_valueString.empty() )
    {
        iface->SetStringValue(m_valueString);
        //Refresh();
    }
}

// Ensures there is atleast the default popup
void wxComboCtrlBase::EnsurePopupControl()
{
    if ( !m_popupInterface )
        SetPopupControl(nullptr);
}

void wxComboCtrlBase::OnButtonClick()
{
    // Derived classes can override this method for totally custom
    // popup action
    switch ( GetPopupWindowState() )
    {
        case Hidden:
        {
            Popup();
            break;
        }

        case Animating:
        case Visible:
        {
            HidePopup(true);
            break;
        }
    }
}

void wxComboCtrlBase::Popup()
{
    wxCommandEvent event(wxEVT_COMBOBOX_DROPDOWN, GetId());
    event.SetEventObject(this);
    HandleWindowEvent(event);

    ShowPopup();
}

void wxComboCtrlBase::ShowPopup()
{
    EnsurePopupControl();
    wxCHECK_RET( !IsPopupWindowState(Visible), wxT("popup window already shown") );

    if ( IsPopupWindowState(Animating) )
        return;

    SetFocus();

    // Space above and below
    int screenHeight;
    wxPoint scrPos;
    int spaceAbove;
    int spaceBelow;
    int maxHeightPopup;
    wxSize ctrlSz = GetSize();

    wxRect displayRect = wxDisplay(this).GetGeometry();
    screenHeight = displayRect.GetHeight();
    scrPos = GetScreenPosition();

    spaceAbove = scrPos.y - displayRect.GetY();
    spaceBelow = screenHeight - spaceAbove - ctrlSz.y;

    maxHeightPopup = spaceBelow;
    if ( spaceAbove > spaceBelow )
        maxHeightPopup = spaceAbove;

    // Width
    int widthPopup = ctrlSz.x + m_extLeft + m_extRight;

    if ( widthPopup < m_widthMinPopup )
        widthPopup = m_widthMinPopup;

    wxWindow* winPopup = m_winPopup;
    wxWindow* popup;

    // Need to disable tab traversal of parent
    //
    // NB: This is to fix a bug in wxMSW. In theory it could also be fixed
    //     by, for instance, adding check to window.cpp:wxWindowMSW::MSWProcessMessage
    //     that if transient popup is open, then tab traversal is to be ignored.
    //     However, I think this code would still be needed for cases where
    //     transient popup doesn't work yet.
    wxWindow* mainCtrl = GetMainWindowOfCompositeControl();
    wxWindow* parent = mainCtrl->GetParent();
    int parentFlags = parent->GetWindowStyle();
    if ( parentFlags & wxTAB_TRAVERSAL )
    {
        parent->SetWindowStyle( parentFlags & ~(wxTAB_TRAVERSAL) );
        m_iFlags |= wxCC_IFLAG_PARENT_TAB_TRAVERSAL;
    }

    if ( !winPopup )
    {
        CreatePopup();
        winPopup = m_winPopup;
        popup = m_popup;
    }
    else
    {
        popup = m_popup;
    }

    winPopup->Enable();

    wxASSERT( !m_popup || m_popup == popup ); // Consistency check.

    wxSize adjustedSize = m_popupInterface->GetAdjustedSize(widthPopup,
                                                            m_heightPopup<=0?DEFAULT_POPUP_HEIGHT:m_heightPopup,
                                                            maxHeightPopup);

    popup->SetSize(adjustedSize);
    popup->Move(0,0);
    m_popupInterface->OnPopup();

    //
    // Reposition and resize popup window
    //

    wxSize szp = popup->GetSize();

    int popupX;
    int popupY = scrPos.y + ctrlSz.y;

    // Default anchor is wxLEFT
    int anchorSide = m_anchorSide;
    if ( !anchorSide )
        anchorSide = wxLEFT;

    int rightX = scrPos.x + ctrlSz.x + m_extRight - szp.x;
    int leftX = scrPos.x - m_extLeft;

    if ( wxTheApp->GetLayoutDirection() == wxLayout_RightToLeft )
        leftX -= ctrlSz.x;

    int screenWidth = displayRect.GetWidth();

    // If there is not enough horizontal space, anchor on the other side.
    // If there is no space even then, place the popup at x 0.
    if ( anchorSide == wxRIGHT )
    {
        if ( rightX < 0 )
        {
            if ( (leftX+szp.x) < screenWidth )
                anchorSide = wxLEFT;
            else
                anchorSide = 0;
        }
    }
    else
    {
        if ( (leftX+szp.x) >= screenWidth )
        {
            if ( rightX >= 0 )
                anchorSide = wxRIGHT;
            else
                anchorSide = 0;
        }
    }

    // Select x coordinate according to the anchor side
    if ( anchorSide == wxRIGHT )
        popupX = rightX;
    else if ( anchorSide == wxLEFT )
        popupX = leftX;
    else
        popupX = 0;

    int showFlags = CanDeferShow;

    if ( spaceBelow < szp.y )
    {
        popupY = scrPos.y - szp.y;
        showFlags |= ShowAbove;
    }

#if !wxUSE_POPUPWIN
    // Put top level window event handler into place
    if ( !m_toplevEvtHandler )
        m_toplevEvtHandler = new wxComboFrameEventHandler(this);

    wxWindow* toplev = ::wxGetTopLevelParent( this );
    wxASSERT( toplev );
    ((wxComboFrameEventHandler*)m_toplevEvtHandler)->OnPopup();
    toplev->PushEventHandler( m_toplevEvtHandler );
#endif

    // Set string selection (must be this way instead of SetStringSelection)
    if ( m_text )
    {
        if ( !(m_iFlags & wxCC_NO_TEXT_AUTO_SELECT) )
            m_text->SelectAll();

        m_popupInterface->SetStringValue( m_text->GetValue() );
    }
    else
    {
        // This is neede since focus/selection indication may change when popup is shown
        Refresh();
    }

    // This must be after SetStringValue
    m_popupWinState = Animating;

    wxRect popupWinRect( popupX, popupY, szp.x, szp.y );

    m_popup = popup;
    if ( (m_iFlags & wxCC_IFLAG_DISABLE_POPUP_ANIM) ||
         AnimateShow( popupWinRect, showFlags ) )
    {
        DoShowPopup( popupWinRect, showFlags );
    }
}


#ifdef __WXMAC__
bool wxComboCtrlBase::AnimateShow( const wxRect& rect, int WXUNUSED(flags) )
{
    // Overridden AnimateShow() will call Raise() and ShowWithEffect() so do
    // here to avoid duplication. Raise and Show are needed for some contained
    // control's scrollbars, selection highlights, hit-test accuracy and popup
    // close via left mousedown when the mouse is not over the parent app.
    if ( GetPopupWindow() )
    {
        GetPopupWindow()->SetSize(rect);
        GetPopupWindow()->Raise();
        GetPopupWindow()->Show();
    }
    return true;
}
#else
bool wxComboCtrlBase::AnimateShow( const wxRect& WXUNUSED(rect), int WXUNUSED(flags) )
{
    return true;
}
#endif

void wxComboCtrlBase::DoShowPopup( const wxRect& rect, int WXUNUSED(flags) )
{
    wxWindow* winPopup = m_winPopup;

    if ( IsPopupWindowState(Animating) )
    {
        // Make sure the popup window is shown in the right position.
        // Should not matter even if animation already did this.

        // Some platforms (GTK) may like SetSize and Move to be separate
        // (though the bug was probably fixed).
        winPopup->SetSize( rect );

#if wxUSE_POPUPWIN
        ((wxPopupTransientWindow*)winPopup)->Popup(m_popup);
#else
        winPopup->Show();
#if !defined(__WXX11__)
        m_popup->SetFocus();
#endif
#endif

        m_popupWinState = Visible;
    }
    else if ( IsPopupWindowState(Hidden) )
    {
        // Animation was aborted

        wxASSERT( !winPopup->IsShown() );

        m_popupWinState = Hidden;
    }

    Refresh();
}

void wxComboCtrlBase::OnPopupDismiss(bool generateEvent)
{
    // Just in case, avoid double dismiss
    if ( IsPopupWindowState(Hidden) )
        return;

    // This must be set before focus - otherwise there will be recursive
    // OnPopupDismisses.
    m_popupWinState = Hidden;

    //SetFocus();
    m_winPopup->Disable();

    // Inform popup control itself
    m_popupInterface->OnDismiss();

    // Reset popup-related flags.
    m_beenInsidePopup = false;
    m_blockEventsToPopup = true;

#if !wxUSE_POPUPWIN
    // Remove top level window event handler
    if ( m_toplevEvtHandler )
    {
        wxWindow* toplev = ::wxGetTopLevelParent( this );
        if ( toplev )
            toplev->RemoveEventHandler( m_toplevEvtHandler );
    }
#endif

    m_timeCanAcceptClick = ::wxGetLocalTimeMillis();

#if wxUSE_POPUPWIN
    m_timeCanAcceptClick += 150;
#endif

    // If cursor not on dropdown button, then clear its state
    // (technically not required by all ports, but do it for all just in case)
    if ( !m_btnArea.Contains(ScreenToClient(::wxGetMousePosition())) )
        m_btnState = 0;

    // Return parent's tab traversal flag.
    // See ShowPopup for notes.
    if ( m_iFlags & wxCC_IFLAG_PARENT_TAB_TRAVERSAL )
    {
        wxWindow* parent = GetParent();
        parent->SetWindowStyle( parent->GetWindowStyle() | wxTAB_TRAVERSAL );
        m_iFlags &= ~(wxCC_IFLAG_PARENT_TAB_TRAVERSAL);
    }

    // refresh control (necessary even if m_text)
    Refresh();

#if !defined(__WXX11__)
    SetFocus();
#endif

    if ( generateEvent )
    {
        wxCommandEvent event(wxEVT_COMBOBOX_CLOSEUP, GetId());
        event.SetEventObject(this);
        HandleWindowEvent(event);
    }
}

void wxComboCtrlBase::HidePopup(bool generateEvent)
{
    // Should be able to call this without popup interface
    if ( IsPopupWindowState(Hidden) || IsPopupWindowState(Closing) )
        return;
    m_popupWinState = Closing; // To prevent from reentering

    // transfer value and show it in textctrl, if any
    if ( !IsPopupWindowState(Animating) )
        SetValueByUser( m_popupInterface->GetStringValue() );

    m_winPopup->Hide();

    OnPopupDismiss(generateEvent);
}

void wxComboCtrlBase::OnPopupDestroy()
{
    m_winPopup = nullptr;

    delete m_popupInterface;
    m_popupInterface = nullptr;
}

// ----------------------------------------------------------------------------
// customization methods
// ----------------------------------------------------------------------------

void wxComboCtrlBase::SetButtonPosition( int width, int height,
                                         int side, int spacingX )
{
    m_btnWid = width;
    m_btnHei = height;
    m_btnSide = side;
    m_btnSpacingX = spacingX;

    if ( width > 0 || height > 0 || spacingX )
        m_iFlags |= wxCC_IFLAG_HAS_NONSTANDARD_BUTTON;

    RecalcAndRefresh();
}

wxSize wxComboCtrlBase::GetButtonSize()
{
    if ( m_btnSize.x > 0 )
        return m_btnSize;

    wxSize retSize(m_btnWid,m_btnHei);

    // Need to call CalculateAreas now if button size
    // is not explicitly specified.
    if ( retSize.x <= 0 || retSize.y <= 0)
    {
        OnResize();

        retSize = m_btnSize;
    }

    return retSize;
}

void wxComboCtrlBase::SetButtonBitmaps( const wxBitmapBundle& bmpNormal,
                                           bool blankButtonBg,
                                           const wxBitmapBundle& bmpPressed,
                                           const wxBitmapBundle& bmpHover,
                                           const wxBitmapBundle& bmpDisabled )
{
    m_bmpNormal = bmpNormal;
    m_blankButtonBg = blankButtonBg;

    if ( bmpPressed.IsOk() )
        m_bmpPressed = bmpPressed;
    else
        m_bmpPressed = bmpNormal;

    if ( bmpHover.IsOk() )
        m_bmpHover = bmpHover;
    else
        m_bmpHover = bmpNormal;

    if ( bmpDisabled.IsOk() )
        m_bmpDisabled = bmpDisabled;
    else
        m_bmpDisabled = bmpNormal;

    RecalcAndRefresh();
}

void wxComboCtrlBase::SetCustomPaintWidth( int width )
{
    if ( m_mainWindow )
    {
        // move the main window accordingly
        wxRect r = m_mainWindow->GetRect();
        int inc = width - m_widthCustomPaint;
        r.x += inc;
        r.width -= inc;
        m_mainWindow->SetSize( r );
    }

    m_widthCustomPaint = width;

    RecalcAndRefresh();
}

bool wxComboCtrlBase::DoSetMargins(const wxPoint& margins)
{
    // For general sanity's sake, we ignore top margin. Instead
    // we will always try to center the text vertically.
    bool res = true;

    if ( margins.x != -1 )
    {
        m_marginLeft = margins.x;
        m_iFlags |= wxCC_IFLAG_LEFT_MARGIN_SET;
    }
    else
    {
        m_marginLeft = GetNativeTextIndent();
        m_iFlags &= ~(wxCC_IFLAG_LEFT_MARGIN_SET);
    }

    if ( margins.y != -1 )
    {
        res = false;
    }

    RecalcAndRefresh();

    return res;
}

wxPoint wxComboCtrlBase::DoGetMargins() const
{
    return wxPoint(m_marginLeft, -1);
}

wxCoord wxComboCtrlBase::GetNativeTextIndent() const
{
    return DEFAULT_TEXT_INDENT;
}

void wxComboCtrlBase::SetTextCtrlStyle( int style )
{
    m_textCtrlStyle = style;

    if ( m_text )
        m_text->SetWindowStyle(style);
}

// ----------------------------------------------------------------------------
// wxTextEntry interface
// ----------------------------------------------------------------------------

wxString wxComboCtrlBase::DoGetValue() const
{
    if ( m_text )
        return m_text->GetValue();
    return m_valueString;
}

void wxComboCtrlBase::SetValueWithEvent(const wxString& value,
                                        bool withEvent)
{
    DoSetValue(value, withEvent ? SetValue_SendEvent : 0);
}

void wxComboCtrlBase::OnSetValue(const wxString& value)
{
    // Note: before wxComboCtrl inherited from wxTextEntry,
    //       this code used to be in SetValueWithEvent().

    // Since wxComboPopup may want to paint the combo as well, we need
    // to set the string value here (as well as sometimes in ShowPopup).
    if ( m_valueString != value )
    {
        bool found = true;
        wxString trueValue = value;

        // Conform to wxComboBox behaviour: read-only control can only accept
        // valid list items and empty string
        if ( m_popupInterface && HasFlag(wxCB_READONLY) && value.length() )
        {
            found = m_popupInterface->FindItem(value,
                                               &trueValue);
        }

        if ( found )
        {
            m_valueString = trueValue;

            EnsurePopupControl();

            if ( m_popupInterface )
                m_popupInterface->SetStringValue(trueValue);
        }
    }

    Refresh();
}

void wxComboCtrlBase::SetValueByUser(const wxString& value)
{
    // NB: Order of function calls is important here. Otherwise
    //     the SelectAll() may not work.

    if ( m_text )
    {
        m_text->SetValue(value);

        if ( !(m_iFlags & wxCC_NO_TEXT_AUTO_SELECT) )
            m_text->SelectAll();
    }

    OnSetValue(value);
}

// In this SetValue variant wxComboPopup::SetStringValue is not called
void wxComboCtrlBase::SetText(const wxString& value)
{
    // Unlike in SetValue(), this must be called here or
    // the behaviour will no be consistent in readonlys.
    EnsurePopupControl();

    m_valueString = value;

    if ( m_text )
    {
        m_ignoreEvtText++;
        m_text->SetValue( value );
    }

    Refresh();
}

void wxComboCtrlBase::Copy()
{
    if ( m_text )
        m_text->Copy();
}

void wxComboCtrlBase::Cut()
{
    if ( m_text )
        m_text->Cut();
}

void wxComboCtrlBase::Paste()
{
    if ( m_text )
        m_text->Paste();
}

void wxComboCtrlBase::SetInsertionPoint(long pos)
{
    if ( m_text )
        m_text->SetInsertionPoint(pos);
}

long wxComboCtrlBase::GetInsertionPoint() const
{
    if ( m_text )
        return m_text->GetInsertionPoint();

    return 0;
}

long wxComboCtrlBase::GetLastPosition() const
{
    if ( m_text )
        return m_text->GetLastPosition();

    return 0;
}

void wxComboCtrlBase::WriteText(const wxString& text)
{
    if ( m_text )
    {
        m_text->WriteText(text);
        OnSetValue(m_text->GetValue());
    }
    else
    {
        OnSetValue(text);
    }
}

void wxComboCtrlBase::DoSetValue(const wxString& value, int flags)
{
    if ( m_text )
    {
        if ( flags & SetValue_SendEvent )
            m_text->SetValue(value);
        else
            m_text->ChangeValue(value);
    }

    OnSetValue(value);
}

void wxComboCtrlBase::Replace(long from, long to, const wxString& value)
{
    if ( m_text )
    {
        m_text->Replace(from, to, value);
        OnSetValue(m_text->GetValue());
    }
}

void wxComboCtrlBase::Remove(long from, long to)
{
    if ( m_text )
    {
        m_text->Remove(from, to);
        OnSetValue(m_text->GetValue());
    }
}

void wxComboCtrlBase::SetSelection(long from, long to)
{
    if ( m_text )
        m_text->SetSelection(from, to);
}

void wxComboCtrlBase::GetSelection(long *from, long *to) const
{
    if ( m_text )
    {
        m_text->GetSelection(from, to);
    }
    else
    {
        *from = 0;
        *to = 0;
    }
}

bool wxComboCtrlBase::IsEditable() const
{
    if ( m_text )
        return m_text->IsEditable();
    return false;
}

void wxComboCtrlBase::SetEditable(bool editable)
{
    if ( m_text )
        m_text->SetEditable(editable);
}

void wxComboCtrlBase::Undo()
{
    if ( m_text )
        m_text->Undo();
}

void wxComboCtrlBase::Redo()
{
    if ( m_text )
        m_text->Redo();
}

bool wxComboCtrlBase::CanUndo() const
{
    if ( m_text )
        return m_text->CanUndo();

    return false;
}

bool wxComboCtrlBase::CanRedo() const
{
    if ( m_text )
        return m_text->CanRedo();

    return false;
}

bool wxComboCtrlBase::SetHint(const wxString& hint)
{
    m_hintText = hint;
    bool res = true;
    if ( m_text )
        res = m_text->SetHint(hint);
    Refresh();
    return res;
}

wxString wxComboCtrlBase::GetHint() const
{
    return m_hintText;
}

#endif // wxUSE_COMBOCTRL
