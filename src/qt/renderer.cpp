///////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/renderer.cpp
// Purpose:     implementation of wxRendererNative for wxQt
// Author:      Kettab Ali
// Created:     2024-01-08
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/renderer.h"

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/dcclient.h"
#endif

#include "wx/headerctrl.h" // for wxHD_BITMAP_ON_RIGHT
#include "wx/qt/private/converter.h"

#include <QtGui/QPainter>
#include <QtWidgets/QStyle>
#include <QtWidgets/QStyleOptionButton>
#include <QtWidgets/QStyleOptionFrame>
#include <QtWidgets/QStyleOptionHeader>

namespace
{
bool wxIsKDEDesktop()
{
    wxString de = wxGetenv(wxS("XDG_CURRENT_DESKTOP"));

    if ( !de.empty() )
    {
        // Can be a colon separated list according to
        // https://wiki.archlinux.org/title/Environment_variables#Examples
        de = de.BeforeFirst(':');
    }

    de.MakeUpper();

    return de.Contains(wxS("KDE"));
}
}

// ----------------------------------------------------------------------------
// wxRendererQt: our wxRendererNative implementation
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRendererQt : public wxDelegateRendererNative
{
public:
    // draw the header control button (used by wxDataViewCtrl)
    virtual int  DrawHeaderButton(wxWindow *win,
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int flags = 0,
                                  wxHeaderSortIconType sortArrow = wxHDR_SORT_ICON_NONE,
                                  wxHeaderButtonParams* params = nullptr) override;

    virtual int GetHeaderButtonHeight(wxWindow *win) override;

    virtual int GetHeaderButtonMargin(wxWindow *win) override;


    // draw the expanded/collapsed icon for a tree control item
    virtual void DrawTreeItemButton(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0) override;

    virtual void DrawSplitterBorder(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0) override;

    virtual void DrawSplitterSash(wxWindow *win,
                                  wxDC& dc,
                                  const wxSize& size,
                                  wxCoord position,
                                  wxOrientation orient,
                                  int flags = 0) override;

    virtual void DrawCheckBox(wxWindow *win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags = 0) override;

    virtual void DrawCheckMark(wxWindow *win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags = 0) override;

    virtual wxSize GetCheckMarkSize(wxWindow *win) override;

    virtual void DrawPushButton(wxWindow *win,
                                wxDC& dc,
                                const wxRect& rect,
                                int flags = 0) override;

    virtual void DrawItemSelectionRect(wxWindow *win,
                                       wxDC& dc,
                                       const wxRect& rect,
                                       int flags = 0) override;

    virtual void DrawChoice(wxWindow* win,
                            wxDC& dc,
                            const wxRect& rect,
                            int flags = 0) override;

    virtual void DrawComboBox(wxWindow* win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags = 0) override;

    virtual void DrawGauge(wxWindow* win,
                           wxDC& dc,
                           const wxRect& rect,
                           int value,
                           int max,
                           int flags = 0);

    virtual void DrawRadioBitmap(wxWindow* win,
                                wxDC& dc,
                                const wxRect& rect,
                                int flags = 0 ) override;

    virtual void DrawTextCtrl(wxWindow* win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags = 0) override;

    virtual void DrawFocusRect(wxWindow* win,
                               wxDC& dc,
                               const wxRect& rect,
                               int flags = 0) override;

    virtual wxSize GetCheckBoxSize(wxWindow *win, int flags = 0) override;

    virtual wxSplitterRenderParams GetSplitterParams(const wxWindow *win) override;

    // Common code for DrawComboBox() and DrawChoice()
    void DoDrawComboBox(wxWindow* win,
                        wxDC& dc,
                        const wxRect& rect,
                        int flags, bool editable = true);
};

// ============================================================================
// implementation
// ============================================================================

/* static */
wxRendererNative& wxRendererNative::GetDefault()
{
    static wxRendererQt s_rendererQt;

    return s_rendererQt;
}

static QPainter* wxGetQtPainter(const wxDC& dc)
{
    return static_cast<QPainter*>(dc.GetHandle());
}

// ----------------------------------------------------------------------------
// list/tree controls drawing
// ----------------------------------------------------------------------------

