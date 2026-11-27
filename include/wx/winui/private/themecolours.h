/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/themecolours.h
// Purpose:     the colours of the WinUI theme, for what wx paints itself
// Author:      wxWidgets development team
// Created:     2026-11-23
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_THEMECOLOURS_H_
#define _WX_WINUI_PRIVATE_THEMECOLOURS_H_

#include "wx/colour.h"
#include "wx/arrstr.h"

#include <functional>

// What wx draws itself (the grid, the lists drawn item by item, what the
// renderer draws for the generic controls) takes its colours here: they are
// the ones XAML gives its own controls, in the light theme and in the dark
// one, so that a painted control does not stand out among them. No WinRT
// type appears here: the generic sources include this header.
#include "wx/bitmap.h"

enum class wxWinUIThemeColour
{
    Accent,             // AccentFillColorDefault: selection marks, focus
    TextOnAccent,       // TextOnAccentFillColorPrimary
    TextPrimary,        // TextFillColorPrimary
    TextSecondary,      // TextFillColorSecondary: headers, labels
    TextDisabled,       // TextFillColorDisabled
    SubtleFill,         // SubtleFillColorSecondary: a row selected or hovered
    SubtleFillPressed,  // SubtleFillColorTertiary
    Divider,            // DividerStrokeColorDefault: hairlines
    ControlStroke,      // ControlStrokeColorDefault: frames
    ControlFill,        // ControlFillColorDefault: the ground of a control
    ControlFillHover,   // ControlFillColorSecondary: under the pointer
    ControlStrongStroke,// ControlStrongStrokeColorDefault: under a field
    CardFill,           // CardBackgroundFillColorDefault: a content area
    BaseFill,           // SolidBackgroundFillColorBase: under everything
    FocusStroke,        // FocusStrokeColorOuter: the frame of the current cell
    FlyoutFill,         // SolidBackgroundFillColorQuarternary: under a flyout
    LayerAlt            // LayerFillColorAlt: the content of a dialog
};

// The measures of a table of WinUI, in DIPs: the height of its header, the
// least height of its rows (ListViewItemMinHeight) and the room before and
// after the text of a cell.
enum
{
    wxWINUI_TABLE_HEADER_HEIGHT = 32,
    wxWINUI_TABLE_ROW_HEIGHT = 40,
    wxWINUI_TABLE_CELL_PADDING = 12
};

// The picture of a colour as WinUI shows one: a swatch with rounded corners
// (4 DIPs) and a thin outline, transparent outside its corners. The size is
// in DIPs.
WXDLLIMPEXP_CORE wxBitmap
wxWinUICreateColourSwatch(const wxColour& colour, const wxSize& size,
                          double scale);

// Most of them are translucent: see wxWinUIBlendOver().
WXDLLIMPEXP_CORE wxColour wxWinUIGetThemeColour(wxWinUIThemeColour which);

// What is seen of a colour, translucent or not, over an opaque one.
WXDLLIMPEXP_CORE wxColour
wxWinUIBlendOver(const wxColour& colour, const wxColour& background);

// The ground of the selected cells of a table: the accent at a third of its
// strength over their own ground. Their text keeps its colour.
WXDLLIMPEXP_CORE wxColour wxWinUIGetAccentTint(const wxColour& background);

// A hairline between the cells of a table, over their ground.
WXDLLIMPEXP_CORE wxColour wxWinUIGetGridLineColour(const wxColour& background);

// The ground of the header of a table, and of the labels of its rows: a
// subtle fill over the ground of the table.
WXDLLIMPEXP_CORE wxColour
wxWinUIGetTableHeaderColour(const wxColour& background);

class WXDLLIMPEXP_FWD_CORE wxDC;
class WXDLLIMPEXP_FWD_CORE wxRect;
class WXDLLIMPEXP_FWD_CORE wxWindow;

// Paints the face of a combo box as the ComboBox of WinUI shows it: a field
// with rounded corners and a stronger line under it, and at its end a
// chevron with no box around it, over a subtle fill when the pointer is
// there. What wxComboCtrl and wxOwnerDrawnComboBox, which wx paints, look
// like. buttonFlags are wxCONTROL_CURRENT and wxCONTROL_PRESSED.
WXDLLIMPEXP_CORE void
wxWinUIDrawComboBoxFace(wxWindow *win, wxDC& dc, const wxRect& rect,
                        const wxRect& button, int buttonFlags, bool focused);

// The face of wxComboCtrl and of wxOwnerDrawnComboBox: a ComboBox of WinUI
// itself, under what the control paints and under its text field, which
// takes no input (the control goes on handling its mouse and its keys) and
// is put in the state the control is in. Nothing of it is painted by wx: its
// frame, its chevron, which is an animated icon, and what they look like
// under the pointer, pressed or with the focus are those of the ComboBox
// beside.
enum
{
    wxWINUI_COMBO_FACE_EDITABLE     = 0x01, // it has a text field
    wxWINUI_COMBO_FACE_HOVER        = 0x02, // the pointer is over it
    wxWINUI_COMBO_FACE_BUTTON_HOVER = 0x04, // ... over its button
    wxWINUI_COMBO_FACE_PRESSED      = 0x08,
    wxWINUI_COMBO_FACE_TEXT_FOCUS   = 0x10, // its text field has the focus
    wxWINUI_COMBO_FACE_DISABLED     = 0x20
};
WXDLLIMPEXP_CORE void wxWinUISetComboBoxFace(wxWindow *win, int flags);

// Whether a control without a text field got the focus from the keyboard:
// a ComboBox shows it then, and not when it was clicked.
WXDLLIMPEXP_CORE void wxWinUISetComboBoxFaceKeyFocus(wxWindow *win, bool on);

// The list of that ComboBox, for a combo whose items are plain text: it is
// the list of a ComboBox of WinUI itself, with its animation, its ground,
// its items and the way it is dismissed, not a window of wx made to look
// like it. Opens it with these items and this one selected (-1 for none);
// closed() is called once when it closes, with the item the user chose or
// -1 if none or the same one. False if it could not be opened.
WXDLLIMPEXP_CORE bool
wxWinUIOpenComboFaceList(wxWindow *win, const wxArrayString& items,
                         int selection, std::function<void (int)> closed);
// While it is open: moves its selection by that many items, as the arrows
// do; closes it, taking the selected item (Enter) or not (Escape).
WXDLLIMPEXP_CORE void wxWinUIMoveComboFaceListSelection(wxWindow *win, int by);
WXDLLIMPEXP_CORE void wxWinUICloseComboFaceList(wxWindow *win, bool accept);
WXDLLIMPEXP_CORE bool wxWinUIIsComboFaceListOpen(const wxWindow *win);
// The window is going away.
WXDLLIMPEXP_CORE void wxWinUIForgetComboFaceList(wxWindow *win);

#endif // _WX_WINUI_PRIVATE_THEMECOLOURS_H_
