/////////////////////////////////////////////////////////////////////////////
// Name:        common/tbarbase.cpp
// Purpose:     wxToolBarBase implementation
// Author:      Julian Smart
// Modified by: VZ at 11.12.99 (wxScrollableToolBar splitted off)
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "tbarbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/frame.h"

// For ::UpdateWindow
#ifdef __WXMSW__
#include <windows.h>
#endif

#if wxUSE_TOOLBAR

#include "wx/tbarbase.h"

// ----------------------------------------------------------------------------
// wxWindows macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
    BEGIN_EVENT_TABLE(wxToolBarBase, wxControl)
        EVT_IDLE(wxToolBarBase::OnIdle)
    END_EVENT_TABLE()
#endif

#include "wx/listimpl.cpp"

WX_DEFINE_LIST(wxToolBarToolsList);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxToolBarToolBase
// ----------------------------------------------------------------------------

bool wxToolBarToolBase::Enable(bool enable)
{
    if ( m_enabled == enable )
        return FALSE;

    m_enabled = enable;

    return TRUE;
}

bool wxToolBarToolBase::Toggle(bool toggle)
{
    wxASSERT_MSG( m_isToggle, _T("can't toggle this tool") );

    if ( m_toggled == toggle )
        return FALSE;

    m_toggled = toggle;

    return TRUE;
}

bool wxToolBarToolBase::SetToggle(bool toggle)
{
    if ( m_isToggle == toggle )
        return FALSE;

    m_isToggle = toggle;

    return TRUE;
}

bool wxToolBarToolBase::SetShortHelp(const wxString& help)
{
    if ( m_shortHelpString == help )
        return FALSE;

    m_shortHelpString = help;

    return TRUE;
}

bool wxToolBarToolBase::SetLongHelp(const wxString& help)
{
    if ( m_longHelpString == help )
        return FALSE;

    m_longHelpString = help;

    return TRUE;
}

wxToolBarToolBase::~wxToolBarToolBase()
{
}

// ----------------------------------------------------------------------------
// wxToolBarBase adding/deleting items
// ----------------------------------------------------------------------------

wxToolBarBase::wxToolBarBase()
{
    // the list owns the pointers
    m_tools.DeleteContents(TRUE);

    m_xMargin = m_yMargin = 0;

    m_maxRows = m_maxCols = 0;
}

wxToolBarToolBase *wxToolBarBase::AddTool(int id,
                                          const wxBitmap& bitmap,
                                          const wxBitmap& pushedBitmap,
                                          bool toggle,
                                          wxCoord WXUNUSED(xPos),
                                          wxCoord WXUNUSED(yPos),
                                          wxObject *clientData,
                                          const wxString& helpString1,
                                          const wxString& helpString2)
{
    return InsertTool(GetToolsCount(), id, bitmap, pushedBitmap,
                      toggle, clientData, helpString1, helpString2);
}

wxToolBarToolBase *wxToolBarBase::InsertTool(size_t pos,
                                             int id,
                                             const wxBitmap& bitmap,
                                             const wxBitmap& pushedBitmap,
                                             bool toggle,
                                             wxObject *clientData,
                                             const wxString& helpString1,
                                             const wxString& helpString2)
{
    wxCHECK_MSG( pos <= GetToolsCount(), (wxToolBarToolBase *)NULL,
                 _T("invalid position in wxToolBar::InsertTool()") );

    wxToolBarToolBase *tool = CreateTool(id, bitmap, pushedBitmap, toggle,
                                         clientData, helpString1, helpString2);

    if ( !tool || !DoInsertTool(pos, tool) )
    {
        delete tool;

        return NULL;
    }

    m_tools.Insert(pos, tool);

    return tool;
}

wxToolBarToolBase *wxToolBarBase::AddControl(wxControl *control)
{
    return InsertControl(GetToolsCount(), control);
}

wxToolBarToolBase *wxToolBarBase::InsertControl(size_t pos, wxControl *control)
{
    wxCHECK_MSG( control, (wxToolBarToolBase *)NULL,
                 _T("toolbar: can't insert NULL control") );

    wxCHECK_MSG( control->GetParent() == this, (wxToolBarToolBase *)NULL,
                 _T("control must have toolbar as parent") );

    wxCHECK_MSG( pos <= GetToolsCount(), (wxToolBarToolBase *)NULL,
                 _T("invalid position in wxToolBar::InsertControl()") );

    wxToolBarToolBase *tool = CreateTool(control);

    if ( !tool || !DoInsertTool(pos, tool) )
    {
        delete tool;

        return NULL;
    }

    m_tools.Insert(pos, tool);

    return tool;
}

wxToolBarToolBase *wxToolBarBase::AddSeparator()
{
    return InsertSeparator(GetToolsCount());
}

