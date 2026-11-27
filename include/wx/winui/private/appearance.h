/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/appearance.h
// Purpose:     Common wx-to-WinUI appearance helpers
// Author:      wxWidgets development team
// Created:     2026-07-26
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_APPEARANCE_H_
#define _WX_WINUI_PRIVATE_APPEARANCE_H_

#include "wx/colour.h"
#include "wx/defs.h"
#include "wx/font.h"
#include "wx/gdicmn.h"
#include "wx/string.h"

// Windows headers may define this function-like macro before a consumer
// includes us. It corrupts a C++/WinRT metadata method with the same name.
#ifdef GetCurrentTime
    #undef GetCurrentTime
#endif

#ifdef min
    #undef min
#endif
#ifdef max
    #undef max
#endif

#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

// The native label is deliberately split once at the port boundary.  XAML's
// AccessKey and UIA AccessKey properties both expect the key without wx's '&'
// marker, while the visible and accessible names must contain the unescaped
// text.
struct WXDLLIMPEXP_CORE wxWinUILabelPresentation
{
    wxString text;
    wxString accessKey;
};

class wxWindow;

// Centralize the native High Contrast policy so controls can relinquish local
// brushes to WinUI ThemeResources.
WXDLLIMPEXP_CORE bool wxWinUIIsHighContrastActive();

WXDLLIMPEXP_CORE wxWinUILabelPresentation
wxWinUIParseLabel(const wxString& label);

WXDLLIMPEXP_CORE
winrt::Microsoft::UI::Xaml::Media::SolidColorBrush
wxWinUICreateColourBrush(const wxColour& colour);

// Measure a single-line label with the actual WinUI typography. XAML reports
// DesiredSize in DIPs; this helper converts it to the wx window coordinate
// space so callers never mix DIPs with physical best-size padding.
WXDLLIMPEXP_CORE wxSize
wxWinUIMeasureText(const wxWindow *window,
                   const wxString& text,
                   const wxFont& font);

WXDLLIMPEXP_CORE void
wxWinUIApplyFont(
    const winrt::Microsoft::UI::Xaml::Controls::Control& control,
    const wxFont& font);

WXDLLIMPEXP_CORE void
wxWinUIApplyFont(
    const winrt::Microsoft::UI::Xaml::Controls::TextBlock& text,
    const wxFont& font);

// The font of the header of a table: the one given, in semi-bold when the
// program left it at its normal weight, as the headers of the tables of
// WinUI are.
WXDLLIMPEXP_CORE void
wxWinUIApplyTableHeaderFont(
    const winrt::Microsoft::UI::Xaml::Controls::Control& control,
    const wxFont& font);

WXDLLIMPEXP_CORE void
wxWinUIApplyTableHeaderFont(
    const winrt::Microsoft::UI::Xaml::Controls::TextBlock& text,
    const wxFont& font);

// The band of the header row of a table of WinUI: a subtle fill with the
// line of the table under it, whose corners are rounded where the header is
// at a corner of the frame of its control (XAML does not clip what is inside
// a frame to its rounded corners: as in WinUI.TableView, what is at a corner
// is rounded itself). The buttons of the columns are laid over it.
WXDLLIMPEXP_CORE winrt::Microsoft::UI::Xaml::Controls::Border
wxWinUICreateTableHeaderBand(double topRadius, double bottomRadius = 0);

// A control which shows the value of the cell of a table while it is not
// being edited (a TextBox, a ComboBox, a NumberBox, a CalendarDatePicker): no
// ground and no frame of its own, so that the value reads as the text of
// the cell, as in the tables of WinUI, where the editor only shows for the
// cell being edited.
WXDLLIMPEXP_CORE void
wxWinUIApplyTableCellValueStyle(
    const winrt::Microsoft::UI::Xaml::Controls::Control& control);

// A Border which draws the lines of a table at its right and under it
// (either may be left out), in the brush of the theme.
WXDLLIMPEXP_CORE winrt::Microsoft::UI::Xaml::Controls::Border
wxWinUICreateTableCellBorder(bool rightLine, bool bottomLine);

WXDLLIMPEXP_CORE void
wxWinUIApplyForeground(
    const winrt::Microsoft::UI::Xaml::Controls::Control& control,
    const wxColour& colour);

WXDLLIMPEXP_CORE void
wxWinUIApplyForeground(
    const winrt::Microsoft::UI::Xaml::Controls::TextBlock& text,
    const wxColour& colour);

WXDLLIMPEXP_CORE void
wxWinUIApplyBackground(
    const winrt::Microsoft::UI::Xaml::Controls::Control& control,
    const wxColour& colour);

WXDLLIMPEXP_CORE void
wxWinUIApplyBackground(
    const winrt::Microsoft::UI::Xaml::Controls::Border& border,
    const wxColour& colour);

