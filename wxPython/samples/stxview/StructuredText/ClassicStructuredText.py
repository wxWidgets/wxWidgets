#! /usr/bin/env python -- # -*- python -*-
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
#      "This product includes software developed by Digital Creations
#      for use in the Z Object Publishing Environment
#      (http://www.zope.org/)."
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
#      "This product includes software developed by Digital Creations
#      for use in the Z Object Publishing Environment
#      (http://www.zope.org/)."
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
#   THIS SOFTWARE IS PROVIDED BY DIGITAL CREATIONS ``AS IS'' AND ANY
#   EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL DIGITAL CREATIONS OR ITS
#   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
#   USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
#   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
#   OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#   SUCH DAMAGE.
# 
# 
# This software consists of contributions made by Digital Creations and
# many individuals on behalf of Digital Creations.  Specific
# attributions are listed in the accompanying credits file.
# 
##############################################################################
'''Structured Text Manipulation

Parse a structured text string into a form that can be used with 
structured formats, like html.

Structured text is text that uses indentation and simple
symbology to indicate the structure of a document.  

A structured string consists of a sequence of paragraphs separated by
one or more blank lines.  Each paragraph has a level which is defined
as the minimum indentation of the paragraph.  A paragraph is a
sub-paragraph of another paragraph if the other paragraph is the last
preceding paragraph that has a lower level.

Special symbology is used to indicate special constructs:

- A single-line paragraph whose immediately succeeding paragraphs are lower
  level is treated as a header.

- A paragraph that begins with a '-', '*', or 'o' is treated as an
  unordered list (bullet) element.

- A paragraph that begins with a sequence of digits followed by a
  white-space character is treated as an ordered list element.

- A paragraph that begins with a sequence of sequences, where each
  sequence is a sequence of digits or a sequence of letters followed
  by a period, is treated as an ordered list element.

- A paragraph with a first line that contains some text, followed by
  some white-space and '--' is treated as
  a descriptive list element. The leading text is treated as the
  element title.

- Sub-paragraphs of a paragraph that ends in the word 'example' or the
  word 'examples', or '::' is treated as example code and is output as is.

- Text enclosed single quotes (with white-space to the left of the
  first quote and whitespace or punctuation to the right of the second quote)
  is treated as example code.

- Text surrounded by '*' characters (with white-space to the left of the
  first '*' and whitespace or punctuation to the right of the second '*')
  is emphasized.

- Text surrounded by '**' characters (with white-space to the left of the
  first '**' and whitespace or punctuation to the right of the second '**')
  is made strong.

- Text surrounded by '_' underscore characters (with whitespace to the left 
  and whitespace or punctuation to the right) is made underlined.

- Text encloded by double quotes followed by a colon, a URL, and concluded
  by punctuation plus white space, *or* just white space, is treated as a
  hyper link. For example:

    "Zope":http://www.zope.org/ is ...

  Is interpreted as '<a href="http://www.zope.org/">Zope</a> is ....'
  Note: This works for relative as well as absolute URLs.

- Text enclosed by double quotes followed by a comma, one or more spaces,
  an absolute URL and concluded by punctuation plus white space, or just
  white space, is treated as a hyper link. For example: 

    "mail me", mailto:amos@digicool.com.

  Is interpreted as '<a href="mailto:amos@digicool.com">mail me</a>.' 

- Text enclosed in brackets which consists only of letters, digits,
  underscores and dashes is treated as hyper links within the document.
  For example:
    
    As demonstrated by Smith [12] this technique is quite effective.

  Is interpreted as '... by Smith <a href="#12">[12]</a> this ...'. Together
  with the next rule this allows easy coding of references or end notes.

- Text enclosed in brackets which is preceded by the start of a line, two
  periods and a space is treated as a named link. For example:

    .. [12] "Effective Techniques" Smith, Joe ... 

  Is interpreted as '<a name="12">[12]</a> "Effective Techniques" ...'.
  Together with the previous rule this allows easy coding of references or
  end notes. 


- A paragraph that has blocks of text enclosed in '||' is treated as a
  table. The text blocks correspond to table cells and table rows are
  denoted by newlines. By default the cells are center aligned. A cell
  can span more than one column by preceding a block of text with an
  equivalent number of cell separators '||'. Newlines and '|' cannot
  be a part of the cell text. For example:

      |||| **Ingredients** ||
      || *Name* || *Amount* ||
      ||Spam||10||
      ||Eggs||3||

  is interpreted as::

    <TABLE BORDER=1 CELLPADDING=2>
     <TR>
      <TD ALIGN=CENTER COLSPAN=2> <strong>Ingredients</strong> </TD>
     </TR>
     <TR>
      <TD ALIGN=CENTER COLSPAN=1> <em>Name</em> </TD>
      <TD ALIGN=CENTER COLSPAN=1> <em>Amount</em> </TD>
     </TR>
     <TR>
      <TD ALIGN=CENTER COLSPAN=1>Spam</TD>
      <TD ALIGN=CENTER COLSPAN=1>10</TD>
     </TR>
     <TR>
      <TD ALIGN=CENTER COLSPAN=1>Eggs</TD>
      <TD ALIGN=CENTER COLSPAN=1>3</TD>
     </TR>
    </TABLE>

'''

