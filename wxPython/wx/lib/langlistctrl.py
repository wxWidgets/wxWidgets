#-----------------------------------------------------------------------------
# Name:        languagectrls.py
# Purpose:     
#
# Author:      Riaan Booysen
#
# Created:     2006
# RCS-ID:      $Id$
# Copyright:   (c) 2006 Riaan Booysen
# License:     wxPython
#-----------------------------------------------------------------------------
""" ListCtrl and functions to display languages and the flags of their countries
"""
import wx

from wx.lib.art import flagart

langIdCountryMap = {
    # generated from wx.Locale info and locale.windows_locale
    wx.LANGUAGE_AFRIKAANS: 'ZA',
    wx.LANGUAGE_ALBANIAN: 'AL',
    wx.LANGUAGE_ARABIC_ALGERIA: 'DZ',
    wx.LANGUAGE_ARABIC_BAHRAIN: 'BH',
    wx.LANGUAGE_ARABIC_EGYPT: 'EG',
    wx.LANGUAGE_ARABIC_IRAQ: 'IQ',
    wx.LANGUAGE_ARABIC_JORDAN: 'JO',
    wx.LANGUAGE_ARABIC_KUWAIT: 'KW',
    wx.LANGUAGE_ARABIC_LEBANON: 'LB',
    wx.LANGUAGE_ARABIC_LIBYA: 'LY',
    wx.LANGUAGE_ARABIC_MOROCCO: 'MA',
    wx.LANGUAGE_ARABIC_OMAN: 'OM',
    wx.LANGUAGE_ARABIC_QATAR: 'QA',
    wx.LANGUAGE_ARABIC_SAUDI_ARABIA: 'SA',
    wx.LANGUAGE_ARABIC_SUDAN: 'SD',
    wx.LANGUAGE_ARABIC_SYRIA: 'SY',
    wx.LANGUAGE_ARABIC_TUNISIA: 'TN',
    wx.LANGUAGE_ARABIC_UAE: 'AE',
    wx.LANGUAGE_ARABIC_YEMEN: 'YE',
    wx.LANGUAGE_ARMENIAN: 'AM',
    wx.LANGUAGE_AZERI: 'AZ',
    wx.LANGUAGE_AZERI_CYRILLIC: 'AZ',
    wx.LANGUAGE_AZERI_LATIN: 'AZ',
    wx.LANGUAGE_BASQUE: 'ES',
    wx.LANGUAGE_BELARUSIAN: 'BY',
    wx.LANGUAGE_BENGALI: 'IN',
    wx.LANGUAGE_BRETON: 'FR',
    wx.LANGUAGE_BULGARIAN: 'BG',
    wx.LANGUAGE_CATALAN: 'ES',
    wx.LANGUAGE_CHINESE: 'TW',
    wx.LANGUAGE_CHINESE_HONGKONG: 'HK',
    wx.LANGUAGE_CHINESE_MACAU: 'MO',
    wx.LANGUAGE_CHINESE_SIMPLIFIED: 'CN',
    wx.LANGUAGE_CHINESE_SINGAPORE: 'SG',
    wx.LANGUAGE_CHINESE_TAIWAN: 'TW',
    wx.LANGUAGE_CHINESE_TRADITIONAL: 'TW',
    wx.LANGUAGE_CROATIAN: 'HR',
    wx.LANGUAGE_CZECH: 'CZ',
    wx.LANGUAGE_DANISH: 'DK',
#    wx.LANGUAGE_DEFAULT: 'ZA',
    wx.LANGUAGE_DUTCH: 'NL',
    wx.LANGUAGE_DUTCH_BELGIAN: 'BE',
    wx.LANGUAGE_ENGLISH: 'GB',
    wx.LANGUAGE_ENGLISH_AUSTRALIA: 'AU',
    wx.LANGUAGE_ENGLISH_BELIZE: 'BZ',
    wx.LANGUAGE_ENGLISH_BOTSWANA: 'BW',
    wx.LANGUAGE_ENGLISH_CANADA: 'CA',
    wx.LANGUAGE_ENGLISH_CARIBBEAN: 'CB',
    wx.LANGUAGE_ENGLISH_DENMARK: 'DK',
    wx.LANGUAGE_ENGLISH_EIRE: 'IE',
    wx.LANGUAGE_ENGLISH_JAMAICA: 'JM',
    wx.LANGUAGE_ENGLISH_NEW_ZEALAND: 'NZ',
    wx.LANGUAGE_ENGLISH_PHILIPPINES: 'PH',
    wx.LANGUAGE_ENGLISH_SOUTH_AFRICA: 'ZA',
    wx.LANGUAGE_ENGLISH_TRINIDAD: 'TT',
    wx.LANGUAGE_ENGLISH_UK: 'GB',
    wx.LANGUAGE_ENGLISH_US: 'US',
    wx.LANGUAGE_ENGLISH_ZIMBABWE: 'ZW',
    wx.LANGUAGE_ESTONIAN: 'EE',
    wx.LANGUAGE_FAEROESE: 'FO',
    wx.LANGUAGE_FARSI: 'IR',
    wx.LANGUAGE_FINNISH: 'FI',
    wx.LANGUAGE_FRENCH: 'FR',
    wx.LANGUAGE_FRENCH_BELGIAN: 'BE',
    wx.LANGUAGE_FRENCH_CANADIAN: 'CA',
    wx.LANGUAGE_FRENCH_LUXEMBOURG: 'LU',
    wx.LANGUAGE_FRENCH_MONACO: 'MC',
    wx.LANGUAGE_FRENCH_SWISS: 'CH',
    wx.LANGUAGE_FRISIAN: 'NL',
    wx.LANGUAGE_GALICIAN: 'ES',
    wx.LANGUAGE_GEORGIAN: 'GE',
    wx.LANGUAGE_GERMAN: 'DE',
    wx.LANGUAGE_GERMAN_AUSTRIAN: 'AT',
    wx.LANGUAGE_GERMAN_BELGIUM: 'BE',
    wx.LANGUAGE_GERMAN_LIECHTENSTEIN: 'LI',
    wx.LANGUAGE_GERMAN_LUXEMBOURG: 'LU',
    wx.LANGUAGE_GERMAN_SWISS: 'CH',
    wx.LANGUAGE_GREEK: 'GR',
    wx.LANGUAGE_GREENLANDIC: 'GL',
    wx.LANGUAGE_GUJARATI: 'IN',
    wx.LANGUAGE_HEBREW: 'IL',
    wx.LANGUAGE_HINDI: 'IN',
    wx.LANGUAGE_HUNGARIAN: 'HU',
    wx.LANGUAGE_ICELANDIC: 'IS',
    wx.LANGUAGE_INDONESIAN: 'ID',
    wx.LANGUAGE_INUKTITUT: 'CA',
    wx.LANGUAGE_IRISH: 'IE',
    wx.LANGUAGE_ITALIAN: 'IT',
    wx.LANGUAGE_ITALIAN_SWISS: 'CH',
    wx.LANGUAGE_JAPANESE: 'JP',
    wx.LANGUAGE_KANNADA: 'IN',
    wx.LANGUAGE_KASHMIRI_INDIA: 'IN',
    wx.LANGUAGE_KAZAKH: 'KZ',
    wx.LANGUAGE_KERNEWEK: 'GB',
    wx.LANGUAGE_KIRGHIZ: 'KG',
    wx.LANGUAGE_KOREAN: 'KR',
    wx.LANGUAGE_LATVIAN: 'LV',
    wx.LANGUAGE_LITHUANIAN: 'LT',
    wx.LANGUAGE_MACEDONIAN: 'MK',
    wx.LANGUAGE_MALAY: 'MY',
    wx.LANGUAGE_MALAYALAM: 'IN',
    wx.LANGUAGE_MALAY_BRUNEI_DARUSSALAM: 'BN',
    wx.LANGUAGE_MALAY_MALAYSIA: 'MY',
    wx.LANGUAGE_MALTESE: 'MT',
    wx.LANGUAGE_MAORI: 'NZ',
    wx.LANGUAGE_MARATHI: 'IN',
    wx.LANGUAGE_MONGOLIAN: 'MN',
    wx.LANGUAGE_NEPALI: 'NP',
    wx.LANGUAGE_NEPALI_INDIA: 'IN',
    wx.LANGUAGE_NORWEGIAN_BOKMAL: 'NO',
    wx.LANGUAGE_NORWEGIAN_NYNORSK: 'NO',
    wx.LANGUAGE_OCCITAN: 'FR',
    wx.LANGUAGE_ORIYA: 'IN',
    wx.LANGUAGE_PASHTO: 'AF',
    wx.LANGUAGE_POLISH: 'PL',
    wx.LANGUAGE_PORTUGUESE: 'PT',
    wx.LANGUAGE_PORTUGUESE_BRAZILIAN: 'BR',
    wx.LANGUAGE_PUNJABI: 'IN',
    wx.LANGUAGE_RHAETO_ROMANCE: 'CH',
    wx.LANGUAGE_ROMANIAN: 'RO',
    wx.LANGUAGE_RUSSIAN: 'RU',
    wx.LANGUAGE_RUSSIAN_UKRAINE: 'UA',
    wx.LANGUAGE_SANSKRIT: 'IN',
    wx.LANGUAGE_SERBIAN_CYRILLIC: 'YU',
    wx.LANGUAGE_SERBIAN_LATIN: 'YU',
    wx.LANGUAGE_SETSWANA: 'ZA',
    wx.LANGUAGE_SLOVAK: 'SK',
    wx.LANGUAGE_SLOVENIAN: 'SI',
    wx.LANGUAGE_SPANISH: 'ES',
    wx.LANGUAGE_SPANISH_ARGENTINA: 'AR',
    wx.LANGUAGE_SPANISH_BOLIVIA: 'BO',
    wx.LANGUAGE_SPANISH_CHILE: 'CL',
    wx.LANGUAGE_SPANISH_COLOMBIA: 'CO',
    wx.LANGUAGE_SPANISH_COSTA_RICA: 'CR',
    wx.LANGUAGE_SPANISH_DOMINICAN_REPUBLIC: 'DO',
    wx.LANGUAGE_SPANISH_ECUADOR: 'EC',
    wx.LANGUAGE_SPANISH_EL_SALVADOR: 'SV',
    wx.LANGUAGE_SPANISH_GUATEMALA: 'GT',
    wx.LANGUAGE_SPANISH_HONDURAS: 'HN',
    wx.LANGUAGE_SPANISH_MEXICAN: 'MX',
    wx.LANGUAGE_SPANISH_MODERN: 'ES',
    wx.LANGUAGE_SPANISH_NICARAGUA: 'NI',
    wx.LANGUAGE_SPANISH_PANAMA: 'PA',
    wx.LANGUAGE_SPANISH_PARAGUAY: 'PY',
    wx.LANGUAGE_SPANISH_PERU: 'PE',
    wx.LANGUAGE_SPANISH_PUERTO_RICO: 'PR',
    wx.LANGUAGE_SPANISH_URUGUAY: 'UY',
    wx.LANGUAGE_SPANISH_US: 'US',
    wx.LANGUAGE_SPANISH_VENEZUELA: 'VE',
    wx.LANGUAGE_SWAHILI: 'KE',
    wx.LANGUAGE_SWEDISH: 'SE',
    wx.LANGUAGE_SWEDISH_FINLAND: 'FI',
    wx.LANGUAGE_TAGALOG: 'PH',
    wx.LANGUAGE_TAMIL: 'IN',
    wx.LANGUAGE_TATAR: 'RU',
    wx.LANGUAGE_TELUGU: 'IN',
    wx.LANGUAGE_THAI: 'TH',
    wx.LANGUAGE_TURKISH: 'TR',
    wx.LANGUAGE_UKRAINIAN: 'UA',
    wx.LANGUAGE_URDU: 'PK',
    wx.LANGUAGE_URDU_INDIA: 'IN',
    wx.LANGUAGE_URDU_PAKISTAN: 'PK',
    wx.LANGUAGE_UZBEK: 'UZ',
    wx.LANGUAGE_UZBEK_CYRILLIC: 'UZ',
    wx.LANGUAGE_UZBEK_LATIN: 'UZ',
    wx.LANGUAGE_VIETNAMESE: 'VN',
    wx.LANGUAGE_WELSH: 'GB',
    wx.LANGUAGE_XHOSA: 'ZA',
    wx.LANGUAGE_ZULU: 'ZA',
    # manually defined language/country mapping
    wx.LANGUAGE_ABKHAZIAN: 'GE',
    wx.LANGUAGE_AFAR: 'ET',
    wx.LANGUAGE_AMHARIC: 'ET',
    wx.LANGUAGE_ASSAMESE: 'IN',
    wx.LANGUAGE_AYMARA: 'BO',
    wx.LANGUAGE_ARABIC: 'SA', 
    wx.LANGUAGE_BASHKIR: 'RU',
    wx.LANGUAGE_BHUTANI: 'BT',
    wx.LANGUAGE_BIHARI: 'IN',
    wx.LANGUAGE_BISLAMA: 'VU',
    wx.LANGUAGE_BURMESE: 'MM',
    wx.LANGUAGE_CAMBODIAN: 'KH',
    wx.LANGUAGE_CORSICAN: 'FR',
    wx.LANGUAGE_ESPERANTO: 'ESPERANTO',
    wx.LANGUAGE_FIJI: 'FJ',
    wx.LANGUAGE_GUARANI: 'PY',
    wx.LANGUAGE_HAUSA: 'NG',
    wx.LANGUAGE_INTERLINGUA: 'US', 
    wx.LANGUAGE_INTERLINGUE: 'US',
    wx.LANGUAGE_INUPIAK: 'US',
    wx.LANGUAGE_JAVANESE: 'IN',
    wx.LANGUAGE_KASHMIRI: 'IN',
    wx.LANGUAGE_KINYARWANDA: 'RW',
    wx.LANGUAGE_KIRUNDI: 'BI',
    wx.LANGUAGE_KONKANI: 'IN',
    wx.LANGUAGE_KURDISH: 'IQ',
    wx.LANGUAGE_LAOTHIAN: 'LA',
    wx.LANGUAGE_LATIN: 'VA',
    wx.LANGUAGE_LINGALA: 'CD',
    wx.LANGUAGE_MALAGASY: 'MG',
    wx.LANGUAGE_MANIPURI: 'IN',
    wx.LANGUAGE_MOLDAVIAN: 'MD',
    wx.LANGUAGE_NAURU: 'NR',
    wx.LANGUAGE_OROMO: 'ET',
    wx.LANGUAGE_QUECHUA: 'BO',
    wx.LANGUAGE_SAMOAN: 'WS',
    wx.LANGUAGE_SANGHO: 'CF',
    wx.LANGUAGE_SCOTS_GAELIC: 'GB',
    wx.LANGUAGE_SERBO_CROATIAN: 'HR',
    wx.LANGUAGE_SESOTHO: 'ZA',
    wx.LANGUAGE_SHONA: 'ZW',
    wx.LANGUAGE_SINDHI: 'PK',
    wx.LANGUAGE_SINHALESE: 'IN',
    wx.LANGUAGE_SISWATI: 'SZ',
    wx.LANGUAGE_SOMALI: 'SB',
    wx.LANGUAGE_SUNDANESE: 'SD',
    wx.LANGUAGE_TAJIK: 'TJ',
    wx.LANGUAGE_TIBETAN: 'CN',
    wx.LANGUAGE_TIGRINYA: 'ET',
    wx.LANGUAGE_TONGA: 'TO',
    wx.LANGUAGE_TSONGA: 'MZ',
    wx.LANGUAGE_TURKMEN: 'TM',
    wx.LANGUAGE_TWI: 'GH',
    wx.LANGUAGE_UIGHUR: 'CN',
    wx.LANGUAGE_VOLAPUK: 'VOLAPUK',
    wx.LANGUAGE_WOLOF: 'SN',
    wx.LANGUAGE_YIDDISH: 'IL',
    wx.LANGUAGE_YORUBA: 'NG',
    wx.LANGUAGE_ZHUANG: 'CN',
}

