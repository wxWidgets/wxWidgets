///////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_aui.cpp
// Purpose:     Implementation of wxAUI XRC handler.
// Author:      Andrea Zanellato, Steve Lamerton (wxAuiNotebook)
// Created:     2011-09-18
// Copyright:   (c) 2011 wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_XRC && wxUSE_AUI

#include "wx/xrc/xh_aui.h"
#include "wx/aui/framemanager.h"
#include "wx/aui/auibook.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxAuiXmlHandler, wxXmlResourceHandler);

wxAuiXmlHandler::wxAuiXmlHandler()
                : wxXmlResourceHandler(),
                  m_manager(nullptr),
                  m_window(nullptr),
                  m_notebook(nullptr),
                  m_mgrInside(false),
                  m_anbInside(false)
{
    XRC_ADD_STYLE(wxAUI_MGR_ALLOW_ACTIVE_PANE);
    XRC_ADD_STYLE(wxAUI_MGR_ALLOW_FLOATING);
    XRC_ADD_STYLE(wxAUI_MGR_DEFAULT);
    XRC_ADD_STYLE(wxAUI_MGR_HINT_FADE);
    XRC_ADD_STYLE(wxAUI_MGR_LIVE_RESIZE);
    XRC_ADD_STYLE(wxAUI_MGR_NO_VENETIAN_BLINDS_FADE);
    XRC_ADD_STYLE(wxAUI_MGR_RECTANGLE_HINT);
    XRC_ADD_STYLE(wxAUI_MGR_TRANSPARENT_DRAG);
    XRC_ADD_STYLE(wxAUI_MGR_TRANSPARENT_HINT);
    XRC_ADD_STYLE(wxAUI_MGR_VENETIAN_BLINDS_HINT);

    XRC_ADD_STYLE(wxAUI_NB_DEFAULT_STYLE);
    XRC_ADD_STYLE(wxAUI_NB_TAB_SPLIT);
    XRC_ADD_STYLE(wxAUI_NB_TAB_MOVE);
    XRC_ADD_STYLE(wxAUI_NB_TAB_EXTERNAL_MOVE);
    XRC_ADD_STYLE(wxAUI_NB_TAB_FIXED_WIDTH);
    XRC_ADD_STYLE(wxAUI_NB_SCROLL_BUTTONS);
    XRC_ADD_STYLE(wxAUI_NB_WINDOWLIST_BUTTON);
    XRC_ADD_STYLE(wxAUI_NB_CLOSE_BUTTON);
    XRC_ADD_STYLE(wxAUI_NB_CLOSE_ON_ACTIVE_TAB);
    XRC_ADD_STYLE(wxAUI_NB_CLOSE_ON_ALL_TABS);
    XRC_ADD_STYLE(wxAUI_NB_MIDDLE_CLICK_CLOSE);
    XRC_ADD_STYLE(wxAUI_NB_TOP);
    XRC_ADD_STYLE(wxAUI_NB_BOTTOM);

    AddWindowStyles();
}

wxAuiManager *wxAuiXmlHandler::GetAuiManager( wxWindow *managed ) const
{
    for ( Managers::const_iterator it = m_managers.begin();
          it != m_managers.end();
          ++it )
    {
        wxAuiManager* const mgr = *it;
        if ( mgr->GetManagedWindow() == managed )
            return mgr;
    }

    return nullptr;
}

void wxAuiXmlHandler::OnManagedWindowClose( wxWindowDestroyEvent &event )
{
    wxWindow *window = wxDynamicCast( event.GetEventObject(), wxWindow );
    for ( Managers::iterator it = m_managers.begin();
          it != m_managers.end();
          ++it )
    {
        wxAuiManager* const mgr = *it;
        if ( mgr->GetManagedWindow() == window )
        {
            mgr->UnInit();
            m_managers.erase(it);
            break;
        }
    }

    event.Skip();
}