import ts_regex
import regex
from ts_regex import gsub
from string import split, join, strip, find
import string,re


def untabify(aString,
             indent_tab=ts_regex.compile('\(\n\|^\)\( *\)\t').search_group,
             ):
    '''\
    Convert indentation tabs to spaces.
    '''
    result=''
    rest=aString
    while 1:
        ts_results = indent_tab(rest, (1,2))
        if ts_results:
            start, grps = ts_results
            lnl=len(grps[0])
            indent=len(grps[1])
            result=result+rest[:start]
            rest="\n%s%s" % (' ' * ((indent/8+1)*8),
                             rest[start+indent+1+lnl:])
        else:
            return result+rest

def indent(aString, indent=2):
    """Indent a string the given number of spaces"""
    r=split(untabify(aString),'\n')
    if not r: return ''
    if not r[-1]: del r[-1]
    tab=' '*level
    return "%s%s\n" % (tab,join(r,'\n'+tab))

def reindent(aString, indent=2, already_untabified=0):
    "reindent a block of text, so that the minimum indent is as given"

    if not already_untabified: aString=untabify(aString)

    l=indent_level(aString)[0]
    if indent==l: return aString

    r=[]

    append=r.append

    if indent > l:
        tab=' ' * (indent-l)
        for s in split(aString,'\n'): append(tab+s)
    else:
        l=l-indent
        for s in split(aString,'\n'): append(s[l:])

    return join(r,'\n')

def indent_level(aString,
                 indent_space=ts_regex.compile('\n\( *\)').search_group,
                 ):
    '''\
    Find the minimum indentation for a string, not counting blank lines.
    '''
    start=0
    text='\n'+aString
    indent=l=len(text)
    while 1:

        ts_results = indent_space(text, (1,2), start)
        if ts_results:
            start, grps = ts_results
            i=len(grps[0])
            start=start+i+1
            if start < l and text[start] != '\n':       # Skip blank lines
                if not i: return (0,aString)
                if i < indent: indent = i
        else:
            return (indent,aString)

def paragraphs(list,start):
    l=len(list)
    level=list[start][0]
    i=start+1
    while i < l and list[i][0] > level: i=i+1
    return i-1-start

def structure(list):
    if not list: return []
    i=0
    l=len(list)
    r=[]
    while i < l:
        sublen=paragraphs(list,i)
        i=i+1
        r.append((list[i-1][1],structure(list[i:i+sublen])))
        i=i+sublen
    return r


class Table:
    CELL='  <TD ALIGN=CENTER COLSPAN=%i>%s</TD>\n'
    ROW=' <TR>\n%s </TR>\n'
    TABLE='\n<TABLE BORDER=1 CELLPADDING=2>\n%s</TABLE>'
    
    def create(self,aPar,
        td_reg=re.compile(r'[ \t\n]*\|\|([^\0x00|]*)')
        ):
        '''parses a table and returns nested list representing the
        table'''
        self.table=[]
        text=filter(None,split(aPar,'\n'))
        for line in text:
            row=[]
            while 1:
                mo =  td_reg.match(line)
                if not mo: return 0
                pos = mo.end(1)
                row.append(mo.group(1))
                if pos==len(line):break
                line=line[pos:]
            self.table.append(row)
        return 1

    def html(self):
        '''Creates an HTML representation of table'''
        htmltable=[]
        for row in self.table:
            htmlrow=[]
            colspan=1
            for cell in row:
                if cell=='':
                    colspan=colspan+1
                    continue
                else:
                    htmlrow.append(self.CELL%(colspan,cell))
                    colspan=1
            htmltable.append(self.ROW%join(htmlrow,''))
        return self.TABLE%join(htmltable,'')

table=Table()