int
wxRendererQt::DrawHeaderButton(wxWindow *win,  wxDC& dc, const wxRect& rect, int flags,
                               wxHeaderSortIconType sortArrow, wxHeaderButtonParams* params)
{
    auto painter = wxGetQtPainter(dc);

    wxCHECK_MSG( painter, 0, "Invalid painter!" );

    wxDCClipper clip(dc, rect);

    auto qtWidget = win->GetHandle();
    auto qtStyle = qtWidget->style();

    int margin = qtStyle->pixelMetric(QStyle::PM_HeaderMargin, nullptr, qtWidget);
    int bestWidth = qtStyle->pixelMetric(QStyle::PM_HeaderDefaultSectionSizeHorizontal, nullptr, qtWidget)
                  + 2 * margin;

    QStyleOptionHeader option;
    option.initFrom(qtWidget);
    option.rect = wxQtConvertRect(rect);

    if ( flags & wxCONTROL_DISABLED )
    {
        option.palette.setCurrentColorGroup(QPalette::Disabled);
    }
    else
    {
        option.state = QStyle::State_Enabled;

        if ( flags & wxCONTROL_CURRENT )
            option.state |= QStyle::State_MouseOver;
        if ( flags & wxCONTROL_SELECTED )
            option.state |= QStyle::State_Sunken;
    }

    if ( params )
    {
        option.text = wxQtConvertString(params->m_labelText);

        if ( params->m_labelColour.IsOk() )
        {
            option.palette.setColor(QPalette::WindowText,
                                    wxQtConvertColour(params->m_labelColour));
        }

        if ( params->m_labelFont.IsOk() )
        {
            option.fontMetrics = QFontMetrics(params->m_labelFont.GetHandle());
        }

        switch ( params->m_labelAlignment )
        {
            default:
            case wxALIGN_LEFT:
                option.textAlignment = Qt::AlignLeft;
                break;

            case wxALIGN_CENTER:
                option.textAlignment = Qt::AlignHCenter;
                break;

            case wxALIGN_RIGHT:
                option.textAlignment = Qt::AlignRight;
                break;
        }

        if ( params->m_labelBitmap.IsOk() )
        {
            option.icon = *params->m_labelBitmap.GetHandle();
            option.iconAlignment = Qt::AlignVCenter;

            if ( win->HasFlag(wxHD_BITMAP_ON_RIGHT) )
                option.iconAlignment |= Qt::AlignRight;

            bestWidth += qtStyle->pixelMetric(QStyle::PM_SmallIconSize, nullptr, qtWidget);
        }

        bestWidth = wxMax(bestWidth, option.fontMetrics.boundingRect(option.text).width());
    }

#if 0
    if ( sortArrow != wxHDR_SORT_ICON_NONE )
    {
        // May or may not be a bug, but this makes drawControl(QStyle::CE_Header, ...)
        // below not draw anything at all under non-KDE desktops! So we'll draw the sort
        // indicator ourselves later.

        option.sortIndicator = (sortArrow == wxHDR_SORT_ICON_UP)
                             ? QStyleOptionHeader::SortUp
                             : QStyleOptionHeader::SortDown;
    }
#endif

    qtStyle->drawControl(QStyle::CE_Header, &option, painter, qtWidget);

    if ( sortArrow != wxHDR_SORT_ICON_NONE )
    {
        option.sortIndicator = (sortArrow == wxHDR_SORT_ICON_UP)
                             ? QStyleOptionHeader::SortUp
                             : QStyleOptionHeader::SortDown;

        const int markSize = qtStyle->pixelMetric(QStyle::PM_HeaderMarkSize, nullptr, qtWidget);
        bestWidth += markSize;

        // Adjust the rect for the arrow
        option.rect.setSize(QSize(markSize, markSize));
        option.rect.moveTo(rect.x + rect.width - markSize - margin,
                           (rect.height - markSize) / 2 );

        qtStyle->drawPrimitive(QStyle::PE_IndicatorHeaderArrow, &option, painter, qtWidget);
    }

    return bestWidth;
}

