/////////////////////////////////////////////////////////////////////////////
// Name:        motif/toolbar.cpp
// Purpose:     wxToolBar
// Author:      Julian Smart
// Modified by: 13.12.99 by VZ during toolbar classes reorganization
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "toolbar.h"
#endif

#ifdef __VMS
#define XtDisplay XTDISPLAY
#endif

#include "wx/wx.h"
#include "wx/app.h"
#include "wx/timer.h"
#include "wx/toolbar.h"

#ifdef __VMS__
#pragma message disable nosimpint
#endif
#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/Form.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

#include "wx/motif/private.h"

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxControl)
#endif

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

static void wxToolButtonCallback (Widget w, XtPointer clientData,
                    XtPointer ptr);
static void wxToolButtonPopupCallback (Widget w, XtPointer client_data,
                     XEvent *event, Boolean *continue_to_dispatch);

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxToolBarTimer : public wxTimer
{
public:
    virtual void Notify();

    static Widget help_popup;
    static Widget buttonWidget;
    static wxString helpString;
};

class wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(wxToolBar *tbar,
                  int id,
                  const wxBitmap& bitmap1,
                  const wxBitmap& bitmap2,
                  bool toggle,
                  wxObject *clientData,
                  const wxString& shortHelpString,
                  const wxString& longHelpString)
        : wxToolBarToolBase(tbar, id, bitmap1, bitmap2, toggle,
                            clientData, shortHelpString, longHelpString)
    {
        Init();
    }

    wxToolBarTool(wxToolBar *tbar, wxControl *control)
        : wxToolBarToolBase(tbar, control)
    {
        Init();
    }

    virtual ~wxToolBarTool();

    // accessors
    void SetWidget(Widget widget) { m_widget = widget; }
    Widget GetButtonWidget() const { return m_widget; }

    void SetPixmap(Pixmap pixmap) { m_pixmap = pixmap; }
    Pixmap GetPixmap() const { return m_pixmap; }

protected:
    void Init();

    Widget m_widget;
    Pixmap m_pixmap;
};

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

static wxToolBarTimer* wxTheToolBarTimer = (wxToolBarTimer*) NULL;

Widget wxToolBarTimer::help_popup = (Widget) 0;
Widget wxToolBarTimer::buttonWidget = (Widget) 0;
wxString wxToolBarTimer::helpString;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxToolBarTool
// ----------------------------------------------------------------------------

wxToolBarToolBase *wxToolBar::CreateTool(int id,
                                         const wxBitmap& bitmap1,
                                         const wxBitmap& bitmap2,
                                         bool toggle,
                                         wxObject *clientData,
                                         const wxString& shortHelpString,
                                         const wxString& longHelpString)
{
    return new wxToolBarTool(this, id, bitmap1, bitmap2, toggle,
                             clientData, shortHelpString, longHelpString);
}

wxToolBarToolBase *wxToolBar::CreateTool(wxControl *control)
{
    return new wxToolBarTool(this, control);
}

void wxToolBarTool::Init()
{
    m_widget = (Widget)0;
    m_pixmap = (Pixmap)0;
}

wxToolBarTool::~wxToolBarTool()
{
    if ( m_widget )
        XtDestroyWidget(m_widget);
    if ( m_pixmap )
        XmDestroyPixmap(DefaultScreenOfDisplay((Display*)wxGetDisplay()),
                        m_pixmap);
}

// ----------------------------------------------------------------------------
// wxToolBar construction
// ----------------------------------------------------------------------------

void wxToolBar::Init()
{
    m_maxWidth = -1;
    m_maxHeight = -1;
    m_defaultWidth = 24;
    m_defaultHeight = 22;
    m_toolPacking = 2;
    m_toolSeparation = 8;
    m_xMargin = 2;
    m_yMargin = 2;
    m_maxRows = 100;
    m_maxCols = 100;
}

bool wxToolBar::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    Init();

    m_windowId = id;

    SetName(name);
    m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE);
    m_foregroundColour = parent->GetForegroundColour();
    m_windowStyle = style;

    SetParent(parent);

    if (parent) parent->AddChild(this);

    Widget parentWidget = (Widget) parent->GetClientWidget();

    Widget toolbar = XtVaCreateManagedWidget("toolbar",
                    xmBulletinBoardWidgetClass, (Widget) parentWidget,
                    XmNmarginWidth, 0,
                    XmNmarginHeight, 0,
                    XmNresizePolicy, XmRESIZE_NONE,
                    NULL);
