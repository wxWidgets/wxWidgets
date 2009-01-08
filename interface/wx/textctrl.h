/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.h
// Purpose:     interface of wxTextAttr
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////



/**
    The following values can be passed to wxTextAttr::SetAlignment to determine paragraph alignment.
*/
enum wxTextAttrAlignment
{
    wxTEXT_ALIGNMENT_DEFAULT,
    wxTEXT_ALIGNMENT_LEFT,
    wxTEXT_ALIGNMENT_CENTRE,
    wxTEXT_ALIGNMENT_CENTER = wxTEXT_ALIGNMENT_CENTRE,
    wxTEXT_ALIGNMENT_RIGHT,

    /** wxTEXT_ALIGNMENT_JUSTIFIED is unimplemented.
        In future justification may be supported when printing or previewing, only. */
    wxTEXT_ALIGNMENT_JUSTIFIED
};

/**
    The following values are passed in a bitlist to wxTextAttr::SetFlags() to
    determine what attributes will be considered when setting the attributes for a text control.
*/
enum wxTextAttrFlags
{
    wxTEXT_ATTR_TEXT_COLOUR          = 0x00000001,
    wxTEXT_ATTR_BACKGROUND_COLOUR    = 0x00000002,
    wxTEXT_ATTR_FONT_FACE            = 0x00000004,
    wxTEXT_ATTR_FONT_SIZE            = 0x00000008,
    wxTEXT_ATTR_FONT_WEIGHT          = 0x00000010,
    wxTEXT_ATTR_FONT_ITALIC          = 0x00000020,
    wxTEXT_ATTR_FONT_UNDERLINE       = 0x00000040,
    wxTEXT_ATTR_FONT_ENCODING        = 0x02000000,
    wxTEXT_ATTR_FONT = \
        ( wxTEXT_ATTR_FONT_FACE | wxTEXT_ATTR_FONT_SIZE | wxTEXT_ATTR_FONT_WEIGHT | \
            wxTEXT_ATTR_FONT_ITALIC | wxTEXT_ATTR_FONT_UNDERLINE | wxTEXT_ATTR_FONT_ENCODING ),

    wxTEXT_ATTR_ALIGNMENT            = 0x00000080,
    wxTEXT_ATTR_LEFT_INDENT          = 0x00000100,
    wxTEXT_ATTR_RIGHT_INDENT         = 0x00000200,
    wxTEXT_ATTR_TABS                 = 0x00000400,

    wxTEXT_ATTR_PARA_SPACING_AFTER   = 0x00000800,
    wxTEXT_ATTR_PARA_SPACING_BEFORE  = 0x00001000,
    wxTEXT_ATTR_LINE_SPACING         = 0x00002000,
    wxTEXT_ATTR_CHARACTER_STYLE_NAME = 0x00004000,
    wxTEXT_ATTR_PARAGRAPH_STYLE_NAME = 0x00008000,
    wxTEXT_ATTR_LIST_STYLE_NAME      = 0x00010000,
    wxTEXT_ATTR_BULLET_STYLE         = 0x00020000,
    wxTEXT_ATTR_BULLET_NUMBER        = 0x00040000,
    wxTEXT_ATTR_BULLET_TEXT          = 0x00080000,
    wxTEXT_ATTR_BULLET_NAME          = 0x00100000,
    wxTEXT_ATTR_URL                  = 0x00200000,
    wxTEXT_ATTR_PAGE_BREAK           = 0x00400000,
    wxTEXT_ATTR_EFFECTS              = 0x00800000,
    wxTEXT_ATTR_OUTLINE_LEVEL        = 0x01000000,

    /**
        Character and paragraph combined styles
    */

    wxTEXT_ATTR_CHARACTER = \
        (wxTEXT_ATTR_FONT|wxTEXT_ATTR_FONT_ENCODING|wxTEXT_ATTR_EFFECTS| \
            wxTEXT_ATTR_BACKGROUND_COLOUR|wxTEXT_ATTR_TEXT_COLOUR|wxTEXT_ATTR_CHARACTER_STYLE_NAME|wxTEXT_ATTR_URL),

    wxTEXT_ATTR_PARAGRAPH = \
        (wxTEXT_ATTR_ALIGNMENT|wxTEXT_ATTR_LEFT_INDENT|wxTEXT_ATTR_RIGHT_INDENT|wxTEXT_ATTR_TABS|\
            wxTEXT_ATTR_PARA_SPACING_BEFORE|wxTEXT_ATTR_PARA_SPACING_AFTER|wxTEXT_ATTR_LINE_SPACING|\
            wxTEXT_ATTR_BULLET_STYLE|wxTEXT_ATTR_BULLET_NUMBER|wxTEXT_ATTR_BULLET_TEXT|wxTEXT_ATTR_BULLET_NAME|\
            wxTEXT_ATTR_PARAGRAPH_STYLE_NAME|wxTEXT_ATTR_LIST_STYLE_NAME|wxTEXT_ATTR_OUTLINE_LEVEL),

    wxTEXT_ATTR_ALL = (wxTEXT_ATTR_CHARACTER|wxTEXT_ATTR_PARAGRAPH)
};

/**
    Styles for wxTextAttr::SetBulletStyle
*/
enum wxTextAttrBulletStyle
{
    wxTEXT_ATTR_BULLET_STYLE_NONE            = 0x00000000,
    wxTEXT_ATTR_BULLET_STYLE_ARABIC          = 0x00000001,
    wxTEXT_ATTR_BULLET_STYLE_LETTERS_UPPER   = 0x00000002,
    wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER   = 0x00000004,
    wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER     = 0x00000008,
    wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER     = 0x00000010,
    wxTEXT_ATTR_BULLET_STYLE_SYMBOL          = 0x00000020,

    /** wxTEXT_ATTR_BULLET_STYLE_BITMAP is unimplemented. */
    wxTEXT_ATTR_BULLET_STYLE_BITMAP          = 0x00000040,
    wxTEXT_ATTR_BULLET_STYLE_PARENTHESES     = 0x00000080,
    wxTEXT_ATTR_BULLET_STYLE_PERIOD          = 0x00000100,
    wxTEXT_ATTR_BULLET_STYLE_STANDARD        = 0x00000200,
    wxTEXT_ATTR_BULLET_STYLE_RIGHT_PARENTHESIS = 0x00000400,
    wxTEXT_ATTR_BULLET_STYLE_OUTLINE         = 0x00000800,

    wxTEXT_ATTR_BULLET_STYLE_ALIGN_LEFT      = 0x00000000,
    wxTEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT     = 0x00001000,
    wxTEXT_ATTR_BULLET_STYLE_ALIGN_CENTRE    = 0x00002000
};

