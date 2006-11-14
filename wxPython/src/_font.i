/////////////////////////////////////////////////////////////////////////////
// Name:        _font.i
// Purpose:     SWIG interface file for wxFont and related classes
//
// Author:      Robin Dunn
//
// Created:     1-Apr-2002
// RCS-ID:      $Id$
// Copyright:   (c) 2002 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
#include <wx/fontutil.h>
#include <wx/fontmap.h>
#include <wx/fontenum.h>
%}

//---------------------------------------------------------------------------
%newgroup


enum wxFontFamily
{
    wxFONTFAMILY_DEFAULT = wxDEFAULT,
    wxFONTFAMILY_DECORATIVE = wxDECORATIVE,
    wxFONTFAMILY_ROMAN = wxROMAN,
    wxFONTFAMILY_SCRIPT = wxSCRIPT,
    wxFONTFAMILY_SWISS = wxSWISS,
    wxFONTFAMILY_MODERN = wxMODERN,
    wxFONTFAMILY_TELETYPE = wxTELETYPE,
    wxFONTFAMILY_MAX,
    wxFONTFAMILY_UNKNOWN = wxFONTFAMILY_MAX
};

// font styles
enum wxFontStyle
{
    wxFONTSTYLE_NORMAL = wxNORMAL,
    wxFONTSTYLE_ITALIC = wxITALIC,
    wxFONTSTYLE_SLANT = wxSLANT,
    wxFONTSTYLE_MAX
};

// font weights
enum wxFontWeight
{
    wxFONTWEIGHT_NORMAL = wxNORMAL,
    wxFONTWEIGHT_LIGHT = wxLIGHT,
    wxFONTWEIGHT_BOLD = wxBOLD,
    wxFONTWEIGHT_MAX
};


// the font flag bits for the new font ctor accepting one combined flags word
enum
{
    // no special flags: font with default weight/slant/anti-aliasing
    wxFONTFLAG_DEFAULT,

    // slant flags (default: no slant)
    wxFONTFLAG_ITALIC,
    wxFONTFLAG_SLANT,

    // weight flags (default: medium)
    wxFONTFLAG_LIGHT,
    wxFONTFLAG_BOLD,

    // anti-aliasing flag: force on or off (default: the current system default)
    wxFONTFLAG_ANTIALIASED,
    wxFONTFLAG_NOT_ANTIALIASED,

    // underlined/strikethrough flags (default: no lines)
    wxFONTFLAG_UNDERLINED,
    wxFONTFLAG_STRIKETHROUGH,

    // the mask of all currently used flags
    wxFONTFLAG_MASK
};


// font encodings
enum wxFontEncoding
{
    wxFONTENCODING_SYSTEM = -1,     // system default
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

    wxFONTENCODING_MACMIN = wxFONTENCODING_MACROMAN ,
    wxFONTENCODING_MACMAX = wxFONTENCODING_MACKEYBOARD ,

    wxFONTENCODING_MAX,             // highest enumerated encoding value


    // aliases for endian-dependent UTF encodings
    wxFONTENCODING_UTF16,  // native UTF-16
    wxFONTENCODING_UTF32,  // native UTF-32

    // alias for the native Unicode encoding on this platform
    // (this is used by wxEncodingConverter and wxUTFFile only for now)
    wxFONTENCODING_UNICODE = wxFONTENCODING_UTF16,

    // alternative names for Far Eastern encodings
        // Chinese
    wxFONTENCODING_GB2312 = wxFONTENCODING_CP936, // Simplified Chinese
    wxFONTENCODING_BIG5 = wxFONTENCODING_CP950,   // Traditional Chinese

        // Japanese (see http://zsigri.tripod.com/fontboard/cjk/jis.html)
    wxFONTENCODING_SHIFT_JIS = wxFONTENCODING_CP932 // Shift JIS
};

//---------------------------------------------------------------------------
%newgroup

// wxNativeFontInfo is platform-specific font representation: this struct
// should be considered as opaque font description only used by the native
// functions, the user code can only get the objects of this type from
// somewhere and pass it somewhere else (possibly save them somewhere using
// ToString() and restore them using FromString())
struct wxNativeFontInfo
{
public:
    wxNativeFontInfo();
    ~wxNativeFontInfo();

