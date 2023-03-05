#!/usr/bin/env python
#----------------------------------------------------------------------------
# Name:         gen_iface.py
# Purpose:      Generate stc.h and stc.cpp from the info in Scintilla.iface
#
# Author:       Robin Dunn
#
# Created:      5-Sept-2000
# Copyright:    (c) 2000 by Total Control Software
# Licence:      wxWindows licence
#----------------------------------------------------------------------------


import sys, string, re, os
from fileinput import FileInput

sys.dont_write_bytecode = True
from gen_docs import categoriesList,buildDocs

IFACE1        = os.path.abspath('./scintilla/include/Scintilla.iface')
IFACE2        = os.path.abspath('./lexilla/include/LexicalStyles.iface')
HDR_SCN       = os.path.abspath('./scintilla/include/Scintilla.h')
H_TEMPLATE    = os.path.abspath('./stc.h.in')
IH_TEMPLATE   = os.path.abspath('./stc.interface.h.in')
CPP_TEMPLATE  = os.path.abspath('./stc.cpp.in')
H_DEST        = os.path.abspath('../../include/wx/stc/stc.h')
IH_DEST       = os.path.abspath('../../interface/wx/stc/stc.h')
CPP_DEST      = os.path.abspath('./stc.cpp')
if len(sys.argv) > 1 and sys.argv[1] == '--wxpython':
    DOCSTR_DEST   = os.path.abspath('../../../wxPython/src/_stc_gendocs.i')
else:
    DOCSTR_DEST   = None


# Value prefixes to convert
valPrefixes = [('SCI_', ''),
               ('SC_',  ''),
               ('SCN_', None),  # just toss these out...
               ('SCEN_', None),
               ('SC_EFF', None),
               ('SCE_', ''),
               ('SCLEX_', 'LEX_'),
               ('SCK_', 'KEY_'),
               ('SCFIND_', 'FIND_'),
               ('SCWS_', 'WS_'),
               ('SCTD_', 'TD_'),
               ('SCVS_', 'VS_'),
               ('SCMOD_', 'KEYMOD_'),
]

# Message function values that should have a CMD_ constant generated
cmdValues = [ 2011,
              2013,
              (2176, 2180),
              (2300, 2349),
              (2390, 2393),
              (2395, 2396),
              2404,
              (2413, 2416),
              (2426, 2442),
              (2450, 2455),
              2518,
              (2619, 2621),
              (2628, 2629),
              (2652, 2653)
            ]


# Should a function be also generated for the CMDs?
FUNC_FOR_CMD = 1

# Should methods and values be generated for the provisional Scintilla items?
GENERATE_PROVISIONAL_ITEMS = 0

# No wxSTC value will be generated for the following Scintilla values.
notMappedSciValues = set([
    'SC_TECHNOLOGY_DIRECTWRITERETAIN',
    'SC_TECHNOLOGY_DIRECTWRITEDC',
    'INDIC0_MASK',
    'INDIC1_MASK',
    'INDIC2_MASK',
    'INDICS_MASK',
    'SCFIND_CXX11REGEX'
])

# Map some generic typenames to wx types, using return value syntax
retTypeMap = {
    'Accessibility': 'int',
    'Alpha': 'int',
    'AnnotationVisible': 'int',
    'AutomaticFold': 'int',
    'CaretPolicy': 'int',
    'CaretSticky': 'int',
    'CaretStyle': 'int',
    'CaseInsensitiveBehaviour': 'int',
    'CaseVisible': 'int',
    'CharacterSet': 'int',
    'colour': 'wxColour',
    'CursorShape': 'int',
    'DocumentOption': 'int',
    'EdgeVisualStyle': 'int',
    'EndOfLine': 'int',
    'EOLAnnotationVisible': 'int',
    'FindOption': 'int',
    'FoldAction': 'int',
    'FoldDisplayTextStyle': 'int',
    'FoldFlag': 'int',
    'FoldLevel': 'int',
    'FontQuality': 'int',
    'FontWeight': 'int',
    'IdleStyling': 'int',
    'IMEInteraction': 'int',
    'IndentView': 'int',
    'IndicatorStyle': 'int',
    'IndicFlag': 'int',
    'line': 'int',
    'LineCache': 'int',
    'LineEndType': 'int',
    'MarginOption': 'int',
    'MarginType': 'int',
    'ModificationFlags': 'int',
    'MultiAutoComplete': 'int',
    'MultiPaste': 'int',
    'Ordering': 'int',
    'PhasesDraw': 'int',
    'PopUp': 'int',
    'position': 'int',
    'PrintOption': 'int',
    'SelectionMode': 'int',
    'Status': 'int',
    'string': 'wxString',
    'TabDrawMode': 'int',
    'Technology': 'int',
    'TypeProperty': 'int',
    'UndoFlags': 'int',
    'VirtualSpace': 'int',
    'VisiblePolicy': 'int',
    'WhiteSpace': 'int',
    'Wrap': 'int',
    'WrapIndentMode': 'int',
    'WrapVisualFlag': 'int',
    'WrapVisualLocation': 'int',
    }

# Map some generic typenames to wx types, using parameter syntax
paramTypeMap = retTypeMap.copy()
paramTypeMap.update({
    'string': 'const wxString&',
    'colour': 'const wxColour&',
    })