/*
    Widget toolbar = XtVaCreateManagedWidget("toolbar",
                xmFormWidgetClass, (Widget) m_clientWidget,
                XmNtraversalOn, False,
                XmNhorizontalSpacing, 0,
                XmNverticalSpacing, 0,
                XmNleftOffset, 0,
                XmNrightOffset, 0,
                XmNmarginWidth, 0,
                XmNmarginHeight, 0,
                NULL);
*/

    m_mainWidget = (WXWidget) toolbar;

    m_font = parent->GetFont();
    ChangeFont(FALSE);

    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, size.x, size.y);

    ChangeBackgroundColour();

    return TRUE;
}

wxToolBar::~wxToolBar()
{
    delete wxTheToolBarTimer;
    wxTheToolBarTimer = NULL;
}

bool wxToolBar::Realize()
{
    if ( m_tools.GetCount() == 0 )
    {
        // nothing to do
        return TRUE;
    }

    // Separator spacing
    const int separatorSize = GetToolSeparation(); // 8;
    wxSize margins = GetToolMargins();
    int packing = GetToolPacking();
    int marginX = margins.x;
    int marginY = margins.y;

    int currentX = marginX;
    int currentY = marginY;

    int buttonHeight = 0;

    int currentSpacing = 0;

    Widget button;
    Pixmap pixmap, insensPixmap;
    wxBitmap bmp;

    wxToolBarToolsList::Node *node = m_tools.GetFirst();
    while ( node )
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->GetData();

        switch ( tool->GetStyle() )
        {
            case wxTOOL_STYLE_CONTROL:
            {
                wxControl* control = tool->GetControl();
                wxSize sz = control->GetSize();
                wxPoint pos = control->GetPosition();
                control->Move(currentX, pos.y);
                currentX += sz.x + packing;

                break;
            }
            case wxTOOL_STYLE_SEPARATOR:
                currentX += separatorSize;
                break;

            case wxTOOL_STYLE_BUTTON:
                button = (Widget) 0;

                if ( tool->CanBeToggled() )
                {
                    button = XtVaCreateWidget("toggleButton",
                            xmToggleButtonWidgetClass, (Widget) m_mainWidget,
                            XmNx, currentX, XmNy, currentY,
                            XmNindicatorOn, False,
                            XmNshadowThickness, 2,
                            XmNborderWidth, 0,
                            XmNspacing, 0,
                            XmNmarginWidth, 0,
                            XmNmarginHeight, 0,
                            XmNmultiClick, XmMULTICLICK_KEEP,
                            XmNlabelType, XmPIXMAP,
                            NULL);
                    XtAddCallback ((Widget) button, XmNvalueChangedCallback, (XtCallbackProc) wxToolButtonCallback,
                            (XtPointer) this);

                    XtVaSetValues ((Widget) button,
                            XmNselectColor, m_backgroundColour.AllocColour(XtDisplay((Widget) button)),
                            NULL);
                }
                else
                {
                    button = XtVaCreateWidget("button",
                            xmPushButtonWidgetClass, (Widget) m_mainWidget,
                            XmNx, currentX, XmNy, currentY,
                            XmNpushButtonEnabled, True,
                            XmNmultiClick, XmMULTICLICK_KEEP,
                            XmNlabelType, XmPIXMAP,
                            NULL);
                    XtAddCallback (button,
                            XmNactivateCallback, (XtCallbackProc) wxToolButtonCallback,
                            (XtPointer) this);
                }

                DoChangeBackgroundColour((WXWidget) button, m_backgroundColour, TRUE);

                tool->SetWidget(button);

                // For each button, if there is a mask, we must create
                // a new wxBitmap that has the correct background colour
                // for the button. Otherwise the background will just be
                // e.g. black if a transparent XPM has been loaded.
                bmp = tool->GetBitmap1();
                if ( bmp.GetMask() )
                {
                    int backgroundPixel;
                    XtVaGetValues(button, XmNbackground, &backgroundPixel,
                            NULL);

                    wxColour col;
                    col.SetPixel(backgroundPixel);

                    bmp = wxCreateMaskedBitmap(bmp, col);

                    tool->SetBitmap1(bmp);
                }

                // Create a selected/toggled bitmap. If there isn't a 2nd
                // bitmap, we need to create it (with a darker, selected
                // background)
                int backgroundPixel;
                if ( tool->CanBeToggled() )
                    XtVaGetValues(button, XmNselectColor, &backgroundPixel,
                            NULL);
                else
                    XtVaGetValues(button, XmNarmColor, &backgroundPixel,
                            NULL);

                wxColour col;
                col.SetPixel(backgroundPixel);

                if (tool->GetBitmap2().Ok() && tool->GetBitmap2().GetMask())
                {
                    // Use what's there
                    wxBitmap newBitmap = wxCreateMaskedBitmap(tool->GetBitmap2(), col);
                    tool->SetBitmap2(newBitmap);
                }
                else
                {
                    // Use unselected bitmap
                    if ( bmp.GetMask() )
                    {
                        wxBitmap newBitmap = wxCreateMaskedBitmap(bmp, col);
                        tool->SetBitmap2(newBitmap);
                    }
                    else
                        tool->SetBitmap2(bmp);
                }

                pixmap = (Pixmap) bmp.GetPixmap();
                insensPixmap = (Pixmap) bmp.GetInsensPixmap();

                if (tool->CanBeToggled())
                {
                    // Toggle button
                    Pixmap pixmap2 = (Pixmap) 0;
                    Pixmap insensPixmap2 = (Pixmap) 0;

                    // If there's a bitmap for the toggled state, use it,
                    // otherwise generate one.
                    if (tool->GetBitmap2().Ok())
                    {
                        wxBitmap bmp2 = tool->GetBitmap2();
                        pixmap2 = (Pixmap) bmp2.GetPixmap();
                        insensPixmap2 = (Pixmap) bmp2.GetInsensPixmap();
                    }
                    else
                    {
                        pixmap2 = (Pixmap) bmp.GetArmPixmap(button);
                        insensPixmap2 = XCreateInsensitivePixmap((Display*) wxGetDisplay(), pixmap2);
                    }

                    tool->SetPixmap(pixmap2);

                    XtVaSetValues (button,
                            XmNfillOnSelect, True,
                            XmNlabelPixmap, pixmap,
                            XmNselectPixmap, pixmap2,
                            XmNlabelInsensitivePixmap, insensPixmap,
                            XmNselectInsensitivePixmap, insensPixmap2,
                            XmNlabelType, XmPIXMAP,
                            NULL);
                }
                else
                {
                    Pixmap pixmap2 = (Pixmap) 0;

                    // If there's a bitmap for the armed state, use it,
                    // otherwise generate one.
                    if (tool->GetBitmap2().Ok())
                    {
                        pixmap2 = (Pixmap) tool->GetBitmap2().GetPixmap();
                    }
                    else
                    {
                        pixmap2 = (Pixmap) bmp.GetArmPixmap(button);

                    }

                    tool->SetPixmap(pixmap2);

                    // Normal button
                    XtVaSetValues(button,
                            XmNlabelPixmap, pixmap,
                            XmNlabelInsensitivePixmap, insensPixmap,
                            XmNarmPixmap, pixmap2,
                            NULL);
                }

                XtManageChild(button);

                {
                    Dimension width, height;
                    XtVaGetValues(button,
                                  XmNwidth, &width,
                                  XmNheight, & height,
                                  NULL);
                    currentX += width + packing;
                    buttonHeight = wxMax(buttonHeight, height);
                }

                XtAddEventHandler (button, EnterWindowMask | LeaveWindowMask,
                        False, wxToolButtonPopupCallback, (XtPointer) this);

                currentSpacing = 0;
                break;
        }

        node = node->GetNext();
    }

    SetSize(-1, -1, currentX, buttonHeight + 2*marginY);

    return TRUE;
}

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord WXUNUSED(x),
                                                  wxCoord WXUNUSED(y)) const
{
    wxFAIL_MSG( _T("TODO") );

    return (wxToolBarToolBase *)NULL;
}

