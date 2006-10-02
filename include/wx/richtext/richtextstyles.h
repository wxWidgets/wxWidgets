/////////////////////////////////////////////////////////////////////////////
// Name:        wx/richtext/richtextstyles.h
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

#include "wx/defs.h"

#if wxUSE_RICHTEXT

#include "wx/richtext/richtextbuffer.h"

#if wxUSE_HTML
#include "wx/htmllbox.h"
#endif

#if wxUSE_COMBOCTRL
#include "wx/combo.h"
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

    wxRichTextStyleDefinition(const wxRichTextStyleDefinition& def)
    : wxObject()
    {
        Copy(def);
    }
    wxRichTextStyleDefinition(const wxString& name = wxEmptyString) { Init(); m_name = name; }
    virtual ~wxRichTextStyleDefinition() {}

    void Init() {}
    void Copy(const wxRichTextStyleDefinition& def);
    bool Eq(const wxRichTextStyleDefinition& def) const;
    void operator =(const wxRichTextStyleDefinition& def) { Copy(def); }
    bool operator ==(const wxRichTextStyleDefinition& def) const { return Eq(def); }

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

    wxRichTextCharacterStyleDefinition(const wxRichTextCharacterStyleDefinition& def): wxRichTextStyleDefinition(def) {}
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

    wxRichTextParagraphStyleDefinition(const wxRichTextParagraphStyleDefinition& def): wxRichTextStyleDefinition(def) { m_nextStyle = def.m_nextStyle; }
    wxRichTextParagraphStyleDefinition(const wxString& name = wxEmptyString):
        wxRichTextStyleDefinition(name) {}
    virtual ~wxRichTextParagraphStyleDefinition() {}

    /// The next style.
    void SetNextStyle(const wxString& name) { m_nextStyle = name; }
    const wxString& GetNextStyle() const { return m_nextStyle; }

    void Copy(const wxRichTextParagraphStyleDefinition& def);
    void operator =(const wxRichTextParagraphStyleDefinition& def) { Copy(def); }
    bool operator ==(const wxRichTextParagraphStyleDefinition& def) const;

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
    wxRichTextStyleSheet(const wxRichTextStyleSheet& sheet)
    : wxObject()
    {
        Copy(sheet);
    }
    wxRichTextStyleSheet() { Init(); }
    virtual ~wxRichTextStyleSheet() { DeleteStyles(); }

    /// Initialisation
    void Init();

    /// Copy
    void Copy(const wxRichTextStyleSheet& sheet);

    /// Assignment
    void operator=(const wxRichTextStyleSheet& sheet) { Copy(sheet); }

    /// Equality
    bool operator==(const wxRichTextStyleSheet& sheet) const;

    /// Add a definition to the character style list
    bool AddCharacterStyle(wxRichTextCharacterStyleDefinition* def);

    /// Add a definition to the paragraph style list
    bool AddParagraphStyle(wxRichTextParagraphStyleDefinition* def);

    /// Remove a character style
    bool RemoveCharacterStyle(wxRichTextStyleDefinition* def, bool deleteStyle = false) { return RemoveStyle(m_characterStyleDefinitions, def, deleteStyle); }

    /// Remove a paragraph style
    bool RemoveParagraphStyle(wxRichTextStyleDefinition* def, bool deleteStyle = false) { return RemoveStyle(m_paragraphStyleDefinitions, def, deleteStyle); }

    /// Find a character definition by name
    wxRichTextCharacterStyleDefinition* FindCharacterStyle(const wxString& name) const { return (wxRichTextCharacterStyleDefinition*) FindStyle(m_characterStyleDefinitions, name); }

    /// Find a paragraph definition by name
    wxRichTextParagraphStyleDefinition* FindParagraphStyle(const wxString& name) const { return (wxRichTextParagraphStyleDefinition*) FindStyle(m_paragraphStyleDefinitions, name); }

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
    wxRichTextStyleListBox()
    {
        Init();
    }
    wxRichTextStyleListBox(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~wxRichTextStyleListBox();

    void Init()
    {
        m_styleSheet = NULL;
        m_richTextCtrl = NULL;
    }

    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0);

    /// Creates a suitable HTML fragment for a definition
    wxString CreateHTML(wxRichTextStyleDefinition* def) const;

    /// Associates the control with a style manager
    void SetStyleSheet(wxRichTextStyleSheet* styleSheet) { m_styleSheet = styleSheet; }
    wxRichTextStyleSheet* GetStyleSheet() const { return m_styleSheet; }

    /// Associates the control with a wxRichTextCtrl
    void SetRichTextCtrl(wxRichTextCtrl* ctrl) { m_richTextCtrl = ctrl; }
    wxRichTextCtrl* GetRichTextCtrl() const { return m_richTextCtrl; }

    /// Get style for index
    wxRichTextStyleDefinition* GetStyle(size_t i) const ;

    /// Get index for style name
    int GetIndexForStyle(const wxString& name) const ;

    /// Set selection for string, returning the index.
    int SetStyleSelection(const wxString& name);

    /// Updates the list
    void UpdateStyles();

    /// Do selection
    void DoSelection(int i);

    /// React to selection
    void OnSelect(wxCommandEvent& event);

    /// Left click
    void OnLeftDown(wxMouseEvent& event);

    /// Auto-select from style under caret in idle time
    void OnIdle(wxIdleEvent& event);