LC_AVAILABLE, LC_ALL, LC_ONLY = 1, 2, 4


# wx.LANGUAGE_SERBIAN gives an error for me
_wxLangIds = [n for n in dir(wx) if n.startswith('LANGUAGE_')]
for _l in ('LANGUAGE_UNKNOWN', 'LANGUAGE_USER_DEFINED', 'LANGUAGE_SERBIAN'):
    if _l in _wxLangIds:
        _wxLangIds.remove(_l)


def CreateLanguagesResourceLists(filter=LC_AVAILABLE, only=()):
    """ Returns a tuple of (bitmaps, language descriptions, language ids) """
    icons = wx.ImageList(16, 11)
    names = []
    langs = []

    langIdNameMap = BuildLanguageCountryMapping()

    wxLangIds = []
    for li in _wxLangIds:
         wxLI = getattr(wx, li)
         try:
             if (filter == LC_ONLY and wxLI in only) or \
                (filter == LC_AVAILABLE and wx.Locale.IsAvailable(wxLI)) or \
                (filter == LC_ALL):
                 wxLangIds.append(wxLI)
         except wx.PyAssertionError: 
             # invalid language assertions
             pass
         except AttributeError:
             # wx 2.6
             wxLangIds.append(wxLI)
                 
    langCodes = [(langIdNameMap[wxLangId], wxLangId) 
                 for wxLangId in wxLangIds 
                 if wxLangId in langIdNameMap]
    
    for lc, wxli in langCodes:
        l, cnt = lc.split('_')
        
        if cnt in flagart.catalog:
            bmp = flagart.catalog[cnt].getBitmap()
        else:
            bmp = flagart.catalog['BLANK'].getBitmap()

        icons.Add(bmp)
        name = wx.Locale.GetLanguageName(wxli)
        if wxli == wx.LANGUAGE_DEFAULT:
            #print cnt, name, lc, wxli
            name = 'Default: '+name
        
        names.append(name)
        langs.append(wxli)
    
    return icons, names, langs