wxToolBarToolBase *wxToolBarBase::InsertSeparator(size_t pos)
{
    wxCHECK_MSG( pos <= GetToolsCount(), (wxToolBarToolBase *)NULL,
                 _T("invalid position in wxToolBar::InsertSeparator()") );

    wxToolBarToolBase *tool = CreateTool(wxID_SEPARATOR,
                                         wxNullBitmap, wxNullBitmap,
                                         FALSE, (wxObject *)NULL,
                                         wxEmptyString, wxEmptyString);

    if ( !tool || !DoInsertTool(pos, tool) )
    {
        delete tool;

        return NULL;
    }

    m_tools.Insert(pos, tool);

    return tool;
}

wxToolBarToolBase *wxToolBarBase::RemoveTool(int id)
{
    size_t pos = 0;
    wxToolBarToolsList::Node *node;
    for ( node = m_tools.GetFirst(); node; node = node->GetNext() )
    {
        if ( node->GetData()->GetId() == id )
            break;

        pos++;
    }

    if ( !node )
    {
        // don't give any error messages - sometimes we might call RemoveTool()
        // without knowing whether the tool is or not in the toolbar
        return (wxToolBarToolBase *)NULL;
    }

    wxToolBarToolBase *tool = node->GetData();
    if ( !DoDeleteTool(pos, tool) )
    {
        return (wxToolBarToolBase *)NULL;
    }

    // the node would delete the data, so set it to NULL to avoid this
    node->SetData(NULL);

    m_tools.DeleteNode(node);

    return tool;
}

bool wxToolBarBase::DeleteToolByPos(size_t pos)
{
    wxCHECK_MSG( pos < GetToolsCount(), FALSE,
                 _T("invalid position in wxToolBar::DeleteToolByPos()") );

    wxToolBarToolsList::Node *node = m_tools.Item(pos);

    if ( !DoDeleteTool(pos, node->GetData()) )
    {
        return FALSE;
    }

    m_tools.DeleteNode(node);

    return TRUE;
}

bool wxToolBarBase::DeleteTool(int id)
{
    size_t pos = 0;
    wxToolBarToolsList::Node *node;
    for ( node = m_tools.GetFirst(); node; node = node->GetNext() )
    {
        if ( node->GetData()->GetId() == id )
            break;

        pos++;
    }

    if ( !node || !DoDeleteTool(pos, node->GetData()) )
    {
        return FALSE;
    }

    m_tools.DeleteNode(node);

    return TRUE;
}

wxToolBarToolBase *wxToolBarBase::FindById(int id) const
{
    wxToolBarToolBase *tool = (wxToolBarToolBase *)NULL;

    for ( wxToolBarToolsList::Node *node = m_tools.GetFirst();
          node;
          node = node->GetNext() )
    {
        tool = node->GetData();
        if ( tool->GetId() == id )
        {
            // found
            break;
        }
    }

    return tool;
}

void wxToolBarBase::ClearTools()
{
    m_tools.Clear();
}

bool wxToolBarBase::Realize()
{
    return TRUE;
}

wxToolBarBase::~wxToolBarBase()
{
}

// ----------------------------------------------------------------------------
// wxToolBarBase tools state
// ----------------------------------------------------------------------------

void wxToolBarBase::EnableTool(int id, bool enable)
{
    wxToolBarToolBase *tool = FindById(id);
    if ( tool )
    {
        if ( tool->Enable(enable) )
        {
            DoEnableTool(tool, enable);
        }
    }
}

void wxToolBarBase::ToggleTool(int id, bool toggle)
{
    wxToolBarToolBase *tool = FindById(id);
    if ( tool && tool->CanBeToggled() )
    {
        if ( tool->Toggle(toggle) )
        {
            DoToggleTool(tool, toggle);
        }
    }
}

void wxToolBarBase::SetToggle(int id, bool toggle)
{
    wxToolBarToolBase *tool = FindById(id);
    if ( tool )
    {
        if ( tool->SetToggle(toggle) )
        {
            DoSetToggle(tool, toggle);
        }
    }
}

void wxToolBarBase::SetToolShortHelp(int id, const wxString& help)
{
    wxToolBarToolBase *tool = FindById(id);
    if ( tool )
    {
        (void)tool->SetShortHelp(help);
    }
}

void wxToolBarBase::SetToolLongHelp(int id, const wxString& help)
{
    wxToolBarToolBase *tool = FindById(id);
    if ( tool )
    {
        (void)tool->SetLongHelp(help);
    }
}

wxObject *wxToolBarBase::GetToolClientData(int id) const
{
    wxToolBarToolBase *tool = FindById(id);

    return tool ? tool->GetClientData() : (wxObject *)NULL;
}