int wxRendererQt::GetHeaderButtonHeight(wxWindow* win)
{
    auto qtWidget = win->GetHandle();
    auto qtStyle = qtWidget->style();

    QStyleOptionHeader option;
    option.initFrom(qtWidget);

    const auto size =
        qtStyle->sizeFromContents(QStyle::CT_HeaderSection, &option, QSize(), qtWidget);

    return size.height();
}

int wxRendererQt::GetHeaderButtonMargin(wxWindow* win)
{
    auto qtWidget = win->GetHandle();
    auto qtStyle = qtWidget->style();

    return qtStyle->pixelMetric(QStyle::PM_HeaderGripMargin, nullptr, qtWidget);
}

// draw a ">" or "v" button
void
wxRendererQt::DrawTreeItemButton(wxWindow* win, wxDC& dc, const wxRect& rect, int flags)
{
    auto painter = wxGetQtPainter(dc);

    wxCHECK_RET( painter, "Invalid painter!" );

    wxDCClipper clip(dc, rect);

    auto qtWidget = win->GetHandle();
    auto qtStyle = qtWidget->style();

    QStyleOption option;
    option.initFrom(qtWidget);
    option.rect = wxQtConvertRect(rect);

    option.state = QStyle::State_Children;

    if ( flags & wxCONTROL_EXPANDED )
        option.state |= QStyle::State_Open;

    qtStyle->drawPrimitive(QStyle::PE_IndicatorBranch, &option, painter, qtWidget);
}


// ----------------------------------------------------------------------------
// splitter sash drawing
// ----------------------------------------------------------------------------

wxSplitterRenderParams
wxRendererQt::GetSplitterParams(const wxWindow* win)
{
    auto qtWidget = win->GetHandle();
    auto qtStyle = qtWidget->style();

    QStyleOption option;
    option.initFrom(qtWidget);

    // Narrow handles don't work well with wxSplitterWindow
    const int handle_size =
        wxMax(5, qtStyle->pixelMetric(QStyle::PM_SplitterWidth, &option, qtWidget));

    // we don't draw any border, hence 0 for the second field
    return wxSplitterRenderParams
           (
               handle_size,
               0,
               true     // hot sensitive
           );
}

void
wxRendererQt::DrawSplitterBorder(wxWindow * WXUNUSED(win),
                                 wxDC& WXUNUSED(dc),
                                 const wxRect& WXUNUSED(rect),
                                 int WXUNUSED(flags))
{
    // nothing to do
}

void
wxRendererQt::DrawSplitterSash(wxWindow* win, wxDC& dc, const wxSize& size,
                               wxCoord position, wxOrientation orient, int flags)
{
    auto painter = wxGetQtPainter(dc);

    wxCHECK_RET( painter, "Invalid painter!" );

    auto qtWidget = win->GetHandle();
    auto qtStyle = qtWidget->style();

    const int handle_size = GetSplitterParams(win).widthSash;

    wxRect rect;

    if ( orient == wxVERTICAL )
    {
        rect.x = position;
        rect.y = 0;
        rect.width = handle_size;
        rect.height = size.y;
    }
    else // horz
    {
        rect.x = 0;
        rect.y = position;
        rect.height = handle_size;
        rect.width = size.x;
    }

    wxDCClipper clip(dc, rect);

    QStyleOption option;
    option.initFrom(qtWidget);
    option.rect = wxQtConvertRect(rect);

    if ( orient == wxVERTICAL )
        option.state = QStyle::State_Horizontal;
    else
        option.state = QStyle::State_None;

    option.state |= QStyle::State_Enabled;

    if ( flags & wxCONTROL_CURRENT )
        option.state |= QStyle::State_MouseOver;
    else if ( flags & wxCONTROL_PRESSED )
        option.state |= QStyle::State_Sunken;

    qtStyle->drawControl(QStyle::CE_Splitter, &option, painter, qtWidget);
}

wxSize
wxRendererQt::GetCheckBoxSize(wxWindow* win, int WXUNUSED(flags))
{
    auto qtWidget = win->GetHandle();
    auto qtStyle = qtWidget->style();

    QStyleOptionButton option;
    option.initFrom(qtWidget);

    const auto qtRect = qtStyle->subElementRect(QStyle::SE_CheckBoxIndicator, &option);

    return wxQtConvertSize( qtRect.size() );
}

