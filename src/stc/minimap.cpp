///////////////////////////////////////////////////////////////////////////////
// Name:        src/stc/minimap.cpp
// Purpose:     Implementation of wxStyledTextCtrlMiniMap.
// Author:      Vadim Zeitlin
// Created:     2025-10-17
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
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

#if wxUSE_STC

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/math.h"
#endif // WX_PRECOMP

#include "wx/stc/minimap.h"

#include "wx/dcgraph.h"

#include <functional>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#if wxDEBUG_LEVEL
// Can be enabled to get trace messages useful for debugging the minimap.
constexpr const char* wxTRACE_STC_MAP = "stcmap";
#endif // wxDEBUG_LEVEL

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

namespace
{

// This is similar to wxMulDivInt32() but doesn't require including windows.h.
int ScaleByRatio(int value, int num, int den)
{
    if ( den == 0 )
        return 0;

    return wxRound(value * static_cast<double>(num) / static_cast<double>(den));
}

// Get the valid line index, clamping it to [0, max] range.
int GetValidLine(int line, int lineMax)
{
    if ( line < 0 )
        return 0;

    if ( line > lineMax )
        return lineMax;

    return line;
}

// This function encapsulates the algorithm iterating over the first
// visible editor line until we getValueForEditFirst() returns the closest
// value to the desired one.
//
// The initial value is forced to be in the correct range.
int
FindClosestEditFirstLine(int editNewInitial,
                         std::function<int (int)> getValueForEditFirst,
                         int valueWanted,
                         int lineMax)
{
    int editNew = GetValidLine(editNewInitial, lineMax);

    wxLogTrace(wxTRACE_STC_MAP, "Iterating: initial estimate=%d", editNew);

    // We use direction, which can be -1, 0 or 1 to avoid looping forever
    // if we can't get any closer, so remember which way we're going and
    // stop if overshoot.
    for ( int direction = 0;; )
    {
        auto const valueCurrent = getValueForEditFirst(editNew);

        if ( valueCurrent < valueWanted )
        {
            if ( editNew >= lineMax )
            {
                // We can't go any further.
                break;
            }

            if ( direction < 0 )
            {
                // We overshot the correct value, we won't get any closer
                // to it than we were before, so stop now to avoid entering
                // an infinite loop alternating between the two values
                // closest to the desired one from above and below.
                break;
            }

            // Remember that we're going down in case it is the first
            // iteration of the loop.
            direction = 1;

            ++editNew;
        }
        else if ( valueCurrent > valueWanted )
        {
            // See comment in the branch above.
            if ( editNew <= 0 )
                break;

            if ( direction > 0 )
                break;

            direction = -1;

            --editNew;
        }
        else // valueCurrent == valueWanted
        {
            break;
        }
    }

    wxLogTrace(wxTRACE_STC_MAP, "Iterating: final line=%d", editNew);

    return editNew;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxStyledTextCtrlMiniMap implementation
// ----------------------------------------------------------------------------

bool
wxStyledTextCtrlMiniMap::Create(wxWindow* parent, wxStyledTextCtrl* edit)
{
    if ( !wxStyledTextCtrl::Create(parent, wxID_ANY) )
        return false;

    // Use semi-transparent grey by default, this works well in both light and
    // dark modes.
    m_thumbColNormal = wxColour(0x80, 0x80, 0x80, 0x40);
    m_thumbColDragging = wxColour(0x80, 0x80, 0x80, 0x80);

    // Map itself can't be modified.
    SetReadOnly(true);

    // Because we draw the thumb over the text ourselves, indicate to the
    // control that it shouldn't scroll itself when redrawing but always
    // invalidate the whole window.
    SetCustomDrawn(true);

    // Configure the map appearance.
    SetZoom(-10 /* maximum zoom out level */);
    SetExtraDescent(-1); // Compress the lines even more.
    SetCaretStyle(wxSTC_CARETSTYLE_INVISIBLE);
    SetMarginCount(0);
    SetIndentationGuides(wxSTC_IV_NONE);
    UsePopUp(wxSTC_POPUP_NEVER);
    SetUseHorizontalScrollBar(false);
    SetUseVerticalScrollBar(false);
    SetCursor(wxCURSOR_ARROW);

    SetEdit(edit);

    // Scroll the editor when the map is scrolled.
    Bind(wxEVT_STC_UPDATEUI, &wxStyledTextCtrlMiniMap::OnMapUpdate, this);

    // Handle mouse events for dragging the visible zone indicator.
    Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& event) {
        HandleMouseClick(event);
    });