class StructuredText:

    """Model text as structured collection of paragraphs.

    Structure is implied by the indentation level.

    This class is intended as a base classes that do actual text
    output formatting.
    """

    def __init__(self, aStructuredString, level=0,
                 paragraph_divider=regex.compile('\(\r?\n *\)+\r?\n'),
                 ):
        '''Convert a structured text string into a structured text object.

        Aguments:

          aStructuredString -- The string to be parsed.
          level -- The level of top level headings to be created.
        '''


        pat = ' \"([%s0-9-_,./?=@~&]*)\":' % string.letters+ \
              '([-:%s0-9_,./?=@#~&]*?)' % string.letters + \
              '([.:?;] )' 

        p_reg = re.compile(pat,re.M)
                
        aStructuredString = p_reg.sub(r'<a href="\2">\1</a>\3 ' , aStructuredString)

        pat = ' \"([%s0-9-_,./?=@~&]*)\", ' % string.letters+ \
              '([-:%s0-9_,./?=@#~&]*?)' % string.letters + \
              '([.:?;] )' 

        p_reg = re.compile(pat,re.M)

        aStructuredString = p_reg.sub(r'<a href="\2">\1</a>\3 ' , aStructuredString)


        protoless = find(aStructuredString, '<a href=":')
        if protoless != -1:
            aStructuredString = re.sub('<a href=":', '<a href="',
                                     aStructuredString)

        self.level=level
        paragraphs=ts_regex.split(untabify(aStructuredString),
                                  paragraph_divider)
        paragraphs=map(indent_level,paragraphs)

        self.structure=structure(paragraphs)


    def __str__(self):
        return str(self.structure)


ctag_prefix=r'([\x00- \\(]|^)' 
ctag_suffix=r'([\x00- ,.:;!?\\)]|$)'         
ctag_middle=r'[%s]([^\x00- %s][^%s]*[^\x00- %s]|[^%s])[%s]' 
ctag_middl2=r'[%s][%s]([^\x00- %s][^%s]*[^\x00- %s]|[^%s])[%s][%s]'    

def ctag(s,
         em=re.compile(
             ctag_prefix+(ctag_middle % (("*",)*6) )+ctag_suffix),
         strong=re.compile(
             ctag_prefix+(ctag_middl2 % (("*",)*8))+ctag_suffix),
         under=re.compile(
             ctag_prefix+(ctag_middle % (("_",)*6) )+ctag_suffix),
         code=re.compile(
             ctag_prefix+(ctag_middle % (("\'",)*6))+ctag_suffix),
         ):
    if s is None: s=''
    s=strong.sub(r'\1<strong>\2</strong>\3',s)
    s=under.sub( r'\1<u>\2</u>\3',s)
    s=code.sub(  r'\1<code>\2</code>\3',s)
    s=em.sub(    r'\1<em>\2</em>\3',s)
    return s    