void wxToolBarBase::SetToolClientData(int id, wxObject *clientData)
{
    wxToolBarToolBase *tool = FindById(id);

    wxCHECK_RET( tool, _T("no such tool in wxToolBar::SetToolClientData") );

    tool->SetClientData(clientData);
}

bool wxToolBarBase::GetToolState(int id) const
{
    wxToolBarToolBase *tool = FindById(id);
    wxCHECK_MSG( tool, FALSE, _T("no such tool") );

    return tool->IsToggled();
}

bool wxToolBarBase::GetToolEnabled(int id) const
{
    wxToolBarToolBase *tool = FindById(id);
    wxCHECK_MSG( tool, FALSE, _T("no such tool") );

    return tool->IsEnabled();
}

wxString wxToolBarBase::GetToolShortHelp(int id) const
{
    wxToolBarToolBase *tool = FindById(id);
    wxCHECK_MSG( tool, _T(""), _T("no such tool") );

    return tool->GetShortHelp();
}

wxString wxToolBarBase::GetToolLongHelp(int id) const
{
    wxToolBarToolBase *tool = FindById(id);
    wxCHECK_MSG( tool, _T(""), _T("no such tool") );

    return tool->GetLongHelp();
}

// ----------------------------------------------------------------------------
// wxToolBarBase geometry
// ----------------------------------------------------------------------------

void wxToolBarBase::SetMargins(int x, int y)
{
    m_xMargin = x;
    m_yMargin = y;
}

void wxToolBarBase::SetRows(int WXUNUSED(nRows))
{
    // nothing
}

// ----------------------------------------------------------------------------
// event processing
// ----------------------------------------------------------------------------

// Only allow toggle if returns TRUE
bool wxToolBarBase::OnLeftClick(int id, bool toggleDown)
{
    wxCommandEvent event(wxEVT_COMMAND_TOOL_CLICKED, id);
    event.SetEventObject(this);
    event.SetExtraLong((long) toggleDown);

    // Send events to this toolbar instead (and thence up the window hierarchy)
    GetEventHandler()->ProcessEvent(event);

    return TRUE;
}

// Call when right button down.
void wxToolBarBase::OnRightClick(int id,
                                 long WXUNUSED(x),
                                 long WXUNUSED(y))
{
    wxCommandEvent event(wxEVT_COMMAND_TOOL_RCLICKED, id);
    event.SetEventObject(this);
    event.SetInt(id);

    GetEventHandler()->ProcessEvent(event);
}

// Called when the mouse cursor enters a tool bitmap (no button pressed).
// Argument is -1 if mouse is exiting the toolbar.
// Note that for this event, the id of the window is used,
// and the integer parameter of wxCommandEvent is used to retrieve
// the tool id.
void wxToolBarBase::OnMouseEnter(int id)
{
    wxCommandEvent event(wxEVT_COMMAND_TOOL_ENTER, GetId());
    event.SetEventObject(this);
    event.SetInt(id);

    (void)GetEventHandler()->ProcessEvent(event);

    wxToolBarToolBase *tool = FindById(id);
    if ( !tool || !tool->GetLongHelp() )
        return;

    wxFrame *frame = wxDynamicCast(GetParent(), wxFrame);
    if ( !frame )
        return;

    frame->SetStatusText(tool->GetLongHelp());
}

// ----------------------------------------------------------------------------
// UI updates
// ----------------------------------------------------------------------------

void wxToolBarBase::OnIdle(wxIdleEvent& event)
{
    DoToolbarUpdates();

    event.Skip();
}

// Do the toolbar button updates (check for EVT_UPDATE_UI handlers)
void wxToolBarBase::DoToolbarUpdates()
{
    wxWindow* parent = this;
    while (parent->GetParent())
        parent = parent->GetParent();

#ifdef __WXMSW__
    wxWindow* focusWin = wxFindFocusDescendant(parent);
#else
    wxWindow* focusWin = (wxWindow*) NULL;
#endif

    wxEvtHandler* evtHandler = focusWin ? focusWin->GetEventHandler() : GetEventHandler() ;

    for ( wxToolBarToolsList::Node* node = m_tools.GetFirst();
          node;
          node = node->GetNext() )
    {
        int id = node->GetData()->GetId();

        wxUpdateUIEvent event(id);
        event.SetEventObject(this);

        if ( evtHandler->ProcessEvent(event) )
        {
            if ( event.GetSetEnabled() )
                EnableTool(id, event.GetEnabled());
            if ( event.GetSetChecked() )
                ToggleTool(id, event.GetChecked());
#if 0
            if ( event.GetSetText() )
                // Set tooltip?
#endif // 0
        }
    }
}

#endif // wxUSE_TOOLBAR