/**
    Styles for wxTextAttr::SetTextEffects().

    Of these, only wxTEXT_ATTR_EFFECT_CAPITALS and wxTEXT_ATTR_EFFECT_STRIKETHROUGH are implemented.
*/
enum wxTextAttrEffects
{
    wxTEXT_ATTR_EFFECT_NONE                  = 0x00000000,
    wxTEXT_ATTR_EFFECT_CAPITALS              = 0x00000001,
    wxTEXT_ATTR_EFFECT_SMALL_CAPITALS        = 0x00000002,
    wxTEXT_ATTR_EFFECT_STRIKETHROUGH         = 0x00000004,
    wxTEXT_ATTR_EFFECT_DOUBLE_STRIKETHROUGH  = 0x00000008,
    wxTEXT_ATTR_EFFECT_SHADOW                = 0x00000010,
    wxTEXT_ATTR_EFFECT_EMBOSS                = 0x00000020,
    wxTEXT_ATTR_EFFECT_OUTLINE               = 0x00000040,
    wxTEXT_ATTR_EFFECT_ENGRAVE               = 0x00000080,
    wxTEXT_ATTR_EFFECT_SUPERSCRIPT           = 0x00000100,
    wxTEXT_ATTR_EFFECT_SUBSCRIPT             = 0x00000200
};

/**
    Line spacing values; see wxTextAttr::SetLineSpacing().
*/
enum wxTextAttrLineSpacing
{
    wxTEXT_ATTR_LINE_SPACING_NORMAL         = 10,
    wxTEXT_ATTR_LINE_SPACING_HALF           = 15,
    wxTEXT_ATTR_LINE_SPACING_TWICE          = 20
};


/**
    Describes the possible return values of wxTextCtrl::HitTest().

    The element names correspond to the relationship between the point asked
    for and the character returned, e.g. @c wxTE_HT_BEFORE means that the point
    is before (leftward or upward) it and so on.
 */
enum wxTextCtrlHitTestResult
{
    /// Indicates that wxTextCtrl::HitTest() is not implemented on this
    /// platform.
    wxTE_HT_UNKNOWN = -2,

    /// The point is before the character returned.
    wxTE_HT_BEFORE,

    /// The point is directly on the character returned.
    wxTE_HT_ON_TEXT,

    /// The point is below the last line of the control.
    wxTE_HT_BELOW,

    /// The point is beyond the end of line containing the character returned.
    wxTE_HT_BEYOND
};


/**
    @class wxTextAttr

    wxTextAttr represents the character and paragraph attributes, or style,
    for a range of text in a wxTextCtrl or wxRichTextCtrl.

    When setting up a wxTextAttr object, pass a bitlist mask to
    wxTextAttr::SetFlags() to indicate which style elements should be changed.
    As a convenience, when you call a setter such as SetFont, the relevant bit
    will be set.

    @library{wxcore}
    @category{richtext}

    @see wxTextCtrl, wxRichTextCtrl
*/
class wxTextAttr
{
public:
    //@{
    /**
        Constructors.
    */
    wxTextAttr();
    wxTextAttr(const wxColour& colText,
               const wxColour& colBack = wxNullColour,
               const wxFont& font = wxNullFont,
               wxTextAttrAlignment alignment = wxTEXT_ALIGNMENT_DEFAULT);
    wxTextAttr(const wxTextAttr& attr);
    //@}

    /**
        Applies the attributes in @a style to the original object, but not those
        attributes from @a style that are the same as those in @a compareWith (if passed).
    */
    bool Apply(const wxTextAttr& style,
               const wxTextAttr* compareWith = NULL);

    /**
        Creates a font from the font attributes.
    */
    wxFont CreateFont() const;

    /**
        Returns the alignment flags.
        See ::wxTextAttrAlignment for a list of available styles.
    */
    wxTextAttrAlignment GetAlignment() const;

    /**
        Returns the background colour.
    */
    const wxColour& GetBackgroundColour() const;

    /**
        Returns a string containing the name of the font associated with the bullet symbol.
        Only valid for attributes with wxTEXT_ATTR_BULLET_SYMBOL.
    */
    const wxString& GetBulletFont() const;

    /**
        Returns the standard bullet name, applicable if the bullet style is
        wxTEXT_ATTR_BULLET_STYLE_STANDARD.

        Currently the following standard bullet names are supported:
         - @c standard/circle
         - @c standard/square
         - @c standard/diamond
         - @c standard/triangle

        @note
        For wxRichTextCtrl users only: if you wish your rich text controls to support
        further bullet graphics, you can derive a class from wxRichTextRenderer or
        wxRichTextStdRenderer, override @c DrawStandardBullet and @c EnumerateStandardBulletNames,
        and set an instance of the class using wxRichTextBuffer::SetRenderer.
    */
    const wxString& GetBulletName() const;

    /**
        Returns the bullet number.
    */
    int GetBulletNumber() const;

    /**
        Returns the bullet style.
        See ::wxTextAttrBulletStyle for a list of available styles.
    */
    int GetBulletStyle() const;

    /**
        Returns the bullet text, which could be a symbol, or (for example) cached
        outline text.
    */
    const wxString& GetBulletText() const;

    /**
        Returns the name of the character style.
    */
    const wxString& GetCharacterStyleName() const;

    /**
        Returns flags indicating which attributes are applicable.
        See SetFlags() for a list of available flags.
    */
    long GetFlags() const;

    /**
        Creates and returns a font specified by the font attributes in the wxTextAttr
        object. Note that wxTextAttr does not store a wxFont object, so this is only
        a temporary font.

        For greater efficiency, access the font attributes directly.
    */
    wxFont GetFont() const;

    /**
        Gets the font attributes from the given font, using only the attributes
        specified by @a flags.
    */
    bool GetFontAttributes(const wxFont& font,
                           int flags = wxTEXT_ATTR_FONT);

    /**
        Returns the font encoding.
    */
    wxFontEncoding GetFontEncoding() const;

    /**
        Returns the font face name.
    */
    const wxString& GetFontFaceName() const;

    /**
        Returns the font size in points.
    */
    int GetFontSize() const;

    /**
        Returns the font style.
    */
    int GetFontStyle() const;

    /**
        Returns @true if the font is underlined.
    */
    bool GetFontUnderlined() const;

    /**
        Returns the font weight.
    */
    int GetFontWeight() const;

    /**
        Returns the left indent in tenths of a millimetre.
    */
    long GetLeftIndent() const;