class HTML(StructuredText):

    '''\
    An HTML structured text formatter.
    '''\

    def __str__(self,
                extra_dl=re.compile("</dl>\n<dl>"),
                extra_ul=re.compile("</ul>\n<ul>"),
                extra_ol=re.compile("</ol>\n<ol>"),
                ):
        '''\
        Return an HTML string representation of the structured text data.

        '''
        s=self._str(self.structure,self.level)
        s=extra_dl.sub('\n',s)
        s=extra_ul.sub('\n',s)
        s=extra_ol.sub('\n',s)
        return s

    def ul(self, before, p, after):
        if p: p="<p>%s</p>" % strip(ctag(p))
        return ('%s<ul><li>%s\n%s\n</li></ul>\n'
                % (before,p,after))

    def ol(self, before, p, after):
        if p: p="<p>%s</p>" % strip(ctag(p))
        return ('%s<ol><li>%s\n%s\n</li></ol>\n'
                % (before,p,after))

    def dl(self, before, t, d, after):
        return ('%s<dl><dt>%s</dt><dd><p>%s</p>\n%s\n</dd></dl>\n'
                % (before,ctag(t),ctag(d),after))

    def head(self, before, t, level, d):
        if level > 0 and level < 6:
            return ('%s<h%d>%s</h%d>\n%s\n'
                    % (before,level,strip(ctag(t)),level,d))
            
        t="<p><strong>%s</strong></p>" % strip(ctag(t))
        return ('%s<dl><dt>%s\n</dt><dd>%s\n</dd></dl>\n'
                % (before,t,d))

    def normal(self,before,p,after):
        return '%s<p>%s</p>\n%s\n' % (before,ctag(p),after)

    def pre(self,structure,tagged=0):
        if not structure: return ''
        if tagged:
            r=''
        else:
            r='<PRE>\n'
        for s in structure:
            r="%s%s\n\n%s" % (r,html_quote(s[0]),self.pre(s[1],1))
        if not tagged: r=r+'</PRE>\n'
        return r
    
    def table(self,before,table,after):
        return '%s<p>%s</p>\n%s\n' % (before,ctag(table),after)
    
    def _str(self,structure,level,
             # Static
             bullet=ts_regex.compile('[ \t\n]*[o*-][ \t\n]+\([^\0]*\)'
                                     ).match_group,
             example=ts_regex.compile('[\0- ]examples?:[\0- ]*$'
                                      ).search,
             dl=ts_regex.compile('\([^\n]+\)[ \t]+--[ \t\n]+\([^\0]*\)'
                                 ).match_group,
             nl=ts_regex.compile('\n').search,
             ol=ts_regex.compile(
                 '[ \t]*\(\([0-9]+\|[%s]+\)[.)]\)+[ \t\n]+\([^\0]*\|$\)' % string.letters
                 ).match_group,
             olp=ts_regex.compile('[ \t]*([0-9]+)[ \t\n]+\([^\0]*\|$\)'
                                  ).match_group,
             ):
        r=''
        for s in structure:

            ts_results = bullet(s[0], (1,))
            if ts_results:
                p = ts_results[1]
                if s[0][-2:]=='::' and s[1]: ps=self.pre(s[1])
                else: ps=self._str(s[1],level)
                r=self.ul(r,p,ps)
                continue
            ts_results = ol(s[0], (3,))
            if ts_results:
                p = ts_results[1]
                if s[0][-2:]=='::' and s[1]: ps=self.pre(s[1])
                else: ps=self._str(s[1],level)
                r=self.ol(r,p,ps)
                continue
            ts_results = olp(s[0], (1,))
            if ts_results:
                p = ts_results[1]
                if s[0][-2:]=='::' and s[1]: ps=self.pre(s[1])
                else: ps=self._str(s[1],level)
                r=self.ol(r,p,ps)
                continue
            ts_results = dl(s[0], (1,2))
            if ts_results:
                t,d = ts_results[1]
                r=self.dl(r,t,d,self._str(s[1],level))
                continue
            if example(s[0]) >= 0 and s[1]:
                # Introduce an example, using pre tags:
                r=self.normal(r,s[0],self.pre(s[1]))
                continue
            if s[0][-2:]=='::' and s[1]:
                # Introduce an example, using pre tags:
                r=self.normal(r,s[0][:-1],self.pre(s[1]))
                continue
            if table.create(s[0]):
                ## table support.
                r=self.table(r,table.html(),self._str(s[1],level))
                continue
            else:

                if nl(s[0]) < 0 and s[1] and s[0][-1:] != ':':
                    # Treat as a heading
                    t=s[0]
                    r=self.head(r,t,level,
                                self._str(s[1],level and level+1))
                else:
                    r=self.normal(r,s[0],self._str(s[1],level))
        return r
        

def html_quote(v,
               character_entities=(
                       (re.compile('&'), '&amp;'),
                       (re.compile("<"), '&lt;' ),
                       (re.compile(">"), '&gt;' ),
                       (re.compile('"'), '&quot;')
                       )): #"
        text=str(v)
        for re,name in character_entities:
            text=re.sub(name,text)
        return text

def html_with_references(text, level=1):
    text = re.sub(
        r'[\0\n]\.\. \[([0-9_%s-]+)\]' % string.letters,
        r'\n  <a name="\1">[\1]</a>',
        text)

    text = re.sub(
        r'([\x00- ,])\[(?P<ref>[0-9_%s-]+)\]([\x00- ,.:])'   % string.letters,
        r'\1<a href="#\2">[\2]</a>\3',
        text)
    
    text = re.sub(
        r'([\0- ,])\[([^]]+)\.html\]([\0- ,.:])',
        r'\1<a href="\2.html">[\2]</a>\3',
        text)

    return HTML(text,level=level)
    

def main():
    import sys, getopt

    opts,args=getopt.getopt(sys.argv[1:],'twl')

    if args:
        [infile]=args
        s=open(infile,'r').read()
    else:
        s=sys.stdin.read()

    if opts:

        if filter(lambda o: o[0]=='-w', opts):
            print 'Content-Type: text/html\n'

        if filter(lambda o: o[0]=='-l', opts):
            import locale
            locale.setlocale(locale.LC_ALL,"")

        if s[:2]=='#!':
            s=re.sub('^#![^\n]+','',s)

        mo = re.compile('([\0-\n]*\n)').match(s)
        if mo is not None:
            s = s[len(mo.group(0)) :]
            
        s=str(html_with_references(s))
        if s[:4]=='<h1>':
            t=s[4:find(s,'</h1>')]
            s='''<html><head><title>%s</title>
            </head><body>
            %s
            </body></html>
            ''' % (t,s)
        print s
    else:
        print html_with_references(s)

if __name__=="__main__": main()