void
wxRendererQt::DrawCheckBox(wxWindow* win,  wxDC& dc, const wxRect& rect, int flags)
{
    auto painter = wxGetQtPainter(dc);

    wxCHECK_RET( painter, "Invalid painter!" );

    wxDCClipper clip(dc, rect);

    auto qtWidget = win->GetHandle();
    auto qtStyle = qtWidget->style();

    QStyleOptionButton option;
    option.initFrom(qtWidget);
    option.rect = wxQtConvertRect(rect);

    if ( flags & wxCONTROL_DISABLED )
    {
        option.state = QStyle::State_None;
        option.palette.setCurrentColorGroup(QPalette::Disabled);
    }
    else
    {
        option.state = QStyle::State_Enabled;

        if ( flags & wxCONTROL_CURRENT )
            option.state |= QStyle::State_MouseOver;
        if ( flags & wxCONTROL_FOCUSED )
            option.state |= QStyle::State_HasFocus;
        if ( flags & wxCONTROL_PRESSED )
            option.state |= QStyle::State_Sunken;
        else
            option.state |= QStyle::State_Raised;
    }

    if ( flags & wxCONTROL_UNDETERMINED )
        option.state |= QStyle::State_NoChange;
    else if ( flags & wxCONTROL_CHECKED )
        option.state |= QStyle::State_On;
    else
        option.state |= QStyle::State_Off;

    qtStyle->drawControl(QStyle::CE_CheckBox, &option, painter, qtWidget);
}

wxSize
wxRendererQt::GetCheckMarkSize(wxWindow* win)
{
    return wxSize(win->GetCharWidth(),
                  win->GetCharHeight());
}

void
wxRendererQt::DrawCheckMark(wxWindow *win, wxDC& dc, const wxRect& rect, int WXUNUSED(flags))
{
    wxDCClipper clip(dc, rect);
    wxDCFontChanger fontChanger(dc, win->GetFont());

    // Draw the unicode character “✓” (U+2713)
    const auto checkMark = wxString::FromUTF8("\xE2\x9C\x93");
    dc.DrawText(checkMark, rect.GetPosition());
}

void
wxRendererQt::DrawPushButton(wxWindow* win, wxDC& dc, const wxRect& rect, int flags)
{
    auto painter = wxGetQtPainter(dc);

    wxCHECK_RET( painter, "Invalid painter!" );

    wxDCClipper clip(dc, rect);

    auto qtWidget = win->GetHandle();
    auto qtStyle = qtWidget->style();

    QStyleOptionButton option;
    option.initFrom(qtWidget);
    option.rect = wxQtConvertRect(rect);

    if ( flags & wxCONTROL_DISABLED )
    {
        option.state = QStyle::State_None;
        option.palette.setCurrentColorGroup(QPalette::Disabled);
    }
    else
    {
        option.state = QStyle::State_Enabled;

        if ( flags & wxCONTROL_CURRENT )
            option.state |= QStyle::State_MouseOver;
        if ( flags & wxCONTROL_PRESSED )
            option.state |= QStyle::State_Sunken;
        else
            option.state |= QStyle::State_Raised;
    }

    qtStyle->drawControl(QStyle::CE_PushButton, &option, painter, qtWidget);
}

void
wxRendererQt::DrawItemSelectionRect(wxWindow* win, wxDC& dc, const wxRect& rect, int flags)
{
    auto painter = wxGetQtPainter(dc);

    wxCHECK_RET( painter, "Invalid painter!" );

    wxDCClipper clip(dc, rect);

    auto qtWidget = win->GetHandle();
    auto qtStyle = qtWidget->style();

    QStyleOptionViewItem option;
    option.initFrom(qtWidget);
    option.rect = wxQtConvertRect(rect);

    if ( flags & wxCONTROL_SELECTED )
    {
        option.state = QStyle::State_Selected;
        option.showDecorationSelected = true;
    }

    qtStyle->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, qtWidget);
}

