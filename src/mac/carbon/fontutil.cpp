///////////////////////////////////////////////////////////////////////////////
// Name:        msw/fontutil.cpp
// Purpose:     font-related helper functions for wxMSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05.11.99
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "fontutil.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
#endif //WX_PRECOMP

#include "wx/fontutil.h"
#include "wx/fontmap.h"
#include "wx/encinfo.h"

#include "wx/tokenzr.h"

#include "ATSUnicode.h"
#include "TextCommon.h"
#include "TextEncodingConverter.h"

#include  "wx/mac/private.h"  // includes mac headers

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNativeEncodingInfo
// ----------------------------------------------------------------------------

// convert to/from the string representation: format is
//      facename[;charset]

bool wxNativeEncodingInfo::FromString(const wxString& s)
{
    wxStringTokenizer tokenizer(s, _T(";"));

    facename = tokenizer.GetNextToken();
    if ( !facename )
        return FALSE;

    wxString tmp = tokenizer.GetNextToken();
    if ( !tmp )
    {
        // default charset (don't use DEFAULT_CHARSET though because of subtle
        // Windows 9x/NT differences in handling it)
        charset = 0;
    }
    else
    {
        if ( wxSscanf(tmp, _T("%u"), &charset) != 1 )
        {
            // should be a number!
            return FALSE;
        }
    }

    return TRUE;
}

wxString wxNativeEncodingInfo::ToString() const
{
    wxString s(facename);
    if ( charset != 0 )
    {
        s << _T(';') << charset;
    }

    return s;
}

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

bool wxGetNativeFontEncoding(wxFontEncoding encoding,
                             wxNativeEncodingInfo *info)
{
    wxCHECK_MSG( info, FALSE, _T("bad pointer in wxGetNativeFontEncoding") );

    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        encoding = wxFont::GetDefaultEncoding();
    }

    info->encoding = encoding ;

    return TRUE;
}

bool wxTestFontEncoding(const wxNativeEncodingInfo& info)
{
    return info.encoding == wxFont::GetDefaultEncoding() ;
}

