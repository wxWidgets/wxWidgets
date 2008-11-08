/////////////////////////////////////////////////////////////////////////////
// Name:        font.h
// Purpose:     interface of wxFont
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/**
    Standard font families: these may be used only for the font creation, it
    doesn't make sense to query an existing font for its font family as,
    especially if the font had been created from a native font description, it
    may be unknown.
*/
enum wxFontFamily
{
    wxFONTFAMILY_DEFAULT = wxDEFAULT,           //!< Chooses a default font.
    wxFONTFAMILY_DECORATIVE = wxDECORATIVE,     //!< A decorative font.
    wxFONTFAMILY_ROMAN = wxROMAN,               //!< A formal, serif font.
    wxFONTFAMILY_SCRIPT = wxSCRIPT,             //!< A handwriting font.
    wxFONTFAMILY_SWISS = wxSWISS,               //!< A sans-serif font.
    wxFONTFAMILY_MODERN = wxMODERN,             //!< A fixed pitch font.
    wxFONTFAMILY_TELETYPE = wxTELETYPE,         //!< A teletype font.
    wxFONTFAMILY_MAX,
    wxFONTFAMILY_UNKNOWN = wxFONTFAMILY_MAX
};

/**
    Font styles.
*/
enum wxFontStyle
{
    wxFONTSTYLE_NORMAL = wxNORMAL,
    wxFONTSTYLE_ITALIC = wxITALIC,
    wxFONTSTYLE_SLANT = wxSLANT,
    wxFONTSTYLE_MAX
};

/**
    Font weights.
*/
enum wxFontWeight
{
    wxFONTWEIGHT_NORMAL = wxNORMAL,  //!< Normal font.
    wxFONTWEIGHT_LIGHT = wxLIGHT,    //!< Light font.
    wxFONTWEIGHT_BOLD = wxBOLD,      //!< Bold font.
    wxFONTWEIGHT_MAX
};

/**
    The font flag bits for the new font ctor accepting one combined flags word.
*/
enum wxFontFlag
{
    /// no special flags: font with default weight/slant/anti-aliasing
    wxFONTFLAG_DEFAULT          = 0,

    /// slant flags (default: no slant)
    wxFONTFLAG_ITALIC           = 1 << 0,
    wxFONTFLAG_SLANT            = 1 << 1,

    /// weight flags (default: medium)
    wxFONTFLAG_LIGHT            = 1 << 2,
    wxFONTFLAG_BOLD             = 1 << 3,

    /// anti-aliasing flag: force on or off (default: the current system default)
    wxFONTFLAG_ANTIALIASED      = 1 << 4,
    wxFONTFLAG_NOT_ANTIALIASED  = 1 << 5,

    /// underlined/strikethrough flags (default: no lines)
    wxFONTFLAG_UNDERLINED       = 1 << 6,
    wxFONTFLAG_STRIKETHROUGH    = 1 << 7,

    /// the mask of all currently used flags
    wxFONTFLAG_MASK = wxFONTFLAG_ITALIC             |
                      wxFONTFLAG_SLANT              |
                      wxFONTFLAG_LIGHT              |
                      wxFONTFLAG_BOLD               |
                      wxFONTFLAG_ANTIALIASED        |
                      wxFONTFLAG_NOT_ANTIALIASED    |
                      wxFONTFLAG_UNDERLINED         |
                      wxFONTFLAG_STRIKETHROUGH
};



/**
    Font encodings.
*/
enum wxFontEncoding
{
    /// Default system encoding.
    wxFONTENCODING_SYSTEM = -1,     // system default

    /// Default application encoding.
    wxFONTENCODING_DEFAULT,         // current default encoding

    // ISO8859 standard defines a number of single-byte charsets
    wxFONTENCODING_ISO8859_1,       // West European (Latin1)
    wxFONTENCODING_ISO8859_2,       // Central and East European (Latin2)
    wxFONTENCODING_ISO8859_3,       // Esperanto (Latin3)
    wxFONTENCODING_ISO8859_4,       // Baltic (old) (Latin4)
    wxFONTENCODING_ISO8859_5,       // Cyrillic
    wxFONTENCODING_ISO8859_6,       // Arabic
    wxFONTENCODING_ISO8859_7,       // Greek
    wxFONTENCODING_ISO8859_8,       // Hebrew
    wxFONTENCODING_ISO8859_9,       // Turkish (Latin5)
    wxFONTENCODING_ISO8859_10,      // Variation of Latin4 (Latin6)
    wxFONTENCODING_ISO8859_11,      // Thai
    wxFONTENCODING_ISO8859_12,      // doesn't exist currently, but put it
                                    // here anyhow to make all ISO8859
                                    // consecutive numbers
    wxFONTENCODING_ISO8859_13,      // Baltic (Latin7)
    wxFONTENCODING_ISO8859_14,      // Latin8
    wxFONTENCODING_ISO8859_15,      // Latin9 (a.k.a. Latin0, includes euro)
    wxFONTENCODING_ISO8859_MAX,