def GetLanguageFlag(lang):
    """ Returns a bitmap of the flag for the country of the language id """
    langIdNameMap = BuildLanguageCountryMapping()
    if lang in langIdNameMap:
        cnt = langIdNameMap[lang].split('_')[1]
        if cnt in flagart.catalog:
            return flagart.catalog[cnt].getBitmap()
    return flagart.catalog['BLANK'].getBitmap()


def BuildLanguageCountryMapping():
    """ Builds a mapping of language ids to LANG_COUNTRY codes """
    res = {}
    for name in _wxLangIds:
        n = 'wx.'+name
        wn = getattr(wx, name)
        
        li = wx.Locale.GetLanguageInfo(wn)
        if li: 
            code = li.CanonicalName

            if wn in langIdCountryMap:
                # override, drop country
                if '_' in code:
                    code = code.split('_')[0]
                code += '_'+langIdCountryMap[wn]

            # map unhandled to blank images
            elif '_' not in code:
                code += '_BLANK'
    
            res[wn] = code
    return res

def GetWxIdentifierForLanguage(lang):
    """ Returns the language id as a string """ 
    for n in dir(wx):
        if n.startswith('LANGUAGE_') and getattr(wx, n) == lang:
            return n
    raise Exception, 'Language %s not found'%lang