void
wxRendererQt::DrawFocusRect(wxWindow* win, wxDC& dc, const wxRect& rect, int WXUNUSED(flags))
{
    auto painter = wxGetQtPainter(dc);

    wxCHECK_RET( painter, "Invalid painter!" );

    wxDCClipper clip(dc, rect);

    auto qtWidget = win->GetHandle();
    auto qtStyle = qtWidget->style();

    QStyleOptionFocusRect option;
    option.initFrom(qtWidget);
    option.rect = wxQtConvertRect(rect);
    option.backgroundColor = wxQtConvertColour(win->GetBackgroundColour());

    option.state = QStyle::State_FocusAtBorder | QStyle::State_HasFocus;

    qtStyle->drawPrimitive(QStyle::PE_FrameFocusRect, &option, painter, qtWidget);
}

void
wxRendererQt::DrawGauge(wxWindow* win, wxDC& dc, const wxRect& rect,
                        int value, int max, int flags)
{
    auto painter = wxGetQtPainter(dc);

    wxCHECK_RET( painter, "Invalid painter!" );

    const bool isKDE = wxIsKDEDesktop();

    wxDCClipper clip(dc, rect);

    auto qtWidget = win->GetHandle();
    auto qtStyle = qtWidget->style();

    QStyleOptionProgressBar option;
    option.initFrom(qtWidget);
    option.rect = wxQtConvertRect(rect);
    option.minimum = 0;
    option.maximum = max;
    option.progress = value;

    if ( flags & wxCONTROL_DISABLED )
    {
        option.state = QStyle::State_None;
        option.palette.setCurrentColorGroup(QPalette::Disabled);
    }
    else
    {
        option.state = QStyle::State_Enabled;

        if ( flags & wxCONTROL_CURRENT )
            option.state |= QStyle::State_MouseOver;
    }

    if ( flags & wxCONTROL_SPECIAL )
    {
        if ( isKDE )
            option.orientation = Qt::Vertical;
    }
    else
    {
        option.state |= QStyle::State_Horizontal;
    }

    if ( isKDE )
    {
        qtStyle->drawControl(QStyle::CE_ProgressBar, &option, painter, qtWidget);
    }
    else
    {
        qtStyle->drawControl(QStyle::CE_ProgressBarGroove, &option, painter, qtWidget);

        if ( !(flags & wxCONTROL_SPECIAL) )
        {
            qtStyle->drawControl(QStyle::CE_ProgressBarContents, &option, painter, qtWidget);

            return;
        }

        option.rect = option.rect.transposed();
        option.invertedAppearance = true;

        const auto& r = option.rect;

        painter->save();
        auto m = painter->worldTransform();
        painter->resetTransform();
        painter->translate(option.rect.topLeft());
        painter->rotate(90);
        painter->translate(-r.topLeft() - QPoint(0, r.height()));
        painter->setWorldTransform(m, true);
        qtStyle->drawControl(QStyle::CE_ProgressBarContents, &option, painter, qtWidget);
        painter->restore();
    }
}


void
wxRendererQt::DoDrawComboBox(wxWindow* win, wxDC& dc, const wxRect& origRect,
                             int flags, bool editable)
{
    auto painter = wxGetQtPainter(dc);

    wxCHECK_RET( painter, "Invalid painter!" );

    // Under Xfce and Ubuntu Budgie desktops, the control cannot be rendered
    // outside a predefined rectangle positioned at (0, 0). So we need to reset
    // the position of the rect here and reposition the control later.
    wxRect rect = origRect;
    rect.SetPosition(wxPoint(0, 0));

    auto qtWidget = win->GetHandle();
    auto qtStyle = qtWidget->style();

    QStyleOptionComboBox option;
    option.initFrom(qtWidget);
    option.rect = wxQtConvertRect(rect);
    option.editable = editable;
    option.frame = true;
    option.activeSubControls = QStyle::SC_ComboBoxEditField;


    if ( flags & wxCONTROL_DISABLED )
    {
        option.state = QStyle::State_None;
        option.palette.setCurrentColorGroup(QPalette::Disabled);
    }
    else
    {
        option.state = QStyle::State_Enabled;

        if ( flags & wxCONTROL_CURRENT )
            option.state |= QStyle::State_MouseOver;
        if ( flags & wxCONTROL_FOCUSED )
            option.state |= QStyle::State_HasFocus;
        if ( flags & wxCONTROL_PRESSED )
            option.state |= QStyle::State_Sunken;
    }

    painter->save();
    painter->translate(wxQtConvertPoint(origRect.GetPosition()));
    qtStyle->drawComplexControl(QStyle::CC_ComboBox, &option, painter, qtWidget);
    painter->restore();
}