    // Cyrillic charset soup (see http://czyborra.com/charsets/cyrillic.html)
    wxFONTENCODING_KOI8,            // KOI8 Russian
    wxFONTENCODING_KOI8_U,          // KOI8 Ukrainian
    wxFONTENCODING_ALTERNATIVE,     // same as MS-DOS CP866
    wxFONTENCODING_BULGARIAN,       // used under Linux in Bulgaria

    // what would we do without Microsoft? They have their own encodings
        // for DOS
    wxFONTENCODING_CP437,           // original MS-DOS codepage
    wxFONTENCODING_CP850,           // CP437 merged with Latin1
    wxFONTENCODING_CP852,           // CP437 merged with Latin2
    wxFONTENCODING_CP855,           // another cyrillic encoding
    wxFONTENCODING_CP866,           // and another one
        // and for Windows
    wxFONTENCODING_CP874,           // WinThai
    wxFONTENCODING_CP932,           // Japanese (shift-JIS)
    wxFONTENCODING_CP936,           // Chinese simplified (GB)
    wxFONTENCODING_CP949,           // Korean (Hangul charset)
    wxFONTENCODING_CP950,           // Chinese (traditional - Big5)
    wxFONTENCODING_CP1250,          // WinLatin2
    wxFONTENCODING_CP1251,          // WinCyrillic
    wxFONTENCODING_CP1252,          // WinLatin1
    wxFONTENCODING_CP1253,          // WinGreek (8859-7)
    wxFONTENCODING_CP1254,          // WinTurkish
    wxFONTENCODING_CP1255,          // WinHebrew
    wxFONTENCODING_CP1256,          // WinArabic
    wxFONTENCODING_CP1257,          // WinBaltic (same as Latin 7)
    wxFONTENCODING_CP12_MAX,

    wxFONTENCODING_UTF7,            // UTF-7 Unicode encoding
    wxFONTENCODING_UTF8,            // UTF-8 Unicode encoding
    wxFONTENCODING_EUC_JP,          // Extended Unix Codepage for Japanese
    wxFONTENCODING_UTF16BE,         // UTF-16 Big Endian Unicode encoding
    wxFONTENCODING_UTF16LE,         // UTF-16 Little Endian Unicode encoding
    wxFONTENCODING_UTF32BE,         // UTF-32 Big Endian Unicode encoding
    wxFONTENCODING_UTF32LE,         // UTF-32 Little Endian Unicode encoding

    wxFONTENCODING_MACROMAN,        // the standard mac encodings
    wxFONTENCODING_MACJAPANESE,
    wxFONTENCODING_MACCHINESETRAD,
    wxFONTENCODING_MACKOREAN,
    wxFONTENCODING_MACARABIC,
    wxFONTENCODING_MACHEBREW,
    wxFONTENCODING_MACGREEK,
    wxFONTENCODING_MACCYRILLIC,
    wxFONTENCODING_MACDEVANAGARI,
    wxFONTENCODING_MACGURMUKHI,
    wxFONTENCODING_MACGUJARATI,
    wxFONTENCODING_MACORIYA,
    wxFONTENCODING_MACBENGALI,
    wxFONTENCODING_MACTAMIL,
    wxFONTENCODING_MACTELUGU,
    wxFONTENCODING_MACKANNADA,
    wxFONTENCODING_MACMALAJALAM,
    wxFONTENCODING_MACSINHALESE,
    wxFONTENCODING_MACBURMESE,
    wxFONTENCODING_MACKHMER,
    wxFONTENCODING_MACTHAI,
    wxFONTENCODING_MACLAOTIAN,
    wxFONTENCODING_MACGEORGIAN,
    wxFONTENCODING_MACARMENIAN,
    wxFONTENCODING_MACCHINESESIMP,
    wxFONTENCODING_MACTIBETAN,
    wxFONTENCODING_MACMONGOLIAN,
    wxFONTENCODING_MACETHIOPIC,
    wxFONTENCODING_MACCENTRALEUR,
    wxFONTENCODING_MACVIATNAMESE,
    wxFONTENCODING_MACARABICEXT,
    wxFONTENCODING_MACSYMBOL,
    wxFONTENCODING_MACDINGBATS,
    wxFONTENCODING_MACTURKISH,
    wxFONTENCODING_MACCROATIAN,
    wxFONTENCODING_MACICELANDIC,
    wxFONTENCODING_MACROMANIAN,
    wxFONTENCODING_MACCELTIC,
    wxFONTENCODING_MACGAELIC,
    wxFONTENCODING_MACKEYBOARD,