    /**
        Returns the left sub-indent in tenths of a millimetre.
    */
    long GetLeftSubIndent() const;

    /**
        Returns the line spacing value, one of ::wxTextAttrLineSpacing values.
    */
    int GetLineSpacing() const;

    /**
        Returns the name of the list style.
    */
    const wxString& GetListStyleName() const;

    /**
        Returns the outline level.
    */
    int GetOutlineLevel() const;

    /**
        Returns the space in tenths of a millimeter after the paragraph.
    */
    int GetParagraphSpacingAfter() const;

    /**
        Returns the space in tenths of a millimeter before the paragraph.
    */
    int GetParagraphSpacingBefore() const;

    /**
        Returns the name of the paragraph style.
    */
    const wxString& GetParagraphStyleName() const;

    /**
        Returns the right indent in tenths of a millimeter.
    */
    long GetRightIndent() const;

    /**
        Returns an array of tab stops, each expressed in tenths of a millimeter.

        Each stop is measured from the left margin and therefore each value must
        be larger than the last.
    */
    const wxArrayInt& GetTabs() const;

    /**
        Returns the text foreground colour.
    */
    const wxColour& GetTextColour() const;

    /**
        Returns the text effect bits of interest.
        See SetFlags() for further information.
    */
    int GetTextEffectFlags() const;

    /**
        Returns the text effects, a bit list of styles.
        See SetTextEffects() for details.
    */
    int GetTextEffects() const;

    /**
        Returns the URL for the content.

        Content with @a wxTEXT_ATTR_URL style causes wxRichTextCtrl to show a
        hand cursor over it, and wxRichTextCtrl generates a wxTextUrlEvent
        when the content is clicked.
    */
    const wxString& GetURL() const;

    /**
        Returns @true if the attribute object specifies alignment.
    */
    bool HasAlignment() const;

    /**
        Returns @true if the attribute object specifies a background colour.
    */
    bool HasBackgroundColour() const;

    /**
        Returns @true if the attribute object specifies a standard bullet name.
    */
    bool HasBulletName() const;

    /**
        Returns @true if the attribute object specifies a bullet number.
    */
    bool HasBulletNumber() const;

    /**
        Returns @true if the attribute object specifies a bullet style.
    */
    bool HasBulletStyle() const;

    /**
        Returns @true if the attribute object specifies bullet text (usually
        specifying a symbol).
    */
    bool HasBulletText() const;

    /**
        Returns @true if the attribute object specifies a character style name.
    */
    bool HasCharacterStyleName() const;

    /**
        Returns @true if the @a flag is present in the attribute object's flag
        bitlist.
    */
    bool HasFlag(long flag) const;

    /**
        Returns @true if the attribute object specifies any font attributes.
    */
    bool HasFont() const;

    /**
        Returns @true if the attribute object specifies an encoding.
    */
    bool HasFontEncoding() const;

    /**
        Returns @true if the attribute object specifies a font face name.
    */
    bool HasFontFaceName() const;

    /**
        Returns @true if the attribute object specifies italic style.
    */
    bool HasFontItalic() const;

    /**
        Returns @true if the attribute object specifies a font point size.
    */
    bool HasFontSize() const;

    /**
        Returns @true if the attribute object specifies either underlining or no
        underlining.
    */
    bool HasFontUnderlined() const;

    /**
        Returns @true if the attribute object specifies font weight (bold, light or
        normal).
    */
    bool HasFontWeight() const;

    /**
        Returns @true if the attribute object specifies a left indent.
    */
    bool HasLeftIndent() const;

    /**
        Returns @true if the attribute object specifies line spacing.
    */
    bool HasLineSpacing() const;

    /**
        Returns @true if the attribute object specifies a list style name.
    */
    bool HasListStyleName() const;

    /**
        Returns @true if the attribute object specifies an outline level.
    */
    bool HasOutlineLevel() const;

    /**
        Returns @true if the attribute object specifies a page break before this
        paragraph.
    */
    bool HasPageBreak() const;

    /**
        Returns @true if the attribute object specifies spacing after a paragraph.
    */
    bool HasParagraphSpacingAfter() const;

    /**
        Returns @true if the attribute object specifies spacing before a paragraph.
    */
    bool HasParagraphSpacingBefore() const;

    /**
        Returns @true if the attribute object specifies a paragraph style name.
    */
    bool HasParagraphStyleName() const;

    /**
        Returns @true if the attribute object specifies a right indent.
    */
    bool HasRightIndent() const;

    /**
        Returns @true if the attribute object specifies tab stops.
    */
    bool HasTabs() const;

    /**
        Returns @true if the attribute object specifies a text foreground colour.
    */
    bool HasTextColour() const;

    /**
        Returns @true if the attribute object specifies text effects.
    */
    bool HasTextEffects() const;

    /**
        Returns @true if the attribute object specifies a URL.
    */
    bool HasURL() const;

    /**
        Returns @true if the object represents a character style, that is,
        the flags specify a font or a text background or foreground colour.
    */
    bool IsCharacterStyle() const;

    /**
        Returns @false if we have any attributes set, @true otherwise.
    */
    bool IsDefault() const;

    /**
        Returns @true if the object represents a paragraph style, that is,
        the flags specify alignment, indentation, tabs, paragraph spacing, or
        bullet style.
    */
    bool IsParagraphStyle() const;

    /**
        Copies all defined/valid properties from overlay to current object.
    */
    void Merge(const wxTextAttr& overlay);

    /**
        Creates a new @c wxTextAttr which is a merge of @a base and @a overlay.

        Properties defined in @a overlay take precedence over those in @a base.
        Properties undefined/invalid in both are undefined in the result.
    */
    static wxTextAttr Merge(const wxTextAttr& base,
                            const wxTextAttr& overlay);

    /**
        Sets the paragraph alignment. See ::wxTextAttrAlignment enumeration values.

        Of these, wxTEXT_ALIGNMENT_JUSTIFIED is unimplemented.
        In future justification may be supported when printing or previewing, only.
    */
    void SetAlignment(wxTextAttrAlignment alignment);

    /**
        Sets the background colour.
    */
    void SetBackgroundColour(const wxColour& colBack);

    /**
        Sets the name of the font associated with the bullet symbol.
        Only valid for attributes with wxTEXT_ATTR_BULLET_SYMBOL.
    */
    void SetBulletFont(const wxString& font);

    /**
        Sets the standard bullet name, applicable if the bullet style is
        wxTEXT_ATTR_BULLET_STYLE_STANDARD.

        See GetBulletName() for a list of supported names, and how
        to expand the range of supported types.
    */
    void SetBulletName(const wxString& name);