wxUint32 wxMacGetSystemEncFromFontEnc(wxFontEncoding encoding)
{    	
	TextEncodingBase enc = 0 ;

	switch( encoding)
	{
		case wxFONTENCODING_ISO8859_1 :
    		enc = kTextEncodingISOLatin1 ;
    		break ;
		case wxFONTENCODING_ISO8859_2 :
    		enc = kTextEncodingISOLatin2;
    		break ;
		case wxFONTENCODING_ISO8859_3 :
    		enc = kTextEncodingISOLatin3 ;
    		break ;
		case wxFONTENCODING_ISO8859_4 :
    		enc = kTextEncodingISOLatin4;
    		break ;
		case wxFONTENCODING_ISO8859_5 :
    		enc = kTextEncodingISOLatinCyrillic;
    		break ;
		case wxFONTENCODING_ISO8859_6 :
    		enc = kTextEncodingISOLatinArabic;
    		break ;
		case wxFONTENCODING_ISO8859_7 :
    		enc = kTextEncodingISOLatinGreek;
    		break ;
		case wxFONTENCODING_ISO8859_8 :
    		enc = kTextEncodingISOLatinHebrew;
    		break ;
		case wxFONTENCODING_ISO8859_9 :
    		enc = kTextEncodingISOLatin5;
    		break ;
		case wxFONTENCODING_ISO8859_10 :
    		enc = kTextEncodingISOLatin6;
    		break ;
		case wxFONTENCODING_ISO8859_13 :
    		enc = kTextEncodingISOLatin7;
    		break ;
		case wxFONTENCODING_ISO8859_14 :
    		enc = kTextEncodingISOLatin8;
    		break ;
		case wxFONTENCODING_ISO8859_15 :
    		enc = kTextEncodingISOLatin9;
    		break ;

		case wxFONTENCODING_KOI8 :
    		enc = kTextEncodingKOI8_R;
    		break ;
		case wxFONTENCODING_ALTERNATIVE : // MS-DOS CP866
    		enc = kTextEncodingDOSRussian;
    		break ;
/*
		case wxFONTENCODING_BULGARIAN : 
    		enc = ;
    		break ;
*/	    		
		case wxFONTENCODING_CP437 : 
    		enc =kTextEncodingDOSLatinUS ;
    		break ;
		case wxFONTENCODING_CP850 :
    		enc = kTextEncodingDOSLatin1;
    		break ;
		case wxFONTENCODING_CP852 : 
    		enc = kTextEncodingDOSLatin2;
    		break ;
		case wxFONTENCODING_CP855 :
    		enc = kTextEncodingDOSCyrillic;
    		break ;
		case wxFONTENCODING_CP866 :
    		enc =kTextEncodingDOSRussian ;
    		break ;
		case wxFONTENCODING_CP874 :
    		enc = kTextEncodingDOSThai;
    		break ;
		case wxFONTENCODING_CP932 : 
    		enc = kTextEncodingDOSJapanese;
    		break ;
		case wxFONTENCODING_CP936 : 
    		enc =kTextEncodingDOSChineseSimplif ;
    		break ;
		case wxFONTENCODING_CP949 : 
    		enc = kTextEncodingDOSKorean;
    		break ;
		case wxFONTENCODING_CP950 : 
    		enc = kTextEncodingDOSChineseTrad;
    		break ;
    		
		case wxFONTENCODING_CP1250 : 
    		enc = kTextEncodingWindowsLatin2;
    		break ;
		case wxFONTENCODING_CP1251 : 
    		enc =kTextEncodingWindowsCyrillic ;
    		break ;
		case wxFONTENCODING_CP1252 : 
    		enc =kTextEncodingWindowsLatin1 ;
    		break ;
		case wxFONTENCODING_CP1253 : 
    		enc = kTextEncodingWindowsGreek;
    		break ;
		case wxFONTENCODING_CP1254 : 
    		enc = kTextEncodingWindowsLatin5;
    		break ;
		case wxFONTENCODING_CP1255 : 
    		enc =kTextEncodingWindowsHebrew ;
    		break ;
		case wxFONTENCODING_CP1256 : 
    		enc =kTextEncodingWindowsArabic ;
    		break ;
		case wxFONTENCODING_CP1257 : 
    		enc = kTextEncodingWindowsBalticRim;
    		break ;
    		
		case wxFONTENCODING_UTF7 : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicodeUTF7Format) ;
    		break ;
		case wxFONTENCODING_UTF8 : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicodeUTF8Format) ;
    		break ;
		case wxFONTENCODING_EUC_JP : 
    		enc = kTextEncodingEUC_JP;
    		break ;
		case wxFONTENCODING_UTF16BE : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode16BitFormat) ;
    		break ;
		case wxFONTENCODING_UTF16LE : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode16BitFormat) ;
    		break ;
		case wxFONTENCODING_UTF32BE : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode32BitFormat) ;
    		break ;
		case wxFONTENCODING_UTF32LE : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode32BitFormat) ;
    		break ;

        case wxFONTENCODING_MACROMAN :
            enc = kTextEncodingMacRoman ;
            break ;
        case wxFONTENCODING_MACJAPANESE :
            enc = kTextEncodingMacJapanese ;
            break ;
        case wxFONTENCODING_MACCHINESETRAD :
            enc = kTextEncodingMacChineseTrad ;
            break ;
        case wxFONTENCODING_MACKOREAN :
            enc = kTextEncodingMacKorean ;
            break ;
        case wxFONTENCODING_MACARABIC :
            enc = kTextEncodingMacArabic ;
            break ;
        case wxFONTENCODING_MACHEBREW :
            enc = kTextEncodingMacHebrew ;
            break ;
        case wxFONTENCODING_MACGREEK :
            enc = kTextEncodingMacGreek ;
            break ;
        case wxFONTENCODING_MACCYRILLIC :
            enc = kTextEncodingMacCyrillic ;
            break ;
        case wxFONTENCODING_MACDEVANAGARI :
            enc = kTextEncodingMacDevanagari ;
            break ;
        case wxFONTENCODING_MACGURMUKHI :
            enc = kTextEncodingMacGurmukhi ;
            break ;
        case wxFONTENCODING_MACGUJARATI :
            enc = kTextEncodingMacGujarati ;
            break ;
        case wxFONTENCODING_MACORIYA :
            enc = kTextEncodingMacOriya ;
            break ;
        case wxFONTENCODING_MACBENGALI :
            enc = kTextEncodingMacBengali ;
            break ;
        case wxFONTENCODING_MACTAMIL :
            enc = kTextEncodingMacTamil ;
            break ;
        case wxFONTENCODING_MACTELUGU :
            enc = kTextEncodingMacTelugu ;
            break ;
        case wxFONTENCODING_MACKANNADA :
            enc = kTextEncodingMacKannada ;
            break ;
        case wxFONTENCODING_MACMALAJALAM :
            enc = kTextEncodingMacMalayalam ;
            break ;
        case wxFONTENCODING_MACSINHALESE :
            enc = kTextEncodingMacSinhalese ;
            break ;
        case wxFONTENCODING_MACBURMESE :
            enc = kTextEncodingMacBurmese ;
            break ;
        case wxFONTENCODING_MACKHMER :
            enc = kTextEncodingMacKhmer ;
            break ;
        case wxFONTENCODING_MACTHAI :
            enc = kTextEncodingMacThai ;
            break ;
        case wxFONTENCODING_MACLAOTIAN :
            enc = kTextEncodingMacLaotian ;
            break ;
        case wxFONTENCODING_MACGEORGIAN :
            enc = kTextEncodingMacGeorgian ;
            break ;
        case wxFONTENCODING_MACARMENIAN :
            enc = kTextEncodingMacArmenian ;
            break ;
        case wxFONTENCODING_MACCHINESESIMP :
            enc = kTextEncodingMacChineseSimp ;
            break ;
        case wxFONTENCODING_MACTIBETAN :
            enc = kTextEncodingMacTibetan ;
            break ;
        case wxFONTENCODING_MACMONGOLIAN :
            enc = kTextEncodingMacMongolian ;
            break ;
        case wxFONTENCODING_MACETHIOPIC :
            enc = kTextEncodingMacEthiopic ;
            break ;
        case wxFONTENCODING_MACCENTRALEUR :
            enc = kTextEncodingMacCentralEurRoman ;
            break ;
        case wxFONTENCODING_MACVIATNAMESE :
            enc = kTextEncodingMacVietnamese ;
            break ;
        case wxFONTENCODING_MACARABICEXT :
            enc = kTextEncodingMacExtArabic ;
            break ;
        case wxFONTENCODING_MACSYMBOL :
            enc = kTextEncodingMacSymbol ;
            break ;
        case wxFONTENCODING_MACDINGBATS :
            enc = kTextEncodingMacDingbats ;
            break ;
        case wxFONTENCODING_MACTURKISH :
            enc = kTextEncodingMacTurkish ;
            break ;
        case wxFONTENCODING_MACCROATIAN :
            enc = kTextEncodingMacCroatian ;
            break ;
        case wxFONTENCODING_MACICELANDIC :
            enc = kTextEncodingMacIcelandic ;
            break ;
        case wxFONTENCODING_MACROMANIAN :
            enc = kTextEncodingMacRomanian ;
            break ;
        case wxFONTENCODING_MACCELTIC :
            enc = kTextEncodingMacCeltic ;
            break ;
        case wxFONTENCODING_MACGAELIC :
            enc = kTextEncodingMacGaelic ;
            break ;
        case wxFONTENCODING_MACKEYBOARD :
            enc = kTextEncodingMacKeyboardGlyphs ;
            break ;       
	} ;
	return enc ;
}