    // more CJK encodings (for historical reasons some are already declared
    // above)
    wxFONTENCODING_ISO2022_JP,      // ISO-2022-JP JIS encoding

    wxFONTENCODING_MAX,             // highest enumerated encoding value

    wxFONTENCODING_MACMIN = wxFONTENCODING_MACROMAN ,
    wxFONTENCODING_MACMAX = wxFONTENCODING_MACKEYBOARD ,

    // aliases for endian-dependent UTF encodings
#ifdef WORDS_BIGENDIAN
    wxFONTENCODING_UTF16 = wxFONTENCODING_UTF16BE,  // native UTF-16
    wxFONTENCODING_UTF32 = wxFONTENCODING_UTF32BE,  // native UTF-32
#else // WORDS_BIGENDIAN
    wxFONTENCODING_UTF16 = wxFONTENCODING_UTF16LE,  // native UTF-16
    wxFONTENCODING_UTF32 = wxFONTENCODING_UTF32LE,  // native UTF-32
#endif // WORDS_BIGENDIAN

    // alias for the native Unicode encoding on this platform
    // (this is used by wxEncodingConverter and wxUTFFile only for now)
#if SIZEOF_WCHAR_T == 2
    wxFONTENCODING_UNICODE = wxFONTENCODING_UTF16,
#else // SIZEOF_WCHAR_T == 4
    wxFONTENCODING_UNICODE = wxFONTENCODING_UTF32,
#endif

    // alternative names for Far Eastern encodings
    // Chinese
    wxFONTENCODING_GB2312 = wxFONTENCODING_CP936, // Simplified Chinese
    wxFONTENCODING_BIG5 = wxFONTENCODING_CP950,   // Traditional Chinese

        // Japanese (see http://zsigri.tripod.com/fontboard/cjk/jis.html)
    wxFONTENCODING_SHIFT_JIS = wxFONTENCODING_CP932 // Shift JIS
};



/**
    @class wxFont

    A font is an object which determines the appearance of text.
    Fonts are used for drawing text to a device context, and setting the appearance
    of a window's text.

    This class uses @ref overview_refcount "reference counting and copy-on-write"
    internally so that assignments between two instances of this class are very
    cheap. You can therefore use actual objects instead of pointers without
    efficiency problems. If an instance of this class is changed it will create
    its own data internally so that other instances, which previously shared the
    data using the reference counting, are not affected.

    You can retrieve the current system font settings with wxSystemSettings.

    @library{wxcore}
    @category{gdi}

    @stdobjects
    ::wxNullFont, ::wxNORMAL_FONT, ::wxSMALL_FONT, ::wxITALIC_FONT, ::wxSWISS_FONT

    @see @ref overview_font, wxDC::SetFont, wxDC::DrawText,
         wxDC::GetTextExtent, wxFontDialog, wxSystemSettings
*/
class wxFont : public wxGDIObject
{
public:
    //@{
    /**
        Default ctor.
    */
    wxFont();

    /**
        Copy constructor, uses @ref overview_refcount "reference counting".
    */
    wxFont(const wxFont& font);

