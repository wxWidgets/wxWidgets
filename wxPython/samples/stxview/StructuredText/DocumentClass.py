##############################################################################
# 
# Zope Public License (ZPL) Version 1.0
# -------------------------------------
# 
# Copyright (c) Digital Creations.  All rights reserved.
# 
# This license has been certified as Open Source(tm).
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
# 1. Redistributions in source code must retain the above copyright
#    notice, this list of conditions, and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions, and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 
# 3. Digital Creations requests that attribution be given to Zope
#    in any manner possible. Zope includes a "Powered by Zope"
#    button that is installed by default. While it is not a license
#    violation to remove this button, it is requested that the
#    attribution remain. A significant investment has been put
#    into Zope, and this effort will continue if the Zope community
#    continues to grow. This is one way to assure that growth.
# 
# 4. All advertising materials and documentation mentioning
#    features derived from or use of this software must display
#    the following acknowledgement:
# 
#       "This product includes software developed by Digital Creations
#       for use in the Z Object Publishing Environment
#       (http://www.zope.org/)."
# 
#    In the event that the product being advertised includes an
#    intact Zope distribution (with copyright and license included)
#    then this clause is waived.
# 
# 5. Names associated with Zope or Digital Creations must not be used to
#    endorse or promote products derived from this software without
#    prior written permission from Digital Creations.
# 
# 6. Modified redistributions of any form whatsoever must retain
#    the following acknowledgment:
# 
#       "This product includes software developed by Digital Creations
#       for use in the Z Object Publishing Environment
#       (http://www.zope.org/)."
# 
#    Intact (re-)distributions of any official Zope release do not
#    require an external acknowledgement.
# 
# 7. Modifications are encouraged but must be packaged separately as
#    patches to official Zope releases.  Distributions that do not
#    clearly separate the patches from the original work must be clearly
#    labeled as unofficial distributions.  Modifications which do not
#    carry the name Zope may be packaged in any form, as long as they
#    conform to all of the clauses above.
# 
# 
# Disclaimer
# 
#    THIS SOFTWARE IS PROVIDED BY DIGITAL CREATIONS ``AS IS'' AND ANY
#    EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#    PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL DIGITAL CREATIONS OR ITS
#    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
#    USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
#    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
#    OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#    SUCH DAMAGE.
# 
# 
# This software consists of contributions made by Digital Creations and
# many individuals on behalf of Digital Creations.  Specific
# attributions are listed in the accompanying credits file.
# 
##############################################################################

import re, ST, STDOM
from string import split, join, replace, expandtabs, strip, find, rstrip

StringType=type('')
ListType=type([])

class StructuredTextExample(ST.StructuredTextParagraph):
    """Represents a section of document with literal text, as for examples"""

    def __init__(self, subs, **kw):
       t=[]; a=t.append
       for s in subs: a(s.getNodeValue())
       apply(ST.StructuredTextParagraph.__init__,
             (self, join(t,'\n\n'), ()),
             kw)

    def getColorizableTexts(self): return ()
    def setColorizableTexts(self, src): pass # never color examples

class StructuredTextBullet(ST.StructuredTextParagraph):
    """Represents a section of a document with a title and a body"""

class StructuredTextNumbered(ST.StructuredTextParagraph):
    """Represents a section of a document with a title and a body"""

class StructuredTextDescriptionTitle(ST.StructuredTextParagraph):
    """Represents a section of a document with a title and a body"""

class StructuredTextDescriptionBody(ST.StructuredTextParagraph):
    """Represents a section of a document with a title and a body"""

class StructuredTextDescription(ST.StructuredTextParagraph):
    """Represents a section of a document with a title and a body"""
    
    def __init__(self, title, src, subs, **kw):
       apply(ST.StructuredTextParagraph.__init__, (self, src, subs), kw)
       self._title=title

    def getColorizableTexts(self): return self._title, self._src
    def setColorizableTexts(self, src): self._title, self._src = src

    def getChildren(self):
       return (StructuredTextDescriptionTitle(self._title),
               StructuredTextDescriptionBody(self._src, self._subs))

