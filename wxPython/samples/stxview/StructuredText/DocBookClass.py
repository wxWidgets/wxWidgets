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

import string
from string import join, split, find, lstrip

class DocBookClass:

    element_types={
        '#text': '_text',
        'StructuredTextDocument': 'document',
        'StructuredTextParagraph': 'paragraph',
        'StructuredTextExample': 'example',
        'StructuredTextBullet': 'bullet',
        'StructuredTextNumbered': 'numbered',
        'StructuredTextDescription': 'description',
        'StructuredTextDescriptionTitle': 'descriptionTitle',
        'StructuredTextDescriptionBody': 'descriptionBody',
        'StructuredTextSection': 'section',
        'StructuredTextSectionTitle': 'sectionTitle',
        'StructuredTextLiteral': 'literal',
        'StructuredTextEmphasis': 'emphasis',
        'StructuredTextStrong': 'strong',
        'StructuredTextLink': 'link',
        'StructuredTextXref': 'xref',
        }        

    def dispatch(self, doc, level, output):
        getattr(self, self.element_types[doc.getNodeName()])(doc, level, output)
        
    def __call__(self, doc, level=1):
        r=[]
        self.dispatch(doc, level-1, r.append)
        return join(r,'')

    def _text(self, doc, level, output):
        if doc.getNodeName() == 'StructuredTextLiteral':
            output(doc.getNodeValue())
        else:
            output(lstrip(doc.getNodeValue()))            

    def document(self, doc, level, output):
        output('<!DOCTYPE book PUBLIC "-//OASIS//DTD DocBook V3.1//EN">\n')
        output('<book>\n')
        children=doc.getChildNodes()
        if (children and
             children[0].getNodeName() == 'StructuredTextSection'):
            output('<title>%s</title>' % children[0].getChildNodes()[0].getNodeValue())
        for c in children:
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('</book>\n')

    def section(self, doc, level, output):
        output('\n<sect%s>\n' % (level + 1))
        children=doc.getChildNodes()
        for c in children:
            getattr(self, self.element_types[c.getNodeName()])(c, level+1, output)
        output('\n</sect%s>\n' % (level + 1))
        
    def sectionTitle(self, doc, level, output):
        output('<title>')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('</title>\n')

    def description(self, doc, level, output):
        p=doc.getPreviousSibling()
        if p is None or  p.getNodeName() is not doc.getNodeName():            
            output('<variablelist>\n')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        n=doc.getNextSibling()
        if n is None or n.getNodeName() is not doc.getNodeName():            
            output('</variablelist>\n')
        
    def descriptionTitle(self, doc, level, output):
        output('<varlistentry><term>\n')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('</term>\n')
        
    def descriptionBody(self, doc, level, output):
        output('<listitem><para>\n')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('</para></listitem>\n')
        output('</varlistentry>\n')

    def bullet(self, doc, level, output):
        p=doc.getPreviousSibling()
        if p is None or p.getNodeName() is not doc.getNodeName():            
            output('<itemizedlist>\n')
        output('<listitem><para>\n')

        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        n=doc.getNextSibling()
        output('</para></listitem>\n')
        if n is None or n.getNodeName() is not doc.getNodeName():            
            output('</itemizedlist>\n')

    def numbered(self, doc, level, output):
        p=doc.getPreviousSibling()
        if p is None or p.getNodeName() is not doc.getNodeName():            
            output('<orderedlist>\n')
        output('<listitem><para>\n')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        n=doc.getNextSibling()
        output('</para></listitem>\n')
        if n is None or n.getNodeName() is not doc.getNodeName():            
            output('</orderedlist>\n')

    def example(self, doc, level, output):
        i=0
        for c in doc.getChildNodes():
            if i==0:
                output('<programlisting>\n<![CDATA[\n')
                ##
                ## eek.  A ']]>' in your body will break this...
                ##
                output(prestrip(c.getNodeValue()))
                output('\n]]></programlisting>\n')
            else:
                getattr(self, self.element_types[c.getNodeName()])(
                    c, level, output)

    def paragraph(self, doc, level, output):
        
        output('<para>\n\n')        
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(
                c, level, output)
        output('</para>\n\n')
                
    def link(self, doc, level, output):
#        output('<link linkend="%s">' % doc.href)
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
#        output('</link>')

    def emphasis(self, doc, level, output):
        output('<emphasis>')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('</emphasis> ')

    def literal(self, doc, level, output):
        output('<literal>')
        for c in doc.getChildNodes():
            output(c.getNodeValue())
        output('</literal>')

    def strong(self, doc, level, output):
        output('<emphasis>')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('</emphasis>')

    def xref(self, doc, level, output):
        output('<xref linkend="%s">' % doc.getNodeValue())

def prestrip(v):
    v=string.replace(v, '\r\n', '\n')
    v=string.replace(v, '\r', '\n')
    v=string.replace(v, '\t', '          ')
    lines=string.split(v, '\n')
    indent=len(lines[0])
    for line in lines:
        if not len(line): continue
        i=len(line)-len(string.lstrip(line))
        if i < indent:
            indent=i
    nlines=[]
    for line in lines:
        nlines.append(line[indent:])
    return string.join(nlines, '\r\n')


class DocBookChapter(DocBookClass):

    def document(self, doc, level, output):
        output('<chapter>\n')
        children=doc.getChildNodes()
        if (children and
             children[0].getNodeName() == 'StructuredTextSection'):
            output('<title>%s</title>' % children[0].getChildNodes()[0].getNodeValue())
        for c in children[0].getChildNodes()[1:]:
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('</chapter>\n')

ets = DocBookClass.element_types
ets.update({'StructuredTextImage': 'image'})        

class DocBookChapterWithFigures(DocBookChapter):

     element_types = ets

     def image(self, doc, level, output):
         if hasattr(doc, 'key'):
             output('<figure id="%s"><title>%s</title>\n' % (doc.key, doc.getNodeValue()) )
         else:
             output('<figure><title>%s</title>\n' % doc.getNodeValue())
##          for c in doc.getChildNodes():
##                getattr(self, self.element_types[c.getNodeName()])(c, level, output)
         output('<graphic fileref="%s"></graphic>\n</figure>\n' % doc.href)     

class DocBookArticle(DocBookClass):

    def document(self, doc, level, output):
        output('<!DOCTYPE article PUBLIC "-//OASIS//DTD DocBook V3.1//EN">\n')
        output('<article>\n')
        children=doc.getChildNodes()
        if (children and
             children[0].getNodeName() == 'StructuredTextSection'):
            output('<artheader>\n<title>%s</title>\n</artheader>\n' %
                     children[0].getChildNodes()[0].getNodeValue())
        for c in children:
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('</article>\n')


class DocBookBook:

    def __init__(self, title=''):
        self.title = title
        self.chapters = []

    def addChapter(self, chapter):
        self.chapters.append(chapter)

    def read(self):
        out = '<!DOCTYPE book PUBLIC "-//OASIS//DTD DocBook V3.1//EN">\n<book>\n'
        out = out + '<title>%s</title>\n' % self.title
        for chapter in self.chapters:
            out = out + chapter + '\n</book>\n'

        return out

    def __str__(self):
        return self.read()
            