    // reset to the default state
    void Init();

    // init with the parameters of the given font
    void InitFromFont(const wxFont& font);

    // accessors and modifiers for the font elements
    int GetPointSize() const;
#ifdef __WXMSW__
    wxSize GetPixelSize() const;
#endif
    wxFontStyle GetStyle() const;
    wxFontWeight GetWeight() const;
    bool GetUnderlined() const;
    wxString GetFaceName() const;
    wxFontFamily GetFamily() const;
    wxFontEncoding GetEncoding() const;

    void SetPointSize(int pointsize);
#ifdef __WXMSW__
    void SetPixelSize(const wxSize& pixelSize);
#endif
    void SetStyle(wxFontStyle style);
    void SetWeight(wxFontWeight weight);
    void SetUnderlined(bool underlined);
    bool SetFaceName(wxString facename);
    void SetFamily(wxFontFamily family);
    void SetEncoding(wxFontEncoding encoding);

// TODO:     
//     // sets the first facename in the given array which is found
//     // to be valid. If no valid facename is given, sets the
//     // first valid facename returned by wxFontEnumerator::GetFacenames().
//     // Does not return a bool since it cannot fail.
//     void SetFaceName(const wxArrayString &facenames);

    
    // it is important to be able to serialize wxNativeFontInfo objects to be
    // able to store them (in config file, for example)
    bool FromString(const wxString& s);
    wxString ToString() const;

    %extend {
        wxString __str__() {
            return self->ToString();
        }
    }

    // we also want to present the native font descriptions to the user in some
    // human-readable form (it is not platform independent neither, but can
    // hopefully be understood by the user)
    bool FromUserString(const wxString& s);
    wxString ToUserString() const;
    
    %property(Encoding, GetEncoding, SetEncoding, doc="See `GetEncoding` and `SetEncoding`");
    %property(FaceName, GetFaceName, SetFaceName, doc="See `GetFaceName` and `SetFaceName`");
    %property(Family, GetFamily, SetFamily, doc="See `GetFamily` and `SetFamily`");
    %property(PointSize, GetPointSize, SetPointSize, doc="See `GetPointSize` and `SetPointSize`");
    %property(Style, GetStyle, SetStyle, doc="See `GetStyle` and `SetStyle`");
    %property(Underlined, GetUnderlined, SetUnderlined, doc="See `GetUnderlined` and `SetUnderlined`");
    %property(Weight, GetWeight, SetWeight, doc="See `GetWeight` and `SetWeight`");
};




struct wxNativeEncodingInfo
{
    wxString facename;          // may be empty meaning "any"
    wxFontEncoding encoding;    // so that we know what this struct represents

    wxNativeEncodingInfo();
    ~wxNativeEncodingInfo();

    // this struct is saved in config by wxFontMapper, so it should know to
    // serialise itself (implemented in platform-specific code)
    bool FromString(const wxString& s);
    wxString ToString() const;
};


#ifndef __WXMSW__
// translate a wxFontEncoding into native encoding parameter (defined above),
// returning a wxNativeEncodingInfo if an (exact) match could be found, NULL
// otherwise.
%inline %{
    wxNativeEncodingInfo* wxGetNativeFontEncoding(wxFontEncoding encoding) {
        static wxNativeEncodingInfo info;
        if ( wxGetNativeFontEncoding(encoding, &info) )
            return &info;
        else
            return NULL;
    }
%}

// test for the existence of the font described by this facename/encoding,
// return True if such font(s) exist, False otherwise
bool wxTestFontEncoding(const wxNativeEncodingInfo& info);

#else

%inline %{
    wxNativeEncodingInfo* wxGetNativeFontEncoding(wxFontEncoding encoding)
        { wxPyRaiseNotImplemented(); return NULL; }

    bool wxTestFontEncoding(const wxNativeEncodingInfo& info)
        { wxPyRaiseNotImplemented(); return false; }
%}
#endif

//---------------------------------------------------------------------------
%newgroup

