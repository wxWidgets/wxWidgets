/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.cpp
// Purpose:     wxToolBar
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "toolbar.h"
#endif

#include "wx/wx.h"
#include "wx/app.h"
#include "wx/timer.h"
#include "wx/motif/toolbar.h"

#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/Form.h>

#include "wx/motif/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxToolBarBase)

BEGIN_EVENT_TABLE(wxToolBar, wxToolBarBase)
END_EVENT_TABLE()
#endif

static void wxToolButtonCallback (Widget w, XtPointer clientData,
		    XtPointer ptr);
static void wxToolButtonPopupCallback (Widget w, XtPointer client_data,
                     XEvent *event, Boolean *continue_to_dispatch);

class wxToolBarTimer: public wxTimer
{
public:
  wxToolBarTimer() { }
  virtual void Notify();

  static Widget help_popup;
  static Widget buttonWidget;
  static wxString helpString;
};

static wxToolBarTimer* wxTheToolBarTimer = (wxToolBarTimer*) NULL;

Widget wxToolBarTimer::help_popup = (Widget) 0;
Widget wxToolBarTimer::buttonWidget = (Widget) 0;
wxString wxToolBarTimer::helpString = "";

wxToolBar::wxToolBar():
  m_widgets(wxKEY_INTEGER)
{
  m_maxWidth = -1;
  m_maxHeight = -1;
  m_defaultWidth = 24;
  m_defaultHeight = 22;
}

bool wxToolBar::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
    m_maxWidth = -1;
    m_maxHeight = -1;
  
    m_defaultWidth = 24;
    m_defaultHeight = 22;
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

    m_windowFont = parent->GetFont();
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
    ClearTools();
    DestroyPixmaps();
}