wxObject *wxAuiXmlHandler::DoCreateResource()
{
    if (m_class == wxS("wxAuiManager"))
    {
        wxAuiManager *manager = nullptr;

        if (m_parentAsWindow)
        {
            // Cache the previous values
            bool          old_ins = m_mgrInside;
            wxAuiManager *old_mgr = m_manager;
            wxWindow     *old_win = m_window;

            // Create the manager with the specified or default style and
            // assign the new values related to this manager
            m_window    = m_parentAsWindow;
            manager     = new wxAuiManager( m_window,
                                            GetStyle(wxS("style"), wxAUI_MGR_DEFAULT) );
            m_manager   = manager;
            m_mgrInside = true;

            // Add this manager to our manager vector
            m_managers.push_back(m_manager);

            // Connect the managed window destroy event to
            // automatically UnInit() later this manager
            m_window ->Bind(wxEVT_DESTROY, &wxAuiXmlHandler::OnManagedWindowClose, this);

            // Add AUI panes to this manager
            CreateChildren(m_manager);

            // Load a custom perspective if any
            if (HasParam(wxS("perspective")))
                m_manager->LoadPerspective( GetParamValue(wxS("perspective")) );

            m_manager->Update();

            // Restore the previous values
            m_window    = old_win;
            m_manager   = old_mgr;
            m_mgrInside = old_ins;
        }
        else
        {
            ReportError("No wxWindow derived class to manage for this wxAuiManager.");
        }

        return manager;
    }
    else if (m_class == "wxAuiPaneInfo")
    {
        wxXmlNode *node   = GetParamNode(wxS("object"));
        wxWindow  *window = nullptr;

        if (!node)
            node = GetParamNode(wxS("object_ref"));

        if (node)
        {
            bool old_ins = m_mgrInside;
            m_mgrInside = false;

            wxObject *object = CreateResFromNode(node, m_window, nullptr);

            m_mgrInside = old_ins;
            window = wxDynamicCast( object, wxWindow );

            if (!window && object)
            {
                ReportError( node, "wxAuiPaneInfo child must be a window." );
            }
        }

        if (window)
        {
            wxAuiPaneInfo paneInfo = wxAuiPaneInfo();

            wxString name = GetName();              paneInfo.Name( name );
// Caption
            if ( HasParam(wxS("caption")) )         paneInfo.Caption( GetText(wxS("caption")) );
            if ( HasParam(wxS("caption_visible")) ) paneInfo.CaptionVisible( GetBool(wxS("caption_visible")) );
// Buttons
            if ( HasParam(wxS("close_button")) )    paneInfo.CloseButton( GetBool(wxS("close_button")) );
            if ( HasParam(wxS("minimize_button")) ) paneInfo.MinimizeButton( GetBool(wxS("minimize_button")) );
            if ( HasParam(wxS("maximize_button")) ) paneInfo.MaximizeButton( GetBool(wxS("maximize_button")) );
            if ( HasParam(wxS("pin_button")) )      paneInfo.PinButton( GetBool(wxS("pin_button")) );
            if ( HasParam(wxS("gripper")) )         paneInfo.Gripper( GetBool(wxS("gripper")) );
// Appearance
            if ( HasParam(wxS("pane_border")) )     paneInfo.PaneBorder( GetBool(wxS("pane_border")) );
// State
            if ( HasParam(wxS("dock")) )            paneInfo.Dock();
            else if ( HasParam(wxS("float")) )      paneInfo.Float();

// Dockable Directions
            if ( HasParam(wxS("top_dockable")) )    paneInfo.TopDockable( GetBool(wxS("top_dockable")) );
            if ( HasParam(wxS("bottom_dockable")) ) paneInfo.BottomDockable( GetBool(wxS("bottom_dockable")) );
            if ( HasParam(wxS("left_dockable")) )   paneInfo.LeftDockable( GetBool(wxS("left_dockable")) );
            if ( HasParam(wxS("right_dockable")) )  paneInfo.RightDockable( GetBool(wxS("right_dockable")) );
// Behaviours
            if ( HasParam(wxS("dock_fixed")) )      paneInfo.DockFixed( GetBool(wxS("dock_fixed")) );
            if ( HasParam(wxS("resizable")) )       paneInfo.Resizable( GetBool(wxS("resizable")) );
            if ( HasParam(wxS("movable")) )         paneInfo.Movable( GetBool(wxS("movable")) );
            if ( HasParam(wxS("floatable")) )       paneInfo.Floatable( GetBool(wxS("floatable")) );
// Sizes
            if ( HasParam(wxS("floating_size")) )   paneInfo.FloatingSize( GetSize(wxS("floating_size")) );
            if ( HasParam(wxS("min_size")) )        paneInfo.MinSize( GetSize(wxS("min_size")) );
            if ( HasParam(wxS("max_size")) )        paneInfo.MaxSize( GetSize(wxS("max_size")) );
            if ( HasParam(wxS("best_size")) )       paneInfo.BestSize( GetSize(wxS("best_size")) );
// Positions
            if ( HasParam(wxS("row")) )             paneInfo.Row( GetLong(wxS("row")) );
            if ( HasParam(wxS("layer")) )           paneInfo.Layer( GetLong(wxS("layer")) );
            if ( HasParam(wxS("default_pane")) )    paneInfo.DefaultPane();
            else if( HasParam(wxS("toolbar_pane")) ) paneInfo.ToolbarPane();

// Directions - CenterPane()/CentrePane != Center()/Centre()
            if ( HasParam(wxS("center_pane") ) ||
                 HasParam(wxS("centre_pane")) )     paneInfo.CenterPane();
            if ( HasParam(wxS("direction")) )       paneInfo.Direction( GetLong(wxS("direction")) );
            else if ( HasParam(wxS("top")) )        paneInfo.Top();
            else if ( HasParam(wxS("bottom")) )     paneInfo.Bottom();
            else if ( HasParam(wxS("left")) )       paneInfo.Left();
            else if ( HasParam(wxS("right")) )      paneInfo.Right();
            else if ( HasParam(wxS("center")) ||
                      HasParam(wxS("centre")) )     paneInfo.Center();

            m_manager->AddPane(window, paneInfo);
        }
        else
        {
            ReportError("No wxWindow derived class object specified inside wxAuiPaneInfo.");
        }

        return window;
    }
    else if (m_class == wxS("notebookpage"))
    {
        wxXmlNode *anb = GetParamNode(wxS("object"));

        if (!anb)
            anb = GetParamNode(wxS("object_ref"));

        if (anb)
        {
            bool old_ins = m_anbInside;
            m_anbInside = false;
            wxObject *item = CreateResFromNode(anb, m_notebook, nullptr);
            m_anbInside = old_ins;
            wxWindow *wnd = wxDynamicCast(item, wxWindow);

            if (wnd)
            {
                if ( HasParam(wxS("bitmap")) )
                {
                    m_notebook->AddPage(wnd,
                                        GetText(wxS("label")),
                                        GetBool(wxS("selected")),
                                        GetBitmapBundle(wxS("bitmap"), wxART_OTHER));
                }
                else
                {
                    m_notebook->AddPage(wnd,
                                        GetText(wxS("label")),
                                        GetBool(wxS("selected")));
                }
            }
            else
            {
                ReportError(anb, "notebookpage child must be a window");
            }
            return wnd;
        }
        else
        {
            ReportError("notebookpage must have a window child");
            return nullptr;
        }
    }
    else // if (m_class == wxS("wxAuiNotebook"))
    {
        XRC_MAKE_INSTANCE(anb, wxAuiNotebook)

        anb->Create(m_parentAsWindow,
                    GetID(),
                    GetPosition(),
                    GetSize(),
                    GetStyle(wxS("style")));

        wxString provider = GetText("art-provider", false);
        if (provider == "default" || provider.IsEmpty())
            anb->SetArtProvider(new wxAuiDefaultTabArt);
        else if (provider.CmpNoCase("simple") == 0)
            anb->SetArtProvider(new wxAuiSimpleTabArt);
        else
            ReportError("invalid wxAuiNotebook art provider");

        SetupWindow(anb);

        wxAuiNotebook *old_par = m_notebook;
        m_notebook = anb;
        bool old_ins = m_anbInside;
        m_anbInside = true;
        CreateChildren(m_notebook, true/*only this handler*/);
        m_anbInside = old_ins;
        m_notebook = old_par;

        return anb;
    }
}

bool wxAuiXmlHandler::CanHandle( wxXmlNode *node )
{
    return ((!m_mgrInside && IsOfClass(node, wxS("wxAuiManager")))  ||
            (m_mgrInside && IsOfClass(node, wxS("wxAuiPaneInfo")))  ||
            (!m_anbInside && IsOfClass(node, wxS("wxAuiNotebook"))) ||
            (m_anbInside && IsOfClass(node, wxS("notebookpage")))   );
}

#endif // wxUSE_XRC && wxUSE_AUI
