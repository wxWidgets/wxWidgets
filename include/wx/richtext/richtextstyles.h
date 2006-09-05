/////////////////////////////////////////////////////////////////////////////
// Name:        richtextstyles.h
// Purpose:     Style management for wxRichTextCtrl
// Author:      Julian Smart
// Modified by: 
// Created:     2005-09-30
// RCS-ID:      
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RICHTEXTSTYLES_H_
#define _WX_RICHTEXTSTYLES_H_

/*!
 * Includes
 */

#include "wx/richtext/richtextbuffer.h"

#if wxUSE_RICHTEXT

#if wxUSE_HTML
#include "wx/htmllbox.h"
#endif

/*!
 * Forward declarations
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextCtrl;
class WXDLLIMPEXP_RICHTEXT wxRichTextBuffer;

/*!
 * wxRichTextStyleDefinition class declaration
 * A base class for paragraph and character styles.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextStyleDefinition: public wxObject
{
    DECLARE_CLASS(wxRichTextStyleDefinition)
public:

// Constructors

    wxRichTextStyleDefinition(const wxString& name = wxEmptyString) { Init(); m_name = name; }
    virtual ~wxRichTextStyleDefinition() {}

    void Init() {}

    /// The name of the style.
    void SetName(const wxString& name) { m_name = name; }
    const wxString& GetName() const { return m_name; }

    /// The name of the style that this style is based on.
    void SetBaseStyle(const wxString& name) { m_baseStyle = name; }
    const wxString& GetBaseStyle() const { return m_baseStyle; }

    /// The style.
    void SetStyle(const wxRichTextAttr& style) { m_style = style; }
    const wxRichTextAttr& GetStyle() const { return m_style; }
    wxRichTextAttr& GetStyle() { return m_style; }

protected:
    wxString        m_name;
    wxString        m_baseStyle;
    wxRichTextAttr  m_style;
};

/*!
 * wxRichTextCharacterStyleDefinition class declaration
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextCharacterStyleDefinition: public wxRichTextStyleDefinition
{
    DECLARE_DYNAMIC_CLASS(wxRichTextCharacterStyleDefinition)
public:

// Constructors

    wxRichTextCharacterStyleDefinition(const wxString& name = wxEmptyString):
        wxRichTextStyleDefinition(name) {}
    virtual ~wxRichTextCharacterStyleDefinition() {}

protected:
};

/*!
 * wxRichTextParagraphStyleDefinition class declaration
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextParagraphStyleDefinition: public wxRichTextStyleDefinition
{
    DECLARE_DYNAMIC_CLASS(wxRichTextParagraphStyleDefinition)
public:

// Constructors

    wxRichTextParagraphStyleDefinition(const wxString& name = wxEmptyString):
        wxRichTextStyleDefinition(name) {}
    virtual ~wxRichTextParagraphStyleDefinition() {}

    /// The next style.
    void SetNextStyle(const wxString& name) { m_nextStyle = name; }
    const wxString& GetNextStyle() const { return m_nextStyle; }

protected:

    /// The next style to use when adding a paragraph after this style.
    wxString    m_nextStyle;
};

/*!
 * The style sheet
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextStyleSheet: public wxObject
{
    DECLARE_CLASS( wxRichTextStyleSheet )

public:
    /// Constructors
    wxRichTextStyleSheet() { Init(); }
    virtual ~wxRichTextStyleSheet() { DeleteStyles(); }

    /// Initialisation
    void Init();

    /// Add a definition to the character style list
    bool AddCharacterStyle(wxRichTextCharacterStyleDefinition* def) { return AddStyle(m_characterStyleDefinitions, def); }

    /// Add a definition to the paragraph style list
    bool AddParagraphStyle(wxRichTextParagraphStyleDefinition* def) { return AddStyle(m_paragraphStyleDefinitions, def); }

    /// Remove a character style
    bool RemoveCharacterStyle(wxRichTextStyleDefinition* def, bool deleteStyle = false) { return RemoveStyle(m_characterStyleDefinitions, def, deleteStyle); }

    /// Remove a paragraph style
    bool RemoveParagraphStyle(wxRichTextStyleDefinition* def, bool deleteStyle = false) { return RemoveStyle(m_characterStyleDefinitions, def, deleteStyle); }

    /// Find a character definition by name
    wxRichTextCharacterStyleDefinition* FindCharacterStyle(const wxString& name) const { return (wxRichTextCharacterStyleDefinition*) FindStyle(m_characterStyleDefinitions, name); }

    /// Find a paragraph definition by name
    wxRichTextParagraphStyleDefinition* FindParagraphStyle(const wxString& name) const { return (wxRichTextParagraphStyleDefinition*) FindStyle(m_characterStyleDefinitions, name); }

    /// Return the number of character styes.
    size_t GetCharacterStyleCount() const { return m_characterStyleDefinitions.GetCount(); }

    /// Return the number of paragraph styes.
    size_t GetParagraphStyleCount() const { return m_paragraphStyleDefinitions.GetCount(); }

    /// Return the nth character style
    wxRichTextCharacterStyleDefinition* GetCharacterStyle(size_t n) const { return (wxRichTextCharacterStyleDefinition*) m_characterStyleDefinitions.Item(n)->GetData(); }

    /// Return the nth paragraph style
    wxRichTextParagraphStyleDefinition* GetParagraphStyle(size_t n) const { return (wxRichTextParagraphStyleDefinition*) m_paragraphStyleDefinitions.Item(n)->GetData(); }

    /// Delete all styles
    void DeleteStyles();

/// Implementation

    /// Add a definition to one of the style lists
    bool AddStyle(wxList& list, wxRichTextStyleDefinition* def);

    /// Remove a style
    bool RemoveStyle(wxList& list, wxRichTextStyleDefinition* def, bool deleteStyle);

    /// Find a definition by name
    wxRichTextStyleDefinition* FindStyle(const wxList& list, const wxString& name) const;

protected:

    wxList  m_characterStyleDefinitions;
    wxList  m_paragraphStyleDefinitions;
};

#if wxUSE_HTML
/*!
 * wxRichTextStyleListBox class declaration
 * A listbox to display styles.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextStyleListBox: public wxHtmlListBox
{
    DECLARE_CLASS(wxRichTextStyleListBox)
    DECLARE_EVENT_TABLE()

public:
    wxRichTextStyleListBox(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~wxRichTextStyleListBox();

    /// Creates a suitable HTML fragment for a definition
    wxString CreateHTML(wxRichTextStyleDefinition* def) const;

    /// Associates the control with a style manager
    void SetStyleSheet(wxRichTextStyleSheet* styleSheet) { m_styleSheet = styleSheet; }
    wxRichTextStyleSheet* GetStyleSheet() const { return m_styleSheet; }

    /// Associates the control with a wxRichTextCtrl
    void SetRichTextCtrl(wxRichTextCtrl* ctrl) { m_richTextCtrl = ctrl; }
    wxRichTextCtrl* GetRichTextCtrl() const { return m_richTextCtrl; }

    // Get style for index
    wxRichTextStyleDefinition* GetStyle(size_t i) const ;

    /// Updates the list
    void UpdateStyles();

    /// React to selection
    void OnSelect(wxCommandEvent& event);

    /// Left click
    void OnLeftDown(wxMouseEvent& event);

#if 0
    virtual wxColour GetSelectedTextColour(const wxColour& colFg) const;
    virtual wxColour GetSelectedTextBgColour(const wxColour& colBg) const;
#endif

    // Convert units in tends of a millimetre to device units
    int ConvertTenthsMMToPixels(wxDC& dc, int units) const;

protected:
    /// Returns the HTML for this item
    virtual wxString OnGetItem(size_t n) const;

private:

    wxRichTextStyleSheet*   m_styleSheet;
    wxRichTextCtrl*         m_richTextCtrl;
};
#endif

#endif
    // wxUSE_RICHTEXT

#endif
    // _WX_RICHTEXTSTYLES_H_
