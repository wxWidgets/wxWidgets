/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/toolbar.cpp
// Author:      Sean D'Epagnier, Mariano Reingart, Peter Most
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_TOOLBAR

#include <QtWidgets/QActionGroup>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QToolBar>

#ifndef WX_PRECOMP
    #include "wx/menu.h"
#endif // WX_PRECOMP

#include "wx/toolbar.h"
#include "wx/qt/private/winevent.h"
#include "wx/qt/private/converter.h"


class wxQtToolButton;
class wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(wxToolBar *tbar, int id, const wxString& label, const wxBitmapBundle& bitmap1,
                  const wxBitmapBundle& bitmap2, wxItemKind kind, wxObject *clientData,
                  const wxString& shortHelpString, const wxString& longHelpString)
        : wxToolBarToolBase(tbar, id, label, bitmap1, bitmap2, kind,
                            clientData, shortHelpString, longHelpString)
    {
        m_qtToolButton = nullptr;
    }

    wxToolBarTool(wxToolBar *tbar, wxControl *control, const wxString& label)
        : wxToolBarToolBase(tbar, control, label)
    {
        m_qtToolButton = nullptr;
    }

    virtual void SetLabel( const wxString &label ) override;
    virtual void SetDropdownMenu(wxMenu* menu) override;

    void SetIcon();
    void ClearToolTip();
    void SetToolTip();

    wxQtToolButton* m_qtToolButton;
};

class wxQtToolButton : public QToolButton, public wxQtSignalHandler< wxToolBarTool >
{

public:
    wxQtToolButton(wxToolBar *parent, wxToolBarTool *handler)
        : QToolButton(parent->GetHandle()),
          wxQtSignalHandler< wxToolBarTool >( handler ) {
        setContextMenuPolicy(Qt::PreventContextMenu);
    }

private:
    void mouseReleaseEvent( QMouseEvent *event ) override;
    void mousePressEvent( QMouseEvent *event ) override;
    void enterEvent( QEvent *event ) override;
};

void wxQtToolButton::mouseReleaseEvent( QMouseEvent *event )
{
    QToolButton::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton) {
        wxToolBarTool *handler = GetHandler();
        wxToolBarBase *toolbar = handler->GetToolBar();
        toolbar->OnLeftClick( handler->GetId(), isCheckable() ? 1 : 0 );
    }
}

void wxQtToolButton::mousePressEvent( QMouseEvent *event )
{
    QToolButton::mousePressEvent(event);
    if (event->button() == Qt::RightButton) {
        wxToolBarTool *handler = GetHandler();
        wxToolBarBase *toolbar = handler->GetToolBar();
        toolbar->OnRightClick( handler->GetId(), event->x(), event->y() );
    }
}

void wxQtToolButton::enterEvent( QEvent *WXUNUSED(event) )
{
    wxToolBarTool *handler = GetHandler();
    wxToolBarBase *toolbar = handler->GetToolBar();
    toolbar->OnMouseEnter( handler->GetId() );
}

wxIMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxControl);

void wxToolBarTool::SetLabel( const wxString &label )
{
    wxToolBarToolBase::SetLabel( label );

    if (m_qtToolButton) {
        m_qtToolButton->setText(wxQtConvertString( label ));
    }
}

void wxToolBarTool::SetDropdownMenu(wxMenu* menu)
{
    wxToolBarToolBase::SetDropdownMenu(menu);
    m_qtToolButton->setMenu(menu->GetHandle());
    menu->SetInvokingWindow(GetToolBar());
}

void wxToolBarTool::SetIcon()
{
    m_qtToolButton->setIcon( QIcon( *GetNormalBitmap().GetHandle() ));
}

void wxToolBarTool::ClearToolTip()
{
    m_qtToolButton->setToolTip("");
}

void wxToolBarTool::SetToolTip()
{
    m_qtToolButton->setToolTip(wxQtConvertString( GetShortHelp() ));
}


class wxQtToolbar : public wxQtEventSignalHandler< QToolBar, wxToolBar >
{
public:
    wxQtToolbar( wxWindow *parent, wxToolBar *handler );

};