#-------------------------------------------------------------------------------

class LanguageListCtrl(wx.ListCtrl):
    """ wx.ListCtrl derived control that displays languages and flags """
    def __init__(self, parent, id=wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.LC_REPORT | wx.LC_NO_HEADER | wx.LC_SINGLE_SEL, 
                 filter=LC_AVAILABLE, only=(), select=None, name='languagelistctrl'):

        wx.ListCtrl.__init__(self, parent, id, pos, size, style, name=name)
        
        self.SetUpFilter(filter, only)
        self.Language = select

    def SetUpFilter(self, filter=LC_AVAILABLE, only=()):
        """ Filters the languages displayed in the control """
        lang = self.GetLanguage()
        
        self.filter, self.only = filter, only
        self.icons, self.choices, self.langs = CreateLanguagesResourceLists(filter, only)
        
        self.AssignImageList(self.icons, wx.IMAGE_LIST_SMALL)
        
        self.ClearAll()
        self.InsertColumn(0, '', width=175)
        for i in range(len(self.choices)):
            self.InsertImageStringItem(i, self.choices[i], i)
        
        self.SetLanguage(lang)

    def GetLanguage(self):
        """ Returns the language id for the currently selected language in the control """
        idx = self.GetFirstSelected()
        if idx != -1:
            return self.langs[idx]
        else:
            None
    
    def SetLanguage(self, lang):
        """ Selects the given language ids item in the control """
        if lang is not None:
            if lang in self.langs:
                idx = self.langs.index(lang)
                self.Select(idx)
                self.Focus(idx)

    Language = property(GetLanguage, SetLanguage, doc="See `GetLanguage` and `SetLanguage`") 
    
#-------------------------------------------------------------------------------


if __name__ == '__main__':
    a = wx.PySimpleApp()
    
    print GetLanguageFlag(wx.LANGUAGE_AFRIKAANS)
    
    f=wx.Frame(None, -1)
    f.p=wx.Panel(f, -1)
    s=wx.BoxSizer(wx.VERTICAL)
    f.p.SetSizer(s)
    try:
        f.lc=LanguageChoice(f.p, pos = (220, 10), size = (200, 25))
        s.Add(f.lc, 0, wx.GROW)
    except:
        pass
    f.llc=LanguageListCtrl(f.p, pos = (10, 10), size = (200, 200), 
          filter=LC_ONLY, 
          only=(wx.LANGUAGE_AFRIKAANS, wx.LANGUAGE_ENGLISH, 
            wx.LANGUAGE_FRENCH, wx.LANGUAGE_GERMAN, wx.LANGUAGE_ITALIAN, 
            wx.LANGUAGE_PORTUGUESE_BRAZILIAN, wx.LANGUAGE_SPANISH), 
          select=wx.LANGUAGE_ENGLISH)
##            filter=LC_ALL)
    s.Add(f.llc, 1, wx.GROW)
    f.Show()
    
    a.MainLoop()