    Bind(wxEVT_MOTION, [this](wxMouseEvent& event) {
        if ( event.LeftIsDown() && IsDragging() )
            DoDrag(event.GetPosition());
    });

    Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& WXUNUSED(event)) {
        if ( IsDragging() )
            DoStopDragging();
    });

    Bind(wxEVT_LEFT_DCLICK, [this](wxMouseEvent& event) {
        // Don't let Scintilla get the double click, handle it as a simple
        // click instead.
        HandleMouseClick(event);
    });

    Bind(wxEVT_MOUSEWHEEL, [](wxMouseEvent& event) {
        // Don't allow zooming into the map.
        if ( event.ControlDown() )
            return;

        // But do allow to scroll in it using the wheel.
        event.Skip();
    });

    Bind(wxEVT_MOUSE_CAPTURE_LOST, [this](wxMouseCaptureLostEvent& WXUNUSED(event)) {
        DoStopDragging();
    });

    // Draw our overlay over the map.
    Bind(wxEVT_PAINT, &wxStyledTextCtrlMiniMap::OnMapPaint, this);

    return true;
}

wxStyledTextCtrlMiniMap::~wxStyledTextCtrlMiniMap()
{
    if ( IsDragging() )
        DoStopDragging();

    // Dissociate from the editor, if any.
    SetEdit(nullptr);
}

void wxStyledTextCtrlMiniMap::SetEdit(wxStyledTextCtrl* edit)
{
    if ( edit == m_edit )
        return;

    if ( m_edit )
    {
        // Dissociate from the old editor.
        m_edit->SetMirrorCtrl(nullptr);

        m_edit->Unbind(wxEVT_STC_UPDATEUI,
                       &wxStyledTextCtrlMiniMap::OnEditUpdate, this);
        m_edit->Unbind(wxEVT_PAINT,
                       &wxStyledTextCtrlMiniMap::OnEditPaint, this);
        m_edit->Unbind(wxEVT_SIZE,
                       &wxStyledTextCtrlMiniMap::OnEditSize, this);
        m_edit->Unbind(wxEVT_DESTROY,
                       &wxStyledTextCtrlMiniMap::OnEditDestroy, this);
    }

    m_edit = edit;

    if ( !m_edit )
    {
        // We must not continue using the old document.
        SetDocPointer(nullptr);

        return;
    }

    // Use the same document as the main editor.
    SetDocPointer(edit->GetDocPointer());


    // Copy the main editor attributes.

    // Do NOT set the lexer: this somehow breaks syntax highlighting and
    // folding in the main editor itself and the map gets syntax
    // highlighting even without it anyhow.

    for ( int style = 0; style < wxSTC_STYLE_MAX; ++style )
    {
        // There is probably no need to set the font for the map: at such
        // size, all fonts are indistinguishable.
        StyleSetForeground(style, edit->StyleGetForeground(style));
        StyleSetBackground(style, edit->StyleGetBackground(style));
        StyleSetBold(style, edit->StyleGetBold(style));
        StyleSetItalic(style, edit->StyleGetItalic(style));
        StyleSetUnderline(style, edit->StyleGetUnderline(style));
        StyleSetCase(style, edit->StyleGetCase(style));
    }

    SetWrapMode(edit->GetWrapMode());
    SetWrapVisualFlags(edit->GetWrapVisualFlags());
    SetWrapVisualFlagsLocation(edit->GetWrapVisualFlagsLocation());
    SetWrapIndentMode(edit->GetWrapIndentMode());
    SetWrapStartIndent(edit->GetWrapStartIndent());

    // Ensure that folds and markers in the map are synchronized with the main
    // editor.
    edit->SetMirrorCtrl(this);

    // Update the map when the editor changes.
    edit->Bind(wxEVT_STC_UPDATEUI, &wxStyledTextCtrlMiniMap::OnEditUpdate, this);

    // And also when the height changes, except we can't do it directly in
    // wxEVT_SIZE handler, so also check for the change in wxEVT_PAINT one.
    edit->Bind(wxEVT_SIZE, &wxStyledTextCtrlMiniMap::OnEditSize, this);
    edit->Bind(wxEVT_PAINT, &wxStyledTextCtrlMiniMap::OnEditPaint, this);

    // Stop using the editor when it is destroyed.
    edit->Bind(wxEVT_DESTROY, &wxStyledTextCtrlMiniMap::OnEditDestroy, this);

    // Note that the line index doesn't matter, as the height is the same
    // for all of them.
    m_mapLineHeight = TextHeight(0);

    UpdateLineInfo();
    SyncMapPosition();
    SyncSelection();

    wxLogTrace(wxTRACE_STC_MAP, "Edit line height: %d, map: %d",
               m_edit->TextHeight(0), TextHeight(0));

}