bool wxToolBar::DoInsertTool(size_t WXUNUSED(pos), wxToolBarToolBase *tool)
{
    tool->Attach(this);

    return TRUE;
}

bool wxToolBar::DoDeleteTool(size_t WXUNUSED(pos), wxToolBarToolBase *tool)
{
    tool->Detach();

    return TRUE;
}

void wxToolBar::DoEnableTool(wxToolBarToolBase *toolBase, bool enable)
{
    wxToolBarTool *tool = (wxToolBarTool *)toolBase;

    XtSetSensitive(tool->GetButtonWidget(), (Boolean) enable);
}

void wxToolBar::DoToggleTool(wxToolBarToolBase *toolBase, bool toggle)
{
    wxToolBarTool *tool = (wxToolBarTool *)toolBase;

    XmToggleButtonSetState(tool->GetButtonWidget(), (Boolean) toggle, False);
}

void wxToolBar::DoSetToggle(wxToolBarToolBase * WXUNUSED(tool),
                            bool WXUNUSED(toggle))
{
    // nothing to do
}

// ----------------------------------------------------------------------------
// Motif callbacks
// ----------------------------------------------------------------------------

wxToolBarToolBase *wxToolBar::FindToolByWidget(WXWidget w) const
{
    wxToolBarToolsList::Node* node = m_tools.GetFirst();
    while ( node )
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->GetData();
        if ( tool->GetButtonWidget() == w)
        {
            return tool;
        }

        node = node->GetNext();
    }

    return (wxToolBarToolBase *)NULL;
}

