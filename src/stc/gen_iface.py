#!/bin/env python
#----------------------------------------------------------------------------
# Name:         gen_iface.py
# Purpose:      Generate stc.h and stc.cpp from the info in Scintilla.iface
#
# Author:       Robin Dunn
#
# Created:      5-Sept-2000
# RCS-ID:       $Id$
# Copyright:    (c) 2000 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------


import sys, string, re, os
from fileinput import FileInput


IFACE         = os.path.abspath('./scintilla/include/Scintilla.iface')
H_TEMPLATE    = os.path.abspath('./stc.h.in')
CPP_TEMPLATE  = os.path.abspath('./stc.cpp.in')
H_DEST        = os.path.abspath('../../include/wx/stc/stc.h')
CPP_DEST      = os.path.abspath('./stc.cpp')
DOCSTR_DEST   = os.path.abspath('../../wxPython/contrib/stc/_stc_gendocs.i')


# Value prefixes to convert
valPrefixes = [('SCI_', ''),
               ('SC_',  ''),
               ('SCN_', None),  # just toss these out...
               ('SCEN_', None),
               ('SCE_', ''),
               ('SCLEX_', 'LEX_'),
               ('SCK_', 'KEY_'),
               ('SCFIND_', 'FIND_'),
               ('SCWS_', 'WS_'),
]

# Message function values that should have a CMD_ constant generated
cmdValues = [ (2300, 2349),
              2011,
              2013,
              (2176, 2180),
              (2390, 2393),
              (2395, 2396),
              2404,
              (2413, 2416),
              (2426, 2442),
              (2450, 2455),
            ]


# Should a funciton be also generated for the CMDs?
FUNC_FOR_CMD = True


# Map some generic typenames to wx types, using return value syntax
retTypeMap = {
    'position': 'int',
    'string':   'wxString',
    'colour':   'wxColour',
    }

# Map some generic typenames to wx types, using parameter syntax
paramTypeMap = {
    'position': 'int',
    'string':   'const wxString&',
    'colour':   'const wxColour&',
    'keymod':   'int',
}