void
wxStyledTextCtrlMiniMap::SetThumbColours(const wxColour& colNormal,
                                         const wxColour& colDragging)
{
    if ( colNormal.IsOk() )
        m_thumbColNormal = colNormal;
    if ( colDragging.IsOk() )
        m_thumbColDragging = colDragging;
}

bool wxStyledTextCtrlMiniMap::UpdateLineInfo()
{
    LinesInfo lineInfo;
    lineInfo.mapVisible = LinesOnScreen();
    lineInfo.editVisible = m_edit->LinesOnScreen();

    auto const mapDisplay = GetDisplayLineCount();
    lineInfo.mapMax = mapDisplay > lineInfo.mapVisible
        ? mapDisplay - lineInfo.mapVisible
        : 0;

    auto const editDisplay = m_edit->GetDisplayLineCount();
    lineInfo.editMax = editDisplay > lineInfo.editVisible
        ? editDisplay - lineInfo.editVisible
        : 0;

    // The last thumb position corresponds to the difference between the
    // map line corresponding to the first visible line in the editor and
    // the first visible line in the map itself.
    lineInfo.thumbMax =
        VisibleFromDocLine(m_edit->DocLineFromVisible(lineInfo.editMax)) -
            lineInfo.mapMax;

    if ( lineInfo == m_lines )
        return false;

    m_lines = lineInfo;
    return true;
}

// We want the physical position of the thumb in the map to show the
// position of the currently visible part of the document, just as a
// normal scrollbar thumb does. So the thumb position is proportional to
// the first visible line in the editor:
//
//          thumbPos = thumbMax / editMax * editFirst
//
// Where the coefficient of proportionality is given by considering
// that the last possible values of the editor first line must be
// mapped to the last valid position of the thumb.
//
// But thumb position (in map line units) is also given by the difference
// between the map line corresponding to the first visible line in the
// editor and the first visible line in the map itself:
//
//          thumbPos = VisibleFromDocLine(editDocFirst) - mapFirst
//
// where
//
//          editDocFirst = m_edit->DocLineFromVisible(editFirst)
//
// This gives us the expression for the map first visible line as a
// function of the editor first visible line:
//
//          mapFirst = VisibleFromDocLine(editDocFirst) -
//                          thumbMax / editMax * editFirst
//
// which can be used to set the map position when the editor position
// changes and is returned by the function below.