static void wxToolButtonCallback(Widget w,
                                 XtPointer clientData,
                                 XtPointer WXUNUSED(ptr))
{
    wxToolBar *toolBar = (wxToolBar *) clientData;
    wxToolBarToolBase *tool = toolBar->FindToolByWidget((WXWidget) w);
    if ( !tool )
        return;

    if ( tool->CanBeToggled() )
        tool->Toggle();

    if ( !toolBar->OnLeftClick(tool->GetId(), tool->IsToggled()) )
    {
        // revert
        tool->Toggle();
    }
}


static void wxToolButtonPopupCallback(Widget w,
                                      XtPointer client_data,
                                      XEvent *event,
                                      Boolean *WXUNUSED(continue_to_dispatch))
{
    // TODO: retrieve delay before popping up tooltip from wxSystemSettings.
    static const int delayMilli = 800;

    wxToolBar* toolBar = (wxToolBar*) client_data;
    wxToolBarToolBase *tool = toolBar->FindToolByWidget((WXWidget) w);

    if ( !tool )
        return;

    wxString tooltip = tool->GetShortHelp();
    if ( !tooltip )
        return;

    if (!wxTheToolBarTimer)
        wxTheToolBarTimer = new wxToolBarTimer;

    wxToolBarTimer::buttonWidget = w;
    wxToolBarTimer::helpString = tooltip;

    /************************************************************/
    /* Popup help label                                         */
    /************************************************************/
    if (event->type == EnterNotify)
    {
        if (wxToolBarTimer::help_popup != (Widget) 0)
        {
            XtDestroyWidget (wxToolBarTimer::help_popup);
            XtPopdown (wxToolBarTimer::help_popup);
        }
        wxToolBarTimer::help_popup = (Widget) 0;

        // One shot
        wxTheToolBarTimer->Start(delayMilli, TRUE);

    }
    /************************************************************/
    /* Popdown help label                                       */
    /************************************************************/
    else if (event->type == LeaveNotify)
    {
        if (wxTheToolBarTimer)
            wxTheToolBarTimer->Stop();
        if (wxToolBarTimer::help_popup != (Widget) 0)
        {
            XtDestroyWidget (wxToolBarTimer::help_popup);
            XtPopdown (wxToolBarTimer::help_popup);
        }
        wxToolBarTimer::help_popup = (Widget) 0;
    }
}

void wxToolBarTimer::Notify()
{
    Position x, y;

        /************************************************************/
        /* Create shell without window decorations                  */
        /************************************************************/
        help_popup = XtVaCreatePopupShell ("shell",
                                           overrideShellWidgetClass, (Widget) wxTheApp->GetTopLevelWidget(),
                                           NULL);

        /************************************************************/
        /* Get absolute position on display of toolbar button       */
        /************************************************************/
        XtTranslateCoords (buttonWidget,
                           (Position) 0,
                           (Position) 0,
                           &x, &y);

        // Move the tooltip more or less above the button
        int yOffset = 20; // TODO: What should be really?
        y -= yOffset;
        if (y < yOffset) y = 0;

        /************************************************************/
        /* Set the position of the help popup                       */
        /************************************************************/
        XtVaSetValues (help_popup,
                       XmNx, (Position) x,
                       XmNy, (Position) y,
                       NULL);

        /************************************************************/
        /* Create help label                                        */
        /************************************************************/
        XmString text = XmStringCreateSimple ((char*) (const char*) helpString);
        XtVaCreateManagedWidget ("help_label",
                                 xmLabelWidgetClass, help_popup,
                                 XmNlabelString, text,
                                 XtVaTypedArg,
                                 XmNforeground, XtRString, "black",
                                                strlen("black")+1,
                                 XtVaTypedArg,
                                 XmNbackground, XtRString, "LightGoldenrod",
                                                strlen("LightGoldenrod")+1,
                                 NULL);
        XmStringFree (text);

        /************************************************************/
        /* Popup help label                                         */
        /************************************************************/
        XtPopup (help_popup, XtGrabNone);
}