class StructuredTextSectionTitle(ST.StructuredTextParagraph):
    """Represents a section of a document with a title and a body"""

class StructuredTextSection(ST.StructuredTextParagraph):
    """Represents a section of a document with a title and a body"""
    def __init__(self, src, subs=None, **kw):
       apply(ST.StructuredTextParagraph.__init__,
             (self, StructuredTextSectionTitle(src), subs),
             kw)

# a StructuredTextTable holds StructuredTextRows
class StructuredTextTable(ST.StructuredTextDocument):
    """
    rows is a list of lists containing tuples, which
    represent the columns/cells in each rows.
    EX
    rows = [[('row 1:column1',1)],[('row2:column1',1)]]
    """
    
    def __init__(self, rows, src, subs, **kw):
        apply(ST.StructuredTextDocument.__init__,(self,subs),kw)
        self._rows = []
        for row in rows:
            if row:
                self._rows.append(StructuredTextRow(row,kw))
    
    def getRows(self):
        return [self._rows]
    
    def _getRows(self):
        return self.getRows()
    
    def getColumns(self):
        result = []
        for row in self._rows:
            result.append(row.getColumns())
        return result
        
    def _getColumns(self):
        return self.getColumns()
    
    def setColumns(self,columns):
        for index in range(len(self._rows)):
            self._rows[index].setColumns(columns[index])
            
    def _setColumns(self,columns):
        return self.setColumns(columns)
             
    def getColorizableTexts(self):
        """
        return a tuple where each item is a column/cell's
        contents. The tuple, result, will be of this format.
        ("r1 col1", "r1=col2", "r2 col1", "r2 col2")
        """
        
        result = []
        for row in self._rows:
            for column in row.getColumns()[0]:
                result.append(column.getColorizableTexts()[0])
        return result
    
    def setColorizableTexts(self,texts):
        """
        texts is going to a tuple where each item is the
        result of being mapped to the colortext function.
        Need to insert the results appropriately into the
        individual columns/cells
        """
        for row_index in range(len(self._rows)):
            for column_index in range(len(self._rows[row_index]._columns)):
                self._rows[row_index]._columns[column_index].setColorizableTexts((texts[0],))
                texts = texts[1:]
        
    def _getColorizableTexts(self):
        return self.getColorizableTexts()
    
    def _setColorizableTexts(self):
        return self.setColorizableTexts()
    
# StructuredTextRow holds StructuredTextColumns
class StructuredTextRow(ST.StructuredTextDocument):
    
    def __init__(self,row,kw):
        """
        row is a list of tuples, where each tuple is
        the raw text for a cell/column and the span
        of that cell/column". 
        EX 
        [('this is column one',1), ('this is column two',1)]
        """
        
        apply(ST.StructuredTextDocument.__init__,(self,[]),kw)
        self._columns = []
        for column in row:            
            self._columns.append(StructuredTextColumn(column[0],column[1],kw))

    def getColumns(self):
        return [self._columns]

    def _getColumns(self):
        return [self._columns]
    
    def setColumns(self,columns):
        self._columns = columns
        
    def _setColumns(self,columns):
        return self.setColumns(columns)

# this holds the text of a table cell
class StructuredTextColumn(ST.StructuredTextParagraph):
    """
    StructuredTextColumn is a cell/column in a table.
    A cell can hold multiple paragraphs. The cell
    is either classified as a StructuredTextTableHeader
    or StructuredTextTableData.
    """
    
    def __init__(self,text,span,kw):
        # print "StructuredTextColumn", text, span
        apply(ST.StructuredTextParagraph.__init__,(self,text,[]),kw)
        self._span = span
    
    def getSpan(self):
        return self._span
    
    def _getSpan(self):
        return self._span

class StructuredTextTableHeader(ST.StructuredTextDocument): pass

class StructuredTextTableData(ST.StructuredTextDocument): pass