// Get the map first visible line corresponding to the given editor
// first visible line.
int wxStyledTextCtrlMiniMap::GetMapFirstFromEditFirst(int editFirst) const
{
    auto const editDocFirst = m_edit->DocLineFromVisible(editFirst);
    auto const thumbPos =
        ScaleByRatio(editFirst, m_lines.thumbMax, m_lines.editMax);

    return VisibleFromDocLine(editDocFirst) - thumbPos;
}

// Scroll the editor to correspond to the current position in the map.
void wxStyledTextCtrlMiniMap::SyncEditPosition()
{
    auto const mapFirst = GetFirstVisibleLine();

    // There is no way to invert the formula above giving mapFirst from
    // editFirst, so we start with the likely candidate for editFirst
    // (which is actually the correct value if no lines are wrapped or
    // folded) and iterate it until we find the value of editFirst giving
    // the desired mapFirst.
    const int editNew = FindClosestEditFirstLine
        (
            ScaleByRatio(mapFirst, m_lines.editMax, m_lines.mapMax),
            [this](int editNew)
            {
                return GetMapFirstFromEditFirst(editNew);
            },
            mapFirst,
            m_lines.editMax
        );

    if ( editNew == m_edit->GetFirstVisibleLine() )
        return;

    SetEditFirstVisibleLine(editNew);
}

// Scroll the map to correspond to the current position in the editor.
void wxStyledTextCtrlMiniMap::SyncMapPosition()
{
    auto const editFirst = m_edit->GetFirstVisibleLine();
    auto const mapFirst = GetMapFirstFromEditFirst(editFirst);

    if ( mapFirst != GetFirstVisibleLine() )
        SetMapFirstVisibleLine(mapFirst);

    Refresh();
}


// Get the current thumb position and height.
wxStyledTextCtrlMiniMap::ThumbInfo
wxStyledTextCtrlMiniMap::GetThumbInfo() const
{
    ThumbInfo thumb;

    // Thumb starts at the first line shown in the editor and is offset
    // from the top of the map window by the number of lines between it and
    // the first line shown in the map, multiplied by the height of one
    // line. In the trivial case when display and physical lines are the
    // same, this would be just the difference between the visible lines in
    // the editor and the map respectively, but in the general case we need
    // to convert the editor visible line to the document line first and
    // then convert it to the map visible line.
    auto const editFirst = m_edit->GetFirstVisibleLine();
    auto const mapThumbFirst = MapVisibleFromEditVisible(editFirst);

    thumb.pos = (mapThumbFirst - GetFirstVisibleLine()) * m_mapLineHeight;

    // Thumb size is the number of display lines in the map corresponding
    // to the lines shown in the editor (in the trivial case when display
    // and physical lines are the same, this is just the number of the
    // lines shown in the editor) multiplied by the height of one line.
    auto const editLast = editFirst + m_lines.editVisible;
    auto const mapThumbLast = MapVisibleFromEditVisible(editLast);

    thumb.height = (mapThumbLast - mapThumbFirst) * m_mapLineHeight;

    return thumb;
}

void wxStyledTextCtrlMiniMap::DrawVisibleZone(wxDC& dc)
{
    // Note that opacity used here is not taken into account under wxMSW where
    // the overlay opacity is set globally.
    dc.SetBrush(IsDragging() ? m_thumbColDragging : m_thumbColNormal);
    dc.SetPen(*wxTRANSPARENT_PEN);

    auto const thumb = GetThumbInfo();
    dc.DrawRectangle(0, thumb.pos, GetClientSize().x, thumb.height);
}

void wxStyledTextCtrlMiniMap::OnMapPaint(wxPaintEvent& event)
{
    if ( !m_edit )
    {
        event.Skip();
        return;
    }

    // Normally we shouldn't call the base class event handler directly
    // like this, but here we really want to draw the text before
    // calling our DrawVisibleZone().
    wxStyledTextCtrl::OnPaint(event);

    // While dragging the visible zone is shown in an overlay.
    if ( !IsDragging() )
    {
        wxPaintDC dc(this);

        // We need to use wxGraphicsContext to draw the translucent
        // rectangle.
        wxGCDC gcdc(dc);
        DrawVisibleZone(gcdc);
    }
}

