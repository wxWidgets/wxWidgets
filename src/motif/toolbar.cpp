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
#include "wx/motif/toolbar.h"

#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/Form.h>

#include "wx/motif/private.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxToolBarBase)

BEGIN_EVENT_TABLE(wxToolBar, wxToolBarBase)
END_EVENT_TABLE()
#endif

wxToolBar::wxToolBar():
  m_widgets(wxKEY_INTEGER)
{
  m_maxWidth = -1;
  m_maxHeight = -1;
  m_defaultWidth = 24;
  m_defaultHeight = 22;
  // TODO
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
                xmFormWidgetClass, parentWidget,
                XmNtraversalOn, False,
                XmNhorizontalSpacing, 0,
                XmNverticalSpacing, 0,
                NULL);

    m_mainWidget = (WXWidget) toolbar;

    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, size.x, size.y);

    SetFont(* parent->GetFont());
    ChangeBackgroundColour();
  
    return TRUE;
}

wxToolBar::~wxToolBar()
{
    // TODO
}

bool wxToolBar::CreateTools()
{
    if (m_tools.Number() == 0)
        return FALSE;

    m_widgets.Clear();
    Widget prevButton = (Widget) 0;
    wxNode* node = m_tools.First();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->Data();
        if ((tool->m_toolStyle != wxTOOL_STYLE_SEPARATOR) && tool->m_bitmap1.Ok())
        {
            Widget button = (Widget) 0;

            if (tool->m_isToggle)
            {
                button = XtVaCreateManagedWidget("toggleButton", 
                   xmToggleButtonWidgetClass, (Widget) m_mainWidget,
                   XmNleftAttachment, (prevButton == (Widget) 0) ? XmATTACH_FORM : XmATTACH_WIDGET,
                   XmNleftWidget, (prevButton == (Widget) 0) ? NULL : prevButton,
                   XmNleftOffset, 0,
                   XmNtopAttachment, XmATTACH_FORM,
						 //                   XmNpushButtonEnabled, True,
                   XmNmultiClick, XmMULTICLICK_KEEP,
                   XmNlabelType, XmPIXMAP,
                   NULL);
            }
            else
            {
                button = XtVaCreateManagedWidget("button", 
                   xmPushButtonWidgetClass, (Widget) m_mainWidget,
                   XmNleftAttachment, (prevButton == (Widget) 0) ? XmATTACH_FORM : XmATTACH_WIDGET,
                   XmNleftWidget, (prevButton == (Widget) 0) ? NULL : prevButton,
                   XmNleftOffset, 0,
                   XmNtopAttachment, XmATTACH_FORM,
                   XmNpushButtonEnabled, True,
                   XmNmultiClick, XmMULTICLICK_KEEP,
                   XmNlabelType, XmPIXMAP,
                   NULL);
	    }

            // For each button, if there is a mask, we must create
            // a new wxBitmap that has the correct background colour
            // for the button. Otherwise the background will just be
            // e.g. black if a transparent XPM has been loaded.
            if (tool->m_bitmap1.GetMask())
            {
                wxBitmap newBitmap(tool->m_bitmap1.GetWidth(),
				   tool->m_bitmap1.GetHeight(),
				   tool->m_bitmap1.GetDepth());
                int backgroundPixel;
                XtVaGetValues(button, XmNbackground, &backgroundPixel,
			      NULL);


                wxColour col;
                col.SetPixel(backgroundPixel);
                
                wxMemoryDC destDC;
                wxMemoryDC srcDC;
                srcDC.SelectObject(tool->m_bitmap1);
                destDC.SelectObject(newBitmap);

                wxBrush brush(col, wxSOLID);
                destDC.SetOptimization(FALSE);
                destDC.SetBackground(brush);
                destDC.Clear();
                destDC.Blit(0, 0, tool->m_bitmap1.GetWidth(), tool->m_bitmap1.GetHeight(), & srcDC, 0, 0, wxCOPY, TRUE);

                tool->m_bitmap1 = newBitmap;
            }
            if (tool->m_bitmap2.Ok() && tool->m_bitmap2.GetMask())
            {
                wxBitmap newBitmap(tool->m_bitmap2.GetWidth(),
				   tool->m_bitmap2.GetHeight(),
				   tool->m_bitmap2.GetDepth());
                int backgroundPixel;
                XtVaGetValues(button, XmNbackground, &backgroundPixel,
			      NULL);


                wxColour col;
                col.SetPixel(backgroundPixel);
                
                wxMemoryDC destDC;
                wxMemoryDC srcDC;
                srcDC.SelectObject(tool->m_bitmap2);
                destDC.SelectObject(newBitmap);

                wxBrush brush(col, wxSOLID);
                destDC.SetOptimization(FALSE);
                destDC.SetBackground(brush);
                destDC.Clear();
                destDC.Blit(0, 0, tool->m_bitmap2.GetWidth(), tool->m_bitmap2.GetHeight(), & srcDC, 0, 0, wxCOPY, TRUE);

                tool->m_bitmap2 = newBitmap;
            }
            Pixmap pixmap = (Pixmap) tool->m_bitmap1.GetPixmap();
            Pixmap insensPixmap = (Pixmap) tool->m_bitmap1.GetInsensPixmap();

            if (tool->m_isToggle)
            {
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
                    // This has to be both toggled and insensitive, but
                    // wxBitmap doesn't yet have a member to store & destroy
                    // it, so make it the same as pixmap2. Actually it's not
                    // used!
                    insensPixmap2 = pixmap2;
                }
                XtVaSetValues (button,
		 XmNlabelPixmap, pixmap,
		 XmNselectPixmap, pixmap,
	  XmNlabelInsensitivePixmap, insensPixmap,
	 XmNselectInsensitivePixmap, insensPixmap,
		 XmNarmPixmap, pixmap2,
		 XmNlabelType, XmPIXMAP,
		 NULL);

	    }
            else
            {
                XtVaSetValues(button,
                   XmNlabelPixmap, pixmap,
		   XmNlabelInsensitivePixmap, insensPixmap,
		   NULL);
            }

            m_widgets.Append(tool->m_index, (wxObject*) button);

            prevButton = button;
            
        }
        node = node->Next();
    }

    return TRUE;
}

void wxToolBar::SetToolBitmapSize(const wxSize& size)
{
    m_defaultWidth = size.x; m_defaultHeight = size.y;
    // TODO
}

wxSize wxToolBar::GetMaxSize() const
{
    // TODO
    return wxSize(0, 0);
}

// The button size is bigger than the bitmap size
wxSize wxToolBar::GetToolSize() const
{
    // TODO
    return wxSize(m_defaultWidth + 8, m_defaultHeight + 7);
}

void wxToolBar::EnableTool(int toolIndex, bool enable)
{
    wxNode *node = m_tools.Find((long)toolIndex);
    if (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->Data();
        tool->m_enabled = enable;
        // TODO enable button
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
            // TODO: set toggle state
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

    wxToolBarBase::ClearTools();
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