// wxFontMapper manages user-definable correspondence between logical font
// names and the fonts present on the machine.
//
// The default implementations of all functions will ask the user if they are
// not capable of finding the answer themselves and store the answer in a
// config file (configurable via SetConfigXXX functions). This behaviour may
// be disabled by giving the value of False to "interactive" parameter.
// However, the functions will always consult the config file to allow the
// user-defined values override the default logic and there is no way to
// disable this - which shouldn't be ever needed because if "interactive" was
// never True, the config file is never created anyhow.
//
// This is a singleton class, font mapper objects can only be accessed using
// wxFontMapper::Get().

class  wxFontMapper
{
public:
    wxFontMapper();
    ~wxFontMapper();

    // return instance of the wxFontMapper singleton
    static wxFontMapper *Get();
    // set the singleton to 'mapper' instance and return previous one
    static wxFontMapper *Set(wxFontMapper *mapper);


    // returns the encoding for the given charset (in the form of RFC 2046) or
    // wxFONTENCODING_SYSTEM if couldn't decode it
    //
    // interactive parameter is ignored in the base class, we behave as if it
    // were always False
    virtual wxFontEncoding CharsetToEncoding(const wxString& charset,
                                             bool interactive = true);


    // get the number of font encodings we know about
    static size_t GetSupportedEncodingsCount();

    // get the n-th supported encoding
    static wxFontEncoding GetEncoding(size_t n);

    // return canonical name of this encoding (this is a short string,
    // GetEncodingDescription() returns a longer one)
    static wxString GetEncodingName(wxFontEncoding encoding);

//     // return a list of all names of this encoding (see GetEncodingName)
//     static const wxChar** GetAllEncodingNames(wxFontEncoding encoding);
    
    // return user-readable string describing the given encoding
    //
    // NB: hard-coded now, but might change later (read it from config?)
    static wxString GetEncodingDescription(wxFontEncoding encoding);

    // find the encoding corresponding to the given name, inverse of
    // GetEncodingName() and less general than CharsetToEncoding()
    //
    // returns wxFONTENCODING_MAX if the name is not a supported encoding
    static wxFontEncoding GetEncodingFromName(const wxString& name);


    // set the root config path to use (should be an absolute path)
    void SetConfigPath(const wxString& prefix);

    // return default config path
    static const wxString GetDefaultConfigPath();



