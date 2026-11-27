/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/private/listitemfont.h
// Purpose:     a typeface of its own for each item of a wxWinUI wxListBox
// Author:      wxWidgets development team
// Created:     2026-11-22
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_PRIVATE_LISTITEMFONT_H_
#define _WX_WINUI_PRIVATE_LISTITEMFONT_H_

#include "wx/string.h"

#include <functional>

class WXDLLIMPEXP_FWD_CORE wxListBox;

// The typeface of one item. The size is always the one of the list.
struct wxWinUIListItemFont
{
    wxString family;

    // As in DirectWrite and in XAML: 100 to 900 for the weight, 1 to 9 for
    // the width, 5 being the normal one.
    int weight = 400;
    int stretch = 5;

    bool italic = false;
};

// Called with the index of an item whenever its XAML peer is filled. Returns
// false for an item which keeps the font of the list.
using wxWinUIListItemFontProvider =
    std::function<bool (unsigned int n, wxWinUIListItemFont& font)>;

// Makes a list box show each of its items in a typeface of its own, as the
// lists of the font dialog do. To be called before the items are added; an
// empty function removes the provider.
//
// The rows of such a list are the compact ones of WinUI, 32 DIPs high and
// all alike whatever the typeface: a list of fonts is a long one.
void wxWinUISetListItemFontProvider(wxListBox *list,
                                    wxWinUIListItemFontProvider provider);

#endif // _WX_WINUI_PRIVATE_LISTITEMFONT_H_
