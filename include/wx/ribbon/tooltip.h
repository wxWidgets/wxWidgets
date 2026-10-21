///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/tooltip.h
// Purpose:     Description of the rich tooltips shown by ribbon controls
// Author:      Blake Madden
// Created:     2026-10-21
// Copyright:   (c) 2026 Blake Madden
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_TOOLTIP_H_
#define _WX_RIBBON_TOOLTIP_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/bmpbndl.h"
#include "wx/string.h"

// Describes the contents of the tooltip shown when hovering over an item.
//
// All the setters return a reference to this object, so that they can be
// chained:
//
//  wxRibbonToolTipInfo("Table").Description("Insert a table.").HelpLink()
class WXDLLIMPEXP_RIBBON wxRibbonToolTipInfo
{
public:
    explicit wxRibbonToolTipInfo(const wxString& title = wxString())
        : m_title(title)
    {
    }

    wxRibbonToolTipInfo& Title(const wxString& title)
        { m_title = title; return *this; }
    wxRibbonToolTipInfo& Description(const wxString& description)
        { m_description = description; return *this; }
    wxRibbonToolTipInfo& DisabledDescription(const wxString& description)
        { m_disabledDescription = description; return *this; }
    wxRibbonToolTipInfo& Image(const wxBitmapBundle& image)
        { m_image = image; return *this; }
    wxRibbonToolTipInfo& HelpLink(bool show = true)
        { m_showHelpLink = show; return *this; }
    wxRibbonToolTipInfo& HelpLinkText(const wxString& text)
        { m_helpLinkText = text; return *this; }

    const wxString& GetTitle() const { return m_title; }
    const wxString& GetDescription() const { return m_description; }
    const wxString& GetDisabledDescription() const
        { return m_disabledDescription; }
    const wxBitmapBundle& GetImage() const { return m_image; }
    bool HasHelpLink() const { return m_showHelpLink; }

    // Returns the default text if no custom text was set.
    wxString GetHelpLinkText() const;

    bool IsEmpty() const
    {
        return m_title.empty() && m_description.empty() &&
               m_disabledDescription.empty() &&
               !m_image.IsOk() && !m_showHelpLink;
    }

private:
    wxString m_title;
    wxString m_description;
    wxString m_disabledDescription;
    wxBitmapBundle m_image;
    wxString m_helpLinkText;
    bool m_showHelpLink = false;
};

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_TOOLTIP_H_