# Map of method info that needs tweaked.  Either the name needs changed, or
# the method definition/implementation.  Tuple items are:
#
#         1. New method name.  None to skip the method, 0 to leave the
#            default name.
#         2. Method definition for the .h file, 0 to leave alone
#         3. Method implementation for the .cpp file, 0 to leave alone.
#         4. tuple of Doc string lines, or 0 to leave alone.
#
methodOverrideMap = {
    'AddText' : (0,
                 'void %s(const wxString& text);',

                 '''void %s(const wxString& text) {
                    wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(text);
                    SendMsg(%s, strlen(buf), (long)(const char*)buf);''',
                 0),

    'AddStyledText' : (0,
                       'void %s(const wxMemoryBuffer& data);',

                       '''void %s(const wxMemoryBuffer& data) {
                          SendMsg(%s, data.GetDataLen(), (long)data.GetData());''',
                       0),

    'AppendText' : (0,
                 'void %s(const wxString& text);',

                 '''void %s(const wxString& text) {
                    wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(text);
                    SendMsg(%s, strlen(buf), (long)(const char*)buf);''',
                 0),

    'GetViewWS' : ( 'GetViewWhiteSpace', 0, 0, 0),
    'SetViewWS' : ( 'SetViewWhiteSpace', 0, 0, 0),

    'GetCharAt' :
    ( 0, 0,
      '''int %s(int pos) {
         return (unsigned char)SendMsg(%s, pos, 0);''',
      0),

    'GetStyleAt' :
    ( 0, 0,
      '''int %s(int pos) {
         return (unsigned char)SendMsg(%s, pos, 0);''',
      0),

    'GetStyledText' :
    (0,
     'wxMemoryBuffer %s(int startPos, int endPos);',

     '''wxMemoryBuffer %s(int startPos, int endPos) {
        wxMemoryBuffer buf;
        if (endPos < startPos) {
            int temp = startPos;
            startPos = endPos;
            endPos = temp;
        }
        int len = endPos - startPos;
        if (!len) return buf;
        TextRange tr;
        tr.lpstrText = (char*)buf.GetWriteBuf(len*2+1);
        tr.chrg.cpMin = startPos;
        tr.chrg.cpMax = endPos;
        len = SendMsg(%s, 0, (long)&tr);
        buf.UngetWriteBuf(len);
        return buf;''',

     ('Retrieve a buffer of cells.',)),


    'PositionFromPoint' :
    (0,
     'int %s(wxPoint pt);',

     '''int %s(wxPoint pt) {
        return SendMsg(%s, pt.x, pt.y);''',
     0),

    'GetCurLine' :
    (0,
     '#ifdef SWIG\n    wxString %s(int* OUTPUT);\n#else\n    wxString GetCurLine(int* linePos=NULL);\n#endif',

        '''wxString %s(int* linePos) {
        int len = LineLength(GetCurrentLine());
        if (!len) {
            if (linePos)  *linePos = 0;
            return wxEmptyString;
        }

        wxMemoryBuffer mbuf(len+1);
        char* buf = (char*)mbuf.GetWriteBuf(len+1);

        int pos = SendMsg(%s, len+1, (long)buf);
        mbuf.UngetWriteBuf(len);
        mbuf.AppendByte(0);
        if (linePos)  *linePos = pos;
        return stc2wx(buf);''',

     0),

    'SetUsePalette' : (None, 0,0,0),

    'MarkerSetFore' : ('MarkerSetForeground', 0, 0, 0),
    'MarkerSetBack' : ('MarkerSetBackground', 0, 0, 0),

    'MarkerDefine' :
    (0,
     '''void %s(int markerNumber, int markerSymbol,
                const wxColour& foreground = wxNullColour,
                const wxColour& background = wxNullColour);''',

     '''void %s(int markerNumber, int markerSymbol,
                const wxColour& foreground,
                const wxColour& background) {

                SendMsg(%s, markerNumber, markerSymbol);
                if (foreground.Ok())
                    MarkerSetForeground(markerNumber, foreground);
                if (background.Ok())
                    MarkerSetBackground(markerNumber, background);''',

     ('Set the symbol used for a particular marker number,',
      'and optionally the fore and background colours.')),


    'MarkerDefinePixmap' :
    ('MarkerDefineBitmap',
     '''void %s(int markerNumber, const wxBitmap& bmp);''',
     '''void %s(int markerNumber, const wxBitmap& bmp) {
        // convert bmp to a xpm in a string
        wxMemoryOutputStream strm;
        wxImage img = bmp.ConvertToImage();
        if (img.HasAlpha())
            img.ConvertAlphaToMask();
        img.SaveFile(strm, wxBITMAP_TYPE_XPM);
        size_t len = strm.GetSize();
        char* buff = new char[len+1];
        strm.CopyTo(buff, len);
        buff[len] = 0;
        SendMsg(%s, markerNumber, (long)buff);
        delete [] buff;
        ''',
     ('Define a marker from a bitmap',)),


    'SetMarginTypeN' : ('SetMarginType', 0, 0, 0),
    'GetMarginTypeN' : ('GetMarginType', 0, 0, 0),
    'SetMarginWidthN' : ('SetMarginWidth', 0, 0, 0),
    'GetMarginWidthN' : ('GetMarginWidth', 0, 0, 0),
    'SetMarginMaskN' : ('SetMarginMask', 0, 0, 0),
    'GetMarginMaskN' : ('GetMarginMask', 0, 0, 0),
    'SetMarginSensitiveN' : ('SetMarginSensitive', 0, 0, 0),
    'GetMarginSensitiveN' : ('GetMarginSensitive', 0, 0, 0),

    'StyleSetFore' : ('StyleSetForeground', 0, 0, 0),
    'StyleSetBack' : ('StyleSetBackground', 0, 0, 0),
    'SetSelFore' : ('SetSelForeground', 0, 0, 0),
    'SetSelBack' : ('SetSelBackground', 0, 0, 0),
    'SetCaretFore' : ('SetCaretForeground', 0, 0, 0),
    'StyleSetFont' : ('StyleSetFaceName', 0, 0, 0),
    'StyleSetCharacterSet' : (None, 0, 0, 0),
    
    'AssignCmdKey' :
    ('CmdKeyAssign',
     'void %s(int key, int modifiers, int cmd);',

     '''void %s(int key, int modifiers, int cmd) {
         SendMsg(%s, MAKELONG(key, modifiers), cmd);''',
     0),


    'ClearCmdKey' :
    ('CmdKeyClear',
     'void %s(int key, int modifiers);',

     '''void %s(int key, int modifiers) {
         SendMsg(%s, MAKELONG(key, modifiers));''',
     0),

    'ClearAllCmdKeys' : ('CmdKeyClearAll', 0, 0, 0),


    'SetStylingEx' :
    ('SetStyleBytes',
     'void %s(int length, char* styleBytes);',

     '''void %s(int length, char* styleBytes) {
        SendMsg(%s, length, (long)styleBytes);''',
     0),


    'IndicSetStyle' : ('IndicatorSetStyle', 0, 0, 0),
    'IndicGetStyle' : ('IndicatorGetStyle', 0, 0, 0),
    'IndicSetFore' : ('IndicatorSetForeground', 0, 0, 0),
    'IndicGetFore' : ('IndicatorGetForeground', 0, 0, 0),

    'SetWhitespaceFore' : ('SetWhitespaceForeground', 0, 0, 0),
    'SetWhitespaceBack' : ('SetWhitespaceBackground', 0, 0, 0),

    'AutoCShow' : ('AutoCompShow', 0, 0, 0),
    'AutoCCancel' : ('AutoCompCancel', 0, 0, 0),
    'AutoCActive' : ('AutoCompActive', 0, 0, 0),
    'AutoCPosStart' : ('AutoCompPosStart', 0, 0, 0),
    'AutoCComplete' : ('AutoCompComplete', 0, 0, 0),
    'AutoCStops' : ('AutoCompStops', 0, 0, 0),
    'AutoCSetSeparator' : ('AutoCompSetSeparator', 0, 0, 0),
    'AutoCGetSeparator' : ('AutoCompGetSeparator', 0, 0, 0),
    'AutoCSelect' : ('AutoCompSelect', 0, 0, 0),
    'AutoCSetCancelAtStart' : ('AutoCompSetCancelAtStart', 0, 0, 0),
    'AutoCGetCancelAtStart' : ('AutoCompGetCancelAtStart', 0, 0, 0),
    'AutoCSetFillUps' : ('AutoCompSetFillUps', 0, 0, 0),
    'AutoCSetChooseSingle' : ('AutoCompSetChooseSingle', 0, 0, 0),
    'AutoCGetChooseSingle' : ('AutoCompGetChooseSingle', 0, 0, 0),
    'AutoCSetIgnoreCase' : ('AutoCompSetIgnoreCase', 0, 0, 0),
    'AutoCGetIgnoreCase' : ('AutoCompGetIgnoreCase', 0, 0, 0),
    'AutoCSetAutoHide' : ('AutoCompSetAutoHide', 0, 0, 0),
    'AutoCGetAutoHide' : ('AutoCompGetAutoHide', 0, 0, 0),
    'AutoCSetDropRestOfWord' : ('AutoCompSetDropRestOfWord', 0,0,0),
    'AutoCGetDropRestOfWord' : ('AutoCompGetDropRestOfWord', 0,0,0),
    'AutoCGetTypeSeparator' : ('AutoCompGetTypeSeparator', 0, 0, 0),
    'AutoCSetTypeSeparator' : ('AutoCompSetTypeSeparator', 0, 0, 0),
    'AutoCGetCurrent'       : ('AutoCompGetCurrent', 0, 0, 0),
    'AutoCSetMaxWidth'      : ('AutoCompSetMaxWidth', 0, 0, 0),
    'AutoCGetMaxWidth'      : ('AutoCompGetMaxWidth', 0, 0, 0),
    'AutoCSetMaxHeight'     : ('AutoCompSetMaxHeight', 0, 0, 0),
    'AutoCGetMaxHeight'     : ('AutoCompGetMaxHeight', 0, 0, 0),
    'AutoCGetMaxHeight'     : ('AutoCompGetMaxHeight', 0, 0, 0),
    
    'RegisterImage' :
    (0,
     '''void %s(int type, const wxBitmap& bmp);''',
     '''void %s(int type, const wxBitmap& bmp) {
        // convert bmp to a xpm in a string
        wxMemoryOutputStream strm;
        wxImage img = bmp.ConvertToImage();
        if (img.HasAlpha())
            img.ConvertAlphaToMask();
        img.SaveFile(strm, wxBITMAP_TYPE_XPM);
        size_t len = strm.GetSize();
        char* buff = new char[len+1];
        strm.CopyTo(buff, len);
        buff[len] = 0;
        SendMsg(%s, type, (long)buff);
        delete [] buff;
     ''',
     ('Register an image for use in autocompletion lists.',)),


    'ClearRegisteredImages' : (0, 0, 0,
                               ('Clear all the registered images.',)),


    'SetHScrollBar' : ('SetUseHorizontalScrollBar', 0, 0, 0),
    'GetHScrollBar' : ('GetUseHorizontalScrollBar', 0, 0, 0),

    'SetVScrollBar' : ('SetUseVerticalScrollBar', 0, 0, 0),
    'GetVScrollBar' : ('GetUseVerticalScrollBar', 0, 0, 0),

    'GetCaretFore' : ('GetCaretForeground', 0, 0, 0),

    'GetUsePalette' : (None, 0, 0, 0),

    'FindText' :
    (0,
     '''int %s(int minPos, int maxPos, const wxString& text, int flags=0);''',

     '''int %s(int minPos, int maxPos,
               const wxString& text,
               int flags) {
            TextToFind  ft;
            ft.chrg.cpMin = minPos;
            ft.chrg.cpMax = maxPos;
            wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(text);
            ft.lpstrText = (char*)(const char*)buf;

            return SendMsg(%s, flags, (long)&ft);''',
     0),

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
             RangeToFormat fr;

             if (endPos < startPos) {
                 int temp = startPos;
                 startPos = endPos;
                 endPos = temp;
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

             return SendMsg(%s, doDraw, (long)&fr);''',
     0),


    'GetLine' :
    (0,
     'wxString %s(int line);',

     '''wxString %s(int line) {
         int len = LineLength(line);
         if (!len) return wxEmptyString;

         wxMemoryBuffer mbuf(len+1);
         char* buf = (char*)mbuf.GetWriteBuf(len+1);
         SendMsg(%s, line, (long)buf);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);''',

     ('Retrieve the contents of a line.',)),

    'SetSel' : ('SetSelection', 0, 0, 0),

    'GetSelText' :
    ('GetSelectedText',
     'wxString %s();',

     '''wxString %s() {
         int   start;
         int   end;

         GetSelection(&start, &end);
         int   len  = end - start;
         if (!len) return wxEmptyString;

         wxMemoryBuffer mbuf(len+2);
         char* buf = (char*)mbuf.GetWriteBuf(len+1);
         SendMsg(%s, 0, (long)buf);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);''',

     ('Retrieve the selected text.',)),


    'GetTextRange' :
    (0,
     'wxString %s(int startPos, int endPos);',

     '''wxString %s(int startPos, int endPos) {
         if (endPos < startPos) {
             int temp = startPos;
             startPos = endPos;
             endPos = temp;
         }
         int   len  = endPos - startPos;
         if (!len) return wxEmptyString;
         wxMemoryBuffer mbuf(len+1);
         char* buf = (char*)mbuf.GetWriteBuf(len);
         TextRange tr;
         tr.lpstrText = buf;
         tr.chrg.cpMin = startPos;
         tr.chrg.cpMax = endPos;
         SendMsg(%s, 0, (long)&tr);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);''',

     ('Retrieve a range of text.',)),

    'PointXFromPosition' : (None, 0, 0, 0),
    'PointYFromPosition' : (None, 0, 0, 0),

    'ScrollCaret' : ('EnsureCaretVisible', 0, 0, 0),
    'ReplaceSel' : ('ReplaceSelection', 0, 0, 0),
    'Null' : (None, 0, 0, 0),

    'GetText' :
    (0,
     'wxString %s();',

     '''wxString %s() {
         int len  = GetTextLength();
         wxMemoryBuffer mbuf(len+1);   // leave room for the null...
         char* buf = (char*)mbuf.GetWriteBuf(len+1);
         SendMsg(%s, len+1, (long)buf);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);''',

     ('Retrieve all the text in the document.', )),

    'GetDirectFunction' : (None, 0, 0, 0),
    'GetDirectPointer' : (None, 0, 0, 0),

    'CallTipPosStart'   : ('CallTipPosAtStart', 0, 0, 0),
    'CallTipSetHlt'     : ('CallTipSetHighlight', 0, 0, 0),
    'CallTipSetBack'    : ('CallTipSetBackground', 0, 0, 0),
    'CallTipSetFore'    : ('CallTipSetForeground', 0, 0, 0),
    'CallTipSetForeHlt' : ('CallTipSetForegroundHighlight', 0, 0, 0),

    'SetHotspotActiveFore' : ('SetHotspotActiveForeground', 0, 0, 0),
    'SetHotspotActiveBack' : ('SetHotspotActiveBackground', 0, 0, 0),
    
    'GetCaretLineBack' : ('GetCaretLineBackground', 0, 0, 0),
    'SetCaretLineBack' : ('SetCaretLineBackground', 0, 0, 0),

    'ReplaceTarget' :
    (0,
     'int %s(const wxString& text);',

     '''
     int %s(const wxString& text) {
         wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(text);
         return SendMsg(%s, strlen(buf), (long)(const char*)buf);''',
     0),

    'ReplaceTargetRE' :
    (0,
     'int %s(const wxString& text);',

     '''
     int %s(const wxString& text) {
         wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(text);
         return SendMsg(%s, strlen(buf), (long)(const char*)buf);''',
     0),

    'SearchInTarget' :
    (0,
     'int %s(const wxString& text);',

     '''
     int %s(const wxString& text) {
         wxWX2MBbuf buf = (wxWX2MBbuf)wx2stc(text);
         return SendMsg(%s, strlen(buf), (long)(const char*)buf);''',
     0),

    # not sure what to do about these yet
    'TargetAsUTF8' :       ( None, 0, 0, 0),
    'SetLengthForEncode' : ( None, 0, 0, 0),
    'EncodedFromUTF8' :    ( None, 0, 0, 0),


    'GetProperty' :
    (0,
     'wxString %s(const wxString& key);',

     '''wxString %s(const wxString& key) {
         int len = SendMsg(SCI_GETPROPERTY, (long)(const char*)wx2stc(key), 0);
         if (!len) return wxEmptyString;

         wxMemoryBuffer mbuf(len+1);
         char* buf = (char*)mbuf.GetWriteBuf(len+1);
         SendMsg(%s, (long)(const char*)wx2stc(key), (long)buf);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);''',
     ("Retrieve a 'property' value previously set with SetProperty.",)),

    'GetPropertyExpanded' :
    (0,
     'wxString %s(const wxString& key);',

     '''wxString %s(const wxString& key) {
         int len = SendMsg(SCI_GETPROPERTYEXPANDED, (long)(const char*)wx2stc(key), 0);
         if (!len) return wxEmptyString;

         wxMemoryBuffer mbuf(len+1);
         char* buf = (char*)mbuf.GetWriteBuf(len+1);
         SendMsg(%s, (long)(const char*)wx2stc(key), (long)buf);
         mbuf.UngetWriteBuf(len);
         mbuf.AppendByte(0);
         return stc2wx(buf);''',
     ("Retrieve a 'property' value previously set with SetProperty,",
      "with '$()' variable replacement on returned buffer.")),

    'GetPropertyInt'   : (0, 0, 0,
       ("Retrieve a 'property' value previously set with SetProperty,",
        "interpreted as an int AFTER any '$()' variable replacement.")),


    'GetDocPointer' :
    (0,
     'void* %s();',
     '''void* %s() {
         return (void*)SendMsg(%s);''',
     0),

    'SetDocPointer' :
    (0,
     'void %s(void* docPointer);',
     '''void %s(void* docPointer) {
         SendMsg(%s, 0, (long)docPointer);''',
     0),

    'CreateDocument' :
    (0,
     'void* %s();',
     '''void* %s() {
         return (void*)SendMsg(%s);''',
     0),

    'AddRefDocument' :
    (0,
     'void %s(void* docPointer);',
     '''void %s(void* docPointer) {
         SendMsg(%s, 0, (long)docPointer);''',
     0),

    'ReleaseDocument' :
    (0,
     'void %s(void* docPointer);',
     '''void %s(void* docPointer) {
         SendMsg(%s, 0, (long)docPointer);''',
     0),

    'SetCodePage' :
    (0,
     0,
     '''void %s(int codePage) {
#if wxUSE_UNICODE
    wxASSERT_MSG(codePage == wxSTC_CP_UTF8,
                 wxT("Only wxSTC_CP_UTF8 may be used when wxUSE_UNICODE is on."));
#else
    wxASSERT_MSG(codePage != wxSTC_CP_UTF8,
                 wxT("wxSTC_CP_UTF8 may not be used when wxUSE_UNICODE is off."));
#endif
    SendMsg(%s, codePage);''',
     ("Set the code page used to interpret the bytes of the document as characters.",) ),


    'GrabFocus' : (None, 0, 0, 0),

    # Rename some that would otherwise hide the wxWindow methods
    'SetFocus'  : ('SetSTCFocus', 0, 0, 0),
    'GetFocus'  : ('GetSTCFocus', 0, 0, 0),
    'SetCursor' : ('SetSTCCursor', 0, 0, 0),
    'GetCursor' : ('GetSTCCursor', 0, 0, 0),

    'LoadLexerLibrary' : (None, 0,0,0),


    '' : ('', 0, 0, 0),

    }

#----------------------------------------------------------------------------

def processIface(iface, h_tmplt, cpp_tmplt, h_dest, cpp_dest, docstr_dest):
    curDocStrings = []
    values = []
    methods = []
    cmds = []

    # parse iface file
    fi = FileInput(iface)
    for line in fi:
        line = line[:-1]
        if line[:2] == '##' or line == '':
            #curDocStrings = []
            continue

        op = line[:4]
        if line[:2] == '# ':   # a doc string
            curDocStrings.append(line[2:])

        elif op == 'val ':
            parseVal(line[4:], values, curDocStrings)
            curDocStrings = []

        elif op == 'fun ' or op == 'set ' or op == 'get ':
            parseFun(line[4:], methods, curDocStrings, cmds)
            curDocStrings = []

        elif op == 'cat ':
            if string.strip(line[4:]) == 'Deprecated':
                break    # skip the rest of the file

        elif op == 'evt ':
            pass

        elif op == 'enu ':
            pass

        elif op == 'lex ':
            pass

        else:
            print '***** Unknown line type: ', line


    # process templates
    data = {}
    data['VALUES'] = processVals(values)
    data['CMDS']   = processVals(cmds)
    defs, imps, docstrings = processMethods(methods)
    data['METHOD_DEFS'] = defs
    data['METHOD_IMPS'] = imps

    # get template text
    h_text = open(h_tmplt).read()
    cpp_text = open(cpp_tmplt).read()

    # do the substitutions
    h_text = h_text % data
    cpp_text = cpp_text % data

    # write out destination files
    open(h_dest, 'w').write(h_text)
    open(cpp_dest, 'w').write(cpp_text)
    open(docstr_dest, 'w').write(docstrings)



#----------------------------------------------------------------------------

def processVals(values):
    text = []
    for name, value, docs in values:
        if docs:
            text.append('')
            for x in docs:
                text.append('// ' + x)
        text.append('#define %s %s' % (name, value))
    return string.join(text, '\n')

#----------------------------------------------------------------------------

def processMethods(methods):
    defs = []
    imps = []
    dstr = []

    for retType, name, number, param1, param2, docs in methods:
        retType = retTypeMap.get(retType, retType)
        params = makeParamString(param1, param2)

        name, theDef, theImp, docs = checkMethodOverride(name, number, docs)

        if name is None:
            continue

        # Build docstrings
        st = 'DocStr(wxStyledTextCtrl::%s,\n' \
             '"%s", "");\n' % (name, '\n'.join(docs))
        dstr.append(st)
        
        # Build the method definition for the .h file
        if docs:
            defs.append('')
            for x in docs:
                defs.append('    // ' + x)
        if not theDef:
            theDef = '    %s %s(%s);' % (retType, name, params)
        defs.append(theDef)

        # Build the method implementation string
        if docs:
            imps.append('')
            for x in docs:
                imps.append('// ' + x)
        if not theImp:
            theImp = '%s wxStyledTextCtrl::%s(%s) {\n    ' % (retType, name, params)

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


    return '\n'.join(defs), '\n'.join(imps), '\n'.join(dstr)


#----------------------------------------------------------------------------

def checkMethodOverride(name, number, docs):
    theDef = theImp = None
    if methodOverrideMap.has_key(name):
        item = methodOverrideMap[name]

        try:
            if item[0] != 0:
                name = item[0]
            if item[1] != 0:
                theDef = '    ' + (item[1] % name)
            if item[2] != 0:
                theImp = item[2] % ('wxStyledTextCtrl::'+name, number) + '\n}'
            if item[3] != 0:
                docs = item[3]
        except:
            print "*************", name
            raise

    return name, theDef, theImp, docs

#----------------------------------------------------------------------------

def makeArgString(param):
    if not param:
        return '0'

    typ, name = param

    if typ == 'string':
        return '(long)(const char*)wx2stc(%s)' % name
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

def parseVal(line, values, docs):
    name, val = string.split(line, '=')

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
                      '([ a-zA-Z0-9_]*)\)')  # param)

def parseFun(line, methods, docs, values):
    def parseParam(param):
        param = string.strip(param)
        if param == '':
            param = None
        else:
            param = tuple(string.split(param))
        return param

    mo = funregex.match(line)
    if mo is None:
        print "***** Line doesn't match! : " + line

    retType, name, number, param1, param2 = mo.groups()

    param1 = parseParam(param1)
    param2 = parseParam(param2)

    # Special case.  For the key command functions we want a value defined too
    num = string.atoi(number)
    for v in cmdValues:
        if (type(v) == type(()) and v[0] <= num <= v[1]) or v == num:
            parseVal('CMD_%s=%s' % (string.upper(name), number), values, docs)
            
            # if we are not also doing a function for CMD values, then
            # just return, otherwise fall through to the append blow.
            if not FUNC_FOR_CMD:
                return
                
    methods.append( (retType, name, number, param1, param2, tuple(docs)) )


#----------------------------------------------------------------------------


def main(args):
    # TODO: parse command line args to replace default input/output files???

    # Now just do it
    processIface(IFACE, H_TEMPLATE, CPP_TEMPLATE, H_DEST, CPP_DEST, DOCSTR_DEST)



if __name__ == '__main__':
    main(sys.argv)

#----------------------------------------------------------------------------