    /**
        Sets the bullet number.
    */
    void SetBulletNumber(int n);

    /**
        Sets the bullet style.

        The ::wxTextAttrBulletStyle enumeration values are all supported,
        except for wxTEXT_ATTR_BULLET_STYLE_BITMAP.
    */
    void SetBulletStyle(int style);

    /**
        Sets the bullet text, which could be a symbol, or (for example) cached
        outline text.
    */
    void SetBulletText(const wxString& text);

    /**
        Sets the character style name.
    */
    void SetCharacterStyleName(const wxString& name);

    /**
        Sets the flags determining which styles are being specified.
        The ::wxTextAttrFlags values can be passed in a bitlist.
    */
    void SetFlags(long flags);

    /**
        Sets the attributes for the given font.
        Note that wxTextAttr does not store an actual wxFont object.
    */
    void SetFont(const wxFont& font, int flags = wxTEXT_ATTR_FONT);

    /**
        Sets the font encoding.
    */
    void SetFontEncoding(wxFontEncoding encoding);

    /**
        Sets the paragraph alignment.
    */
    void SetFontFaceName(const wxString& faceName);

    /**
        Sets the font size in points.
    */
    void SetFontSize(int pointSize);

    /**
        Sets the font style (normal, italic or slanted).
    */
    void SetFontStyle(int fontStyle);

    /**
        Sets the font underlining.
    */
    void SetFontUnderlined(bool underlined);

    /**
        Sets the font weight.
    */
    void SetFontWeight(int fontWeight);

    /**
        Sets the left indent and left subindent in tenths of a millimetre.
        The sub-indent is an offset from the left of the paragraph, and is used for all
        but the first line in a paragraph.

        A positive value will cause the first line to appear to the left
        of the subsequent lines, and a negative value will cause the first line to be
        indented relative to the subsequent lines.

        wxRichTextBuffer uses indentation to render a bulleted item.
        The left indent is the distance between the margin and the bullet.
        The content of the paragraph, including the first line, starts
        at leftMargin + leftSubIndent.
        So the distance between the left edge of the bullet and the
        left of the actual paragraph is leftSubIndent.
    */
    void SetLeftIndent(int indent, int subIndent = 0);

    /**
        Sets the line spacing. @a spacing is a multiple, where 10 means single-spacing,
        15 means 1.5 spacing, and 20 means double spacing.
        The ::wxTextAttrLineSpacing values are defined for convenience.
    */
    void SetLineSpacing(int spacing);

    /**
        Sets the list style name.
    */
    void SetListStyleName(const wxString& name);

    /**
        Specifies the outline level. Zero represents normal text.
        At present, the outline level is not used, but may be used in future for
        determining list levels and for applications that need to store document
        structure information.
    */
    void SetOutlineLevel(int level);

    /**
        Specifies a page break before this paragraph.
    */
    void SetPageBreak(bool pageBreak = true);

    /**
        Sets the spacing after a paragraph, in tenths of a millimetre.
    */
    void SetParagraphSpacingAfter(int spacing);

    /**
        Sets the spacing before a paragraph, in tenths of a millimetre.
    */
    void SetParagraphSpacingBefore(int spacing);

    /**
        Sets the name of the paragraph style.
    */
    void SetParagraphStyleName(const wxString& name);

    /**
        Sets the right indent in tenths of a millimetre.
    */
    void SetRightIndent(int indent);

    /**
        Sets the tab stops, expressed in tenths of a millimetre.
        Each stop is measured from the left margin and therefore each value must be
        larger than the last.
    */
    void SetTabs(const wxArrayInt& tabs);

    /**
        Sets the text foreground colout.
    */
    void SetTextColour(const wxColour& colText);

    /**
        Sets the text effect bits of interest.

        You should also pass wxTEXT_ATTR_EFFECTS to SetFlags().
        See SetFlags() for further information.
    */
    void SetTextEffectFlags(int flags);

    /**
        Sets the text effects, a bit list of styles.
        The ::wxTextAttrEffects enumeration values can be used.

        Of these, only wxTEXT_ATTR_EFFECT_CAPITALS and wxTEXT_ATTR_EFFECT_STRIKETHROUGH
        are implemented.

        wxTEXT_ATTR_EFFECT_CAPITALS capitalises text when displayed (leaving the case
        of the actual buffer text unchanged), and wxTEXT_ATTR_EFFECT_STRIKETHROUGH draws
        a line through text.

        To set effects, you should also pass wxTEXT_ATTR_EFFECTS to SetFlags(), and call
        SetTextEffectFlags() with the styles (taken from the above set) that you
        are interested in setting.
    */
    void SetTextEffects(int effects);

    /**
        Sets the URL for the content. Sets the wxTEXT_ATTR_URL style; content with this
        style causes wxRichTextCtrl to show a hand cursor over it, and wxRichTextCtrl
        generates a wxTextUrlEvent when the content is clicked.
    */
    void SetURL(const wxString& url);

    /**
        Assignment from a wxTextAttr object.
    */
    void operator operator=(const wxTextAttr& attr);
};


