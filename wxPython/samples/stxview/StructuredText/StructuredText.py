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
  first quote and whitespace or puctuation to the right of the second quote)
  is treated as example code.

- Text surrounded by '*' characters (with white-space to the left of the
  first '*' and whitespace or puctuation to the right of the second '*')
  is emphasized.

- Text surrounded by '**' characters (with white-space to the left of the
  first '**' and whitespace or puctuation to the right of the second '**')
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

    
$Id$'''
#     Copyright 
#
#       Copyright 1996 Digital Creations, L.C., 910 Princess Anne
#       Street, Suite 300, Fredericksburg, Virginia 22401 U.S.A. All
#       rights reserved.  Copyright in this software is owned by DCLC,
#       unless otherwise indicated. Permission to use, copy and
#       distribute this software is hereby granted, provided that the
#       above copyright notice appear in all copies and that both that
#       copyright notice and this permission notice appear. Note that
#       any product, process or technology described in this software
#       may be the subject of other Intellectual Property rights
#       reserved by Digital Creations, L.C. and are not licensed
#       hereunder.
#
#     Trademarks 
#
#       Digital Creations & DCLC, are trademarks of Digital Creations, L.C..
#       All other trademarks are owned by their respective companies. 
#
#     No Warranty 
#
#       The software is provided "as is" without warranty of any kind,
#       either express or implied, including, but not limited to, the
#       implied warranties of merchantability, fitness for a particular
#       purpose, or non-infringement. This software could include
#       technical inaccuracies or typographical errors. Changes are
#       periodically made to the software; these changes will be
#       incorporated in new editions of the software. DCLC may make
#       improvements and/or changes in this software at any time
#       without notice.
#
#     Limitation Of Liability 
#
#       In no event will DCLC be liable for direct, indirect, special,
#       incidental, economic, cover, or consequential damages arising
#       out of the use of or inability to use this software even if
#       advised of the possibility of such damages. Some states do not
#       allow the exclusion or limitation of implied warranties or
#       limitation of liability for incidental or consequential
#       damages, so the above limitation or exclusion may not apply to
#       you.
#  
#
# If you have questions regarding this software,
# contact:
#
#   Jim Fulton, jim@digicool.com
#
#   (540) 371-6909
#
# $Log$
# Revision 1.1  2001/03/10 05:07:20  RD
# Added some simple sample apps
#
# Revision 1.27  2000/04/21 13:38:10  jim
# Added closing list tags. Woo hoo!
#
# Revision 1.26  2000/03/14 17:22:04  brian
# Allow ~ in hrefs.
#
# Revision 1.25  2000/02/17 00:53:24  klm
# HTML._str(): We were getting preformatted examples rendered twice,
# second time without preformatting.  Problem was a missing 'continue'
# in one of the cases.
#
# Revision 1.24  1999/12/13 16:32:48  klm
# Incorporated pavlos christoforou's mods to handle simple tables.  From
# his web page at http://www.zope.org/Members/gaaros/StructuredText:
#
#   Structured Text module with table support
#
#   A paragraph that has blocks of text enclosed in '||' is treated as a
#   table. The text blocks correspond to table cells and table rows are
#   denoted by newlines. By default the cells are center aligned. You can
#   change the defaults by modifying the CELL,ROW and TABLE class
#   attributes in class Table. A cell can span more than one column by
#   preceding a block of text with an equivalent number of cell separators
#   '||'. Newlines and '|' cannot be a part of the cell text. If you need
#   newlines use <BR>. For example:
#
#        |||| **Ingredients** ||
#        || *Name* || *Amount* ||
#        ||Spam||10||
#        ||Eggs||3||
#
# Revision 1.23  1999/08/03 20:49:05  jim
# Fixed to allow list elements to introduce examples.
#
# Restructured _str using continue to avoid excessive nesting.
#
# Revision 1.22  1999/08/02 22:01:28  jim
# Fixed a bunch of bugs introduced by making ts_regex actually thread
# safe.
#
# Also localized a bunch of regular expressions
# using "static" variables (aka always default arguments).
#
# Revision 1.21  1999/08/02 13:26:52  jim
# paragraph_divider needs to be a regular (thread-unsafe) regex
# since it gets passed to ts_regex.split, which is thread-safe
# and wants to use regs.
#
# Revision 1.20  1999/07/21 13:33:59  jim
# untabified.
#
# Revision 1.19  1999/07/15 16:43:15  jim
# Checked in Scott Robertson's thread-safety fixes.
#
# Revision 1.18  1999/03/24 00:03:18  klm
# Provide for relative links, eg <a href="file_in_same_dir">whatever</a>,
# as:
#
#   "whatever", :file_in_same_dir
#
# or
#
#   "whatever"::file_in_same_dir
#
# .__init__(): relax the second gsub, using a '*' instead of a '+', so
# the stuff before the ':' can be missing, and also do postprocessing so
# any resulting '<a href=":file_in_same_dir">'s have the superfluous ':'
# removed.  *Seems* good!
#
# Revision 1.17  1999/03/12 23:21:39  klm
# Gratuituous checkin to test my cvs *update* logging hook.
#
# Revision 1.16  1999/03/12 17:12:12  klm
# Added support for underlined elements, in the obvious way (and
# included an entry in the module docstring for it).
#
# Added an entry in the module docstring describing what i *guess* is
# the criterion for identifying header elements.  (I'm going to have to
# delve into and understand the framework a bit better before *knowing*
# this is the case.)
#
# Revision 1.15  1999/03/11 22:40:18  klm
# Handle links that include '#' named links.
#
# Revision 1.14  1999/03/11 01:35:19  klm
# Fixed a small typo, and refined the module docstring link example, in
# order to do a checkin to exercise the CVS repository mirroring.  Might
# as well include my last checkin message, with some substantial stuff:
#
# Links are now recognized whether or not the candidate strings are
# terminated with punctuation before the trailing whitespace.  The old
# form - trailing punctuation then whitespace - is preserved, but the
# punctuation is now unnecessary.
#
# The regular expressions are a bit more complicated, but i've factored
# out the common parts and but them in variables with suggestive names,
# which may make them easier to understand.
#
# Revision 1.13  1999/03/11 00:49:57  klm
# Links are now recognized whether or not the candidate strings are
# terminated with punctuation before the trailing whitespace.  The old
# form - trailing punctuation then whitespace - is preserved, but the
# punctuation is now unnecessary.
#
# The regular expressions are a bit more complicated, but i've factored
# out the common parts and but them in variables with suggestive names,
# which may make them easier to understand.
#
# Revision 1.12  1999/03/10 00:15:46  klm
# Committing with version 1.0 of the license.
#
# Revision 1.11  1999/02/08 18:13:12  klm
# Trival checkin (spelling fix "preceedeing" -> "preceding" and similar)
# to see what pitfalls my environment presents to accomplishing a
# successful checkin.  (It turns out that i can't do it from aldous because
# the new version of cvs doesn't support the '-t' and '-f' options in the
# cvswrappers file...)
#
# Revision 1.10  1998/12/29 22:30:43  amos
# Improved doc string to describe hyper link and references capabilities.
#
# Revision 1.9  1998/12/04 20:15:31  jim
# Detabification and new copyright.
#
# Revision 1.8  1998/02/27 18:45:22  jim
# Various updates, including new indentation utilities.
#
# Revision 1.7  1997/12/12 15:39:54  jim
# Added level as argument for html_with_references.
#
# Revision 1.6  1997/12/12 15:27:25  jim
# Added additional pattern matching for HTML references.
#
# Revision 1.5  1997/03/08 16:01:03  jim
# Moved code to recognize: "foo bar", url.
# into object initializer, so it gets applied in all cases.
#
# Revision 1.4  1997/02/17 23:36:35  jim
# Added support for "foo title", http:/foohost/foo
#
# Revision 1.3  1996/12/06 15:57:37  jim
# Fixed bugs in character tags.
#
# Added -t command-line option to generate title if:
#
#    - The first paragraph is one line (i.e. a heading) and
#
#    - All other paragraphs are indented.
#
# Revision 1.2  1996/10/28 13:56:02  jim
# Fixed bug in ordered lists.
# Added option for either HTML-style headings or descriptive-list style
# headings.
#
# Revision 1.1  1996/10/23 14:00:45  jim
# *** empty log message ***
#
#
#

import ts_regex, regex
from ts_regex import gsub
from string import split, join, strip, find

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
    
    def create(self,aPar,td=ts_regex.compile(
        '[ \t\n]*||\([^\0|]*\)').match_group):
        '''parses a table and returns nested list representing the
        table'''
        self.table=[]
        text=filter(None,split(aPar,'\n'))
        for line in text:
            row=[]
            while 1:
                pos=td(line,(1,))
                if not pos:return 0
                row.append(pos[1])
                if pos[0]==len(line):break
                line=line[pos[0]:]
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

optional_trailing_punctuation = '\(,\|\([.:?;]\)\)?'
trailing_space = '\([\0- ]\)'
not_punctuation_or_whitespace = "[^-,.?:\0- ]"
table=Table()

class StructuredText:

    """Model text as structured collection of paragraphs.

    Structure is implied by the indentation level.

    This class is intended as a base classes that do actual text
    output formatting.
    """

    def __init__(self, aStructuredString, level=0,
                 paragraph_divider=regex.compile('\(\n *\)+\n'),
                 ):
        '''Convert a structured text string into a structured text object.

        Aguments:

          aStructuredString -- The string to be parsed.
          level -- The level of top level headings to be created.
        '''

        aStructuredString = gsub(
            '\"\([^\"\0]+\)\":'         # title: <"text":>
            + ('\([-:a-zA-Z0-9_,./?=@#~]+%s\)'
               % not_punctuation_or_whitespace)
            + optional_trailing_punctuation
            + trailing_space,
            '<a href="\\2">\\1</a>\\4\\5\\6',
            aStructuredString)

        aStructuredString = gsub(
            '\"\([^\"\0]+\)\",[\0- ]+'            # title: <"text", >
            + ('\([a-zA-Z]*:[-:a-zA-Z0-9_,./?=@#~]*%s\)'
               % not_punctuation_or_whitespace)
            + optional_trailing_punctuation
            + trailing_space,
            '<a href="\\2">\\1</a>\\4\\5\\6',
            aStructuredString)

        protoless = find(aStructuredString, '<a href=":')
        if protoless != -1:
            aStructuredString = gsub('<a href=":', '<a href="',
                                     aStructuredString)

        self.level=level
        paragraphs=ts_regex.split(untabify(aStructuredString),
                                  paragraph_divider)
        paragraphs=map(indent_level,paragraphs)

        self.structure=structure(paragraphs)


    def __str__(self):
        return str(self.structure)


ctag_prefix="\([\0- (]\|^\)"
ctag_suffix="\([\0- ,.:;!?)]\|$\)"
ctag_middle="[%s]\([^\0- %s][^%s]*[^\0- %s]\|[^%s]\)[%s]"
ctag_middl2="[%s][%s]\([^\0- %s][^%s]*[^\0- %s]\|[^%s]\)[%s][%s]"
        
def ctag(s,
         em=regex.compile(
             ctag_prefix+(ctag_middle % (("*",)*6) )+ctag_suffix),
         strong=regex.compile(
             ctag_prefix+(ctag_middl2 % (("*",)*8))+ctag_suffix),
         under=regex.compile(
             ctag_prefix+(ctag_middle % (("_",)*6) )+ctag_suffix),
         code=regex.compile(
             ctag_prefix+(ctag_middle % (("\'",)*6))+ctag_suffix),
         ):
    if s is None: s=''
    s=gsub(strong,'\\1<strong>\\2</strong>\\3',s)
    s=gsub(under, '\\1<u>\\2</u>\\3',s)
    s=gsub(code,  '\\1<code>\\2</code>\\3',s)
    s=gsub(em,    '\\1<em>\\2</em>\\3',s)
    return s    

class HTML(StructuredText):

    '''\
    An HTML structured text formatter.
    '''\

    def __str__(self,
                extra_dl=regex.compile("</dl>\n<dl>"),
                extra_ul=regex.compile("</ul>\n<ul>"),
                extra_ol=regex.compile("</ol>\n<ol>"),
                ):
        '''\
        Return an HTML string representation of the structured text data.

        '''
        s=self._str(self.structure,self.level)
        s=gsub(extra_dl,'\n',s)
        s=gsub(extra_ul,'\n',s)
        s=gsub(extra_ol,'\n',s)
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
            
        t="<p><strong>%s</strong><p>" % strip(ctag(t))
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
                 '[ \t]*\(\([0-9]+\|[a-zA-Z]+\)[.)]\)+[ \t\n]+\([^\0]*\|$\)'
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
                       (regex.compile('&'), '&amp;'),
                       (regex.compile("<"), '&lt;' ),
                       (regex.compile(">"), '&gt;' ),
                       (regex.compile('"'), '&quot;')
                       )): #"
        text=str(v)
        for re,name in character_entities:
            text=gsub(re,name,text)
        return text

def html_with_references(text, level=1):
    text = gsub(
        '[\0\n].. \[\([-_0-9_a-zA-Z-]+\)\]',
        '\n  <a name="\\1">[\\1]</a>',
        text)
    
    text = gsub(
        '\([\0- ,]\)\[\([0-9_a-zA-Z-]+\)\]\([\0- ,.:]\)',
        '\\1<a href="#\\2">[\\2]</a>\\3',
        text)
    
    text = gsub(
        '\([\0- ,]\)\[\([^]]+\)\.html\]\([\0- ,.:]\)',
        '\\1<a href="\\2.html">[\\2]</a>\\3',
        text)

    return HTML(text,level=level)
    

def main():
    import sys, getopt

    opts,args=getopt.getopt(sys.argv[1:],'tw')

    if args:
        [infile]=args
        s=open(infile,'r').read()
    else:
        s=sys.stdin.read()

    if opts:

        if filter(lambda o: o[0]=='-w', opts):
            print 'Content-Type: text/html\n'

        if s[:2]=='#!':
            s=ts_regex.sub('^#![^\n]+','',s)

        r=ts_regex.compile('\([\0-\n]*\n\)')
        ts_results = r.match_group(s, (1,))
        if ts_results:
            s=s[len(ts_results[1]):]
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