void wxStyledTextCtrlMiniMap::OnEditDestroy(wxWindowDestroyEvent& event)
{
    SetEdit(nullptr);

    event.Skip();
}

void wxStyledTextCtrlMiniMap::OnEditSize(wxSizeEvent& event)
{
    wxLogTrace(wxTRACE_STC_MAP,
               "Edit resized to %dx%d, line info: %s",
               event.GetSize().x, event.GetSize().y,
               m_lines.Dump());

    // Resizing the map may change its first visible line, however we
    // shouldn't synchronize the editor with this change, as it would be
    // wrong to change the position in it on resize, so ignore any upcoming
    // notifications about the new map position.
    m_lastSetMapFirst = GetFirstVisibleLine();

    event.Skip();
}

void wxStyledTextCtrlMiniMap::OnEditPaint(wxPaintEvent& event)
{
    // The number of displayed lines may have changed due to
    // rewrapping, in which case we need to update the map.
    if ( UpdateLineInfo() )
    {
        wxLogTrace(wxTRACE_STC_MAP, "Sync map from paint: %s",
                    m_lines.Dump());
        SyncMapPosition();
    }

    event.Skip();
}

void wxStyledTextCtrlMiniMap::OnEditUpdate(wxStyledTextEvent& event)
{
    if ( event.GetUpdated() & wxSTC_UPDATE_V_SCROLL )
    {
        auto const editFirst = m_edit->GetFirstVisibleLine();
        if ( editFirst == m_lastSetEditFirst )
        {
            m_lastSetEditFirst = -1;
            return;
        }

        wxLogTrace(wxTRACE_STC_MAP, "Sync map from scroll: %d",
                   editFirst);

        SyncMapPosition();
    }

    if ( event.GetUpdated() & wxSTC_UPDATE_SELECTION )
        SyncSelection();

    // Note that we don't need to refresh the map for wxSTC_UPDATE_CONTENT
    // changes, as Scintilla already refreshs it on its own when the shared
    // document contents change.
}

void wxStyledTextCtrlMiniMap::OnMapUpdate(wxStyledTextEvent& event)
{
    if ( !m_edit )
        return;

    // The order is important here: the only changes in content that we get
    // must come from the changes done in the editor, and in this case we don't
    // want to sync the editor back with the map position, even if this content
    // change also resulted in a vertical scrolling position change.
    if ( event.GetUpdated() & wxSTC_UPDATE_CONTENT )
        return;

    if ( event.GetUpdated() & wxSTC_UPDATE_V_SCROLL )
    {
        auto const mapFirst = GetFirstVisibleLine();
        if ( mapFirst == m_lastSetMapFirst )
        {
            m_lastSetMapFirst = -1;
            return;
        }

        wxLogTrace(wxTRACE_STC_MAP, "Sync edit from scroll: %d",
                   mapFirst);

        SyncEditPosition();
    }
}

void wxStyledTextCtrlMiniMap::HandleMouseClick(wxMouseEvent& event)
{
    if ( !m_edit )
        return;

    auto const posMouse = event.GetPosition();
    auto const docLine = GetDocLineAtMapPoint(posMouse);
    auto const editLine = m_edit->VisibleFromDocLine(docLine);

    wxLogTrace(wxTRACE_STC_MAP, "Clicked doc/editor line %d/%d",
               docLine, editLine);

    auto const editFirst = m_edit->GetFirstVisibleLine();
    auto const docFirst = m_edit->DocLineFromVisible(editFirst);

    auto const editLast = editFirst + m_lines.editVisible;
    auto const docLast = m_edit->DocLineFromVisible(editLast);

    // If the line is in the visible zone indicator, start dragging it.
    if ( docLine >= docFirst && docLine < docLast )
    {
        StartDragging(posMouse);
        return;
    }

    // The clicked line becomes the center of the visible zone indicator.
    // unless it's too close to the top or bottom of the map.
    auto const editNew =
        GetValidLine(editLine - m_lines.editVisible / 2, m_lines.editMax);

    if ( editNew != editFirst )
    {
        wxLogTrace(wxTRACE_STC_MAP, "Editor first %d -> %d",
                   editFirst, editNew);
        SetEditFirstVisibleLine(editNew);
        SyncMapPosition();

        Refresh();
    }
}