/**
    @class wxTextCtrl

    A text control allows text to be displayed and edited.

    It may be single line or multi-line.

    @beginStyleTable
    @style{wxTE_PROCESS_ENTER}
           The control will generate the event wxEVT_COMMAND_TEXT_ENTER
           (otherwise pressing Enter key is either processed internally by the
           control or used for navigation between dialog controls).
    @style{wxTE_PROCESS_TAB}
           The control will receive wxEVT_CHAR events for TAB pressed -
           normally, TAB is used for passing to the next control in a dialog
           instead. For the control created with this style, you can still use
           Ctrl-Enter to pass to the next control from the keyboard.
    @style{wxTE_MULTILINE}
           The text control allows multiple lines.
    @style{wxTE_PASSWORD}
           The text will be echoed as asterisks.
    @style{wxTE_READONLY}
           The text will not be user-editable.
    @style{wxTE_RICH}
           Use rich text control under Win32, this allows to have more than
           64KB of text in the control even under Win9x. This style is ignored
           under other platforms.
    @style{wxTE_RICH2}
           Use rich text control version 2.0 or 3.0 under Win32, this style is
           ignored under other platforms
    @style{wxTE_AUTO_URL}
           Highlight the URLs and generate the wxTextUrlEvents when mouse
           events occur over them. This style is only supported for wxTE_RICH
           Win32 and multi-line wxGTK2 text controls.
    @style{wxTE_NOHIDESEL}
           By default, the Windows text control doesn't show the selection
           when it doesn't have focus - use this style to force it to always
           show it. It doesn't do anything under other platforms.
    @style{wxHSCROLL}
           A horizontal scrollbar will be created and used, so that text won't
           be wrapped. No effect under wxGTK1.
    @style{wxTE_NO_VSCROLL}
           For multiline controls only: vertical scrollbar will never be
           created. This limits the amount of text which can be entered into
           the control to what can be displayed in it under MSW but not under
           GTK2. Currently not implemented for the other platforms.
    @style{wxTE_LEFT}
           The text in the control will be left-justified (default).
    @style{wxTE_CENTRE}
           The text in the control will be centered (currently wxMSW and
           wxGTK2 only).
    @style{wxTE_RIGHT}
           The text in the control will be right-justified (currently wxMSW
           and wxGTK2 only).
    @style{wxTE_DONTWRAP}
           Same as wxHSCROLL style: don't wrap at all, show horizontal
           scrollbar instead.
    @style{wxTE_CHARWRAP}
           Wrap the lines too long to be shown entirely at any position
           (wxUniv and wxGTK2 only).
    @style{wxTE_WORDWRAP}
           Wrap the lines too long to be shown entirely at word boundaries
           (wxUniv and wxGTK2 only).
    @style{wxTE_BESTWRAP}
           Wrap the lines at word boundaries or at any other character if
           there are words longer than the window width (this is the default).
    @style{wxTE_CAPITALIZE}
           On PocketPC and Smartphone, causes the first letter to be
           capitalized.
    @endStyleTable

    Note that alignment styles (wxTE_LEFT, wxTE_CENTRE and wxTE_RIGHT) can be
    changed dynamically after control creation on wxMSW and wxGTK. wxTE_READONLY,
    wxTE_PASSWORD and wrapping styles can be dynamically changed under wxGTK but
    not wxMSW. The other styles can be only set during control creation.


    @section textctrl_text_format wxTextCtrl Text Format

    The multiline text controls always store the text as a sequence of lines
    separated by @c '\\n' characters, i.e. in the Unix text format even on
    non-Unix platforms. This allows the user code to ignore the differences
    between the platforms but at a price: the indices in the control such as
    those returned by GetInsertionPoint() or GetSelection() can @b not be used
    as indices into the string returned by GetValue() as they're going to be
    slightly off for platforms using @c "\\r\\n" as separator (as Windows
    does).

    Instead, if you need to obtain a substring between the 2 indices obtained
    from the control with the help of the functions mentioned above, you should
    use GetRange(). And the indices themselves can only be passed to other
    methods, for example SetInsertionPoint() or SetSelection().

    To summarize: never use the indices returned by (multiline) wxTextCtrl as
    indices into the string it contains, but only as arguments to be passed
    back to the other wxTextCtrl methods. This problem doesn't arise for
    single-line platforms however where the indices in the control do
    correspond to the positions in the value string.


    @section textctrl_styles wxTextCtrl Styles.

    Multi-line text controls support styling, i.e. provide a possibility to set
    colours and font for individual characters in it (note that under Windows
    @c wxTE_RICH style is required for style support). To use the styles you
    can either call SetDefaultStyle() before inserting the text or call
    SetStyle() later to change the style of the text already in the control
    (the first solution is much more efficient).

    In either case, if the style doesn't specify some of the attributes (for
    example you only want to set the text colour but without changing the font
    nor the text background), the values of the default style will be used for
    them. If there is no default style, the attributes of the text control
    itself are used.

    So the following code correctly describes what it does: the second call to
    SetDefaultStyle() doesn't change the text foreground colour (which stays
    red) while the last one doesn't change the background colour (which stays
    grey):

    @code
    text->SetDefaultStyle(wxTextAttr(*wxRED));
    text->AppendText("Red text\n");
    text->SetDefaultStyle(wxTextAttr(wxNullColour, *wxLIGHT_GREY));
    text->AppendText("Red on grey text\n");
    text->SetDefaultStyle(wxTextAttr(*wxBLUE);
    text->AppendText("Blue on grey text\n");
    @endcode


    @section textctrl_cpp_streams wxTextCtrl and C++ Streams

    This class multiply-inherits from @c std::streambuf (except for some really
    old compilers using non-standard iostream library), allowing code such as
    the following:

    @code
    wxTextCtrl *control = new wxTextCtrl(...);

    ostream stream(control)

    stream << 123.456 << " some text\n";
    stream.flush();
    @endcode

    Note that even if your compiler doesn't support this (the symbol
    @c wxHAS_TEXT_WINDOW_STREAM has value of 0 then) you can still use
    wxTextCtrl itself in a stream-like manner:

    @code
    wxTextCtrl *control = new wxTextCtrl(...);

    *control << 123.456 << " some text\n";
    @endcode

    However the possibility to create an ostream associated with wxTextCtrl may
    be useful if you need to redirect the output of a function taking an
    ostream as parameter to a text control.

    Another commonly requested need is to redirect @c std::cout to the text
    control. This may be done in the following way:

    @code
    #include <iostream>

    wxTextCtrl *control = new wxTextCtrl(...);

    std::streambuf *sbOld = std::cout.rdbuf();
    std::cout.rdbuf(control);

    // use cout as usual, the output appears in the text control
    ...

    std::cout.rdbuf(sbOld);
    @endcode

    But wxWidgets provides a convenient class to make it even simpler so
    instead you may just do

    @code
    #include <iostream>

    wxTextCtrl *control = new wxTextCtrl(...);

    wxStreamToTextRedirector redirect(control);

    // all output to cout goes into the text control until the exit from
    // current scope
    @endcode

    See wxStreamToTextRedirector for more details.


    @section textctrl_event_handling Event Handling.

    The following commands are processed by default event handlers in
    wxTextCtrl: @c wxID_CUT, @c wxID_COPY, @c wxID_PASTE, @c wxID_UNDO, @c
    wxID_REDO. The associated UI update events are also processed
    automatically, when the control has the focus.

    @beginEventTable{wxCommandEvent}
    @event{EVT_TEXT(id, func)}
        Respond to a wxEVT_COMMAND_TEXT_UPDATED event, generated when the text
        changes. Notice that this event will be sent when the text controls
        contents changes -- whether this is due to user input or comes from the
        program itself (for example, if wxTextCtrl::SetValue() is called); see
        wxTextCtrl::ChangeValue() for a function which does not send this event.
        This event is however not sent during the control creation.
    @event{EVT_TEXT_ENTER(id, func)}
        Respond to a wxEVT_COMMAND_TEXT_ENTER event, generated when enter is
        pressed in a text control which must have wxTE_PROCESS_ENTER style for
        this event to be generated.
    @event{EVT_TEXT_URL(id, func)}
        A mouse event occurred over an URL in the text control (wxMSW and
        wxGTK2 only currently).
    @event{EVT_TEXT_MAXLEN(id, func)}
        This event is generated when the user tries to enter more text into the
        control than the limit set by wxTextCtrl::SetMaxLength(), see its description.
    @endEventTable

    @library{wxcore}
    @category{ctrl}
    @appearance{textctrl.png}

    @see wxTextCtrl::Create, wxValidator
*/
class wxTextCtrl : public wxControl
{
public:
    /**
        Default ctor.
    */
    wxTextCtrl();