wxFontEncoding wxMacGetFontEncFromSystemEnc(wxUint32 encoding)
{    	
	wxFontEncoding enc = wxFONTENCODING_DEFAULT ;

	switch( encoding)
	{
		case kTextEncodingISOLatin1  :
    		enc = wxFONTENCODING_ISO8859_1 ;
    		break ;
		case kTextEncodingISOLatin2 :
    		enc = wxFONTENCODING_ISO8859_2;
    		break ;
		case kTextEncodingISOLatin3 :
    		enc = wxFONTENCODING_ISO8859_3 ;
    		break ;
		case kTextEncodingISOLatin4 :
    		enc = wxFONTENCODING_ISO8859_4;
    		break ;
		case kTextEncodingISOLatinCyrillic :
    		enc = wxFONTENCODING_ISO8859_5;
    		break ;
		case kTextEncodingISOLatinArabic :
    		enc = wxFONTENCODING_ISO8859_6;
    		break ;
		case kTextEncodingISOLatinGreek :
    		enc = wxFONTENCODING_ISO8859_7;
    		break ;
		case kTextEncodingISOLatinHebrew :
    		enc = wxFONTENCODING_ISO8859_8;
    		break ;
		case kTextEncodingISOLatin5 :
    		enc = wxFONTENCODING_ISO8859_9;
    		break ;
		case kTextEncodingISOLatin6 :
    		enc = wxFONTENCODING_ISO8859_10;
    		break ;
		case kTextEncodingISOLatin7 :
    		enc = wxFONTENCODING_ISO8859_13;
    		break ;
		case kTextEncodingISOLatin8 :
    		enc = wxFONTENCODING_ISO8859_14;
    		break ;
		case kTextEncodingISOLatin9 :
    		enc =wxFONTENCODING_ISO8859_15 ;
    		break ;

		case kTextEncodingKOI8_R :
    		enc = wxFONTENCODING_KOI8;
    		break ;
/*
		case  : 
    		enc = wxFONTENCODING_BULGARIAN;
    		break ;
*/	    		
		casekTextEncodingDOSLatinUS : 
    		enc = wxFONTENCODING_CP437;
    		break ;
		case kTextEncodingDOSLatin1 :
    		enc = wxFONTENCODING_CP850;
    		break ;
		case kTextEncodingDOSLatin2 : 
    		enc =wxFONTENCODING_CP852 ;
    		break ;
		case kTextEncodingDOSCyrillic :
    		enc = wxFONTENCODING_CP855;
    		break ;
		case kTextEncodingDOSRussian :
    		enc = wxFONTENCODING_CP866;
    		break ;
		case kTextEncodingDOSThai :
    		enc =wxFONTENCODING_CP874 ;
    		break ;
		case kTextEncodingDOSJapanese : 
    		enc = wxFONTENCODING_CP932;
    		break ;
		case kTextEncodingDOSChineseSimplif : 
    		enc = wxFONTENCODING_CP936;
    		break ;
		case kTextEncodingDOSKorean : 
    		enc = wxFONTENCODING_CP949;
    		break ;
		case kTextEncodingDOSChineseTrad : 
    		enc = wxFONTENCODING_CP950;
    		break ;
    		
		case kTextEncodingWindowsLatin2 : 
    		enc = wxFONTENCODING_CP1250;
    		break ;
		case kTextEncodingWindowsCyrillic : 
    		enc = wxFONTENCODING_CP1251;
    		break ;
		case kTextEncodingWindowsLatin1 : 
    		enc = wxFONTENCODING_CP1252;
    		break ;
		case kTextEncodingWindowsGreek : 
    		enc = wxFONTENCODING_CP1253;
    		break ;
		case kTextEncodingWindowsLatin5 : 
    		enc = wxFONTENCODING_CP1254;
    		break ;
		case kTextEncodingWindowsHebrew : 
    		enc = wxFONTENCODING_CP1255;
    		break ;
		case kTextEncodingWindowsArabic : 
    		enc = wxFONTENCODING_CP1256;
    		break ;
		case kTextEncodingWindowsBalticRim : 
    		enc =wxFONTENCODING_CP1257 ;
    		break ;
		case kTextEncodingEUC_JP : 
    		enc = wxFONTENCODING_EUC_JP;
    		break ;
    		/*
		case wxFONTENCODING_UTF7 : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicodeUTF7Format) ;
    		break ;
		case wxFONTENCODING_UTF8 : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicodeUTF8Format) ;
    		break ;
		case wxFONTENCODING_UTF16BE : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode16BitFormat) ;
    		break ;
		case wxFONTENCODING_UTF16LE : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode16BitFormat) ;
    		break ;
		case wxFONTENCODING_UTF32BE : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode32BitFormat) ;
    		break ;
		case wxFONTENCODING_UTF32LE : 
    		enc = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode32BitFormat) ;
    		break ;
        */
        case kTextEncodingMacRoman :
            enc = wxFONTENCODING_MACROMAN ;
            break ;
        case kTextEncodingMacJapanese :
            enc = wxFONTENCODING_MACJAPANESE ;
            break ;
        case kTextEncodingMacChineseTrad :
            enc = wxFONTENCODING_MACCHINESETRAD ;
            break ;
        case kTextEncodingMacKorean :
            enc = wxFONTENCODING_MACKOREAN ;
            break ;
        case kTextEncodingMacArabic :
            enc =wxFONTENCODING_MACARABIC ;
            break ;
        case kTextEncodingMacHebrew :
            enc = wxFONTENCODING_MACHEBREW ;
            break ;
        case kTextEncodingMacGreek :
            enc = wxFONTENCODING_MACGREEK ;
            break ;
        case kTextEncodingMacCyrillic :
            enc = wxFONTENCODING_MACCYRILLIC ;
            break ;
        case kTextEncodingMacDevanagari :
            enc = wxFONTENCODING_MACDEVANAGARI ;
            break ;
        case kTextEncodingMacGurmukhi :
            enc = wxFONTENCODING_MACGURMUKHI ;
            break ;
        case kTextEncodingMacGujarati :
            enc = wxFONTENCODING_MACGUJARATI ;
            break ;
        case kTextEncodingMacOriya :
            enc =wxFONTENCODING_MACORIYA ;
            break ;
        case kTextEncodingMacBengali :
            enc =wxFONTENCODING_MACBENGALI ;
            break ;
        case kTextEncodingMacTamil :
            enc = wxFONTENCODING_MACTAMIL ;
            break ;
        case kTextEncodingMacTelugu :
            enc = wxFONTENCODING_MACTELUGU ;
            break ;
        case kTextEncodingMacKannada :
            enc = wxFONTENCODING_MACKANNADA ;
            break ;
        case kTextEncodingMacMalayalam :
            enc = wxFONTENCODING_MACMALAJALAM ;
            break ;
        case kTextEncodingMacSinhalese :
            enc = wxFONTENCODING_MACSINHALESE ;
            break ;
        case kTextEncodingMacBurmese :
            enc = wxFONTENCODING_MACBURMESE ;
            break ;
        case kTextEncodingMacKhmer :
            enc = wxFONTENCODING_MACKHMER ;
            break ;
        case kTextEncodingMacThai :
            enc = wxFONTENCODING_MACTHAI ;
            break ;
        case kTextEncodingMacLaotian :
            enc = wxFONTENCODING_MACLAOTIAN ;
            break ;
        case kTextEncodingMacGeorgian :
            enc = wxFONTENCODING_MACGEORGIAN ;
            break ;
        case kTextEncodingMacArmenian :
            enc = wxFONTENCODING_MACARMENIAN ;
            break ;
        case kTextEncodingMacChineseSimp :
            enc = wxFONTENCODING_MACCHINESESIMP ;
            break ;
        case kTextEncodingMacTibetan :
            enc = wxFONTENCODING_MACTIBETAN ;
            break ;
        case kTextEncodingMacMongolian :
            enc = wxFONTENCODING_MACMONGOLIAN ;
            break ;
        case kTextEncodingMacEthiopic :
            enc = wxFONTENCODING_MACETHIOPIC ;
            break ;
        case kTextEncodingMacCentralEurRoman:
            enc = wxFONTENCODING_MACCENTRALEUR  ;
            break ;
        case kTextEncodingMacVietnamese:
            enc = wxFONTENCODING_MACVIATNAMESE  ;
            break ;
        case kTextEncodingMacExtArabic :
            enc = wxFONTENCODING_MACARABICEXT ;
            break ;
        case kTextEncodingMacSymbol :
            enc = wxFONTENCODING_MACSYMBOL ;
            break ;
        case kTextEncodingMacDingbats :
            enc = wxFONTENCODING_MACDINGBATS ;
            break ;
        case kTextEncodingMacTurkish :
            enc = wxFONTENCODING_MACTURKISH ;
            break ;
        case kTextEncodingMacCroatian :
            enc = wxFONTENCODING_MACCROATIAN ;
            break ;
        case kTextEncodingMacIcelandic :
            enc = wxFONTENCODING_MACICELANDIC ;
            break ;
        case kTextEncodingMacRomanian :
            enc = wxFONTENCODING_MACROMANIAN ;
            break ;
        case kTextEncodingMacCeltic :
            enc = wxFONTENCODING_MACCELTIC ;
            break ;
        case kTextEncodingMacGaelic :
            enc = wxFONTENCODING_MACGAELIC ;
            break ;
        case kTextEncodingMacKeyboardGlyphs :
            enc = wxFONTENCODING_MACKEYBOARD ;
            break ;       
	} ;
	return enc ;
}