bool wxToolBar::CreateTools()
{
    if (m_tools.Number() == 0)
        return FALSE;

    // Separator spacing
    const int separatorSize = GetToolSeparation(); // 8;
    wxSize margins = GetToolMargins();
    int marginX = margins.x;
    int marginY = margins.y;

    int currentX = marginX;
    int currentY = marginY;

    int buttonHeight = 0;

    int currentSpacing = 0;

    m_widgets.Clear();
    Widget prevButton = (Widget) 0;
    wxNode* node = m_tools.First();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->Data();

        if (tool->m_toolStyle == wxTOOL_STYLE_SEPARATOR)
            currentX += separatorSize;
        else if (tool->m_bitmap1.Ok())
        {
            Widget button = (Widget) 0;

            if (tool->m_isToggle)
            {
                button = XtVaCreateWidget("toggleButton", 
                   xmToggleButtonWidgetClass, (Widget) m_mainWidget,
                   XmNx, currentX, XmNy, currentY,
						 //                   XmNpushButtonEnabled, True,
                   XmNmultiClick, XmMULTICLICK_KEEP,
                   XmNlabelType, XmPIXMAP,
                   NULL);
                XtAddCallback ((Widget) button, XmNvalueChangedCallback, (XtCallbackProc) wxToolButtonCallback,
		 (XtPointer) this);
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

            // For each button, if there is a mask, we must create
            // a new wxBitmap that has the correct background colour
            // for the button. Otherwise the background will just be
            // e.g. black if a transparent XPM has been loaded.
            wxBitmap originalBitmap = tool->m_bitmap1;

            if (tool->m_bitmap1.GetMask())
            {
                int backgroundPixel;
                XtVaGetValues(button, XmNbackground, &backgroundPixel,
			      NULL);


                wxColour col;
                col.SetPixel(backgroundPixel);
                
                wxBitmap newBitmap = wxCreateMaskedBitmap(tool->m_bitmap1, col);

                tool->m_bitmap1 = newBitmap;
            }

            // Create a selected/toggled bitmap. If there isn't a m_bitmap2,
            // we need to create it (with a darker, selected background)
            int backgroundPixel;
            if (tool->m_isToggle)
                XtVaGetValues(button, XmNselectColor, &backgroundPixel,
		      NULL);
            else
                XtVaGetValues(button, XmNarmColor, &backgroundPixel,
		      NULL);

            wxColour col;
            col.SetPixel(backgroundPixel);

            if (tool->m_bitmap2.Ok() && tool->m_bitmap2.GetMask())
            {
                // Use what's there
                wxBitmap newBitmap = wxCreateMaskedBitmap(tool->m_bitmap2, col);
                tool->m_bitmap2 = newBitmap;
            }
            else
            {
                // Use unselected bitmap
                if (originalBitmap.GetMask())
                {
                    wxBitmap newBitmap = wxCreateMaskedBitmap(originalBitmap, col);
                    tool->m_bitmap2 = newBitmap;
		}
                else
                    tool->m_bitmap2 = tool->m_bitmap1;
            }

            Pixmap pixmap = (Pixmap) tool->m_bitmap1.GetPixmap();
            Pixmap insensPixmap = (Pixmap) tool->m_bitmap1.GetInsensPixmap();

            if (tool->m_isToggle)
            {
                // Toggle button
                Pixmap pixmap2 = (Pixmap) 0;
                Pixmap insensPixmap2 = (Pixmap) 0;

                // If there's a bitmap for the toggled state, use it,
                // otherwise generate one.
                if (tool->m_bitmap2.Ok())
                {
                    pixmap2 = (Pixmap) tool->m_bitmap2.GetPixmap();
                    insensPixmap2 = (Pixmap) tool->m_bitmap2.GetInsensPixmap();
                }
                else
                {
                    pixmap2 = (Pixmap) tool->m_bitmap1.GetArmPixmap(button);
                    insensPixmap2 = XCreateInsensitivePixmap((Display*) wxGetDisplay(), pixmap2);
                    m_pixmaps.Append((wxObject*) insensPixmap2); // Store for later deletion
                }
                XtVaSetValues (button,
                    XmNindicatorOn, False,
		    XmNshadowThickness, 2,
			       //		    XmNborderWidth, 0,
			       //		    XmNspacing, 0,
		    XmNmarginWidth, 0,
		    XmNmarginHeight, 0,
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
                if (tool->m_bitmap2.Ok())
                {
                    pixmap2 = (Pixmap) tool->m_bitmap2.GetPixmap();
                }
                else
                {
                    pixmap2 = (Pixmap) tool->m_bitmap1.GetArmPixmap(button);

                }
                // Normal button
                XtVaSetValues(button,
                   XmNlabelPixmap, pixmap,
                   XmNlabelInsensitivePixmap, insensPixmap,
                   XmNarmPixmap, pixmap2,
                   NULL);
            }
            XtManageChild(button);

            Dimension width, height;
            XtVaGetValues(button, XmNwidth, & width, XmNheight, & height,
			      NULL);
            currentX += width + marginX;
            buttonHeight = wxMax(buttonHeight, height);

            XtAddEventHandler (button, EnterWindowMask | LeaveWindowMask, 
                       False, wxToolButtonPopupCallback, (XtPointer) this);
            m_widgets.Append(tool->m_index, (wxObject*) button);

            prevButton = button;
            currentSpacing = 0;
        }
        node = node->Next();
    }

    SetSize(-1, -1, currentX, buttonHeight + 2*marginY);

    return TRUE;
}

// Old version, assuming we use a form. Now we use
// a bulletin board, so we can create controls on the toolbar.
#if 0
bool wxToolBar::CreateTools()
{
    if (m_tools.Number() == 0)
        return FALSE;

    // Separator spacing
    const int separatorSize = GetToolSeparation(); // 8;

    int currentSpacing = 0;

    m_widgets.Clear();
    Widget prevButton = (Widget) 0;
    wxNode* node = m_tools.First();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->Data();

        if (tool->m_toolStyle == wxTOOL_STYLE_SEPARATOR)
            currentSpacing = separatorSize;
        else if (tool->m_bitmap1.Ok())
        {
            Widget button = (Widget) 0;

            if (tool->m_isToggle)
            {
                button = XtVaCreateManagedWidget("toggleButton", 
                   xmToggleButtonWidgetClass, (Widget) m_mainWidget,
                   XmNleftAttachment, (prevButton == (Widget) 0) ? XmATTACH_FORM : XmATTACH_WIDGET,
                   XmNleftWidget, (prevButton == (Widget) 0) ? NULL : prevButton,
                   XmNleftOffset, currentSpacing,
                   XmNtopAttachment, XmATTACH_FORM,
						 //                   XmNpushButtonEnabled, True,
                   XmNmultiClick, XmMULTICLICK_KEEP,
                   XmNlabelType, XmPIXMAP,
                   NULL);
                XtAddCallback ((Widget) button, XmNvalueChangedCallback, (XtCallbackProc) wxToolButtonCallback,
		 (XtPointer) this);
            }
            else
            {
                button = XtVaCreateManagedWidget("button", 
                   xmPushButtonWidgetClass, (Widget) m_mainWidget,
                   XmNleftAttachment, (prevButton == (Widget) 0) ? XmATTACH_FORM : XmATTACH_WIDGET,
                   XmNleftWidget, (prevButton == (Widget) 0) ? NULL : prevButton,
                   XmNleftOffset, currentSpacing,
                   XmNtopAttachment, XmATTACH_FORM,
                   XmNpushButtonEnabled, True,
                   XmNmultiClick, XmMULTICLICK_KEEP,
                   XmNlabelType, XmPIXMAP,
                   NULL);
                XtAddCallback (button,
                   XmNactivateCallback, (XtCallbackProc) wxToolButtonCallback,
                   (XtPointer) this);
            }

            // For each button, if there is a mask, we must create
            // a new wxBitmap that has the correct background colour
            // for the button. Otherwise the background will just be
            // e.g. black if a transparent XPM has been loaded.
            wxBitmap originalBitmap = tool->m_bitmap1;

            if (tool->m_bitmap1.GetMask())
            {
                int backgroundPixel;
                XtVaGetValues(button, XmNbackground, &backgroundPixel,
			      NULL);


                wxColour col;
                col.SetPixel(backgroundPixel);
                
                wxBitmap newBitmap = wxCreateMaskedBitmap(tool->m_bitmap1, col);

                tool->m_bitmap1 = newBitmap;
            }

            // Create a selected/toggled bitmap. If there isn't a m_bitmap2,
            // we need to create it (with a darker, selected background)
            int backgroundPixel;
            if (tool->m_isToggle)
                XtVaGetValues(button, XmNselectColor, &backgroundPixel,
		      NULL);
            else
                XtVaGetValues(button, XmNarmColor, &backgroundPixel,
		      NULL);

            wxColour col;
            col.SetPixel(backgroundPixel);

            if (tool->m_bitmap2.Ok() && tool->m_bitmap2.GetMask())
            {
                // Use what's there
                wxBitmap newBitmap = wxCreateMaskedBitmap(tool->m_bitmap2, col);
                tool->m_bitmap2 = newBitmap;
            }
            else
            {
                // Use unselected bitmap
                if (originalBitmap.GetMask())
                {
                    wxBitmap newBitmap = wxCreateMaskedBitmap(originalBitmap, col);
                    tool->m_bitmap2 = newBitmap;
		}
                else
                    tool->m_bitmap2 = tool->m_bitmap1;
            }

            Pixmap pixmap = (Pixmap) tool->m_bitmap1.GetPixmap();
            Pixmap insensPixmap = (Pixmap) tool->m_bitmap1.GetInsensPixmap();

            if (tool->m_isToggle)
            {
                // Toggle button
                Pixmap pixmap2 = (Pixmap) 0;
                Pixmap insensPixmap2 = (Pixmap) 0;

                // If there's a bitmap for the toggled state, use it,
                // otherwise generate one.
                if (tool->m_bitmap2.Ok())
                {
                    pixmap2 = (Pixmap) tool->m_bitmap2.GetPixmap();
                    insensPixmap2 = (Pixmap) tool->m_bitmap2.GetInsensPixmap();
                }
                else
                {
                    pixmap2 = (Pixmap) tool->m_bitmap1.GetArmPixmap(button);
                    insensPixmap2 = XCreateInsensitivePixmap((Display*) wxGetDisplay(), pixmap2);
                    m_pixmaps.Append((wxObject*) insensPixmap2); // Store for later deletion
                }
                XtVaSetValues (button,
                    XmNindicatorOn, False,
		    XmNshadowThickness, 2,
			       //		    XmNborderWidth, 0,
			       //		    XmNspacing, 0,
		    XmNmarginWidth, 0,
		    XmNmarginHeight, 0,
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
                if (tool->m_bitmap2.Ok())
                {
                    pixmap2 = (Pixmap) tool->m_bitmap2.GetPixmap();
                }
                else
                {
                    pixmap2 = (Pixmap) tool->m_bitmap1.GetArmPixmap(button);

                }
                // Normal button
                XtVaSetValues(button,
                   XmNlabelPixmap, pixmap,
                   XmNlabelInsensitivePixmap, insensPixmap,
                   XmNarmPixmap, pixmap2,
                   NULL);
            }

            XtAddEventHandler (button, EnterWindowMask | LeaveWindowMask, 
                       False, wxToolButtonPopupCallback, (XtPointer) this);
            m_widgets.Append(tool->m_index, (wxObject*) button);

            prevButton = button;
            currentSpacing = 0;
        }
        node = node->Next();
    }

    return TRUE;
}
#endif

void wxToolBar::SetToolBitmapSize(const wxSize& size)
{
    // TODO not necessary?
    m_defaultWidth = size.x; m_defaultHeight = size.y;
}

wxSize wxToolBar::GetMaxSize() const
{
    int w, h;
    GetSize(& w, & h);

    return wxSize(w, h);
}

// The button size is bigger than the bitmap size
wxSize wxToolBar::GetToolSize() const
{
    // TODO not necessary?
    return wxSize(m_defaultWidth + 8, m_defaultHeight + 7);
}

void wxToolBar::EnableTool(int toolIndex, bool enable)
{
    wxNode *node = m_tools.Find((long)toolIndex);
    if (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->Data();
        tool->m_enabled = enable;

        WXWidget widget = FindWidgetForIndex(tool->m_index);
        if (widget == (WXWidget) 0)
            return;

        XtSetSensitive((Widget) widget, (Boolean) enable);
    }
}

void wxToolBar::ToggleTool(int toolIndex, bool toggle)
{
    wxNode *node = m_tools.Find((long)toolIndex);
    if (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->Data();
        if (tool->m_isToggle)
        {
            tool->m_toggleState = toggle;

            WXWidget widget = FindWidgetForIndex(tool->m_index);
            if (widget == (WXWidget) 0)
                return;

            XmToggleButtonSetState((Widget) widget, (Boolean) toggle, False);
        }
    }
}

void wxToolBar::ClearTools()
{
    wxNode* node = m_widgets.First();
    while (node)
    {
        Widget button = (Widget) node->Data();
        XtDestroyWidget(button);
        node = node->Next();
    }
    m_widgets.Clear();
    DestroyPixmaps();

    wxToolBarBase::ClearTools();
}

void wxToolBar::DestroyPixmaps()
{
    wxNode* node = m_pixmaps.First();
    while (node)
    {
        Pixmap pixmap = (Pixmap) node->Data();
        XmDestroyPixmap (DefaultScreenOfDisplay ((Display*) GetXDisplay()), pixmap);
        node = node->Next();
    }
    m_pixmaps.Clear();
}

// If pushedBitmap is NULL, a reversed version of bitmap is
// created and used as the pushed/toggled image.
// If toggle is TRUE, the button toggles between the two states.

wxToolBarTool *wxToolBar::AddTool(int index, const wxBitmap& bitmap, const wxBitmap& pushedBitmap,
             bool toggle, long xPos, long yPos, wxObject *clientData, const wxString& helpString1, const wxString& helpString2)
{
  wxToolBarTool *tool = new wxToolBarTool(index, bitmap, (wxBitmap *)NULL, toggle, xPos, yPos, helpString1, helpString2);
  tool->m_clientData = clientData;

  if (xPos > -1)
    tool->m_x = xPos;
  else
    tool->m_x = m_xMargin;

  if (yPos > -1)
    tool->m_y = yPos;
  else
    tool->m_y = m_yMargin;

  tool->SetSize(GetDefaultButtonWidth(), GetDefaultButtonHeight());

  m_tools.Append((long)index, tool);
  return tool;
}

int wxToolBar::FindIndexForWidget(WXWidget w)
{
    wxNode* node = m_widgets.First();
    while (node)
    {
        WXWidget widget = (WXWidget) node->Data();
        if (widget == w)
            return (int) node->key.integer;
        node = node->Next();
    }
    return -1;
}

WXWidget wxToolBar::FindWidgetForIndex(int index)
{
    wxNode* node = m_widgets.Find((long) index);
    if (!node)
        return (WXWidget) 0;
    else
        return (WXWidget) node->Data();
}

WXWidget wxToolBar::GetTopWidget() const
{
    return m_mainWidget;
}

WXWidget wxToolBar::GetClientWidget() const
{
    return m_mainWidget;
}

WXWidget wxToolBar::GetMainWidget() const
{
    return m_mainWidget;
}


void wxToolButtonCallback (Widget w, XtPointer clientData,
		    XtPointer ptr)
{
    wxToolBar *toolBar = (wxToolBar *) clientData;
    int index = toolBar->FindIndexForWidget((WXWidget) w);

    if (index != -1)
    {
        wxNode *node = toolBar->GetTools().Find((long)index);
        if (!node)
            return;
        wxToolBarTool *tool = (wxToolBarTool *)node->Data();
        if (tool->m_isToggle)
            tool->m_toggleState = toolBar->GetToolState(index);

        (void) toolBar->OnLeftClick(index, tool->m_toggleState);
    }

}


static void wxToolButtonPopupCallback (Widget w, XtPointer client_data,
                     XEvent *event, Boolean *continue_to_dispatch)
{
    // TODO: retrieve delay before popping up tooltip from wxSystemSettings.
    int delayMilli = 800;
    wxToolBar* toolBar = (wxToolBar*) client_data;

    int index = toolBar->FindIndexForWidget((WXWidget) w);

    if (index != -1)
    {
        wxNode *node = toolBar->GetTools().Find((long)index);
        if (!node)
            return;
        wxToolBarTool *tool = (wxToolBarTool *)node->Data();
        wxString str(toolBar->GetToolShortHelp(index));
        if (str.IsNull() || str == "")
            return;

        if (!wxTheToolBarTimer)
          wxTheToolBarTimer = new wxToolBarTimer;

	wxToolBarTimer::buttonWidget = w;
	wxToolBarTimer::helpString = str;


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