class StructuredTextMarkup(STDOM.Element):
    
    def __init__(self, v, **kw):
       self._value=v
       self._attributes=kw.keys()
       for k, v in kw.items(): setattr(self, k, v)

    def getChildren(self, type=type, lt=type([])):
       v=self._value
       if type(v) is not lt: v=[v]
       return v

    def getColorizableTexts(self): return self._value,
    def setColorizableTexts(self, v): self._value=v[0]

    def __repr__(self):
       return '%s(%s)' % (self.__class__.__name__, `self._value`)

class StructuredTextLiteral(StructuredTextMarkup):
    def getColorizableTexts(self): return ()
    def setColorizableTexts(self, v): pass

class StructuredTextEmphasis(StructuredTextMarkup): pass

class StructuredTextStrong(StructuredTextMarkup): pass

class StructuredTextInnerLink(StructuredTextMarkup): pass

class StructuredTextNamedLink(StructuredTextMarkup): pass

class StructuredTextUnderline(StructuredTextMarkup): pass

class StructuredTextSGML(StructuredTextMarkup): pass

class StructuredTextLink(StructuredTextMarkup): pass    

class DocumentClass:    
    """
    Class instance calls [ex.=> x()] require a structured text
    structure. Doc will then parse each paragraph in the structure
    and will find the special structures within each paragraph.
    Each special structure will be stored as an instance. Special
    structures within another special structure are stored within
    the 'top' structure
    EX : '-underline this-' => would be turned into an underline
    instance. '-underline **this**' would be stored as an underline
    instance with a strong instance stored in its string
    """
    
    #'doc_table',
    paragraph_types  = [
        'doc_bullet',
        'doc_numbered',
        'doc_description',
        'doc_header',
        'doc_table',
        ]
    
    #'doc_inner_link',
    #'doc_named_link',
    #'doc_underline',
    text_types = [
        'doc_href',
        'doc_strong',
        'doc_emphasize',
        'doc_literal',
        'doc_sgml'
        ]

    def __call__(self, doc):
        if type(doc) is type(''):
           doc=ST.StructuredText(doc)
           doc.setSubparagraphs(self.color_paragraphs(
              doc.getSubparagraphs()))
        else:
           doc=ST.StructuredTextDocument(self.color_paragraphs(
              doc.getSubparagraphs()))
        return doc

    def parse(self, raw_string, text_type,
              type=type, st=type(''), lt=type([])):

       """
       Parse accepts a raw_string, an expr to test the raw_string,
       and the raw_string's subparagraphs.
       
       Parse will continue to search through raw_string until 
       all instances of expr in raw_string are found. 
       
       If no instances of expr are found, raw_string is returned.
       Otherwise a list of substrings and instances is returned
       """

       tmp = []    # the list to be returned if raw_string is split
       append=tmp.append

       if type(text_type) is st: text_type=getattr(self, text_type)

       while 1:
          t = text_type(raw_string)
          if not t: break
          #an instance of expr was found
          t, start, end    = t

          if start: append(raw_string[0:start])

          tt=type(t)
          if tt is st:
             # if we get a string back, add it to text to be parsed
             raw_string = t+raw_string[end:len(raw_string)]
          else:
             if tt is lt:
                # is we get a list, append it's elements
                tmp[len(tmp):]=t
             else:
                # normal case, an object
                append(t)
             raw_string = raw_string[end:len(raw_string)]

       if not tmp: return raw_string # nothing found
       
       if raw_string: append(raw_string)
       elif len(tmp)==1: return tmp[0]
       
       return tmp


    def color_text(self, str, types=None):
       """Search the paragraph for each special structure
       """
       if types is None: types=self.text_types

       for text_type in types:

          if type(str) is StringType:
             str = self.parse(str, text_type)
          elif type(str) is ListType:
             r=[]; a=r.append
             for s in str:
                if type(s) is StringType:
                    s=self.parse(s, text_type)
                    if type(s) is ListType: r[len(r):]=s
                    else: a(s)
                else:
                    s.setColorizableTexts(
                       map(self.color_text,
                           s.getColorizableTexts()
                           ))
                    a(s)
             str=r
          else:
             r=[]; a=r.append; color=self.color_text
             for s in str.getColorizableTexts():
                color(s, (text_type,))
                a(s)
                
             str.setColorizableTexts(r)

       return str

    def color_paragraphs(self, raw_paragraphs,
                           type=type, sequence_types=(type([]), type(())),
                           st=type('')):
       result=[]
       for paragraph in raw_paragraphs:
          #print type(paragraph)
          if paragraph.getNodeName() != 'StructuredTextParagraph':
             result.append(paragraph)
             continue
          
          for pt in self.paragraph_types:
             if type(pt) is st:
                # grab the corresponding function
                pt=getattr(self, pt)
             # evaluate the paragraph
             r=pt(paragraph)
             if r:
                if type(r) not in sequence_types:
                    r=r,
                new_paragraphs=r
                for paragraph in new_paragraphs:
                    paragraph.setSubparagraphs(self.color_paragraphs(paragraph.getSubparagraphs()))
                break
          else:
             new_paragraphs=ST.StructuredTextParagraph(paragraph.getColorizableTexts()[0],
                                                          self.color_paragraphs(paragraph.getSubparagraphs()),
                                                          indent=paragraph.indent),
        
          # color the inline StructuredText types
          # for each StructuredTextParagraph
          for paragraph in new_paragraphs:
             
             if paragraph.getNodeName() is "StructuredTextTable":
                #print "we have a table"
                cells = paragraph.getColumns()
                text = paragraph.getColorizableTexts()
                text = map(ST.StructuredText,text)
                text = map(self.__call__,text)
                #for index in range(len(text)):
                #    text[index].setColorizableTexts(map(self.color_text,text[index].getColorizableTexts()))
                paragraph.setColorizableTexts(text)
                        
             paragraph.setColorizableTexts(
                map(self.color_text,
                    paragraph.getColorizableTexts()
                    ))
             result.append(paragraph)

       return result
    
    def doc_table(self, paragraph, expr = re.compile('\s*\|[-]+\|').match):
        text    = paragraph.getColorizableTexts()[0]
        m       = expr(text)
        
        subs = paragraph.getSubparagraphs()
        
        if not (m):
            return None
        rows = []
                                                                
        rows = split(text,'\n')        
        
        spans   = []
        ROWS    = []
        COLS    = []
    
        TDdivider = re.compile("[\-]+").match
        THdivider = re.compile("[\=]+").match
    
        # find where the column markers are located
        col = re.compile('\|').search
        text = strip(text)
        rows = split(text,'\n')
        for row in range(len(rows)):
            rows[row] = strip(rows[row])
    
        for row in rows:
            tmp = strip(row)
            tmp = row[1:len(tmp)-1] # remove leading and trailing |
            offset = 0
            if col(tmp):
                while col(tmp):
                    start,end = col(tmp).span()
                    if not start+offset in spans:
                        spans.append(start + offset)
                    COLS.append((tmp[0:start],start+offset))
                    tmp = " " + tmp[end:]
                    offset = offset + (start)
            if not offset+len(tmp) in spans:
                spans.append(offset+len(tmp))
            COLS.append((tmp,offset+len(tmp)))
            ROWS.append(COLS)
            COLS = []
    
        spans.sort()
    
        ROWS = ROWS[1:len(ROWS)]        
        
        # find each column span
        cols    = []
        tmp     = []
    
        for row in ROWS:
            for c in row:
                tmp.append(c[1])
            cols.append(tmp)
            tmp = []
    
        cur = 1     # the current column span
        tmp = []    
        C   = []    # holds the span of each cell
        for col in cols:
            for span in spans:
                if not span in col:
                    cur = cur + 1
                else:
                    tmp.append(cur)
                    cur = 1
            C.append(tmp)
            tmp = []
        
        # make rows contain the cell's text and the span
        # of that cell
        for index in range(len(C)):
            for i in range(len(C[index])):
                ROWS[index][i] = (ROWS[index][i][0],C[index][i])
        rows = ROWS
        
        # now munge the table cells together
        ROWS    = []
        COLS    = []
        for row in rows:
            for index in range(len(row)):
                if not COLS:
                    COLS = range(len(row))
                    for i in range(len(COLS)):
                        COLS[i] = ["",1]
                if TDdivider(row[index][0]) or THdivider(row[index][0]):
                    ROWS.append(COLS)
                    COLS = []
                else:
                    COLS[index][0] = COLS[index][0] + rstrip(row[index][0]) + "\n"
                    COLS[index][1] = row[index][1]
        return StructuredTextTable(ROWS,text,subs,indent=paragraph.indent)
            
    def doc_bullet(self, paragraph, expr = re.compile('\s*[-*o]\s+').match):
        top=paragraph.getColorizableTexts()[0]
        m=expr(top)

        if not m:
            return None
            
        subs=paragraph.getSubparagraphs()
        if top[-2:]=='::':
           subs=[StructuredTextExample(subs)]
           top=top[:-1]
        return StructuredTextBullet(top[m.span()[1]:], subs,
                                     indent=paragraph.indent,
                                     bullet=top[:m.span()[1]]
                                     )

    def doc_numbered(
        self, paragraph,
        expr = re.compile('(\s*[a-zA-Z]+\.)|(\s*[0-9]+\.)|(\s*[0-9]+\s+)').match):
        
        # This is the old expression. It had a nasty habit
        # of grabbing paragraphs that began with a single
        # letter word even if there was no following period.
        
        #expr = re.compile('\s*'
        #                   '(([a-zA-Z]|[0-9]+|[ivxlcdmIVXLCDM]+)\.)*'
        #                   '([a-zA-Z]|[0-9]+|[ivxlcdmIVXLCDM]+)\.?'
        #                   '\s+').match):
        
        top=paragraph.getColorizableTexts()[0]
        m=expr(top)
        if not m: return None
        subs=paragraph.getSubparagraphs()
        if top[-2:]=='::':
           subs=[StructuredTextExample(subs)]
           top=top[:-1]
        return StructuredTextNumbered(top[m.span()[1]:], subs,
                                        indent=paragraph.indent,
                                        number=top[:m.span()[1]])

    def doc_description(
        self, paragraph,
        delim = re.compile('\s+--\s+').search,
        nb=re.compile(r'[^\0- ]').search,
        ):

        top=paragraph.getColorizableTexts()[0]
        d=delim(top)
        if not d: return None
        start, end = d.span()
        title=top[:start]
        if find(title, '\n') >= 0: return None
        if not nb(title): return None
        d=top[start:end]
        top=top[end:]

        subs=paragraph.getSubparagraphs()
        if top[-2:]=='::':
           subs=[StructuredTextExample(subs)]
           top=top[:-1]

        return StructuredTextDescription(
           title, top, subs,
           indent=paragraph.indent,
           delim=d)

    def doc_header(self, paragraph,
                    expr    = re.compile('[ a-zA-Z0-9.:/,-_*<>\?\'\"]+').match
                    ):
        subs=paragraph.getSubparagraphs()
        if not subs: return None
        top=paragraph.getColorizableTexts()[0]
        if not strip(top): return None
        if top[-2:]=='::':
           subs=StructuredTextExample(subs)
           if strip(top)=='::': return subs
           return ST.StructuredTextParagraph(
              top[:-1], [subs], indent=paragraph.indent)

        if find(top,'\n') >= 0: return None
        return StructuredTextSection(top, subs, indent=paragraph.indent)

    def doc_literal(
        self, s,
        expr=re.compile(
          "(?:\s|^)'"                                                  # open
          "([^ \t\n\r\f\v']|[^ \t\n\r\f\v'][^\n']*[^ \t\n\r\f\v'])" # contents
          "'(?:\s|[,.;:!?]|$)"                                        # close
          ).search):
        
        r=expr(s)
        if r:
           start, end = r.span(1)
           return (StructuredTextLiteral(s[start:end]), start-1, end+1)
        else:
           return None

    def doc_emphasize(
        self, s,
        expr = re.compile('\s*\*([ \na-zA-Z0-9.:/;,\'\"\?]+)\*(?!\*|-)').search
        ):

        r=expr(s)
        if r:
           start, end = r.span(1)
           return (StructuredTextEmphasis(s[start:end]), start-1, end+1)
        else:
           return None
    
    def doc_inner_link(self,
                       s,
                       expr1 = re.compile("\.\.\s*").search,
                       expr2 = re.compile("\[[a-zA-Z0-9]+\]").search):
        
        # make sure we dont grab a named link
        if expr2(s) and expr1(s):
            start1,end1 = expr1(s).span()
            start2,end2 = expr2(s).span()
            if end1 == start2:
                # uh-oh, looks like a named link
                return None
            else:
                # the .. is somewhere else, ignore it
                return (StructuredTextInnerLink(s[start2+1,end2-1],start2,end2))
            return None
        elif expr2(s) and not expr1(s):
            start,end = expr2(s).span()
            return (StructuredTextInnerLink(s[start+1:end-1]),start,end)
        return None
    
    def doc_named_link(self,
                       s,
                       expr=re.compile("(\.\.\s)(\[[a-zA-Z0-9]+\])").search):
        
        result = expr(s)
        if result:
            start,end   = result.span(2)
            a,b = result.span(1)
            str = strip(s[a:b]) + s[start:end]
            st,en       = result.span()
            return (StructuredTextNamedLink(str),st,en)
            #return (StructuredTextNamedLink(s[st:en]),st,en)
        return None
    
    def doc_underline(self,
                      s,
                      expr=re.compile("\_([a-zA-Z0-9\s\.,\?]+)\_").search):
        
        result = expr(s)
        if result:
            start,end = result.span(1)
            st,e = result.span()
            return (StructuredTextUnderline(s[start:end]),st,e)
        else:
            return None
    
    def doc_strong(self, 
                   s,
        expr = re.compile('\s*\*\*([ \na-zA-Z0-9.:/;\-,!\?\'\"]+)\*\*').search
        ):

        r=expr(s)
        if r:
           start, end = r.span(1)
           return (StructuredTextStrong(s[start:end]), start-2, end+2)
        else:
           return None
    
    def doc_href(
        
        self, s,
        expr1 = re.compile("(\"[ a-zA-Z0-9\n\-\.\,\;\(\)\/\:\/]+\")(:)([a-zA-Z0-9\:\/\.\~\-]+)([,]*\s*)").search,
        expr2 = re.compile('(\"[ a-zA-Z0-9\n\-\.\:\;\(\)\/]+\")([,]+\s+)([a-zA-Z0-9\@\.\,\?\!\/\:\;\-\#]+)(\s*)').search):
        
        punctuation = re.compile("[\,\.\?\!\;]+").match
        r=expr1(s) or expr2(s)

        if r:
            # need to grab the href part and the
            # beginning part
                        
            start,e = r.span(1)
            name    = s[start:e]
            name    = replace(name,'"','',2)
            #start   = start + 1
            st,end   = r.span(3)
            if punctuation(s[end-1:end]):
                end = end -1
            link    = s[st:end]
            #end     = end - 1                        
            
            # name is the href title, link is the target
            # of the href
            return (StructuredTextLink(name, href=link),
                    start, end)
            
            #return (StructuredTextLink(s[start:end], href=s[start:end]),
            #        start, end)
        else:
            return None
    
    def doc_sgml(self,s,expr=re.compile("\<[a-zA-Z0-9\.\=\'\"\:\/\-\#\+\s]+\>").search):
        """
        SGML text is ignored and outputed as-is
        """
        r = expr(s)
        if r:
            start,end = r.span()
            text = s[start:end]
            return (StructuredTextSGML(text),start,end)