wxQtToolbar::wxQtToolbar( wxWindow *parent, wxToolBar *handler )
    : wxQtEventSignalHandler< QToolBar, wxToolBar >( parent, handler )
{
}


QWidget *wxToolBar::GetHandle() const
{
    return m_qtToolBar;
}

void wxToolBar::Init()
{
    m_qtToolBar = nullptr;
}

wxToolBar::~wxToolBar()
{
}

bool wxToolBar::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos,
                       const wxSize& size, long style, const wxString& name)
{
    m_qtToolBar = new wxQtToolbar( parent, this );
    m_qtToolBar->setWindowTitle( wxQtConvertString( name ) );

    SetWindowStyleFlag(style);

    // not calling to wxWindow::Create, so do the rest of initialization:
    if (parent)
        parent->AddChild( this );

    PostCreation();

    return wxWindowBase::CreateBase( parent, id, pos, size, style, wxDefaultValidator, name );
}

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord WXUNUSED(x),
                                                  wxCoord WXUNUSED(y)) const
{
//    actionAt(x, y);
    wxFAIL_MSG( wxT("wxToolBar::FindToolForPosition() not implemented") );
    return nullptr;
}

void wxToolBar::SetToolShortHelp( int id, const wxString& helpString )
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(FindById(id));
    if ( tool )
    {
        (void)tool->SetShortHelp(helpString);
        //TODO - other qt actions for tool tip string
//        if (tool->m_item)
//        {}
    }
}

void wxToolBar::SetToolNormalBitmap( int id, const wxBitmapBundle& bitmap )
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(FindById(id));
    if ( tool )
    {
        wxCHECK_RET( tool->IsButton(), wxT("Can only set bitmap on button tools."));

        tool->SetNormalBitmap(bitmap);
        tool->SetIcon();
    }
}

void wxToolBar::SetToolDisabledBitmap( int id, const wxBitmapBundle& bitmap )
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(FindById(id));
    if ( tool )
    {
        wxCHECK_RET( tool->IsButton(), wxT("Can only set bitmap on button tools."));

        tool->SetDisabledBitmap(bitmap);
    }
}

void wxToolBar::SetWindowStyleFlag( long style )
{
    wxToolBarBase::SetWindowStyleFlag(style);

    if ( !m_qtToolBar )
        return;

    m_qtToolBar->setOrientation( IsVertical() ? Qt::Vertical : Qt::Horizontal);

    Qt::ToolButtonStyle buttonStyle = (Qt::ToolButtonStyle)GetButtonStyle();

    // bring the initial state of all the toolbar items in line with the
    for ( wxToolBarToolsList::const_iterator i = m_tools.begin();
          i != m_tools.end();         ++i )
    {
        wxToolBarTool* tool = static_cast<wxToolBarTool*>(*i);
        if (!tool->m_qtToolButton)
            continue;

        tool->m_qtToolButton->setToolButtonStyle(buttonStyle);
    }
}

bool wxToolBar::Realize()
{
    if ( !wxToolBarBase::Realize() )
        return false;

    // bring the initial state of all the toolbar items in line with the
    for ( wxToolBarToolsList::const_iterator i = m_tools.begin();
          i != m_tools.end();         ++i )
    {
        wxToolBarTool* tool = static_cast<wxToolBarTool*>(*i);
        if (!tool->m_qtToolButton)
            continue;

        tool->m_qtToolButton->setEnabled(tool->IsEnabled());
        tool->m_qtToolButton->setChecked(tool->IsToggled());

        if (HasFlag(wxTB_NO_TOOLTIPS))
            tool->ClearToolTip();
        else
            tool->SetToolTip();
    }

    return true;
}

QActionGroup* wxToolBar::GetActionGroup(size_t pos)
{
    QActionGroup *actionGroup = nullptr;
    if (pos > 0)
        actionGroup = m_qtToolBar->actions().at(pos-1)->actionGroup();
    if (actionGroup == nullptr && (int)pos < m_qtToolBar->actions().size() - 1)
        actionGroup = m_qtToolBar->actions().at(pos+1)->actionGroup();
    if (actionGroup == nullptr)
        actionGroup = new QActionGroup(m_qtToolBar);
    return actionGroup;
}