    /**
        Creates a font object with the specified attributes.

        @param pointSize
            Size in points.
        @param family
            Font family, a generic way of referring to fonts without specifying actual
            facename. One of the ::wxFontFamily enumeration values.
        @param style
            One of wxFONTSTYLE_NORMAL, wxFONTSTYLE_SLANT and wxFONTSTYLE_ITALIC.
        @param weight
            Font weight, sometimes also referred to as font boldness. One of
            the ::wxFontWeight enumeration values.
        @param underline
            The value can be @true or @false.
            At present this has an effect on Windows and Motif 2.x only.
        @param faceName
            An optional string specifying the actual typeface to be used.
            If it is an empty string, a default typeface will be chosen based on the family.
        @param encoding
            An encoding which may be one of the enumeration values of ::wxFontEncoding.
            Briefly these can be summed up as:
            <TABLE>
                <TR><TD>wxFONTENCODING_SYSTEM</TD><TD>Default system encoding.</TD></TR>
                <TR><TD>wxFONTENCODING_DEFAULT</TD><TD>
                    Default application encoding: this
                    is the encoding set by calls to
                    SetDefaultEncoding and which may be set to,
                    say, KOI8 to create all fonts by default with KOI8 encoding. Initially, the
                    default application encoding is the same as default system encoding.</TD></TR>
                <TR><TD>wxFONTENCODING_ISO8859_1...15</TD><TD>ISO8859 encodings.</TD></TR>
                <TR><TD>wxFONTENCODING_KOI8</TD><TD>The standard Russian encoding for Internet.</TD></TR>
                <TR><TD>wxFONTENCODING_CP1250...1252</TD><TD>Windows encodings similar to ISO8859 (but not identical).</TD></TR>
            </TABLE>
            If the specified encoding isn't available, no font is created
            (see also font encoding overview).

        @remarks If the desired font does not exist, the closest match will be
                 chosen. Under Windows, only scalable TrueType fonts are used.
    */
    wxFont(int pointSize, wxFontFamily family, int style,
           wxFontWeight weight,
           const bool underline = false,
           const wxString& faceName = "",
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    /**
        Creates a font object with the specified attributes.

        @param pixelSize
            Size in pixels: this is directly supported only under MSW currently
            where this constructor can be used directly, under other platforms a
            font with the closest size to the given one is found using binary search
            and the static New method must be used.
        @param family
            Font family, a generic way of referring to fonts without specifying actual
            facename. One of ::wxFontFamily enumeration values.
        @param style
            One of wxFONTSTYLE_NORMAL, wxFONTSTYLE_SLANT and wxFONTSTYLE_ITALIC.
        @param weight
            Font weight, sometimes also referred to as font boldness.
            One of the ::wxFontWeight enumeration values.
        @param underline
            The value can be @true or @false.
            At present this has an effect on Windows and Motif 2.x only.
        @param faceName
            An optional string specifying the actual typeface to be used.
            If it is an empty string, a default typeface will be chosen based on the family.
        @param encoding
            An encoding which may be one of the enumeration values of ::wxFontEncoding.
            Briefly these can be summed up as:
            <TABLE>
                <TR><TD>wxFONTENCODING_SYSTEM</TD><TD>Default system encoding.</TD></TR>
                <TR><TD>wxFONTENCODING_DEFAULT</TD><TD>
                    Default application encoding: this
                    is the encoding set by calls to
                    SetDefaultEncoding and which may be set to,
                    say, KOI8 to create all fonts by default with KOI8 encoding. Initially, the
                    default application encoding is the same as default system encoding.</TD></TR>
                <TR><TD>wxFONTENCODING_ISO8859_1...15</TD><TD>ISO8859 encodings.</TD></TR>
                <TR><TD>wxFONTENCODING_KOI8</TD><TD>The standard Russian encoding for Internet.</TD></TR>
                <TR><TD>wxFONTENCODING_CP1250...1252</TD><TD>Windows encodings similar to ISO8859 (but not identical).</TD></TR>
            </TABLE>
            If the specified encoding isn't available, no font is created
            (see also font encoding overview).

        @remarks If the desired font does not exist, the closest match will be
                 chosen. Under Windows, only scalable TrueType fonts are used.
    */
    wxFont(const wxSize& pixelSize, wxFontFamily family,
           int style, wxFontWeight weight,
           const bool underline = false,
           const wxString& faceName = "",
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT);
    //@}

    /**
        Destructor.

        See @ref overview_refcount_destruct "reference-counted object destruction"
        for more info.

        @remarks Although all remaining fonts are deleted when the application
                 exits, the application should try to clean up all fonts
                 itself. This is because wxWidgets cannot know if a
                 pointer to the font object is stored in an application
                 data structure, and there is a risk of double deletion.
    */
    virtual ~wxFont();

    /**
        Returns the current application's default encoding.

        @see @ref overview_fontencoding, SetDefaultEncoding()
    */
    static wxFontEncoding GetDefaultEncoding();

    /**
        Returns the typeface name associated with the font, or the empty string if
        there is no typeface information.

        @see SetFaceName()
    */
    virtual wxString GetFaceName() const;

    /**
        Gets the font family. See SetFamily() for a list of valid
        family identifiers.

        @see SetFamily()
    */
    virtual wxFontFamily GetFamily() const;

    /**
        Returns the platform-dependent string completely describing this font.
        Returned string is always non-empty.

        Note that the returned string is not meant to be shown or edited by the user: a
        typical use of this function is for serializing in string-form a wxFont object.

        @see SetNativeFontInfo(),GetNativeFontInfoUserDesc()
    */
    wxString GetNativeFontInfoDesc() const;

    /**
        Returns a user-friendly string for this font object.
        Returned string is always non-empty.

        Some examples of the formats of returned strings (which are platform-dependent)
        are in SetNativeFontInfoUserDesc().

        @see GetNativeFontInfoDesc()
    */
    wxString GetNativeFontInfoUserDesc() const;

    /**
        Gets the point size.

        @see SetPointSize()
    */
    virtual int GetPointSize() const;

    /**
        Gets the font style. See wxFontStyle for a list of valid styles.

        @see SetStyle()
    */
    virtual wxFontStyle GetStyle() const;

    /**
        Returns @true if the font is underlined, @false otherwise.

        @see SetUnderlined()
    */
    virtual bool GetUnderlined() const;

    /**
        Gets the font weight. See wxFontWeight for a list of valid weight identifiers.

        @see SetWeight()
    */
    virtual wxFontWeight GetWeight() const;

    /**
        Returns @true if the font is a fixed width (or monospaced) font,
        @false if it is a proportional one or font is invalid.
    */
    virtual bool IsFixedWidth() const;

    /**
        Returns @true if this object is a valid font, @false otherwise.
    */
    virtual bool IsOk() const;

    //@{
    /**
        These functions take the same parameters as
        @ref wxFont::wxFont "wxFont constructors" and return a new font
        object allocated on the heap.

        Using @c New() is currently the only way to directly create a font with
        the given size in pixels on platforms other than wxMSW.
    */
    static wxFont* New(int pointSize, wxFontFamily family, int style,
                       wxFontWeight weight,
                       const bool underline = false,
                       const wxString& faceName = "",
                       wxFontEncoding encoding = wxFONTENCODING_DEFAULT);
    static wxFont* New(int pointSize, wxFontFamily family,
                       int flags = wxFONTFLAG_DEFAULT,
                       const wxString& faceName = "",
                       wxFontEncoding encoding = wxFONTENCODING_DEFAULT);
    static wxFont* New(const wxSize& pixelSize,
                       wxFontFamily family,
                       int style,
                       wxFontWeight weight,
                       const bool underline = false,
                       const wxString& faceName = "",
                       wxFontEncoding encoding = wxFONTENCODING_DEFAULT);
    static wxFont* New(const wxSize& pixelSize,
                       wxFontFamily family,
                       int flags = wxFONTFLAG_DEFAULT,
                       const wxString& faceName = "",
                       wxFontEncoding encoding = wxFONTENCODING_DEFAULT);
    //@}

    /**
        Sets the default font encoding.

        @see @ref overview_fontencoding, GetDefaultEncoding()
    */
    static void SetDefaultEncoding(wxFontEncoding encoding);

    /**
        Sets the facename for the font.
        Returns @true if the given face name exists; @false otherwise.

        @param faceName
            A valid facename, which should be on the end-user's system.

        @remarks To avoid portability problems, don't rely on a specific face,
                 but specify the font family instead or as well.
                 A suitable font will be found on the end-user's system.
                 If both the family and the facename are specified,
                 wxWidgets will first search for the specific face, and
                 then for a font belonging to the same family.

        @see GetFaceName(), SetFamily()
    */
    virtual bool SetFaceName(const wxString& faceName);

    /**
        Sets the font family.

        @param family
            One of the ::wxFontFamily values.

        @see GetFamily(), SetFaceName()
    */
    void SetFamily(wxFontFamily family);

    /**
        Creates the font corresponding to the given native font description string
        which must have been previously returned by GetNativeFontInfoDesc().

        If the string is invalid, font is unchanged.
        This function is typically used for de-serializing a wxFont object
        previously saved in a string-form.

        @return @true if the creation was successful.

        @see SetNativeFontInfoUserDesc()
    */
    bool SetNativeFontInfo(const wxString& info);

    /**
        Creates the font corresponding to the given native font description string and
        returns @true if the creation was successful.

        Unlike SetNativeFontInfo(), this function accepts strings which are user-friendly.
        Examples of accepted string formats are:

        @beginTable
        @hdr3col{platform, generic syntax, example}
        @row3col{wxGTK2, @c [FACE-NAME] [bold] [oblique|italic] [POINTSIZE], Monospace bold 10}
        @row3col{wxMSW, @c [light|bold] [italic] [FACE-NAME] [POINTSIZE] [ENCODING], Tahoma 10 WINDOWS-1252}
        @endTable

        @todo add an example for wxMac

        For more detailed information about the allowed syntaxes you can look at the
        documentation of the native API used for font-rendering
        (e.g. @c pango_font_description_from_string on GTK).

        @see SetNativeFontInfo()
    */
    bool SetNativeFontInfoUserDesc(const wxString& info);

    /**
        Sets the point size.

        @param pointSize
            Size in points.

        @see GetPointSize()
    */
    virtual void SetPointSize(int pointSize);

    /**
        Sets the font style.

        @param style
            One of the ::wxFontStyle enumeration values.

        @see GetStyle()
    */
    void SetStyle(wxFontStyle style);

    /**
        Sets underlining.

        @param underlined
            @true to underline, @false otherwise.

        @see GetUnderlined()
    */
    virtual void SetUnderlined(bool underlined);

    /**
        Sets the font weight.

        @param weight
            One of the ::wxFontWeight values.

        @see GetWeight()
    */
    void SetWeight(wxFontWeight weight);

    /**
        Inequality operator.

        See @ref overview_refcount_equality "reference-counted object comparison" for
        more info.
    */
    bool operator!=(const wxFont& font) const;

    /**
        Equality operator.

        See @ref overview_refcount_equality "reference-counted object comparison" for
        more info.
    */
    bool operator==(const wxFont& font) const;

    /**
        Assignment operator, using @ref overview_refcount "reference counting".
    */
    wxFont& operator =(const wxFont& font);
};


/**
    An empty wxFont.
*/
wxFont wxNullFont;

/**
    Equivalent to wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).
*/
wxFont wxNORMAL_FONT;

/**
    A font using the wxFONTFAMILY_SWISS family and 2 points smaller than
    ::wxNORMAL_FONT.
*/
wxFont wxSMALL_FONT;

/**
    A font using the wxFONTFAMILY_ROMAN family and wxFONTSTYLE_ITALIC style and
    of the same size of ::wxNORMAL_FONT.
*/
wxFont wxITALIC_FONT;

/**
    A font identic to ::wxNORMAL_FONT except for the family used which is
    wxFONTFAMILY_SWISS.
*/
wxFont wxSWISS_FONT;


/**
    @class wxFontList

    A font list is a list containing all fonts which have been created.
    There is only one instance of this class: ::wxTheFontList.

    Use this object to search for a previously created font of the desired type
    and create it if not already found.

    In some windowing systems, the font may be a scarce resource, so it is best to
    reuse old resources if possible.  When an application finishes, all fonts will
    be deleted and their resources freed, eliminating the possibility of 'memory
    leaks'.

    @library{wxcore}
    @category{gdi}

    @see wxFont
*/
class wxFontList : public wxList
{
public:
    /**
        Constructor. The application should not construct its own font list:
        use the object pointer ::wxTheFontList.
    */
    wxFontList();

    /**
        Finds a font of the given specification, or creates one and adds it to the
        list. See the @ref wxFont "wxFont constructor" for details of the arguments.
    */
    wxFont* FindOrCreateFont(int point_size, int family, int style,
                             int weight,
                             bool underline = false,
                             const wxString& facename = NULL,
                             wxFontEncoding encoding = wxFONTENCODING_DEFAULT);
};


/**
    The global wxFontList instance.
*/
wxFontList* wxTheFontList;


// ============================================================================
// Global functions/macros
// ============================================================================

/** @ingroup group_funcmacro_misc */
//@{

/**
    Converts string to a wxFont best represented by the given string. Returns
    @true on success.

    @see wxToString(const wxFont&)

    @header{wx/font.h}
*/
bool wxFromString(const wxString& string, wxFont* font);

/**
    Converts the given wxFont into a string.

    @see wxFromString(const wxString&, wxFont*)

    @header{wx/font.h}
*/
wxString wxToString(const wxFont& font);

//@}