void wxRendererQt::DrawComboBox(wxWindow* win, wxDC& dc, const wxRect& rect, int flags)
{
    DoDrawComboBox(win, dc, rect, flags);
}

void wxRendererQt::DrawChoice(wxWindow* win, wxDC& dc, const wxRect& rect, int flags)
{
    DoDrawComboBox(win, dc, rect, flags, false);
}

void
wxRendererQt::DrawRadioBitmap(wxWindow* win, wxDC& dc, const wxRect& rect, int flags)
{
    auto painter = wxGetQtPainter(dc);

    wxCHECK_RET( painter, "Invalid painter!" );

    wxDCClipper clip(dc, rect);

    auto qtWidget = win->GetHandle();
    auto qtStyle = qtWidget->style();

    QStyleOptionButton option;
    option.initFrom(qtWidget);
    option.rect = wxQtConvertRect(rect);

    if ( flags & wxCONTROL_DISABLED )
    {
        option.state = QStyle::State_None;
        option.palette.setCurrentColorGroup(QPalette::Disabled);
    }
    else
    {
        option.state = QStyle::State_Enabled;

        if ( flags & wxCONTROL_CURRENT )
            option.state |= QStyle::State_MouseOver;
        if ( flags & wxCONTROL_FOCUSED )
            option.state |= QStyle::State_HasFocus;
        if ( flags & wxCONTROL_PRESSED )
            option.state |= QStyle::State_Sunken;
    }

    if ( flags & wxCONTROL_UNDETERMINED )
        option.state |= QStyle::State_NoChange;
    else if ( flags & wxCONTROL_CHECKED )
        option.state |= QStyle::State_On;
    else
        option.state |= QStyle::State_Off;

    qtStyle->drawControl(QStyle::CE_RadioButton, &option, painter, qtWidget);
}

void
wxRendererQt::DrawTextCtrl(wxWindow* win, wxDC& dc, const wxRect& origRect, int flags)
{
    auto painter = wxGetQtPainter(dc);

    wxCHECK_RET( painter, "Invalid painter!" );

    auto qtWidget = win->GetHandle();
    auto qtStyle = qtWidget->style();

    const int frameWidth = qtStyle->pixelMetric(QStyle::PM_DefaultFrameWidth);
    // Notice that KDE refuses to render the frame around the control if its
    // height is less than _minHeight_ (this is also the case for the native
    // QLineEdit)
    const int minHeight  = 2 * frameWidth + win->GetCharHeight() * 3 / 2;

    wxRect rect = origRect;

    if ( rect.GetHeight() < minHeight )
    {
        rect.SetHeight(minHeight);
    }

    wxDCClipper clip(dc, rect);

    QStyleOptionFrame option;
    option.initFrom(qtWidget);
    option.rect = wxQtConvertRect(rect);

    if ( flags & wxCONTROL_DISABLED )
    {
        option.state = QStyle::State_None;
        option.palette.setCurrentColorGroup(QPalette::Disabled);
    }
    else
    {
        option.state = QStyle::State_Enabled | QStyle::State_Sunken;

        if ( flags & wxCONTROL_CURRENT )
            option.state |= QStyle::State_MouseOver;
        if ( flags & wxCONTROL_FOCUSED )
            option.state |= QStyle::State_HasFocus;
    }

    option.lineWidth = frameWidth;
    option.midLineWidth = 0;

    qtStyle->drawPrimitive(QStyle::PE_PanelLineEdit, &option, painter, qtWidget);
    qtStyle->drawPrimitive(QStyle::PE_FrameLineEdit, &option, painter, qtWidget);
}