    /**
        Constructor, creating and showing a text control.

        @param parent
            Parent window. Should not be @NULL.
        @param id
            Control identifier. A value of -1 denotes a default value.
        @param value
            Default text value.
        @param pos
            Text control position.
        @param size
            Text control size.
        @param style
            Window style. See wxTextCtrl.
        @param validator
            Window validator.
        @param name
            Window name.

        @remarks
            The horizontal scrollbar (wxHSCROLL style flag) will only be
            created for multi-line text controls. Without a horizontal
            scrollbar, text lines that don't fit in the control's size will be
            wrapped (but no newline character is inserted).
            Single line controls don't have a horizontal scrollbar, the text is
            automatically scrolled so that the insertion point is always visible.

        @see Create(), wxValidator
    */
    wxTextCtrl(wxWindow* parent, wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxTextCtrlNameStr);

    /**
        Destructor, destroying the text control.
    */
    virtual ~wxTextCtrl();

    /**
        Appends the text to the end of the text control.

        @param text
            Text to write to the text control.

        @remarks
            After the text is appended, the insertion point will be at the
            end of the text control. If this behaviour is not desired, the
            programmer should use GetInsertionPoint() and SetInsertionPoint().

        @see WriteText()
    */
    virtual void AppendText(const wxString& text);

    /**
        Call this function to enable auto-completion of the text typed in a
        single-line text control using the given @a choices.

        Notice that currently this function is only implemented in wxGTK2 and
        wxMSW ports and does nothing under the other platforms.

        @since 2.9.0

        @return
            @true if the auto-completion was enabled or @false if the operation
            failed, typically because auto-completion is not supported by the
            current platform.

        @see AutoCompleteFileNames()
    */
    virtual bool AutoComplete(const wxArrayString& choices);

    /**
        Call this function to enable auto-completion of the text typed in a
        single-line text control using all valid file system paths.

        Notice that currently this function is only implemented in wxGTK2 port
        and does nothing under the other platforms.

        @since 2.9.0

        @return
            @true if the auto-completion was enabled or @false if the operation
            failed, typically because auto-completion is not supported by the
            current platform.

        @see AutoComplete()
    */
    virtual bool AutoCompleteFileNames();

    /**
        Returns @true if the selection can be copied to the clipboard.
    */
    virtual bool CanCopy() const;

    /**
        Returns @true if the selection can be cut to the clipboard.
    */
    virtual bool CanCut() const;

    /**
        Returns @true if the contents of the clipboard can be pasted into the
        text control.

        On some platforms (Motif, GTK) this is an approximation and returns
        @true if the control is editable, @false otherwise.
    */
    virtual bool CanPaste() const;

    /**
        Returns @true if there is a redo facility available and the last
        operation can be redone.
    */
    virtual bool CanRedo() const;

    /**
        Returns @true if there is an undo facility available and the last
        operation can be undone.
    */
    virtual bool CanUndo() const;

    /**
        Sets the new text control value.

        It also marks the control as not-modified which means that IsModified()
        would return @false immediately after the call to SetValue().

        The insertion point is set to the start of the control (i.e. position
        0) by this function.

        This functions does not generate the @c wxEVT_COMMAND_TEXT_UPDATED
        event but otherwise is identical to SetValue().

        See @ref overview_eventhandling_prog for more information.

        @since 2.7.1

        @param value
            The new value to set. It may contain newline characters if the text
            control is multi-line.
    */
    virtual void ChangeValue(const wxString& value);

    /**
        Clears the text in the control.

        Note that this function will generate a @c wxEVT_COMMAND_TEXT_UPDATED
        event, i.e. its effect is identical to calling @c SetValue("").
    */
    virtual void Clear();

    /**
        Copies the selected text to the clipboard.
    */
    virtual void Copy();

    /**
        Creates the text control for two-step construction.

        This method should be called if the default constructor was used for
        the control creation. Its parameters have the same meaning as for the
        non-default constructor.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxTextCtrlNameStr);

    /**
        Copies the selected text to the clipboard and removes the selection.
    */
    virtual void Cut();

    /**
        Resets the internal modified flag as if the current changes had been
        saved.
    */
    virtual void DiscardEdits();

    /**
        This functions inserts into the control the character which would have
        been inserted if the given key event had occurred in the text control.

        The @a event object should be the same as the one passed to @c EVT_KEY_DOWN
        handler previously by wxWidgets. Please note that this function doesn't
        currently work correctly for all keys under any platform but MSW.

        @return
            @true if the event resulted in a change to the control, @false otherwise.
    */
    virtual bool EmulateKeyPress(const wxKeyEvent& event);

    /**
        Returns the style currently used for the new text.

        @see SetDefaultStyle()
    */
    virtual const wxTextAttr& GetDefaultStyle() const;

    /**
        Returns the insertion point, or cursor, position.

        This is defined as the zero based index of the character position to
        the right of the insertion point. For example, if the insertion point
        is at the end of the single-line text control, it is equal to both
        GetLastPosition() and @c "GetValue().Length()" (but notice that the latter
        equality is not necessarily true for multiline edit controls which may
        use multiple new line characters).

        The following code snippet safely returns the character at the insertion
        point or the zero character if the point is at the end of the control.

        @code
        char GetCurrentChar(wxTextCtrl *tc) {
            if (tc->GetInsertionPoint() == tc->GetLastPosition())
            return '\0';
            return tc->GetValue[tc->GetInsertionPoint()];
        }
        @endcode
    */
    virtual long GetInsertionPoint() const;

    /**
        Returns the zero based index of the last position in the text control,
        which is equal to the number of characters in the control.
    */
    virtual wxTextPos GetLastPosition() const;

    /**
        Gets the length of the specified line, not including any trailing
        newline character(s).

        @param lineNo
            Line number (starting from zero).

        @return
            The length of the line, or -1 if @a lineNo was invalid.
    */
    virtual int GetLineLength(long lineNo) const;