    // Find an alternative for the given encoding (which is supposed to not be
    // available on this system). If successful, returns the encoding otherwise
    // returns None.
    %extend {
        PyObject* GetAltForEncoding(wxFontEncoding encoding,
                                    const wxString& facename = wxPyEmptyString,
                                    bool interactive = true) {
            wxFontEncoding alt_enc;
            if (self->GetAltForEncoding(encoding, &alt_enc, facename, interactive))
                return PyInt_FromLong(alt_enc);
            else {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }
    }


    // checks whether given encoding is available in given face or not.
    // If no facename is given (default), return true if it's available in any
    // facename at all.
    bool IsEncodingAvailable(wxFontEncoding encoding,
                             const wxString& facename = wxPyEmptyString);

    // the parent window for modal dialogs
    void SetDialogParent(wxWindow *parent);

    // the title for the dialogs (note that default is quite reasonable)
    void SetDialogTitle(const wxString& title);


     %property(AltForEncoding, GetAltForEncoding, doc="See `GetAltForEncoding`");
};



//---------------------------------------------------------------------------
%newgroup


MustHaveApp(wxFont);
MustHaveApp(wxFont::GetDefaultEncoding);
MustHaveApp(wxFont::SetDefaultEncoding);

DocStr(wxFont,
"A font is an object which determines the appearance of text. Fonts are
used for drawing text to a device context, and setting the appearance
of a window's text.

You can retrieve the current system font settings with `wx.SystemSettings`.", "

The possible values for the family parameter of wx.Font constructor are:

    ========================  =============================
    wx.FONTFAMILY_DEFAULT     Chooses a default font.
    wx.FONTFAMILY_DECORATIVE  A decorative font. 
    wx.FONTFAMILY_ROMAN       A formal, serif font.
    wx.FONTFAMILY_SCRIPT      A handwriting font. 
    wx.FONTFAMILY_SWISS       A sans-serif font. 
    wx.FONTFAMILY_MODERN      Usually a fixed pitch font.    
    wx.FONTFAMILY_TELETYPE    A teletype font. 
    ========================  =============================

The possible values for the weight parameter are:

    ====================  ==
    wx.FONTWEIGHT_NORMAL
    wx.FONTWEIGHT_LIGHT
    wx.FONTWEIGHT_BOLD
    ====================  ==

The known font encodings are:

    ===========================       ====================================
    wx.FONTENCODING_SYSTEM            system default
    wx.FONTENCODING_DEFAULT           current default encoding
    wx.FONTENCODING_ISO8859_1         West European (Latin1)
    wx.FONTENCODING_ISO8859_2         Central and East European (Latin2)
    wx.FONTENCODING_ISO8859_3         Esperanto (Latin3)
    wx.FONTENCODING_ISO8859_4         Baltic (old) (Latin4)
    wx.FONTENCODING_ISO8859_5         Cyrillic
    wx.FONTENCODING_ISO8859_6         Arabic
    wx.FONTENCODING_ISO8859_7         Greek
    wx.FONTENCODING_ISO8859_8         Hebrew
    wx.FONTENCODING_ISO8859_9         Turkish (Latin5)
    wx.FONTENCODING_ISO8859_10        Variation of Latin4 (Latin6)
    wx.FONTENCODING_ISO8859_11        Thai
    wx.FONTENCODING_ISO8859_12        doesn't exist currently, but put it
                                      here anyhow to make all ISO8859
                                      consecutive numbers
    wx.FONTENCODING_ISO8859_13        Baltic (Latin7)
    wx.FONTENCODING_ISO8859_14        Latin8
    wx.FONTENCODING_ISO8859_15        Latin9 (a.k.a. Latin0, includes euro)
    wx.FONTENCODING_KOI8              Cyrillic charset
    wx.FONTENCODING_ALTERNATIVE       same as MS-DOS CP866
    wx.FONTENCODING_BULGARIAN         used under Linux in Bulgaria
    wx.FONTENCODING_CP437             original MS-DOS codepage
    wx.FONTENCODING_CP850             CP437 merged with Latin1
    wx.FONTENCODING_CP852             CP437 merged with Latin2
    wx.FONTENCODING_CP855             another cyrillic encoding
    wx.FONTENCODING_CP866             and another one
    wx.FONTENCODING_CP874             WinThai
    wx.FONTENCODING_CP1250            WinLatin2
    wx.FONTENCODING_CP1251            WinCyrillic
    wx.FONTENCODING_CP1252            WinLatin1
    wx.FONTENCODING_CP1253            WinGreek (8859-7)
    wx.FONTENCODING_CP1254            WinTurkish
    wx.FONTENCODING_CP1255            WinHebrew
    wx.FONTENCODING_CP1256            WinArabic
    wx.FONTENCODING_CP1257            WinBaltic (same as Latin 7)
    wx.FONTENCODING_UTF7              UTF-7 Unicode encoding
    wx.FONTENCODING_UTF8              UTF-8 Unicode encoding
    ===========================       ====================================

");

class wxFont : public wxGDIObject {
public:
    %pythonPrepend wxFont   "if kwargs.has_key('faceName'): kwargs['face'] = kwargs['faceName'];del kwargs['faceName']"

    DocCtorStr(
        wxFont( int pointSize, int family, int style, int weight,
                bool underline=false, const wxString& face = wxPyEmptyString,
                wxFontEncoding encoding = wxFONTENCODING_DEFAULT),
        "Creates a font object with the specified attributes.

    :param pointSize:  The size of the font in points.

    :param family: Font family.  A generic way of referring to fonts
        without specifying actual facename.  It can be One of 
        the ``wx.FONTFAMILY_xxx`` constants.

    :param style: One of the ``wx.FONTSTYLE_xxx`` constants.

    :param weight: Font weight, sometimes also referred to as font
        boldness. One of the ``wx.FONTWEIGHT_xxx`` constants.

    :param underline: The value can be ``True`` or ``False`` and
        indicates whether the font will include an underline.  This
        may not be supported on all platforms.

    :param face: An optional string specifying the actual typeface to
        be used. If it is an empty string, a default typeface will be
        chosen based on the family.

    :param encoding: An encoding which may be one of the
        ``wx.FONTENCODING_xxx`` constants.  If the specified encoding isn't
        available, no font is created.

:see: `wx.FFont`, `wx.FontFromPixelSize`, `wx.FFontFromPixelSize`,
    `wx.FontFromNativeInfoString`, `wx.FontFromNativeInfo`
", "");

    ~wxFont();

    %RenameDocCtor(
        FontFromNativeInfo,
        "Construct a `wx.Font` from a `wx.NativeFontInfo` object.", "",
        wxFont(const wxNativeFontInfo& info));
    
    %extend {
        %RenameDocCtor(
            FontFromNativeInfoString,
            "Construct a `wx.Font` from the string representation of a
`wx.NativeFontInfo` object.", "", 
            wxFont(const wxString& info))
            {
                wxNativeFontInfo nfi;
                nfi.FromString(info);
                return new wxFont(nfi);
            }
    }

    
    %extend {        
        %RenameDocCtor(
            FFont,
            "A bit of a simpler way to create a `wx.Font` using flags instead of
individual attribute settings.  The value of flags can be a
combination of the following:

    ============================  ==
    wx.FONTFLAG_DEFAULT
    wx.FONTFLAG_ITALIC
    wx.FONTFLAG_SLANT
    wx.FONTFLAG_LIGHT
    wx.FONTFLAG_BOLD
    wx.FONTFLAG_ANTIALIASED
    wx.FONTFLAG_NOT_ANTIALIASED
    wx.FONTFLAG_UNDERLINED
    wx.FONTFLAG_STRIKETHROUGH
    ============================  ==

:see: `wx.Font.__init__`", "",

            wxFont(int pointSize,
                   wxFontFamily family,
                   int flags = wxFONTFLAG_DEFAULT,
                   const wxString& face = wxPyEmptyString,
                   wxFontEncoding encoding = wxFONTENCODING_DEFAULT))
            {
                return wxFont::New(pointSize, family, flags, face, encoding);
            }


        // There is a real ctor for this on wxMSW, but not the others, so just
        // use the factory function in all cases.
        
        %RenameDocCtor(
            FontFromPixelSize,
            "Creates a font using a size in pixels rather than points.  If there is
platform API support for this then it is used, otherwise a font with
the closest size is found using a binary search.

:see: `wx.Font.__init__`", "", 
            wxFont(const wxSize& pixelSize,
                   int family,
                   int style,
                   int weight,
                   bool underlined = false,
                   const wxString& face = wxEmptyString,
                   wxFontEncoding encoding = wxFONTENCODING_DEFAULT))
            {
                return wxFontBase::New(pixelSize, family,
                                       style, weight, underlined,
                                       face, encoding);
            }

        %RenameDocCtor(
            FFontFromPixelSize,
            "Creates a font using a size in pixels rather than points.  If there is
platform API support for this then it is used, otherwise a font with
the closest size is found using a binary search.

:see: `wx.Font.__init__`, `wx.FFont`", "",
            wxFont(const wxSize& pixelSize,
                   wxFontFamily family,
                   int flags = wxFONTFLAG_DEFAULT,
                   const wxString& face = wxEmptyString,
                   wxFontEncoding encoding = wxFONTENCODING_DEFAULT))
            {
                return wxFontBase::New(pixelSize, family, flags, face, encoding);
            }
    }

    

    // was the font successfully created?
    DocDeclStr(
        bool , IsOk() const,
        "Returns ``True`` if this font was successfully created.", "");    
    %pythoncode { Ok = IsOk }
    %pythoncode { def __nonzero__(self): return self.IsOk() }

    
    // comparison
    %extend {
        bool __eq__(const wxFont* other) { return other ? (*self == *other) : false; }
        bool __ne__(const wxFont* other) { return other ? (*self != *other) : true;  }
    }

    
    DocDeclStr(
        virtual int , GetPointSize() const,
        "Gets the point size.", "");
    
    DocDeclStr(
        virtual wxSize , GetPixelSize() const,
        "Returns the size in pixels if the font was constructed with a pixel
size.", "");
    
    DocDeclStr(
        virtual bool , IsUsingSizeInPixels() const,
        "Returns ``True`` if the font is using a pixelSize.", "");
    
    
    DocDeclStr(
        virtual int , GetFamily() const,
        "Gets the font family. ", "");
    
    DocDeclStr(
        virtual int , GetStyle() const,
        "Gets the font style.", "");
    
    DocDeclStr(
        virtual int , GetWeight() const,
        "Gets the font weight. ", "");
    
    DocDeclStr(
        virtual bool , GetUnderlined() const,
        "Returns ``True`` if the font is underlined, ``False`` otherwise.", "");
    
    DocDeclStr(
        virtual wxString , GetFaceName() const,
        "Returns the typeface name associated with the font, or the empty
string if there is no typeface information.", "");
    
    DocDeclStr(
        virtual wxFontEncoding , GetEncoding() const,
        "Get the font's encoding.", "");
    
    DocDeclStr(
        virtual const wxNativeFontInfo *, GetNativeFontInfo() const,
        "Constructs a `wx.NativeFontInfo` object from this font.", "");
    

    DocDeclStr(
        virtual bool , IsFixedWidth() const,
        "Returns true if the font is a fixed width (or monospaced) font, false
if it is a proportional one or font is invalid.", "");
    

    DocDeclStr(
        wxString , GetNativeFontInfoDesc() const,
        "Returns the platform-dependent string completely describing this font
or an empty string if the font isn't valid.", "");
    
    DocDeclStr(
        wxString , GetNativeFontInfoUserDesc() const,
        "Returns a human readable version of `GetNativeFontInfoDesc`.", "");
    

    // change the font characteristics
    DocDeclStr(
        virtual void , SetPointSize( int pointSize ),
        "Sets the point size.", "");
    
    DocDeclStr(
        virtual void , SetPixelSize( const wxSize& pixelSize ),
        "Sets the size in pixels rather than points.  If there is platform API
support for this then it is used, otherwise a font with the closest
size is found using a binary search.", "");
    
    DocDeclStr(
        virtual void , SetFamily( int family ),
        "Sets the font family.", "");
    
    DocDeclStr(
        virtual void , SetStyle( int style ),
        "Sets the font style.", "");
    
    DocDeclStr(
        virtual void , SetWeight( int weight ),
        "Sets the font weight.", "");
    
    DocDeclStr(
        virtual bool , SetFaceName( const wxString& faceName ),
        "Sets the facename for the font.  The facename, which should be a valid
font installed on the end-user's system.

To avoid portability problems, don't rely on a specific face, but
specify the font family instead or as well. A suitable font will be
found on the end-user's system. If both the family and the facename
are specified, wxWidgets will first search for the specific face, and
then for a font belonging to the same family.", "");
    
    DocDeclStr(
        virtual void , SetUnderlined( bool underlined ),
        "Sets underlining.", "");
    
    DocDeclStr(
        virtual void , SetEncoding(wxFontEncoding encoding),
        "Set the font encoding.", "");
    
    DocDeclStr(
        void , SetNativeFontInfo(const wxNativeFontInfo& info),
        "Set the font's attributes from a `wx.NativeFontInfo` object.", "");
    

    DocDeclStrName(
        bool , SetNativeFontInfo(const wxString& info),
        "Set the font's attributes from string representation of a
`wx.NativeFontInfo` object.", "",
        SetNativeFontInfoFromString);
    
    DocDeclStr(
        bool , SetNativeFontInfoUserDesc(const wxString& info),
        "Set the font's attributes from a string formerly returned from
`GetNativeFontInfoDesc`.", "");
    

    DocDeclStr(
        wxString , GetFamilyString() const,
        "Returns a string representation of the font family.", "");
    
    DocDeclStr(
        wxString , GetStyleString() const,
        "Returns a string representation of the font style.", "");
    
    DocDeclStr(
        wxString , GetWeightString() const,
        "Return a string representation of the font weight.", "");
    

    virtual void SetNoAntiAliasing( bool no = true );
    virtual bool GetNoAntiAliasing() const;

    // the default encoding is used for creating all fonts with default
    // encoding parameter
    DocDeclStr(
        static wxFontEncoding , GetDefaultEncoding(),
        "Returns the encoding used for all fonts created with an encoding of
``wx.FONTENCODING_DEFAULT``.", "");
    
    DocDeclStr(
        static void , SetDefaultEncoding(wxFontEncoding encoding),
        "Sets the default font encoding.", "");

    %property(Encoding, GetEncoding, SetEncoding, doc="See `GetEncoding` and `SetEncoding`");
    %property(FaceName, GetFaceName, SetFaceName, doc="See `GetFaceName` and `SetFaceName`");
    %property(Family, GetFamily, SetFamily, doc="See `GetFamily` and `SetFamily`");
    %property(FamilyString, GetFamilyString, doc="See `GetFamilyString`");
    %property(NativeFontInfo, GetNativeFontInfo, SetNativeFontInfo, doc="See `GetNativeFontInfo` and `SetNativeFontInfo`");
    %property(NativeFontInfoDesc, GetNativeFontInfoDesc, doc="See `GetNativeFontInfoDesc`");
    %property(NativeFontInfoUserDesc, GetNativeFontInfoUserDesc, SetNativeFontInfoUserDesc, doc="See `GetNativeFontInfoUserDesc` and `SetNativeFontInfoUserDesc`");
    %property(NoAntiAliasing, GetNoAntiAliasing, SetNoAntiAliasing, doc="See `GetNoAntiAliasing` and `SetNoAntiAliasing`");
    %property(PixelSize, GetPixelSize, SetPixelSize, doc="See `GetPixelSize` and `SetPixelSize`");
    %property(PointSize, GetPointSize, SetPointSize, doc="See `GetPointSize` and `SetPointSize`");
    %property(Style, GetStyle, SetStyle, doc="See `GetStyle` and `SetStyle`");
    %property(StyleString, GetStyleString, doc="See `GetStyleString`");
    %property(Underlined, GetUnderlined, SetUnderlined, doc="See `GetUnderlined` and `SetUnderlined`");
    %property(Weight, GetWeight, SetWeight, doc="See `GetWeight` and `SetWeight`");
    %property(WeightString, GetWeightString, doc="See `GetWeightString`");
    
};

%pythoncode { Font2 = wx._deprecated(FFont, "Use `wx.FFont` instead.") }

//---------------------------------------------------------------------------
%newgroup

// wxFontEnumerator
%{
class wxPyFontEnumerator : public wxFontEnumerator {
public:
    wxPyFontEnumerator() {}
    ~wxPyFontEnumerator() {}

    DEC_PYCALLBACK_BOOL_STRING(OnFacename);
    DEC_PYCALLBACK_BOOL_STRINGSTRING(OnFontEncoding);

    PYPRIVATE;
};

IMP_PYCALLBACK_BOOL_STRING(wxPyFontEnumerator, wxFontEnumerator, OnFacename);
IMP_PYCALLBACK_BOOL_STRINGSTRING(wxPyFontEnumerator, wxFontEnumerator, OnFontEncoding);

%}

MustHaveApp(wxPyFontEnumerator);

%rename(FontEnumerator) wxPyFontEnumerator;
class wxPyFontEnumerator {
public:
    %pythonAppend wxPyFontEnumerator setCallbackInfo(FontEnumerator)

    wxPyFontEnumerator();
    ~wxPyFontEnumerator();
    void _setCallbackInfo(PyObject* self, PyObject* _class, int incref=0);

    bool EnumerateFacenames(
        wxFontEncoding encoding = wxFONTENCODING_SYSTEM, // all
        bool fixedWidthOnly = false);

    bool EnumerateEncodings(const wxString& facename = wxPyEmptyString);

    %extend {
        static PyObject* GetEncodings() {
            PyObject* ret;
            wxArrayString arr = wxFontEnumerator::GetEncodings();
            wxPyBlock_t blocked = wxPyBeginBlockThreads();            
            ret = wxArrayString2PyList_helper(arr);
            wxPyEndBlockThreads(blocked);
            return ret;
        }

        static PyObject* GetFacenames() {
            PyObject* ret;
            wxArrayString arr = wxFontEnumerator::GetFacenames();
            wxPyBlock_t blocked = wxPyBeginBlockThreads();            
            ret = wxArrayString2PyList_helper(arr);
            wxPyEndBlockThreads(blocked);
            return ret;
        }
    }

    DocDeclStr(
        static bool , IsValidFacename(const wxString &str),
        "Convenience function that returns true if the given face name exist in
the user's system", "");
    
};



%init %{
    wxPyPtrTypeMap_Add("wxFontEnumerator", "wxPyFontEnumerator");
%}

//---------------------------------------------------------------------------