// Called when the user starts dragging at the given point.
void wxStyledTextCtrlMiniMap::StartDragging(const wxPoint& pos)
{
    m_isDragging = true;

    auto const thumb = GetThumbInfo();
    m_dragOffset = pos.y - (thumb.pos + thumb.height / 2);

    wxLogTrace(wxTRACE_STC_MAP, "Dragging: start at offset=%d",
               m_dragOffset);

    SetCursor(wxCURSOR_HAND);
    CaptureMouse();

    // This is required by wxMSW to ensure that the map shows from behind
    // the overlay and does nothing elsewhere.
    m_overlay.SetOpacity(m_thumbColDragging.Alpha());

    wxOverlayDC dc(m_overlay, this);
    dc.Clear();
    DrawVisibleZone(dc);

    Refresh();
}

// Called while dragging the visible zone indicator.
void wxStyledTextCtrlMiniMap::DoDrag(wxPoint pos)
{
    if ( !m_edit )
        return;

    // We want the thumb to follow the mouse when dragging, i.e. its middle
    // must remain at the same distance from the mouse pointer as it was
    // when the dragging started.
    pos.y -= m_dragOffset;

    // This is the position at which we want thumb middle to be.
    const int thumbWanted = (pos.y + m_mapLineHeight - 1) / m_mapLineHeight;

    // First visible line in the editor is proportional to the thumb
    // position, but we only have the position of the middle of the thumb
    // and the thumb height depends on the position too, so we need to
    // iterate until we find the right position.
    //
    // Start with the position corresponding to the current thumb height.
    int thumbPos = thumbWanted - GetThumbInfo().height / (2 * m_mapLineHeight);

    // Adjust until the actual position of the middle of the thumb if
    // this were the first visible line in the editor becomes close to
    // the desired position.
    const int editNew = FindClosestEditFirstLine
        (
            ScaleByRatio(thumbPos, m_lines.editMax, m_lines.thumbMax),
            [this](int editNew)
            {
                // Thumb top corresponds to the visible line in the map
                // corresponding to the document line corresponding to the
                // line editNew in the editor and, similarly, thumb bottom
                // corresponds to editNew + m_lines.editVisible.
                return
                    (MapVisibleFromEditVisible(editNew) +
                     MapVisibleFromEditVisible(editNew + m_lines.editVisible))
                    / 2
                    - GetMapFirstFromEditFirst(editNew);
            },
            thumbWanted,
            m_lines.editMax
        );

    if ( editNew == m_edit->GetFirstVisibleLine() )
        return;

    SetEditFirstVisibleLine(editNew);
    SyncMapPosition();

    wxOverlayDC dc(m_overlay, this);
    dc.Clear();
    DrawVisibleZone(dc);
}

// Called when we stop dragging, either because the mouse button was
// released or because the mouse capture was lost.
void wxStyledTextCtrlMiniMap::DoStopDragging()
{
    m_isDragging = false;
    m_dragOffset = 0;

    SetCursor(wxCURSOR_ARROW);
    ReleaseMouse();

    m_overlay.Reset();

    Refresh();
}

wxString wxStyledTextCtrlMiniMap::LinesInfo::Dump() const
{
    return wxString::Format
           (
            "map/edit visible: %d/%d, map/edit first line max: %d/%d",
            mapVisible, editVisible, mapMax, editMax
           );
}

#endif // wxUSE_STC