    /**
        Returns the contents of a given line in the text control, not including
        any trailing newline character(s).

        @param lineNo
            The line number, starting from zero.

        @return
            The contents of the line.
    */
    virtual wxString GetLineText(long lineNo) const;

    /**
        Returns the number of lines in the text control buffer.

        @remarks
            Note that even empty text controls have one line (where the
            insertion point is), so GetNumberOfLines() never returns 0.
            For wxGTK using GTK+ 1.2.x and earlier, the number of lines in a
            multi-line text control is calculated by actually counting newline
            characters in the buffer, i.e. this function returns the number of
            logical lines and doesn't depend on whether any of them are wrapped.
            For all the other platforms, the number of physical lines in the
            control is returned.
            Also note that you may wish to avoid using functions that work with
            line numbers if you are working with controls that contain large
            amounts of text as this function has O(N) complexity for N being
            the number of lines.
    */
    virtual int GetNumberOfLines() const;

    /**
        Returns the string containing the text starting in the positions
        @a from and up to @a to in the control.

        The positions must have been returned by another wxTextCtrl method.
        Please note that the positions in a multiline wxTextCtrl do @b not
        correspond to the indices in the string returned by GetValue() because
        of the different new line representations (@c CR or @c CR LF) and so
        this method should be used to obtain the correct results instead of
        extracting parts of the entire value. It may also be more efficient,
        especially if the control contains a lot of data.
    */
    virtual wxString GetRange(long from, long to) const;

    /**
        Gets the current selection span.

        If the returned values are equal, there was no selection. Please note
        that the indices returned may be used with the other wxTextCtrl methods
        but don't necessarily represent the correct indices into the string
        returned by GetValue() for multiline controls under Windows (at least,)
        you should use GetStringSelection() to get the selected text.

        @param from
            The returned first position.
        @param to
            The returned last position.
    */
    virtual void GetSelection(long* from, long* to) const;

    /**
        Gets the text currently selected in the control.

        If there is no selection, the returned string is empty.
    */
    virtual wxString GetStringSelection() const;

    /**
        Returns the style at this position in the text control.

        Not all platforms support this function.

        @return
            @true on success, @false if an error occurred (this may also mean
            that the styles are not supported under this platform).

        @see SetStyle(), wxTextAttr
    */
    virtual bool GetStyle(long position, wxTextAttr& style);

    /**
        Gets the contents of the control.

        Notice that for a multiline text control, the lines will be separated
        by (Unix-style) @c \\n characters, even under Windows where they are
        separated by a @c \\r\\n sequence in the native control.
    */
    virtual wxString GetValue() const;

    /**
        This function finds the character at the specified position expressed
        in pixels.

        If the return code is not @c wxTE_HT_UNKNOWN the row and column of the
        character closest to this position are returned in the @a col and @a
        row parameters (unless the pointers are @NULL which is allowed).
        Please note that this function is currently only implemented in wxUniv, wxMSW
        and wxGTK2 ports.

        @see PositionToXY(), XYToPosition()
    */
    wxTextCtrlHitTestResult HitTest(const wxPoint& pt,
                                    wxTextCoord col,
                                    wxTextCoord row) const;

    /**
        Returns @true if the controls contents may be edited by user (note that
        it always can be changed by the program).

        In other words, this functions returns @true if the control hasn't been
        put in read-only mode by a previous call to SetEditable().
    */
    virtual bool IsEditable() const;

    /**
        Returns @true if the control is currently empty.

        This is the same as @c GetValue().empty() but can be much more
        efficient for the multiline controls containing big amounts of text.

        @since 2.7.1
    */
    virtual bool IsEmpty() const;

    /**
        Returns @true if the text has been modified by user.

        Note that calling SetValue() doesn't make the control modified.

        @see MarkDirty()
    */
    virtual bool IsModified() const;

    /**
        Returns @true if this is a multi line edit control and @false otherwise.

        @see IsSingleLine()
    */
    bool IsMultiLine() const;

    /**
        Returns @true if this is a single line edit control and @false otherwise.

        @see IsSingleLine(), IsMultiLine()
    */
    bool IsSingleLine() const;

    /**
        Loads and displays the named file, if it exists.

        @param filename
            The filename of the file to load.
        @param fileType
            The type of file to load. This is currently ignored in wxTextCtrl.

        @return
            @true if successful, @false otherwise.
    */
    bool LoadFile(const wxString& filename,
                  int fileType = wxTEXT_TYPE_ANY);

    /**
        Mark text as modified (dirty).

        @see IsModified()
    */
    virtual void MarkDirty();

    /**
        This event handler function implements default drag and drop behaviour,
        which is to load the first dropped file into the control.

        @param event
            The drop files event.

        @remarks This is not implemented on non-Windows platforms.

        @see wxDropFilesEvent
    */
    void OnDropFiles(wxDropFilesEvent& event);

    /**
        Pastes text from the clipboard to the text item.
    */
    virtual void Paste();

    /**
        Converts given position to a zero-based column, line number pair.

        @param pos
            Position.
        @param x
            Receives zero based column number.
        @param y
            Receives zero based line number.

        @return
            @true on success, @false on failure (most likely due to a too large
            position parameter).

        @see XYToPosition()
    */
    virtual bool PositionToXY(long pos, long* x, long* y) const;

    /**
        If there is a redo facility and the last operation can be redone,
        redoes the last operation.

        Does nothing if there is no redo facility.
    */
    virtual void Redo();

    /**
        Removes the text starting at the first given position up to
        (but not including) the character at the last position.

        This function puts the current insertion point position at @a to as a
        side effect.

        @param from
            The first position.
        @param to
            The last position.
    */
    virtual void Remove(long from, long to);

    /**
        Replaces the text starting at the first position up to
        (but not including) the character at the last position with the given text.

        This function puts the current insertion point position at @a to as a
        side effect.

        @param from
            The first position.
        @param to
            The last position.
        @param value
            The value to replace the existing text with.
    */
    virtual void Replace(long from, long to, const wxString& value);

    /**
        Saves the contents of the control in a text file.

        @param filename
            The name of the file in which to save the text.
        @param fileType
            The type of file to save. This is currently ignored in wxTextCtrl.

        @return
            @true if the operation was successful, @false otherwise.
    */
    bool SaveFile(const wxString& filename = wxEmptyString,
                  int fileType = wxTEXT_TYPE_ANY);

    /**
        Changes the default style to use for the new text which is going to be
        added to the control using WriteText() or AppendText().

        If either of the font, foreground, or background colour is not set in
        @a style, the values of the previous default style are used for them.
        If the previous default style didn't set them neither, the global font
        or colours of the text control itself are used as fall back.

        However if the @a style parameter is the default wxTextAttr, then the default
        style is just reset (instead of being combined with the new style which
        wouldn't change it at all).

        @param style
            The style for the new text.

        @return
            @true on success, @false if an error occurred (this may also mean
            that the styles are not supported under this platform).

        @see GetDefaultStyle()
    */
    virtual bool SetDefaultStyle(const wxTextAttr& style);