#if 0
    virtual wxColour GetSelectedTextColour(const wxColour& colFg) const;
    virtual wxColour GetSelectedTextBgColour(const wxColour& colBg) const;
#endif

    /// Convert units in tends of a millimetre to device units
    int ConvertTenthsMMToPixels(wxDC& dc, int units) const;

    /// Can we set the selection based on the editor caret position?
    /// Need to override this if being used in a combobox popup
    virtual bool CanAutoSetSelection() { return true; }

protected:
    /// Returns the HTML for this item
    virtual wxString OnGetItem(size_t n) const;

private:

    wxRichTextStyleSheet*   m_styleSheet;
    wxRichTextCtrl*         m_richTextCtrl;
};

#if wxUSE_COMBOCTRL

/*!
 * Style drop-down for a wxComboCtrl
 */

class wxRichTextStyleComboPopup : public wxRichTextStyleListBox, public wxComboPopup
{
public:
    virtual void Init()
    {
        m_itemHere = -1; // hot item in list
        m_value = -1;
    }

    virtual bool Create( wxWindow* parent )
    {
        return wxRichTextStyleListBox::Create(parent, wxID_ANY,
                                  wxPoint(0,0), wxDefaultSize,
                                  wxSIMPLE_BORDER);
    }

    virtual wxWindow *GetControl() { return this; }

    virtual void SetStringValue( const wxString& s );

    virtual wxString GetStringValue() const;

    /// Can we set the selection based on the editor caret position?
    // virtual bool CanAutoSetSelection() { return ((m_combo == NULL) || !m_combo->IsPopupShown()); }
    virtual bool CanAutoSetSelection() { return false; }

    //
    // Popup event handlers
    //

    // Mouse hot-tracking
    void OnMouseMove(wxMouseEvent& event);

    // On mouse left, set the value and close the popup
    void OnMouseClick(wxMouseEvent& WXUNUSED(event));

protected:

    int             m_itemHere; // hot item in popup
    int             m_value;

private:
    DECLARE_EVENT_TABLE()
};

/*!
 * wxRichTextStyleComboCtrl
 * A combo for applying styles.
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextStyleComboCtrl: public wxComboCtrl
{
    DECLARE_CLASS(wxRichTextStyleComboCtrl)
    DECLARE_EVENT_TABLE()

public:
    wxRichTextStyleComboCtrl()
    {
        Init();
    }

    wxRichTextStyleComboCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxCB_READONLY)
    {
        Init();
        Create(parent, id, pos, size, style);
    }

    virtual ~wxRichTextStyleComboCtrl() {}

    void Init()
    {
        m_stylePopup = NULL;
    }

    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0);

    /// Updates the list
    void UpdateStyles() { m_stylePopup->UpdateStyles(); }

    /// Associates the control with a style manager
    void SetStyleSheet(wxRichTextStyleSheet* styleSheet) { m_stylePopup->SetStyleSheet(styleSheet); }
    wxRichTextStyleSheet* GetStyleSheet() const { return m_stylePopup->GetStyleSheet(); }

    /// Associates the control with a wxRichTextCtrl
    void SetRichTextCtrl(wxRichTextCtrl* ctrl) { m_stylePopup->SetRichTextCtrl(ctrl); }
    wxRichTextCtrl* GetRichTextCtrl() const { return m_stylePopup->GetRichTextCtrl(); }

    /// Gets the style popup
    wxRichTextStyleComboPopup* GetStylePopup() const { return m_stylePopup; }

    /// Auto-select from style under caret in idle time
    void OnIdle(wxIdleEvent& event);

protected:
    wxRichTextStyleComboPopup*  m_stylePopup;
};

#endif
    // wxUSE_COMBOCTRL

#endif
    // wxUSE_HTML

#endif
    // wxUSE_RICHTEXT

#endif
    // _WX_RICHTEXTSTYLES_H_
