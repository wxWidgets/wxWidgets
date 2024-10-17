///////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/overlay.cpp
// Author:      Ali Kettab
// Created:     2023-11-01
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __WXQT__

#include "wx/private/overlay.h"
#include "wx/dc.h"
#include "wx/window.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QPainter>
#include <QtGui/QPicture>
#include <QtGui/QScreen>
#include <QtWidgets/QWidget>

// Inspired by QRubberBand implementation

class wxOverlayWindow : public QWidget
{
public:
    explicit wxOverlayWindow(QWidget* parent)
        : QWidget(parent, parent ? Qt::Widget : Qt::ToolTip)
    {
        if ( !parent )
            setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_WState_ExplicitShowHide);
        setVisible(false);
    }

    void SetPicture(QPicture& pict)
    {
        m_pict.swap(pict);
        update();
    }

private:

    QPicture m_pict;

    virtual void paintEvent(QPaintEvent* WXUNUSED(event)) override
    {
        QPainter painter(this);
        m_pict.play(&painter);
    }
};

class wxOverlayImpl: public wxOverlay::Impl
{
public:
    wxOverlayImpl() = default;
    ~wxOverlayImpl();

    virtual bool IsOk() override;
    virtual void Init(wxDC*, int, int, int, int) override;
    virtual void BeginDrawing(wxDC* dc) override;
    virtual void EndDrawing(wxDC* dc) override;
    virtual void Clear(wxDC* dc) override;
    virtual void Reset() override;

    wxWindow*        m_target  = nullptr;
    wxOverlayWindow* m_overlay = nullptr;
};

wxOverlay::Impl* wxOverlay::Create()
{
    return new wxOverlayImpl;
}


wxOverlayImpl::~wxOverlayImpl()
{
    if ( m_target && !m_target->IsBeingDeleted() )
    {
        if ( m_overlay )
            wxDELETE(m_overlay);
    }
}

bool wxOverlayImpl::IsOk()
{
    return m_overlay != nullptr;
}

void wxOverlayImpl::Init(wxDC* dc, int , int , int , int )
{
    wxASSERT_MSG( !IsOk() , "You cannot Init an overlay twice" );

    wxCHECK_RET( dc, "Invalid dc for wxOverlay" );

    m_target = dc->GetWindow();

    m_overlay = new wxOverlayWindow(m_target ? m_target->GetHandle() : nullptr);
}

void wxOverlayImpl::BeginDrawing(wxDC* WXUNUSED(dc))
{
    wxCHECK_RET( IsOk(), "wxOverlay not initialized" );

    QRect qtRect = m_target ? m_target->GetHandle()->rect()
                            : qGuiApp->primaryScreen()->geometry();

    m_overlay->setGeometry( qtRect );
    m_overlay->show();
    m_overlay->raise();
}

void wxOverlayImpl::EndDrawing(wxDC* dc)
{
    wxCHECK_RET( dc, "Invalid dc for wxOverlay" );

    auto painter = static_cast<QPainter*>(dc->GetHandle());
    auto picture = dynamic_cast<QPicture*>(painter->device());

    if ( picture && !picture->isNull() )
    {
        // take over the picture and draw it on the overlay window
        m_overlay->SetPicture(*picture);
    }
}

void wxOverlayImpl::Clear(wxDC* WXUNUSED(dc))
{
    // Already cleared by wxQt
}

void wxOverlayImpl::Reset()
{
    if ( m_overlay )
    {
        if ( m_target )
            m_overlay->hide();
        else
            wxDELETE(m_overlay);
    }
}

#endif // __WXQT__