    /**
        Makes the text item editable or read-only, overriding the
        @b wxTE_READONLY flag.

        @param editable
            If @true, the control is editable. If @false, the control is
            read-only.

        @see IsEditable()
    */
    virtual void SetEditable(bool editable);

    /**
        Sets the insertion point at the given position.

        @param pos
            Position to set, in the range from 0 to GetLastPosition() inclusive.
    */
    virtual void SetInsertionPoint(long pos);

    /**
        Sets the insertion point at the end of the text control.

        This is equivalent to calling wxTextCtrl::SetInsertionPoint() with
        wxTextCtrl::GetLastPosition() argument.
    */
    virtual void SetInsertionPointEnd();

    /**
        This function sets the maximum number of characters the user can enter
        into the control.

        In other words, it allows to limit the text value length to @a len not
        counting the terminating @c NUL character.

        If @a len is 0, the previously set max length limit, if any, is discarded
        and the user may enter as much text as the underlying native text control widget
        supports (typically at least 32Kb).
        If the user tries to enter more characters into the text control when it
        already is filled up to the maximal length, a @c wxEVT_COMMAND_TEXT_MAXLEN
        event is sent to notify the program about it (giving it the possibility
        to show an explanatory message, for example) and the extra input is discarded.

        Note that in wxGTK this function may only be used with single line text controls.
    */
    virtual void SetMaxLength(unsigned long len);

    /**
        Marks the control as being modified by the user or not.

        @see MarkDirty(), DiscardEdits()
    */
    void SetModified(bool modified);

    /**
        Selects the text starting at the first position up to (but not
        including) the character at the last position.

        If both parameters are equal to -1 all text in the control is selected.

        Notice that the insertion point will be moved to @a from by this
        function.

        @param from
            The first position.
        @param to
            The last position.

        @see SelectAll()
    */
    virtual void SetSelection(long from, long to);

    /**
        Selects all text in the control.

        @see SetSelection()
    */
    virtual void SelectAll();

    /**
        Changes the style of the given range.

        If any attribute within @a style is not set, the corresponding
        attribute from GetDefaultStyle() is used.

        @param start
            The start of the range to change.
        @param end
            The end of the range to change.
        @param style
            The new style for the range.

        @return
            @true on success, @false if an error occurred (this may also mean
            that the styles are not supported under this platform).

        @see GetStyle(), wxTextAttr
    */
    virtual bool SetStyle(long start, long end, const wxTextAttr& style);

    /**
        Sets the new text control value.

        It also marks the control as not-modified which means that IsModified()
        would return @false immediately after the call to SetValue().

        The insertion point is set to the start of the control (i.e. position
        0) by this function.

        Note that, unlike most other functions changing the controls values,
        this function generates a @c wxEVT_COMMAND_TEXT_UPDATED event. To avoid
        this you can use ChangeValue() instead.

        @param value
            The new value to set. It may contain newline characters if the text
            control is multi-line.
    */
    virtual void SetValue(const wxString& value);

    /**
        Makes the line containing the given position visible.

        @param pos
            The position that should be visible.
    */
    virtual void ShowPosition(long pos);

    /**
        If there is an undo facility and the last operation can be undone,
        undoes the last operation.

        Does nothing if there is no undo facility.
    */
    virtual void Undo();

    /**
        Writes the text into the text control at the current insertion position.

        @param text
            Text to write to the text control.

        @remarks
            Newlines in the text string are the only control characters
            allowed, and they will cause appropriate line breaks.
            See operator<<() and AppendText() for more convenient ways of
            writing to the window.
            After the write operation, the insertion point will be at the end
            of the inserted text, so subsequent write operations will be appended.
            To append text after the user may have interacted with the control,
            call wxTextCtrl::SetInsertionPointEnd() before writing.
    */
    virtual void WriteText(const wxString& text);

    /**
        Converts the given zero based column and line number to a position.

        @param x
            The column number.
        @param y
            The line number.

        @return
            The position value, or -1 if x or y was invalid.
    */
    virtual long XYToPosition(long x, long y) const;

    //@{
    /**
        Operator definitions for appending to a text control.

        These operators can be used as with the standard C++ streams, for
        example:
        @code
            wxTextCtrl *wnd = new wxTextCtrl(my_frame);

            (*wnd) << "Welcome to text control number " << 1 << ".\n";
        @endcode
    */

    wxTextCtrl& operator<<(const wxString& s);
    wxTextCtrl& operator<<(int i);
    wxTextCtrl& operator<<(long i);
    wxTextCtrl& operator<<(float f);
    wxTextCtrl& operator<<(double d);
    wxTextCtrl& operator<<(char c);
    wxTextCtrl& operator<<(wchar_t c);
    //@}
};



/**
    @class wxStreamToTextRedirector

    This class can be used to (temporarily) redirect all output sent to a C++
    ostream object to a wxTextCtrl instead.

    @note
        Some compilers and/or build configurations don't support multiply
        inheriting wxTextCtrl from @c std::streambuf in which case this class is
        not compiled in.
        You also must have @c wxUSE_STD_IOSTREAM option on (i.e. set to 1) in your
        @c setup.h to be able to use it. Under Unix, specify @c --enable-std_iostreams
        switch when running configure for this.

    Example of usage:

    @code
    using namespace std;
    wxTextCtrl* text = new wxTextCtrl(...);
    {
        wxStreamToTextRedirector redirect(text);

        // this goes to the text control
        cout << "Hello, text!" << endl;
    }
    // this goes somewhere else, presumably to stdout
    cout << "Hello, console!" << endl;
    @endcode

    @library{wxcore}
    @category{logging}

    @see wxTextCtrl
*/
class wxStreamToTextRedirector
{
public:
    /**
        The constructor starts redirecting output sent to @a ostr or @a cout for
        the default parameter value to the text control @a text.

        @param text
            The text control to append output too, must be non-@NULL
        @param ostr
            The C++ stream to redirect, cout is used if it is @NULL
    */
    wxStreamToTextRedirector(wxTextCtrl *text, ostream* ostr);

    /**
        When a wxStreamToTextRedirector object is destroyed, the redirection is ended
        and any output sent to the C++ ostream which had been specified at the time of
        the object construction will go to its original destination.
    */
    ~wxStreamToTextRedirector();
};