# Map of method info that needs tweaked.  Either the name needs changed, or
# the method definition/implementation.  Tuple items are:
#
#         1. New method name.  None to skip the method, 0 to leave the
#            default name.
#         2. Method definition for the .h file, 0 to leave alone
#         3. Method implementation for the .cpp file, 0 to leave alone.
#
methodOverrideMap = {
    'AddText' : (0,
                 'void %s(const wxString& text);',

                 '''void %s(const wxString& text) {
                    const wxWX2MBbuf buf = wx2stc(text);
                    SendMsg(%s, wx2stclen(text, buf), (sptr_t)(const char*)buf);'''
                 ),

    'AddStyledText' : (0,
                       'void %s(const wxMemoryBuffer& data);',

                       '''void %s(const wxMemoryBuffer& data) {
                          SendMsg(%s, data.GetDataLen(), (sptr_t)data.GetData());'''
                       ),

    'AppendText' : (0,
                 'void %s(const wxString& text) override;',

                 '''void %s(const wxString& text) {
                    const wxWX2MBbuf buf = wx2stc(text);
                    SendMsg(%s, wx2stclen(text, buf), (sptr_t)(const char*)buf);'''
                 ),

    'GetViewWS' : ( 'GetViewWhiteSpace', 0, 0),
    'SetViewWS' : ( 'SetViewWhiteSpace', 0, 0),

    'GetCharAt' :
    ( 0, 0,
      '''int %s(int pos) const {
         return (unsigned char)SendMsg(%s, pos, 0);'''
    ),

    'GetStyleAt' :
    ( 0, 0,
      '''int %s(int pos) const {
         return (unsigned char)SendMsg(%s, pos, 0);'''
    ),

    'GetStyledText' :
    (0,
     'wxMemoryBuffer %s(int startPos, int endPos);',

     '''wxMemoryBuffer %s(int startPos, int endPos) {
        wxMemoryBuffer buf;
        if (endPos < startPos) {
            wxSwap(startPos, endPos);
        }
        int len = endPos - startPos;
        if (!len) return buf;
        Sci_TextRange tr;
        tr.lpstrText = (char*)buf.GetWriteBuf(len*2+1);
        tr.chrg.cpMin = startPos;
        tr.chrg.cpMax = endPos;
        len = SendMsg(%s, 0, (sptr_t)&tr);
        buf.UngetWriteBuf(len);
        return buf;'''
    ),


    'PositionFromPoint' :
    (0,
     'int %s(wxPoint pt) const;',

     '''int %s(wxPoint pt) const {
        return SendMsg(%s, pt.x, pt.y);'''
    ),

    'GetCurLine' :
    (0,
     '#ifdef SWIG\n    wxString %s(int* OUTPUT);\n#else\n    wxString GetCurLine(int* linePos=nullptr);\n#endif',

        '''wxString %s(int* linePos) {
        int len = LineLength(GetCurrentLine());
        if (!len) {
            if (linePos)  *linePos = 0;
            return wxEmptyString;
        }

        wxCharBuffer buf(len);
        int pos = SendMsg(%s, len+1, (sptr_t)buf.data());
        if (linePos)  *linePos = pos;
        return stc2wx(buf);'''
    ),

    'MarkerSetFore' : ('MarkerSetForeground', 0, 0),
    'MarkerSetBack' : ('MarkerSetBackground', 0, 0),
    'MarkerSetBackSelected' : ('MarkerSetBackgroundSelected', 0,0),

    'MarkerSymbolDefined' : ('GetMarkerSymbolDefined', 0, 0),

    'MarkerDefine' :
    (0,
     '''void %s(int markerNumber, int markerSymbol,
                const wxColour& foreground = wxNullColour,
                const wxColour& background = wxNullColour);''',

     '''void %s(int markerNumber, int markerSymbol,
                const wxColour& foreground,
                const wxColour& background) {

                SendMsg(%s, markerNumber, markerSymbol);
                if (foreground.IsOk())
                    MarkerSetForeground(markerNumber, foreground);
                if (background.IsOk())
                    MarkerSetBackground(markerNumber, background);'''
    ),


   'MarkerDefinePixmap' :
    (0,
     '''void %s(int markerNumber, const char* const* xpmData);''',
     '''void %s(int markerNumber, const char* const* xpmData) {
        SendMsg(%s, markerNumber, (sptr_t)xpmData);'''
    ),

    'GetMargins' : ('GetMarginCount', 0, 0),
    'SetMargins' : ('SetMarginCount', 0, 0),
    'GetMarginBackN' : ('GetMarginBackground', 0, 0),
    'SetMarginBackN' : ('SetMarginBackground', 0, 0),
    'SetMarginTypeN' : ('SetMarginType', 0, 0),
    'GetMarginTypeN' : ('GetMarginType', 0, 0),
    'SetMarginWidthN' : ('SetMarginWidth', 0, 0),
    'GetMarginWidthN' : ('GetMarginWidth', 0, 0),
    'SetMarginMaskN' : ('SetMarginMask', 0, 0),
    'GetMarginMaskN' : ('GetMarginMask', 0, 0),
    'SetMarginSensitiveN' : ('SetMarginSensitive', 0, 0),
    'GetMarginSensitiveN' : ('GetMarginSensitive', 0, 0),
    'SetMarginCursorN' : ('SetMarginCursor', 0, 0),
    'GetMarginCursorN' : ('GetMarginCursor', 0, 0),

    'MarginGetText' :
    (0,
    'wxString %s(int line) const;',

     '''wxString %s(int line) const {
         const int msg = %s;
         long len = SendMsg(msg, line, 0);

         wxCharBuffer buf(len);
         SendMsg(msg, line, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'MarginGetStyles' :
     (0,
    'wxString %s(int line) const;',

     '''wxString %s(int line) const {
         const int msg = %s;
         long len = SendMsg(msg, line, 0);

         wxMemoryBuffer mbuf(len+1);
         char* buf = (char*)mbuf.GetWriteBuf(len+1);
         SendMsg(msg, line, (sptr_t)buf);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);'''
    ),

    'SetAdditionalSelFore' : ('SetAdditionalSelForeground', 0, 0),
    'SetAdditionalSelBack' : ('SetAdditionalSelBackground', 0, 0),
    'SetAdditionalCaretFore' : ('SetAdditionalCaretForeground', 0, 0),
    'GetAdditionalCaretFore' : ('GetAdditionalCaretForeground', 0, 0),

    'AnnotationGetText' :
    (0,
    'wxString %s(int line) const;',

     '''wxString %s(int line) const {
         const int msg = %s;
         long len = SendMsg(msg, line, 0);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, line, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'AnnotationGetStyles' :
    (0,
    'wxString %s(int line) const;',

     '''wxString %s(int line) const {
         const int msg = %s;
         long len = SendMsg(msg, line, 0);

         wxMemoryBuffer mbuf(len+1);
         char* buf = (char*)mbuf.GetWriteBuf(len+1);
         SendMsg(msg, line, (sptr_t)buf);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);'''
    ),

    'StyleGetFore' : ('StyleGetForeground', 0, 0),
    'StyleGetBack' : ('StyleGetBackground', 0, 0),
    'StyleSetFore' : ('StyleSetForeground', 0, 0),
    'StyleSetBack' : ('StyleSetBackground', 0, 0),
    'SetSelFore' : ('SetSelForeground', 0, 0),
    'SetSelBack' : ('SetSelBackground', 0, 0),
    'SetCaretFore' : ('SetCaretForeground', 0, 0),

    'StyleGetFont' :
    ('StyleGetFaceName',
     'wxString %s(int style);',
      '''wxString %s(int style) {
         const int msg = %s;
         long len = SendMsg(msg, style, 0);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, style, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'StyleSetFont' : ('StyleSetFaceName', 0, 0),

    'StyleSetCharacterSet' : (0, 0,
    '''void %s(int style, int characterSet) {
        wxFontEncoding encoding;

        // Translate the Scintilla characterSet to a wxFontEncoding
        switch (characterSet) {
            default:
            case wxSTC_CHARSET_ANSI:
            case wxSTC_CHARSET_DEFAULT:
                encoding = wxFONTENCODING_DEFAULT;
                break;

            case wxSTC_CHARSET_BALTIC:
                encoding = wxFONTENCODING_ISO8859_13;
                break;

            case wxSTC_CHARSET_CHINESEBIG5:
                encoding = wxFONTENCODING_CP950;
                break;

            case wxSTC_CHARSET_EASTEUROPE:
                encoding = wxFONTENCODING_ISO8859_2;
                break;

            case wxSTC_CHARSET_GB2312:
                encoding = wxFONTENCODING_CP936;
                break;

            case wxSTC_CHARSET_GREEK:
                encoding = wxFONTENCODING_ISO8859_7;
                break;

            case wxSTC_CHARSET_HANGUL:
                encoding = wxFONTENCODING_CP949;
                break;

            case wxSTC_CHARSET_MAC:
                encoding = wxFONTENCODING_DEFAULT;
                break;

            case wxSTC_CHARSET_OEM:
                encoding = wxFONTENCODING_DEFAULT;
                break;

            case wxSTC_CHARSET_RUSSIAN:
                encoding = wxFONTENCODING_KOI8;
                break;

            case wxSTC_CHARSET_SHIFTJIS:
                encoding = wxFONTENCODING_CP932;
                break;

            case wxSTC_CHARSET_SYMBOL:
                encoding = wxFONTENCODING_DEFAULT;
                break;

            case wxSTC_CHARSET_TURKISH:
                encoding = wxFONTENCODING_ISO8859_9;
                break;

            case wxSTC_CHARSET_JOHAB:
                encoding = wxFONTENCODING_DEFAULT;
                break;

            case wxSTC_CHARSET_HEBREW:
                encoding = wxFONTENCODING_ISO8859_8;
                break;

            case wxSTC_CHARSET_ARABIC:
                encoding = wxFONTENCODING_ISO8859_6;
                break;

            case wxSTC_CHARSET_VIETNAMESE:
                encoding = wxFONTENCODING_DEFAULT;
                break;

            case wxSTC_CHARSET_THAI:
                encoding = wxFONTENCODING_ISO8859_11;
                break;

            case wxSTC_CHARSET_CYRILLIC:
                encoding = wxFONTENCODING_ISO8859_5;
                break;

            case wxSTC_CHARSET_8859_15:
                encoding = wxFONTENCODING_ISO8859_15;
                break;
        }

        // We just have Scintilla track the wxFontEncoding for us.  It gets used
        // in Font::Create in PlatWX.cpp.  We add one to the value so that the
        // effective wxFONENCODING_DEFAULT == SC_SHARSET_DEFAULT and so when
        // Scintilla internally uses SC_CHARSET_DEFAULT we will translate it back
        // to wxFONENCODING_DEFAULT in Font::Create.
        SendMsg(%s, style, encoding+1);'''
    ),

    'AssignCmdKey' :
    ('CmdKeyAssign',
     'void %s(int key, int modifiers, int cmd);',

     '''void %s(int key, int modifiers, int cmd) {
         SendMsg(%s, MAKELONG(key, modifiers), cmd);'''
    ),


    'ClearCmdKey' :
    ('CmdKeyClear',
     'void %s(int key, int modifiers);',

     '''void %s(int key, int modifiers) {
         SendMsg(%s, MAKELONG(key, modifiers));'''
    ),

    'ClearAllCmdKeys' : ('CmdKeyClearAll', 0, 0),

    'StartStyling' :
    (0,
     'void %s(int start);',

     '''void %s(int start) {
        SendMsg(%s, start, 0);'''
    ),

    'SetStylingEx' :
    ('SetStyleBytes',
     'void %s(int length, char* styleBytes);',

     '''void %s(int length, char* styleBytes) {
        SendMsg(%s, length, (sptr_t)styleBytes);'''
    ),


    'IndicSetAlpha' : ('IndicatorSetAlpha', 0, 0),
    'IndicGetAlpha' : ('IndicatorGetAlpha', 0, 0),
    'IndicSetOutlineAlpha' : ('IndicatorSetOutlineAlpha', 0, 0),
    'IndicGetOutlineAlpha' : ('IndicatorGetOutlineAlpha', 0, 0),
    'IndicSetStyle' : ('IndicatorSetStyle', 0, 0),
    'IndicGetStyle' : ('IndicatorGetStyle', 0, 0),
    'IndicSetFore' : ('IndicatorSetForeground', 0, 0),
    'IndicGetFore' : ('IndicatorGetForeground', 0, 0),
    'IndicSetUnder': ('IndicatorSetUnder', 0, 0),
    'IndicGetUnder': ('IndicatorGetUnder', 0, 0),
    'IndicSetHoverStyle': ('IndicatorSetHoverStyle', 0, 0),
    'IndicGetHoverStyle': ('IndicatorGetHoverStyle', 0, 0),
    'IndicSetHoverFore': ('IndicatorSetHoverForeground', 0, 0),
    'IndicGetHoverFore': ('IndicatorGetHoverForeground', 0, 0),
    'IndicSetFlags': ('IndicatorSetFlags', 0, 0),
    'IndicGetFlags': ('IndicatorGetFlags', 0, 0),

    'SetWhitespaceFore' : ('SetWhitespaceForeground', 0, 0),
    'SetWhitespaceBack' : ('SetWhitespaceBackground', 0, 0),

    'AutoCShow' : ('AutoCompShow', 0, 0),
    'AutoCCancel' : ('AutoCompCancel', 0, 0),
    'AutoCActive' : ('AutoCompActive', 0, 0),
    'AutoCPosStart' : ('AutoCompPosStart', 0, 0),
    'AutoCComplete' : ('AutoCompComplete', 0, 0),
    'AutoCStops' : ('AutoCompStops', 0, 0),
    'AutoCSetSeparator' : ('AutoCompSetSeparator', 0, 0),
    'AutoCGetSeparator' : ('AutoCompGetSeparator', 0, 0),
    'AutoCSelect' : ('AutoCompSelect', 0, 0),
    'AutoCSetCancelAtStart' : ('AutoCompSetCancelAtStart', 0, 0),
    'AutoCGetCancelAtStart' : ('AutoCompGetCancelAtStart', 0, 0),
    'AutoCSetFillUps' : ('AutoCompSetFillUps', 0, 0),
    'AutoCSetChooseSingle' : ('AutoCompSetChooseSingle', 0, 0),
    'AutoCGetChooseSingle' : ('AutoCompGetChooseSingle', 0, 0),
    'AutoCSetIgnoreCase' : ('AutoCompSetIgnoreCase', 0, 0),
    'AutoCGetIgnoreCase' : ('AutoCompGetIgnoreCase', 0, 0),
    'AutoCSetAutoHide' : ('AutoCompSetAutoHide', 0, 0),
    'AutoCGetAutoHide' : ('AutoCompGetAutoHide', 0, 0),
    'AutoCSetDropRestOfWord' : ('AutoCompSetDropRestOfWord', 0,0),
    'AutoCGetDropRestOfWord' : ('AutoCompGetDropRestOfWord', 0,0),
    'AutoCGetTypeSeparator' : ('AutoCompGetTypeSeparator', 0, 0),
    'AutoCSetTypeSeparator' : ('AutoCompSetTypeSeparator', 0, 0),
    'AutoCGetCurrent'       : ('AutoCompGetCurrent', 0, 0),

    'AutoCGetCurrentText' :
    ('AutoCompGetCurrentText',
    'wxString %s() const;',

     '''wxString %s() const {
         const int msg = %s;
         long len = SendMsg(msg, 0, 0);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, 0, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'AutoCSetMaxWidth'      : ('AutoCompSetMaxWidth', 0, 0),
    'AutoCGetMaxWidth'      : ('AutoCompGetMaxWidth', 0, 0),
    'AutoCSetMaxHeight'     : ('AutoCompSetMaxHeight', 0, 0),
    'AutoCGetMaxHeight'     : ('AutoCompGetMaxHeight', 0, 0),
    'AutoCGetMaxHeight'     : ('AutoCompGetMaxHeight', 0, 0),
    'AutoCSetCaseInsensitiveBehaviour'     : ('AutoCompSetCaseInsensitiveBehaviour', 0, 0),
    'AutoCGetCaseInsensitiveBehaviour'     : ('AutoCompGetCaseInsensitiveBehaviour', 0, 0),
    'AutoCSetMulti'         : ('AutoCompSetMulti', 0, 0),
    'AutoCGetMulti'         : ('AutoCompGetMulti', 0, 0),
    'AutoCSetOrder'         : ('AutoCompSetOrder', 0, 0),
    'AutoCGetOrder'         : ('AutoCompGetOrder', 0, 0),

    'RegisterImage' :
    (0,
     '''void %s(int type, const char* const* xpmData);''',
     '''void %s(int type, const char* const* xpmData) {
        SendMsg(%s, type, (sptr_t)xpmData);'''
    ),

    'SetHScrollBar' : ('SetUseHorizontalScrollBar', 0, 0),
    'GetHScrollBar' : ('GetUseHorizontalScrollBar', 0, 0),

    'SetVScrollBar' : ('SetUseVerticalScrollBar', 0, 0),
    'GetVScrollBar' : ('GetUseVerticalScrollBar', 0, 0),

    'GetCaretFore' : ('GetCaretForeground', 0, 0),

    'FindText' :
    (0,
     '''int %s(int minPos, int maxPos, const wxString& text, int flags=0,
                 int* findEnd=nullptr);''',

     '''int %s(int minPos, int maxPos, const wxString& text,
                               int flags, int* findEnd) {
            Sci_TextToFind  ft;
            ft.chrg.cpMin = minPos;
            ft.chrg.cpMax = maxPos;
            const wxWX2MBbuf buf = wx2stc(text);
            ft.lpstrText = buf;

            int pos = SendMsg(%s, flags, (sptr_t)&ft);
            if (findEnd) *findEnd=(pos==-1?wxSTC_INVALID_POSITION:ft.chrgText.cpMax);
            return pos;'''
    ),

    'FormatRange' :
    (0,
     '''int %s(bool   doDraw,
               int    startPos,
               int    endPos,
               wxDC*  draw,
               wxDC*  target,
               wxRect renderRect,
               wxRect pageRect);''',
     ''' int %s(bool   doDraw,
                int    startPos,
                int    endPos,
                wxDC*  draw,
                wxDC*  target,
                wxRect renderRect,
                wxRect pageRect) {
             Sci_RangeToFormat fr;

             if (endPos < startPos) {
                 wxSwap(startPos, endPos);
             }
             fr.hdc = draw;
             fr.hdcTarget = target;
             fr.rc.top = renderRect.GetTop();
             fr.rc.left = renderRect.GetLeft();
             fr.rc.right = renderRect.GetRight();
             fr.rc.bottom = renderRect.GetBottom();
             fr.rcPage.top = pageRect.GetTop();
             fr.rcPage.left = pageRect.GetLeft();
             fr.rcPage.right = pageRect.GetRight();
             fr.rcPage.bottom = pageRect.GetBottom();
             fr.chrg.cpMin = startPos;
             fr.chrg.cpMax = endPos;

             return SendMsg(%s, doDraw, (sptr_t)&fr);'''
    ),


    'GetLine' :
    (0,
     'wxString %s(int line) const;',

     '''wxString %s(int line) const {
         int len = LineLength(line);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(%s, line, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'SetSel' : (None, 0,0), #'SetSelection', 0, 0, 0),

    'GetSelText' :
    ('GetSelectedText',
     'wxString %s();',

     '''wxString %s() {
         const int msg = %s;
         long len = SendMsg(msg, 0, (sptr_t)0);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, 0, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'GetTextRange' :
    (0,
     'wxString %s(int startPos, int endPos);',

     '''wxString %s(int startPos, int endPos) {
         if (endPos < startPos) {
             wxSwap(startPos, endPos);
         }
         int len = endPos - startPos;
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         Sci_TextRange tr;
         tr.lpstrText = buf.data();
         tr.chrg.cpMin = startPos;
         tr.chrg.cpMax = endPos;
         tr.lpstrText[0] = '\\0'; // initialize with 0 in case the range is invalid
         SendMsg(%s, 0, (sptr_t)&tr);
         return stc2wx(buf);'''
    ),

    'PointXFromPosition' :
    ('PointFromPosition',
     'wxPoint %s(int pos);',
     '''wxPoint %s(int pos) {
         int x = SendMsg(%s, 0, pos);
         int y = SendMsg(SCI_POINTYFROMPOSITION, 0, pos);
         return wxPoint(x, y);'''
    ),

    'PointYFromPosition' : (None, 0, 0),

    'ScrollCaret' : ('EnsureCaretVisible', 0, 0),
    'ReplaceSel' : ('ReplaceSelection', 0, 0),
    'Null' : (None, 0, 0),

    'GetText' :
    (0,
     'wxString %s() const;',

     '''wxString %s() const {
         int len = GetTextLength();
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(%s, len+1, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'GetDirectFunction' :
    (0,
     'void* %s() const;',
     '''void* %s() const {
         return (void*)SendMsg(%s);'''
    ),

    'GetDirectPointer' :
    (0,
     'void* %s() const;',
     '''void* %s() const {
         return (void*)SendMsg(%s);'''
    ),

    'GetTargetText' :
    (0,
     'wxString %s() const;',

     '''wxString %s() const {
         int len = GetTargetEnd() - GetTargetStart();
         wxCharBuffer buf(len);
         SendMsg(%s, 0, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'CallTipPosStart'   : ('CallTipPosAtStart', 0, 0),
    'CallTipSetPosStart': ('CallTipSetPosAtStart', 0, 0),
    'CallTipSetHlt'     : ('CallTipSetHighlight', 0, 0),
    'CallTipSetBack'    : ('CallTipSetBackground', 0, 0),
    'CallTipSetFore'    : ('CallTipSetForeground', 0, 0),
    'CallTipSetForeHlt' : ('CallTipSetForegroundHighlight', 0, 0),

    'SetHotspotActiveFore' : ('SetHotspotActiveForeground', 0, 0),
    'SetHotspotActiveBack' : ('SetHotspotActiveBackground', 0, 0),
    'GetHotspotActiveFore' : ('GetHotspotActiveForeground', 0, 0),
    'GetHotspotActiveBack' : ('GetHotspotActiveBackground', 0, 0),

    'GetCaretLineBack' : ('GetCaretLineBackground', 0, 0),
    'SetCaretLineBack' : ('SetCaretLineBackground', 0, 0),

    'ReplaceTarget' :
    (0,
     'int %s(const wxString& text);',

     '''
     int %s(const wxString& text) {
         const wxWX2MBbuf buf = wx2stc(text);
         return SendMsg(%s, wx2stclen(text, buf), (sptr_t)(const char*)buf);'''
    ),

    'ReplaceTargetRE' :
    (0,
     'int %s(const wxString& text);',

     '''
     int %s(const wxString& text) {
         const wxWX2MBbuf buf = wx2stc(text);
         return SendMsg(%s, wx2stclen(text, buf), (sptr_t)(const char*)buf);'''
    ),

    'SearchInTarget' :
    (0,
     'int %s(const wxString& text);',

     '''
     int %s(const wxString& text) {
         const wxWX2MBbuf buf = wx2stc(text);
         return SendMsg(%s, wx2stclen(text, buf), (sptr_t)(const char*)buf);'''
    ),

    # not sure what to do about these yet
    'TargetAsUTF8' :       ( None, 0, 0),
    'SetLengthForEncode' : ( None, 0, 0),
    'EncodedFromUTF8' :    ( None, 0, 0),


    'GetProperty' :
    (0,
     'wxString %s(const wxString& key);',

     '''wxString %s(const wxString& key) {
         const int msg = %s;
         const wxWX2MBbuf keyBuf = wx2stc(key);
         long len = SendMsg(msg, (uptr_t)(const char*)keyBuf, 0);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, (uptr_t)(const char*)keyBuf, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'GetPropertyExpanded' :
    (0,
     'wxString %s(const wxString& key);',

     '''wxString %s(const wxString& key) {
         const int msg = %s;
         const wxWX2MBbuf keyBuf = wx2stc(key);
         long len = SendMsg(msg, (uptr_t)(const char*)keyBuf, 0);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, (uptr_t)(const char*)keyBuf, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'GetPropertyInt' :
    (0,
    'int %s(const wxString &key, int defaultValue=0) const;',
    '''int %s(const wxString &key, int defaultValue) const {
        return SendMsg(%s, (uptr_t)(const char*)wx2stc(key), defaultValue);'''
    ),

    'BraceMatch' :
    (0,
    'int %s(int pos, int maxReStyle=0);',
    '''int %s(int pos, int maxReStyle){
        wxASSERT_MSG(maxReStyle==0,
                     "The second argument passed to BraceMatch should be 0");

        return SendMsg(%s, pos, maxReStyle);'''
    ),

    'GetDocPointer' :
    (0,
     'void* %s();',
     '''void* %s() {
         return (void*)SendMsg(%s);'''
    ),

    'SetDocPointer' :
    (0,
     'void %s(void* docPointer);',
     '''void %s(void* docPointer) {
         SendMsg(%s, 0, (sptr_t)docPointer);'''
    ),

    'CreateDocument' :
    (0,
     'void* %s();',
     '''void* %s() {
         return (void*)SendMsg(%s);'''
    ),

    'AddRefDocument' :
    (0,
     'void %s(void* docPointer);',
     '''void %s(void* docPointer) {
         SendMsg(%s, 0, (sptr_t)docPointer);'''
    ),

    'ReleaseDocument' :
    (0,
     'void %s(void* docPointer);',
     '''void %s(void* docPointer) {
         SendMsg(%s, 0, (sptr_t)docPointer);'''
    ),

    'SetCodePage' :
    (0,
     0,
     '''void %s(int codePage) {
    wxASSERT_MSG(codePage == wxSTC_CP_UTF8,
                 wxT("Only wxSTC_CP_UTF8 may be used."));
    SendMsg(%s, codePage);'''
    ),


    'GrabFocus' : (None, 0, 0),

    # Rename some that would otherwise hide the wxWindow methods
    'SetFocus'  : ('SetSTCFocus', 0, 0),
    'GetFocus'  : ('GetSTCFocus', 0, 0),
    'SetCursor' : ('SetSTCCursor', 0, 0),
    'GetCursor' : ('GetSTCCursor', 0, 0),

    'SetPositionCache' : ('SetPositionCacheSize', 0, 0),
    'GetPositionCache' : ('GetPositionCacheSize', 0, 0),

    'GetLexerLanguage' :(0,
     'wxString %s() const;',

     '''wxString %s() const {
         const int msg = %s;
         int len = SendMsg(msg, 0, (sptr_t)nullptr);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, 0, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'SetSelection' : (None, 0, 0),

    'GetCharacterPointer' : (0,
                             'const char* %s() const;',
                             'const char* %s() const {\n'
                             '    return (const char*)SendMsg(%s, 0, 0);'
                            ),

    'GetRangePointer' : (0,
                             'const char* %s(int position, int rangeLength) const;',
                             'const char* %s(int position, int rangeLength) const {\n'
                             '    return (const char*)SendMsg(%s, position, rangeLength);'
                        ),

    'GetWordChars' :
    (0,
     'wxString %s() const;',

     '''wxString %s() const {
         const int msg = %s;
         int len = SendMsg(msg, 0, (sptr_t)nullptr);
         if (!len) return wxEmptyString;

         wxMemoryBuffer mbuf(len+1);
         char* buf = (char*)mbuf.GetWriteBuf(len+1);
         SendMsg(msg, 0, (sptr_t)buf);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);'''
    ),

    'GetTag' :
    (0,
     'wxString %s(int tagNumber) const;',

     '''wxString %s(int tagNumber) const {
         const int msg = %s;
         long len = SendMsg(msg, tagNumber, (sptr_t)nullptr);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, tagNumber, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'GetWhitespaceChars' :
    (0,
     'wxString %s() const;',

     '''wxString %s() const {
         const int msg = %s;
         int len = SendMsg(msg, 0, (sptr_t)nullptr);
         if (!len) return wxEmptyString;

         wxMemoryBuffer mbuf(len+1);
         char* buf = (char*)mbuf.GetWriteBuf(len+1);
         SendMsg(msg, 0, (sptr_t)buf);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);'''
    ),


    'GetPunctuationChars' :
    (0,
     'wxString %s() const;',

     '''wxString %s() const {
         const int msg = %s;
         int len = SendMsg(msg, 0, (sptr_t)nullptr);
         if (!len) return wxEmptyString;

         wxMemoryBuffer mbuf(len+1);
         char* buf = (char*)mbuf.GetWriteBuf(len+1);
         SendMsg(msg, 0, (sptr_t)buf);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);'''
    ),


    'PropertyNames' :
    (0,
     'wxString %s() const;',

     '''wxString %s() const {
         const int msg = %s;
         long len = SendMsg(msg, 0, (sptr_t)nullptr);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, 0, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),



    'DescribeProperty' :
    (0,
     'wxString %s(const wxString& name) const;',

     '''wxString %s(const wxString& name) const {
         const int msg = %s;
         const wxWX2MBbuf nameBuf = wx2stc(name);
         long len = SendMsg(msg, (uptr_t)(const char*)nameBuf, (sptr_t)nullptr);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, (uptr_t)(const char*)nameBuf, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),



    'DescribeKeyWordSets' :
    (0,
     'wxString %s() const;',

     '''wxString %s() const {
         const int msg = %s;
         long len = SendMsg(msg, 0, (sptr_t)nullptr);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, 0, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'MarkerDefineRGBAImage' :
    (0,
    'void %s(int markerNumber, const unsigned char* pixels);',
    '''void %s(int markerNumber, const unsigned char* pixels) {
           SendMsg(%s, markerNumber, (sptr_t)pixels);'''
    ),


    'RegisterRGBAImage' :
    (0,
    'void %s(int type, const unsigned char* pixels);',
    '''void %s(int type, const unsigned char* pixels) {
           SendMsg(%s, type, (sptr_t)pixels);'''
    ),


    # I think these are only available on the native OSX backend, so
    # don't add them to the wx API...
    'FindIndicatorShow' : (None, 0,0),
    'FindIndicatorFlash' : (None, 0,0),
    'FindIndicatorHide' : (None, 0,0),

    'CreateLoader' :
    (0,
     'void* %s(int bytes) const;',
     """void* %s(int bytes) const {
         return (void*)(sptr_t)SendMsg(%s, bytes);"""
    ),

    'GetRepresentation' :
    (0,
     'wxString %s(const wxString& encodedCharacter) const;',
     '''wxString %s(const wxString& encodedCharacter) const {
         const int msg = %s;
         const wxWX2MBbuf encCharBuf = wx2stc(encodedCharacter);
         long len = SendMsg(msg, (sptr_t)(const char*)encCharBuf, (sptr_t)nullptr);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, (sptr_t)(const char*)encCharBuf, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

     'PrivateLexerCall' :
     (0,
      'void* %s(int operation, void* pointer);',
      """void* %s(int operation, void* pointer) {
           return (void*)(sptr_t)SendMsg(%s, operation, (sptr_t)pointer);"""
     ),

    'GetSubStyleBases' :
    (0,
     'wxString %s() const;',
     '''wxString %s() const {
         const int msg = %s;
         long len = SendMsg(msg, 0, (sptr_t)nullptr);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, 0, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'SetKeysUnicode' : (None,0,0),
    'GetKeysUnicode' : (None,0,0),

    'NameOfStyle' :
    (0,
     'wxString %s(int style) const;',
     '''wxString %s(int style) const {
         const int msg = %s;
         long len = SendMsg(msg, style, (sptr_t)nullptr);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, style, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'TagsOfStyle' :
    (0,
     'wxString %s(int style) const;',
     '''wxString %s(int style) const {
         const int msg = %s;
         long len = SendMsg(msg, style, (sptr_t)nullptr);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, style, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'DescriptionOfStyle' :
    (0,
     'wxString %s(int style) const;',
     '''wxString %s(int style) const {
         const int msg = %s;
         long len = SendMsg(msg, style, (sptr_t)nullptr);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, style, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'GetDefaultFoldDisplayText' :
    (0,
     'wxString %s() const;',
     '''wxString %s() const {
         const int msg = %s;
         long len = SendMsg(msg, 0, (sptr_t)nullptr);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, 0, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'EOLAnnotationGetText' :
    (0,
     'wxString %s(int line) const;',
     '''wxString %s(int line) const {
         const int msg = %s;
         long len = SendMsg(msg, line, (sptr_t)nullptr);
         if (!len) return wxEmptyString;

         wxCharBuffer buf(len);
         SendMsg(msg, line, (sptr_t)buf.data());
         return stc2wx(buf);'''
    ),

    'SetILexer' :
    (0,
     'void %s(void* ilexer);',
     '''void %s(void* ilexer) {
         SendMsg(%s, 0, (sptr_t)ilexer);'''
    ),

    '' : ('', 0, 0),

    }

# all Scintilla getters are transformed into const member of wxSTC class but
# some non-getter methods are also logically const and this set contains their
# names (notice that it's useless to include here methods manually overridden
# above)
constNonGetterMethods = (
    'LineFromPosition',
    'PositionFromLine',
    'LineLength',
    'CanPaste',
    'CanRedo',
    'CanUndo',
)

# several methods require override
overrideNeeded = (
    'Redo',
    'SelectAll',
    'Undo',
    'Cut',
    'Copy',
    'Paste',
    'CanPaste',
    'CanRedo',
    'CanUndo',
    'EmptyUndoBuffer',
    'Clear',
    'AppendText',
)

#----------------------------------------------------------------------------

def processIface(iface, h_tmplt, cpp_tmplt, ih_tmplt, h_dest, cpp_dest, docstr_dest, ih_dest, msgcodes):
    curDocStrings = []
    values = []
    methods = []
    cmds = []
    icat = 'Basics'

    # parse iface file
    for line in iface:
        line = line[:-1]
        if line[:2] == '##' or line == '':
            #curDocStrings = []
            continue

        op = line[:4]
        if line[:2] == '# ':   # a doc string
            curDocStrings.append(line[2:])

        elif op == 'val ':
            parseVal(line[4:], values, curDocStrings, icat)
            curDocStrings = []

        elif op == 'fun ' or op == 'set ' or op == 'get ':
            parseFun(line[4:], methods, curDocStrings, cmds, op == 'get ', msgcodes, icat)
            curDocStrings = []

        elif op == 'cat ':
            icat = line[4:].strip()
            curDocStrings = []

        elif op == 'evt ':
            pass

        elif op == 'enu ':
            pass

        elif op == 'lex ':
            pass

        elif op == 'ali ':
            pass

        else:
            print('***** Unknown line type: %s' % line)

    # build the items for the table of contents in the interface header
    tableitems=''
    firstitem=True
    for category, title, description  in categoriesList:
        if firstitem:
            firstitem=False
        else:
            tableitems+='\n'
        tableitems+='    - @ref_member_group{'+category+', '+title+'}'

    # process templates
    data = {}
    data['VALUES'] = processVals(values)
    data['CMDS']   = processVals(cmds)
    defs, imps, docstrings, idefs = processMethods(methods)
    data['METHOD_DEFS'] = defs
    data['METHOD_IDEFS'] = idefs
    data['METHOD_IMPS'] = imps
    data['TABLE_OF_CONTENTS'] = tableitems

    # get template text
    h_text = open(h_tmplt).read()
    ih_text = open(ih_tmplt).read()
    cpp_text = open(cpp_tmplt).read()

    # do the substitutions
    h_text = h_text % data
    cpp_text = cpp_text % data
    ih_text = ih_text % data

    # write out destination files
    # Use 'wb' with Python 2 to enforce LF newlines. This does not work with
    # Python 3, use 'newline':'\n' there (which in turn does not work with 2).
    if sys.version_info[0] == 2:
        acc = 'wb'
        eol = {}
    else:
        acc = 'wt'
        eol = {'newline':'\n'}

    open(h_dest, acc, **eol).write(h_text)
    open(cpp_dest, acc, **eol).write(cpp_text)
    if docstr_dest:
        open(docstr_dest, acc, **eol).write(docstrings)
    open(ih_dest, acc, **eol).write(ih_text)


def joinWithNewLines(values):
    return '\n'.join(values)

#----------------------------------------------------------------------------

# parse header file for message codes
def processHeader(hdr_scn, codeDict):
    fh = FileInput(hdr_scn)
    for line in fh:
        line = line[:-1]
        if line[:8] != '#define ':
            continue

        op = line[8:]
        tokens = op.split()
        if len(tokens) != 2:
            continue

        symbname = tokens[0]
        symbval = tokens[1]
        if symbname[:4] == 'SCI_':
            # add symbol and its value to the dictionary
            if symbval in codeDict:
                print("***** Duplicated message code for " + symbname)
            else:
                codeDict[symbval] = symbname

#----------------------------------------------------------------------------

def processVals(values):
    text = []
    for name, value, docs in values:
        if docs:
            text.append('')
            for x in docs:
                text.append('/// ' + x)
        text.append('#define %s %s' % (name, value))
    return joinWithNewLines(text)

#----------------------------------------------------------------------------

def processMethods(methods):
    defs = []
    idefs = []
    imps = []
    dstr = []
    piecesForInterface = {}

    # Initialize each of the piecesForInterface with an empty list
    for c in categoriesList:
        piecesForInterface[c[0]]=[]

    for retType, interfName, number, param1, param2, docs, is_const, is_override, icat  in methods:
        retType = retTypeMap.get(retType, retType)
        params = makeParamString(param1, param2)

        if icat=='Provisional' and not GENERATE_PROVISIONAL_ITEMS:
            continue

        name, theDef, theImp = checkMethodOverride(interfName, number)

        if name is None:
            continue

        category, docs, docsLong = buildDocs(interfName, docs, icat)

        # Build docstrings
        st = 'DocStr(wxStyledTextCtrl::%s,\n' \
             '"%s", "");\n' % (name, joinWithNewLines(docs))
        dstr.append(st)

        # Build the method definition for the .h file
        if docs:
            defs.append('')
            for x in docs:
                defs.append('    // ' + x)
        if not theDef:
            theDef = '    %s %s(%s)' % (retType, name, params)
            if is_const:
                theDef = theDef + ' const'
            if is_override:
                theDef = theDef + ' override'
            theDef = theDef + ';'
        if category=='DeprecatedMessages' or icat=='Deprecated':
            defs.append('    wxDEPRECATED_MSG( "This method uses a function '
                        'deprecated in the Scintilla library." )')
        defs.append(theDef)

        # Skip override from the interface file
        if is_override:
          theDef = theDef.replace(' override', '')

        # Build the method definition for the interface .h file
        intrflines = []
        if docs:
            intrflines.append('')
            intrflines.append('    /**')
            for x in docs:
                intrflines.append('        ' + x)

            if docsLong != 0:
                intrflines.append('')
                for y in docsLong:
                    intrflines.append('        ' + y)
            intrflines.append('    */')
        else:
            print('warning: ' + name + ' is undocumented.')

        if name == 'GetCurLine':
            intrflines.append('    wxString GetCurLine(int* linePos=nullptr);')
        else:
            intrflines.append(theDef)

        piecesForInterface[category]+=intrflines


        # Build the method implementation string
        if docs:
            imps.append('')
            for x in docs:
                imps.append('// ' + x)
        if not theImp:
            theImp = '%s wxStyledTextCtrl::%s(%s)' % (retType, name, params)
            if is_const:
                theImp = theImp + ' const'
            theImp = theImp + '\n{\n    '
            if retType == 'wxColour':
                theImp = theImp + 'long c = '
            elif retType != 'void':
                theImp = theImp + 'return '
            theImp = theImp + 'SendMsg(%s, %s, %s)' % (number,
                                                       makeArgString(param1),
                                                       makeArgString(param2))
            if retType == 'bool':
                theImp = theImp + ' != 0'
            if retType == 'wxColour':
                theImp = theImp + ';\n    return wxColourFromLong(c)'

            theImp = theImp + ';\n}'
        imps.append(theImp)

    # For the interface file, merge all the pieces into one list
    for c in categoriesList:
        idefs.append('')
        idefs.append('    /**')
        idefs.append('        @member_group_name{' + c[0] + ', ' + c[1] + '}')

        if c[2] != 0:
            idefs.append('')
            for z in c[2]:
                idefs.append('        ' + z)

        idefs.append('    */')
        idefs.append('    //@{')

        idefs+=piecesForInterface[c[0]]
        idefs.append('')
        idefs.append('    //@}')

    return joinWithNewLines(defs), joinWithNewLines(imps), joinWithNewLines(dstr), joinWithNewLines(idefs)

#----------------------------------------------------------------------------

def checkMethodOverride(name, number):
    theDef = theImp = None
    if name in methodOverrideMap:
        item = methodOverrideMap[name]

        try:
            if item[0] != 0:
                name = item[0]
            if item[1] != 0:
                theDef = '    ' + (item[1] % name)
            if item[2] != 0:
                theImp = item[2] % ('wxStyledTextCtrl::'+name, number) + '\n}'
        except:
            print("************* " + name)
            raise

    return name, theDef, theImp

#----------------------------------------------------------------------------

def makeArgString(param):
    if not param:
        return '0'

    typ, name = param

    if typ == 'string':
        return '(sptr_t)(const char*)wx2stc(%s)' % name
    if typ == 'colour':
        return 'wxColourAsLong(%s)' % name

    return name

#----------------------------------------------------------------------------

def makeParamString(param1, param2):
    def doOne(param):
        if param:
            aType = paramTypeMap.get(param[0], param[0])
            return aType + ' ' + param[1]
        else:
            return ''

    st = doOne(param1)
    if st and param2:
        st = st + ', '
    st = st + doOne(param2)
    return st


#----------------------------------------------------------------------------

def parseVal(line, values, docs, icat):
    name, val = line.split('=')

    if name in notMappedSciValues:
            return

    if icat=='Deprecated':
        docs.append('@deprecated')

    if icat=='Provisional':
        if GENERATE_PROVISIONAL_ITEMS:
            docs.append('This item is a provisional value and is subject '
                        'to change')
            docs.append('in future versions of wxStyledTextCtrl.')
        else:
            return

    # remove prefixes such as SCI, etc.
    for old, new in valPrefixes:
        lo = len(old)
        if name[:lo] == old:
            if new is None:
                return
            name = new + name[lo:]

    # add it to the list
    values.append( ('wxSTC_' + name, val, docs) )

#----------------------------------------------------------------------------

funregex = re.compile(r'\s*([a-zA-Z0-9_]+)'  # <ws>return type
                      '\s+([a-zA-Z0-9_]+)='  # <ws>name=
                      '([0-9]+)'             # number
                      '\(([ a-zA-Z0-9_]*),'  # (param,
                      '([ a-zA-Z0-9_]*),*\)')  # param)

def parseFun(line, methods, docs, values, is_const, msgcodes, icat):
    def parseParam(param):
        param = param.strip()
        if param == '':
            param = None
        else:
            param = tuple(param.split())
        return param

    mo = funregex.match(line)
    if mo is None:
        print("***** Line doesn't match! : %s" % line)

    retType, name, number, param1, param2 = mo.groups()

    param1 = parseParam(param1)
    param2 = parseParam(param2)

    # Special case.  For the key command functions we want a value defined too
    num = int(number)
    for v in cmdValues:
        if (type(v) == type(()) and v[0] <= num <= v[1]) or v == num:
            parseVal('CMD_%s=%s' % (name.upper(), number), values, docs, 'Basics')

            # if we are not also doing a function for CMD values, then
            # just return, otherwise fall through to the append blow.
            if not FUNC_FOR_CMD:
                return

    # if possible, replace numeric value with symbol
    if number in msgcodes:
        code = msgcodes[number]
    else:
        code = number
    methods.append( (retType, name, code, param1, param2, tuple(docs),
                     is_const or name in constNonGetterMethods,
                     name in overrideNeeded, icat) )


#----------------------------------------------------------------------------


def main(args):
    # TODO: parse command line args to replace default input/output files???

    if not os.path.exists(IFACE1):
        print('Please run this script from src/stc subdirectory.')
        sys.exit(1)

    # parse header file for message codes and create dictionary
    msgcodes = {}
    processHeader(HDR_SCN, msgcodes)

    i1_lines = open(IFACE1).readlines()
    i2_lines = open(IFACE2).readlines()
    iface = i1_lines + i2_lines

    # Now just do it
    processIface(iface, H_TEMPLATE, CPP_TEMPLATE, IH_TEMPLATE, H_DEST, CPP_DEST, DOCSTR_DEST, IH_DEST, msgcodes)



if __name__ == '__main__':
    main(sys.argv)

#----------------------------------------------------------------------------