// Set the visual and UIA access-key properties only. Slot synchronization is
// the sole authority for AutomationProperties.Name on hosted controls, so
// normal control implementations should use this function.
WXDLLIMPEXP_CORE void
wxWinUIApplyAccessKey(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const wxString& label);

// Explicitly author a UIA name for elements which are not slot-controlled
// (dialog internals and test probes). Passing an empty localized type clears
// the previous local type. Do not use this on an ordinary hosted control.
WXDLLIMPEXP_CORE void
wxWinUIApplyAccessibleLabel(
    const winrt::Microsoft::UI::Xaml::UIElement& element,
    const wxString& label,
    const wxString& localizedControlType = wxString());

// Grid, Border, StackPanel and ContentControl don't create an automation peer
// of their own. Composite wx controls must therefore use this root instead of
// relying on AutomationProperties alone: the latter decorate an existing peer
// but never create one. The returned Grid remains a normal layout container
// while exposing the requested stable UIA role and class name.
WXDLLIMPEXP_CORE
winrt::Microsoft::UI::Xaml::Controls::Grid
wxWinUICreateAccessibleGrid(
    winrt::Microsoft::UI::Xaml::Automation::Peers::AutomationControlType
        controlType,
    const wxString& className);

enum class wxWinUIThemeBrushProperty
{
    BorderBrush,
    Background
};

// Return a Border whose requested property still contains the ThemeResource
// expression. Returning the Brush value itself would detach the expression
// from its target and freeze the current theme. Failure intentionally returns
// null instead of substituting a fixed light/dark colour.
WXDLLIMPEXP_CORE
winrt::Microsoft::UI::Xaml::Controls::Border
wxWinUICreateThemeBrushBorder(
    const wxString& resourceKey,
    wxWinUIThemeBrushProperty property);

// Gives a button the look of the cell of a table: flat, no corner, no
// border of its own but the ones it is given, and nothing changing under
// the pointer unless asked (the labels of the rows and columns of a grid).
// Its ground, its border and its text are the ones set on it.
WXDLLIMPEXP_CORE void
wxWinUIApplyTableCellStyle(
    const winrt::Microsoft::UI::Xaml::Controls::Button& button,
    bool hover);

// A ListBox with the look of the lists of WinUI. The ListBox of XAML is the
// control of older Windows: a selected item is a full block in the accent
// colour with white text. This one has the items of ListView: a subtle fill
// with rounded corners under the pointer and for the selected item, a short
// bar in the accent colour at its start, the text in its own colour, and a
// thin rounded frame. It is the list of a simple combo box (wxCB_SIMPLE).
WXDLLIMPEXP_CORE winrt::Microsoft::UI::Xaml::Controls::ListBox
wxWinUICreateStyledListBox();

// Takes the ground and the border of a TextBox or of a RichEditBox away, in
// all its states: a wxTextCtrl created without a border (wxBORDER_NONE),
// which is the text field inside another control, a combo box or the cell
// of a table being edited, which draws the frame around it.
WXDLLIMPEXP_CORE void
wxWinUIApplyChromelessTextBoxStyle(
    const winrt::Microsoft::UI::Xaml::Controls::Control& textBox);

// The tab of a notebook whose tabs are not at the top. TabView only has its
// strip above its content, so these tabs are TabViewItems on their own,
// whose default template shows nothing once selected: this gives them the
// look of the items of a WinUI navigation list, a subtle fill under the
// pointer and for the selected one, with a bar in the accent colour on the
// side away from the pages (side: 0 left, 1 right, 2 bottom).
WXDLLIMPEXP_CORE void
wxWinUIApplySideTabStyle(
    const winrt::Microsoft::UI::Xaml::Controls::TabViewItem& item, int side);

// Gives a button the look of the quiet buttons of WinUI (the chevron of a
// tree, the arrows of a pager): no ground and no border until the pointer
// is over it, then a subtle fill with rounded corners.
WXDLLIMPEXP_CORE void
wxWinUIApplySubtleButtonStyle(
    const winrt::Microsoft::UI::Xaml::Controls::Button& button);

// Gives a button the look of the cell of a table header, the one of the
// lists of Windows 11 and of the DataGrid of the toolkit: flat, its text in
// the colour of second rank, a subtle fill under the pointer, a hairline
// under it and at its right. The headers of wxListCtrl, wxDataViewCtrl and
// wxHeaderCtrl are made of such buttons. The colours stay those of the
// theme when it changes.
WXDLLIMPEXP_CORE void
wxWinUIApplyTableHeaderStyle(
    const winrt::Microsoft::UI::Xaml::Controls::Button& button);

#endif // _WX_WINUI_PRIVATE_APPEARANCE_H_
