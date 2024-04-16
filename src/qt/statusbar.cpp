/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/statusbar.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart, Sean D'Epagnier
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STATUSBAR

#include "wx/statusbr.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QStatusBar>
#include <QtWidgets/QLabel>

class wxQtStatusBar : public wxQtEventSignalHandler< QStatusBar, wxStatusBar >
{

public:
    wxQtStatusBar( wxWindow *parent, wxStatusBar *handler );
};

wxQtStatusBar::wxQtStatusBar( wxWindow *parent, wxStatusBar *handler )
    : wxQtEventSignalHandler< QStatusBar, wxStatusBar >( parent, handler )
{
}

//==============================================================================


wxStatusBar::wxStatusBar(wxWindow *parent, wxWindowID winid,
            long style,
            const wxString& name)
{
    Create( parent, winid, style, name );
}

bool wxStatusBar::Create(wxWindow *parent, wxWindowID id,
                         long style, const wxString& name)
{
    m_qtStatusBar = new wxQtStatusBar( parent, this );

    if ( !wxStatusBarBase::Create( parent, id, wxDefaultPosition, wxDefaultSize,
                           style, wxDefaultValidator, name ) )
        return false;

    if ( style & wxSTB_SIZEGRIP )
        m_qtStatusBar->setSizeGripEnabled(true);

    SetFieldsCount(1);

    // Notice that child controls, if any, will be added using addWidget() in
    // CreateFieldsIfNeeded() function. So Unbind the base class handler which is not
    // needed here. And more importantely, it won't work properly either.
    Unbind(wxEVT_SIZE, &wxStatusBar::OnSize, static_cast<wxStatusBarBase*>(this));

    Bind(wxEVT_SIZE, [this](wxSizeEvent& event)
        {
            const int n = this->GetFieldsCount();
            for ( int i = 0; i < n; ++i )
            {
                // Update ellipsized status texts.
                this->DoUpdateStatusText(i);
            }

            event.Skip();
        });

    return true;
}

void wxStatusBar::SetStatusWidths(int number, const int widths[])
{
    if ( number != (int)m_panes.GetCount() )
        return;

    if ( !m_qtPanes.empty() )
    {
        int i = 0;
        for ( auto pane : m_qtPanes )
        {
            m_qtStatusBar->removeWidget(pane);

            // Do not delete user-added controls.
            if ( !m_panes[i++].GetFieldControl() )
            {
                delete pane;
            }
        }

        m_qtPanes.clear();
    }

    wxStatusBarBase::SetStatusWidths(number, widths);
}

bool wxStatusBar::GetFieldRect(int i, wxRect& rect) const
{
    wxCHECK_MSG( (i >= 0) && ((size_t)i < m_panes.GetCount()), false,
                 "invalid statusbar field index" );

    const_cast<wxStatusBar *>(this)->CreateFieldsIfNeeded();

    rect = wxQtConvertRect(m_qtPanes[i]->geometry());
    return true;
}

void wxStatusBar::SetMinHeight(int height)
{
    m_qtStatusBar->setMinimumHeight(height);
}

int wxStatusBar::GetBorderX() const
{
    return 0;
}

int wxStatusBar::GetBorderY() const
{
    return 0;
}

void wxStatusBar::DoUpdateStatusText(int number)
{
    CreateFieldsIfNeeded();

    const auto pane = dynamic_cast<QLabel*>(m_qtPanes[number]);

    if ( !pane )
    {
        // do nothing if this pane is a field control.
        return;
    }

    QString text = wxQtConvertString( m_panes[number].GetText() );

    // do we need to ellipsize this string?
    Qt::TextElideMode ellmode = Qt::ElideNone;
    if ( HasFlag(wxSTB_ELLIPSIZE_START) )       ellmode = Qt::ElideLeft;
    else if ( HasFlag(wxSTB_ELLIPSIZE_MIDDLE) ) ellmode = Qt::ElideMiddle;
    else if ( HasFlag(wxSTB_ELLIPSIZE_END) )    ellmode = Qt::ElideRight;

    if ( ellmode != Qt::ElideNone )
    {
        QFontMetrics metrics(pane->font());
        QString elidedText = metrics.elidedText(text, ellmode, pane->width());

        if ( HasFlag(wxSTB_SHOW_TIPS) )
        {
            elidedText != text ? pane->setToolTip(text) : pane->setToolTip(QString());
        }

        text = elidedText;
    }

    pane->setText(text);
}

void wxStatusBar::CreateFieldsIfNeeded()
{
    if ( !m_qtPanes.empty() )
        return;

    for ( size_t i = 0; i < m_panes.GetCount(); ++i )
    {
        //Set sizes
        const int width = m_bSameWidthForAllPanes ? -1 : m_panes[i].GetWidth();

        QWidget* pane;
        wxWindow* win = m_panes[i].GetFieldControl();

        if ( win )
        {
            pane = win->GetHandle();
        }
        else
        {
            pane = new QLabel;
            if ( width >= 0 )
                pane->setMinimumWidth(width);

            int style;
            switch( m_panes[i].GetStyle() )
            {
                case wxSB_RAISED:
                    style = QFrame::Panel | QFrame::Raised;
                    break;
                case wxSB_SUNKEN:
                    style = QFrame::Panel | QFrame::Sunken;
                    break;
                case wxSB_NORMAL:
                case wxSB_FLAT:
                default:
                    style = QFrame::Plain | QFrame::NoFrame;
                    break;
            }

            static_cast<QLabel*>(pane)->setFrameStyle(style);
        }

        m_qtPanes.push_back(pane);

        m_qtStatusBar->addWidget(pane, width < 0 ? -width : 0);
    }
}

QWidget *wxStatusBar::GetHandle() const
{
    return m_qtStatusBar;
}

#endif