bool wxToolBar::DoInsertTool(size_t pos, wxToolBarToolBase *toolBase)
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(toolBase);
    QAction *before = nullptr;
    if (pos < (size_t)m_qtToolBar->actions().size())
        before = m_qtToolBar->actions().at(pos);

    QAction *action;
    switch ( tool->GetStyle() )
    {
        case wxTOOL_STYLE_BUTTON:
            tool->m_qtToolButton = new wxQtToolButton(this, tool);
            tool->m_qtToolButton->setToolButtonStyle((Qt::ToolButtonStyle)GetButtonStyle());
            tool->SetLabel( tool->GetLabel() );

            if (!HasFlag(wxTB_NOICONS))
                tool->SetIcon();
            if (!HasFlag(wxTB_NO_TOOLTIPS))
                tool->SetToolTip();

            action = m_qtToolBar->insertWidget(before, tool->m_qtToolButton);

            switch (tool->GetKind())
            {
            default:
                wxFAIL_MSG("unknown toolbar child type");
                // fall through
            case wxITEM_RADIO:
                GetActionGroup(pos)->addAction(action);
                // fall-through
            case wxITEM_CHECK:
                tool->m_qtToolButton->setCheckable(true);
            case wxITEM_DROPDOWN:
            case wxITEM_NORMAL:
                break;
            }
            break;

        case wxTOOL_STYLE_SEPARATOR:
            if (tool->IsStretchable()) {
                QWidget* spacer = new QWidget();
                spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                m_qtToolBar->insertWidget(before, spacer);
            } else
                m_qtToolBar->insertSeparator(before);
            break;

        case wxTOOL_STYLE_CONTROL:
            wxWindow* control = tool->GetControl();
            m_qtToolBar->insertWidget(before, control->GetHandle());
            break;
    }

    InvalidateBestSize();

    return true;
}

bool wxToolBar::DoDeleteTool(size_t /* pos */, wxToolBarToolBase *toolBase)
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(toolBase);
    delete tool->m_qtToolButton;
    tool->m_qtToolButton = nullptr;

    InvalidateBestSize();
    return true;
}

void wxToolBar::DoEnableTool(wxToolBarToolBase *toolBase, bool enable)
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(toolBase);
    tool->m_qtToolButton->setEnabled(enable);
}

void wxToolBar::DoToggleTool(wxToolBarToolBase *toolBase, bool toggle)
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(toolBase);
    tool->m_qtToolButton->setChecked(toggle);
}

void wxToolBar::DoSetToggle(wxToolBarToolBase * WXUNUSED(tool),
                            bool WXUNUSED(toggle))
{
    // VZ: absolutely no idea about how to do it
    wxFAIL_MSG( wxT("not implemented") );
}

wxToolBarToolBase *wxToolBar::CreateTool(int id, const wxString& label, const wxBitmapBundle& bmpNormal,
                                      const wxBitmapBundle& bmpDisabled, wxItemKind kind, wxObject *clientData,
                                      const wxString& shortHelp, const wxString& longHelp)
{
    return new wxToolBarTool(this, id, label, bmpNormal, bmpDisabled, kind,
                             clientData, shortHelp, longHelp);
}

wxToolBarToolBase *wxToolBar::CreateTool(wxControl *control,
                                          const wxString& label)
{
    return new wxToolBarTool(this, control, label);
}

long wxToolBar::GetButtonStyle()
{
    if (!HasFlag(wxTB_NOICONS)) {
        if (HasFlag(wxTB_HORZ_LAYOUT))
            return Qt::ToolButtonTextBesideIcon;
        else if (HasFlag(wxTB_TEXT))
            return Qt::ToolButtonTextUnderIcon;
        else
            return Qt::ToolButtonIconOnly;
    }
    return Qt::ToolButtonTextOnly;
}

#endif // wxUSE_TOOLBAR

