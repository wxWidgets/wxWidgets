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


import sys, string, re
from fileinput import FileInput


IFACE         = './scintilla/include/Scintilla.iface'
H_TEMPLATE    = './stc.h.in'
CPP_TEMPLATE  = './stc.cpp.in'
H_DEST        = '../../include/wx/stc/stc.h' # './stc_test.h' #
CPP_DEST      = './stc.cpp' #'./stc_test.cpp'


# Value prefixes to convert
valPrefixes = [('SCI_', ''),
               ('SC_',  ''),
               ('SCN_', None),  # just toss these...
               ('SCEN_', None),
               ('SCE_', ''),
               ('SCLEX_', 'LEX_'),
               ('SCK_', 'KEY_'),
               ('SCFIND_', 'FIND_'),
               ('SCWS_', 'WS_'),
]

# Message funcion values that should have a CMD_ constant as well
cmdValues = [ (2300, 2350), 2011, 2013, (2176, 2180) ]


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
                    SendMsg(%s, text.Len(), (long)text.c_str());''',
                 0),

    'AddStyledText' : (0,
                       'void %s(const wxString& text);',

                       '''void %s(const wxString& text) {
                          SendMsg(%s, text.Len(), (long)text.c_str());''',
                       0),

    'GetViewWS' : ( 'GetViewWhiteSpace', 0, 0, 0),
    'SetViewWS' : ( 'SetViewWhiteSpace', 0, 0, 0),

    'GetStyledText' : (0,
                       'wxString %s(int startPos, int endPos);',

                       '''wxString %s(int startPos, int endPos) {
                          wxString text;
                          int len = endPos - startPos;
                          TextRange tr;
                          tr.lpstrText = text.GetWriteBuf(len*2+1);
                          tr.chrg.cpMin = startPos;
                          tr.chrg.cpMax = endPos;
                          SendMsg(%s, 0, (long)&tr);
                          text.UngetWriteBuf(len*2);
                          return text;''',

                       ('Retrieve a buffer of cells.',)),


    'PositionFromPoint' : (0,
                           'int %s(wxPoint pt);',

                           '''int %s(wxPoint pt) {
                              return SendMsg(%s, pt.x, pt.y);''',

                           0),

    'GetCurLine' : (0,
                    'wxString %s(int* OUTPUT=NULL);',

                    '''wxString %s(int* linePos) {
                       wxString text;
                       int len = LineLength(GetCurrentLine());
                       char* buf = text.GetWriteBuf(len+1);

                       int pos = SendMsg(%s, len, (long)buf);
                       text.UngetWriteBuf();
                       if (linePos)  *linePos = pos;

                       return text;''',

                    0),

    'SetUsePalette' : (None, 0,0,0),

    'MarkerSetFore' : ('MarkerSetForeground', 0, 0, 0),
    'MarkerSetBack' : ('MarkerSetBackground', 0, 0, 0),

    'MarkerDefine' : (0,
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
                       'and optionally the for and background colours.')),

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

    # need to fix this to map between wx and scintilla encoding flags, leave it out for now...
    'StyleSetCharacterSet' : (None, 0, 0, 0),

    'AssignCmdKey' : ('CmdKeyAssign',
                      'void %s(int key, int modifiers, int cmd);',

                      '''void %s(int key, int modifiers, int cmd) {
                          SendMsg(%s, MAKELONG(key, modifiers), cmd);''',

                      0),

    'ClearCmdKey' : ('CmdKeyClear',
                      'void %s(int key, int modifiers);',

                      '''void %s(int key, int modifiers) {
                          SendMsg(%s, MAKELONG(key, modifiers));''',

                      0),

    'ClearAllCmdKeys' : ('CmdKeyClearAll', 0, 0, 0),


    'SetStylingEx' : ('SetStyleBytes',
                      'void %s(int length, char* styleBytes);',

                      '''void %s(int length, char* styleBytes) {
                          SendMsg(%s, length, (long)styleBytes);''',

                      0),


    'IndicSetStyle' : ('IndicatorSetStyle', 0, 0, 0),
    'IndicGetStyle' : ('IndicatorGetStyle', 0, 0, 0),
    'IndicSetFore' : ('IndicatorSetForeground', 0, 0, 0),
    'IndicGetFore' : ('IndicatorGetForeground', 0, 0, 0),

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

    'SetHScrollBar' : ('SetUseHorizontalScrollBar', 0, 0, 0),
    'GetHScrollBar' : ('GetUseHorizontalScrollBar', 0, 0, 0),

    'GetCaretFore' : ('GetCaretForeground', 0, 0, 0),

    'GetUsePalette' : (None, 0, 0, 0),

    'FindText' : (0,
                  '''int %s(int minPos, int maxPos,
                               const wxString& text,
                               bool caseSensitive, bool wholeWord);''',
                  '''int %s(int minPos, int maxPos,
                               const wxString& text,
                               bool caseSensitive, bool wholeWord) {
                     TextToFind  ft;
                     int         flags = 0;

                     flags |= caseSensitive ? SCFIND_MATCHCASE : 0;
                     flags |= wholeWord     ? SCFIND_WHOLEWORD : 0;
                     ft.chrg.cpMin = minPos;
                     ft.chrg.cpMax = maxPos;
                     ft.lpstrText = (char*)text.c_str();

                     return SendMsg(%s, flags, (long)&ft);''',
                  0),

    'FormatRange' : (0,
                     '''int %s(bool   doDraw,
                               int    startPos,
                               int    endPos,
                               wxDC*  draw,
                               wxDC*  target,  // Why does it use two? Can they be the same?
                               wxRect renderRect,
                               wxRect pageRect);''',
                     ''' int %s(bool   doDraw,
                                int    startPos,
                                int    endPos,
                                wxDC*  draw,
                                wxDC*  target,  // Why does it use two? Can they be the same?
                                wxRect renderRect,
                                wxRect pageRect) {
                            RangeToFormat fr;

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


    'GetLine' : (0,
                    'wxString %s(int line);',

                    '''wxString %s(int line) {
                       wxString text;
                       int len = LineLength(line);
                       char* buf = text.GetWriteBuf(len+1);

                       int pos = SendMsg(%s, line, (long)buf);
                       text.UngetWriteBuf();

                       return text;''',

                    ('Retrieve the contents of a line.',)),

    'SetSel' : ('SetSelection', 0, 0, 0),
    'GetSelText' : ('GetSelectedText',
                    'wxString %s();',

                    '''wxString %s() {
                            wxString text;
                            int   start;
                            int   end;

                            GetSelection(&start, &end);
                            int   len  = end - start;
                            char* buff = text.GetWriteBuf(len+1);

                            SendMsg(%s, 0, (long)buff);
                            text.UngetWriteBuf();
                            return text;''',

                    ('Retrieve the selected text.',)),

    'GetTextRange' : (0,
                      'wxString %s(int startPos, int endPos);',

                      '''wxString %s(int startPos, int endPos) {
                            wxString text;
                            int   len  = endPos - startPos;
                            char* buff = text.GetWriteBuf(len+1);
                            TextRange tr;
                            tr.lpstrText = buff;
                            tr.chrg.cpMin = startPos;
                            tr.chrg.cpMax = endPos;

                            SendMsg(%s, 0, (long)&tr);
                            text.UngetWriteBuf();
                            return text;''',

                       ('Retrieve a range of text.',)),

    'PointXFromPosition' : (None, 0, 0, 0),
    'PointYFromPosition' : (None, 0, 0, 0),

    'ScrollCaret' : ('EnsureCaretVisible', 0, 0, 0),
    'ReplaceSel' : ('ReplaceSelection', 0, 0, 0),
    'Null' : (None, 0, 0, 0),

    'GetText' : (0,
                 'wxString %s();',

                 '''wxString %s() {
                        wxString text;
                        int   len  = GetTextLength();
                        char* buff = text.GetWriteBuf(len+1);

                        SendMsg(%s, len, (long)buff);
                        buff[len] = 0;
                        text.UngetWriteBuf();
                        return text;''',

                 ('Retrieve all the text in the document.', )),

    'GetDirectFunction' : (None, 0, 0, 0),
    'GetDirectPointer' : (None, 0, 0, 0),

    'CallTipPosStart' : ('CallTipPosAtStart', 0, 0, 0),
    'CallTipSetHlt' : ('CallTipSetHighlight', 0, 0, 0),
    'CallTipSetBack' : ('CallTipSetBackground', 0, 0, 0),


    # Remove all methods that are key commands since they can be
    # executed with CmdKeyExecute
    'LineDown' : (None, 0, 0, 0),
    'LineDownExtend' : (None, 0, 0, 0),
    'LineUp' : (None, 0, 0, 0),
    'LineUpExtend' : (None, 0, 0, 0),
    'CharLeft' : (None, 0, 0, 0),
    'CharLeftExtend' : (None, 0, 0, 0),
    'CharRight' : (None, 0, 0, 0),
    'CharRightExtend' : (None, 0, 0, 0),
    'WordLeft' : (None, 0, 0, 0),
    'WordLeftExtend' : (None, 0, 0, 0),
    'WordRight' : (None, 0, 0, 0),
    'WordRightExtend' : (None, 0, 0, 0),
    'Home' : (None, 0, 0, 0),
    'HomeExtend' : (None, 0, 0, 0),
    'LineEnd' : (None, 0, 0, 0),
    'LineEndExtend' : (None, 0, 0, 0),
    'DocumentStart' : (None, 0, 0, 0),
    'DocumentStartExtend' : (None, 0, 0, 0),
    'DocumentEnd' : (None, 0, 0, 0),
    'DocumentEndExtend' : (None, 0, 0, 0),
    'PageUp' : (None, 0, 0, 0),
    'PageUpExtend' : (None, 0, 0, 0),
    'PageDown' : (None, 0, 0, 0),
    'PageDownExtend' : (None, 0, 0, 0),
    'EditToggleOvertype' : (None, 0, 0, 0),
    'Cancel' : (None, 0, 0, 0),
    'DeleteBack' : (None, 0, 0, 0),
    'Tab' : (None, 0, 0, 0),
    'BackTab' : (None, 0, 0, 0),
    'NewLine' : (None, 0, 0, 0),
    'FormFeed' : (None, 0, 0, 0),
    'VCHome' : (None, 0, 0, 0),
    'VCHomeExtend' : (None, 0, 0, 0),
    'ZoomIn' : (None, 0, 0, 0),
    'ZoomOut' : (None, 0, 0, 0),
    'DelWordLeft' : (None, 0, 0, 0),
    'DelWordRight' : (None, 0, 0, 0),
    'LineCut' : (None, 0, 0, 0),
    'LineDelete' : (None, 0, 0, 0),
    'LineTranspose' : (None, 0, 0, 0),
    'LowerCase' : (None, 0, 0, 0),
    'UpperCase' : (None, 0, 0, 0),
    'LineScrollDown' : (None, 0, 0, 0),
    'LineScrollUp' : (None, 0, 0, 0),


    'GetDocPointer' : (0,
                       'void* %s();',
                       '''void* %s() {
                           return (void*)SendMsg(%s);''',
                       0),

    'SetDocPointer' : (0,
                       'void %s(void* docPointer);',
                       '''void %s(void* docPointer) {
                           SendMsg(%s, (long)docPointer);''',
                       0),

    'CreateDocument' : (0,
                       'void* %s();',
                       '''void* %s() {
                           return (void*)SendMsg(%s);''',
                        0),

    'AddRefDocument' : (0,
                       'void %s(void* docPointer);',
                       '''void %s(void* docPointer) {
                           SendMsg(%s, (long)docPointer);''',
                        0),

    'ReleaseDocument' : (0,
                       'void %s(void* docPointer);',
                       '''void %s(void* docPointer) {
                           SendMsg(%s, (long)docPointer);''',
                         0),

    'GrabFocus' : (None, 0, 0, 0),

    '' : ('', 0, 0, 0),

    }

#----------------------------------------------------------------------------

def processIface(iface, h_tmplt, cpp_tmplt, h_dest, cpp_dest):
    curDocStrings = []
    values = []
    methods = []

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
            parseFun(line[4:], methods, curDocStrings, values)
            curDocStrings = []

        elif op == 'cat ':
            if string.strip(line[4:]) == 'Deprecated':
                break    # skip the rest of the file

        elif op == 'evt ':
            pass

        else:
            print '***** Unknown line type: ', line


    # process templates
    data = {}
    data['VALUES'] = processVals(values)
    defs, imps = processMethods(methods)
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

    for retType, name, number, param1, param2, docs in methods:
        retType = retTypeMap.get(retType, retType)
        params = makeParamString(param1, param2)

        name, theDef, theImp, docs = checkMethodOverride(name, number, docs)

        if name is None:
            continue

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


    return string.join(defs, '\n'), string.join(imps, '\n')


#----------------------------------------------------------------------------

def checkMethodOverride(name, number, docs):
    theDef = theImp = None
    if methodOverrideMap.has_key(name):
        item = methodOverrideMap[name]

        if item[0] != 0:
            name = item[0]
        if item[1] != 0:
            theDef = '    ' + (item[1] % name)
        if item[2] != 0:
            theImp = item[2] % ('wxStyledTextCtrl::'+name, number) + '\n}'
        if item[3] != 0:
            docs = item[3]

    return name, theDef, theImp, docs

#----------------------------------------------------------------------------

def makeArgString(param):
    if not param:
        return '0'

    typ, name = param

    if typ == 'string':
        return '(long)%s.c_str()' % name
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

    # Special case.  For the key command functionss we want a value defined too
    num = string.atoi(number)
    for v in cmdValues:
        if (type(v) == type(()) and v[0] <= num < v[1]) or v == num:
            parseVal('CMD_%s=%s' % (string.upper(name), number), values, ())

    #if retType == 'void' and not param1 and not param2:

    methods.append( (retType, name, number, param1, param2, tuple(docs)) )


#----------------------------------------------------------------------------


def main(args):
    # TODO: parse command line args to replace default input/output files???

    # Now just do it
    processIface(IFACE, H_TEMPLATE, CPP_TEMPLATE, H_DEST, CPP_DEST)



if __name__ == '__main__':
    main(sys.argv)

#----------------------------------------------------------------------------


