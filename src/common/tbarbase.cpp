/////////////////////////////////////////////////////////////////////////////
// Name:        common/tbarbase.cpp
// Purpose:     wxToolBarBase implementation
// Author:      Julian Smart
// Modified by: VZ at 11.12.99 (wxScrollableToolBar splitted off)
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
    #pragma implementation "tbarbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TOOLBAR

#ifndef WX_PRECOMP
    #include "wx/control.h"
#endif

#include "wx/frame.h"
#include "wx/image.h"
#include "wx/settings.h"

#include "wx/tbarbase.h"

// ----------------------------------------------------------------------------
// wxWindows macros
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxToolBarBase, wxControl)

BEGIN_EVENT_TABLE(wxToolBarBase, wxControl)
    EVT_IDLE(wxToolBarBase::OnIdle)
END_EVENT_TABLE()

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
    wxASSERT_MSG( CanBeToggled(), _T("can't toggle this tool") );

    if ( m_toggled == toggle )
        return FALSE;

    m_toggled = toggle;

    return TRUE;
}

bool wxToolBarToolBase::SetToggle(bool toggle)
{
    wxItemKind kind = toggle ? wxITEM_CHECK : wxITEM_NORMAL;
    if ( m_kind == kind )
        return FALSE;

    m_kind = kind;

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

wxToolBarToolBase *wxToolBarBase::DoAddTool(int id,
                                            const wxString& label,
                                            const wxBitmap& bitmap,
                                            const wxBitmap& bmpDisabled,
                                            wxItemKind kind,
                                            const wxString& shortHelp,
                                            const wxString& longHelp,
                                            wxObject *clientData,
                                            wxCoord WXUNUSED(xPos),
                                            wxCoord WXUNUSED(yPos))
{
    return InsertTool(GetToolsCount(), id, label, bitmap, bmpDisabled,
                      kind, shortHelp, longHelp, clientData);
}

wxToolBarToolBase *wxToolBarBase::InsertTool(size_t pos,
                                             int id,
                                             const wxString& label,
                                             const wxBitmap& bitmap,
                                             const wxBitmap& bmpDisabled,
                                             wxItemKind kind,
                                             const wxString& shortHelp,
                                             const wxString& longHelp,
                                             wxObject *clientData)
{
    wxCHECK_MSG( pos <= GetToolsCount(), (wxToolBarToolBase *)NULL,
                 _T("invalid position in wxToolBar::InsertTool()") );

    wxToolBarToolBase *tool = CreateTool(id, label, bitmap, bmpDisabled, kind,
                                         clientData, shortHelp, longHelp);

    if ( !InsertTool(pos, tool) )
    {
        delete tool;

        return NULL;
    }

    return tool;
}

wxToolBarToolBase *wxToolBarBase::AddTool(wxToolBarToolBase *tool)
{
    return InsertTool(GetToolsCount(), tool);
}

wxToolBarToolBase *
wxToolBarBase::InsertTool(size_t pos, wxToolBarToolBase *tool)
{
    wxCHECK_MSG( pos <= GetToolsCount(), (wxToolBarToolBase *)NULL,
                 _T("invalid position in wxToolBar::InsertTool()") );

    if ( !tool || !DoInsertTool(pos, tool) )
    {
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

    if ( !InsertTool(pos, tool) )
    {
        delete tool;

        return NULL;
    }

    return tool;
}

wxControl *wxToolBarBase::FindControl( int id )
{
    for ( wxToolBarToolsList::Node* node = m_tools.GetFirst();
          node;
          node = node->GetNext() )
    {
        const wxToolBarToolBase * const tool = node->GetData();
        if ( tool->IsControl() )
        {
            wxControl * const control = tool->GetControl();

            if ( !control )
            {
                wxFAIL_MSG( _T("NULL control in toolbar?") );
            }
            else if ( control->GetId() == id )
            {
                // found
                return control;
            }
        }
    }

   return NULL;
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
                                         wxEmptyString,
                                         wxNullBitmap, wxNullBitmap,
                                         wxITEM_SEPARATOR, (wxObject *)NULL,
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

        tool = NULL;
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

int wxToolBarBase::GetToolPos(int id) const
{
    size_t pos = 0;
    wxToolBarToolsList::Node *node;

    for ( node = m_tools.GetFirst(); node; node = node->GetNext() )
    {
        if ( node->GetData()->GetId() == id )
            return pos;

        pos++;
    }

    return wxNOT_FOUND;
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

    // we use SetInt() to make wxCommandEvent::IsChecked() return toggleDown
    event.SetInt((int)toggleDown);

    // and SetExtraLong() for backwards compatibility
    event.SetExtraLong((long)toggleDown);

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

    wxFrame *frame = wxDynamicCast(GetParent(), wxFrame);
    if( frame )
    {
        wxToolBarToolBase* tool = id == -1 ? (wxToolBarToolBase*)0 : FindById(id);
        wxString help = tool ? tool->GetLongHelp() : wxString();
        frame->DoGiveHelp( help, id != -1 );
    }

    (void)GetEventHandler()->ProcessEvent(event);
}

// ----------------------------------------------------------------------------
// UI updates
// ----------------------------------------------------------------------------

void wxToolBarBase::OnIdle(wxIdleEvent& event)
{
    if (wxUpdateUIEvent::CanUpdate())
        DoToolbarUpdates();

    event.Skip();
}

// Do the toolbar button updates (check for EVT_UPDATE_UI handlers)
void wxToolBarBase::DoToolbarUpdates()
{
    wxEvtHandler* evtHandler = GetEventHandler() ;

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

// Helper function, used by wxCreateGreyedImage

static void wxGreyOutImage( const wxImage& src,
                            wxImage& dest,
                            const wxColour& darkCol,
                            const wxColour& lightCol,
                            const wxColour& bgCol )
{
    // Second attempt, just making things monochrome
    int width = src.GetWidth();
    int height = src.GetHeight();

    int redCur, greenCur, blueCur;
    for ( int x = 0; x < width; x++ )
    {
        for ( int y = 1; y < height; y++ )
        {
            redCur = src.GetRed(x, y);
            greenCur = src.GetGreen(x, y);
            blueCur = src.GetBlue(x, y);

            // Change light things to the background colour
            if ( redCur >= (lightCol.Red() - 50) && greenCur >= (lightCol.Green() - 50) && blueCur >= (lightCol.Blue() - 50) )
            {
                dest.SetRGB(x,y, bgCol.Red(), bgCol.Green(), bgCol.Blue());
            }
            else if ( redCur == bgCol.Red() && greenCur == bgCol.Green() && blueCur == bgCol.Blue() )
            {
                // Leave the background colour as-is
                // dest.SetRGB(x,y, bgCol.Red(), bgCol.Green(), bgCol.Blue());
            }
            else // if ( redCur <= darkCol.Red() && greenCur <= darkCol.Green() && blueCur <= darkCol.Blue() )
            {
                // Change dark things to really dark
                dest.SetRGB(x,y, darkCol.Red(), darkCol.Green(), darkCol.Blue());
            }
        }
    }
}

/*
 * Make a greyed-out image suitable for disabled buttons.
 * This code is adapted from wxNewBitmapButton in FL.
 */

bool wxCreateGreyedImage(const wxImage& in, wxImage& out)
{
    out = in.Copy();

    // assuming the pixels along the edges are of the background color
    wxColour bgCol(in.GetRed(0, 0), in.GetGreen(0, 0), in.GetBlue(0, 0));

    wxColour darkCol = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW) ;
    wxColour lightCol = wxSystemSettings::GetColour(wxSYS_COLOUR_3DHIGHLIGHT) ;

    wxGreyOutImage(in, out, darkCol, lightCol, bgCol);

    return TRUE;
}

#endif // wxUSE_TOOLBAR
