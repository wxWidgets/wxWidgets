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
#     notice, this list of conditions, and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions, and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
# 
# 3. Digital Creations requests that attribution be given to Zope
#     in any manner possible. Zope includes a "Powered by Zope"
#     button that is installed by default. While it is not a license
#     violation to remove this button, it is requested that the
#     attribution remain. A significant investment has been put
#     into Zope, and this effort will continue if the Zope community
#     continues to grow. This is one way to assure that growth.
# 
# 4. All advertising materials and documentation mentioning
#     features derived from or use of this software must display
#     the following acknowledgement:
# 
#        "This product includes software developed by Digital Creations
#        for use in the Z Object Publishing Environment
#        (http://www.zope.org/)."
# 
#     In the event that the product being advertised includes an
#     intact Zope distribution (with copyright and license included)
#     then this clause is waived.
# 
# 5. Names associated with Zope or Digital Creations must not be used to
#     endorse or promote products derived from this software without
#     prior written permission from Digital Creations.
# 
# 6. Modified redistributions of any form whatsoever must retain
#     the following acknowledgment:
# 
#        "This product includes software developed by Digital Creations
#        for use in the Z Object Publishing Environment
#        (http://www.zope.org/)."
# 
#     Intact (re-)distributions of any official Zope release do not
#     require an external acknowledgement.
# 
# 7. Modifications are encouraged but must be packaged separately as
#     patches to official Zope releases.  Distributions that do not
#     clearly separate the patches from the original work must be clearly
#     labeled as unofficial distributions.  Modifications which do not
#     carry the name Zope may be packaged in any form, as long as they
#     conform to all of the clauses above.
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

from string import join, split, find
import re, sys, ST

class HTMLClass:

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
        'StructuredTextInnerLink':'innerLink',
        'StructuredTextNamedLink':'namedLink',
        'StructuredTextUnderline':'underline',
        'StructuredTextTable':'table',
        'StructuredTextSGML':'sgml',
        }

    def dispatch(self, doc, level, output):
        getattr(self, self.element_types[doc.getNodeName()])(doc, level, output)
        
    def __call__(self, doc, level=1):
        r=[]
        self.dispatch(doc, level-1, r.append)
        return join(r,'')

    def _text(self, doc, level, output):
        output(doc.getNodeValue())

    def document(self, doc, level, output):
        output('<html>\n')
        children=doc.getChildNodes()
        if (children and
             children[0].getNodeName() == 'StructuredTextSection'):
            output('<head>\n<title>%s</title>\n</head>\n' %
                     children[0].getChildNodes()[0].getNodeValue())
        output('<body>\n')
        for c in children:
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('</body>\n')
        output('</html>\n')

    def section(self, doc, level, output):
        children=doc.getChildNodes()
        for c in children:
            getattr(self, self.element_types[c.getNodeName()])(c, level+1, output)
        
    def sectionTitle(self, doc, level, output):
        output('<h%d>' % (level))
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('</h%d>\n' % (level))

    def description(self, doc, level, output):
        p=doc.getPreviousSibling()
        if p is None or  p.getNodeName() is not doc.getNodeName():            
            output('<dl>\n')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        n=doc.getNextSibling()
        if n is None or n.getNodeName() is not doc.getNodeName():            
            output('</dl>\n')
        
    def descriptionTitle(self, doc, level, output):
        output('<dt>')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('</dt>\n')
        
    def descriptionBody(self, doc, level, output):
        output('<dd>')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('</dd>\n')

    def bullet(self, doc, level, output):
        p=doc.getPreviousSibling()
        if p is None or p.getNodeName() is not doc.getNodeName():
            output('<ul>\n')
        output('<li>')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        n=doc.getNextSibling()
        output('</li>\n')
        if n is None or n.getNodeName() is not doc.getNodeName():            
            output('</ul>\n')

    def numbered(self, doc, level, output):
        p=doc.getPreviousSibling()
        if p is None or p.getNodeName() is not doc.getNodeName():            
            output('<ol>\n')
        output('<li>')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        n=doc.getNextSibling()
        output('</li>\n')
        if n is None or n.getNodeName() is not doc.getNodeName():
            output('</ol>\n')

    def example(self, doc, level, output):
        i=0
        for c in doc.getChildNodes():
            if i==0:
                output('<pre>')
                output(html_quote(c.getNodeValue()))
                output('</pre>\n')
            else:
                getattr(self, self.element_types[c.getNodeName()])(
                    c, level, output)

    def paragraph(self, doc, level, output):
        i=0
        output('<p>')
        for c in doc.getChildNodes():
            if c.getNodeName() in ['StructuredTextParagraph']:
                getattr(self, self.element_types[c.getNodeName()])(
                    c, level, output)
            else:
                getattr(self, self.element_types[c.getNodeName()])(
                    c, level, output)
        output('</p>')

    def link(self, doc, level, output):
        output('<a href="%s">' % doc.href)
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('</a>')

    def emphasis(self, doc, level, output):
        output('<em>')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('</em>')

    def literal(self, doc, level, output):
        output('<code>')
        for c in doc.getChildNodes():
            output(html_quote(c.getNodeValue()))
        output('</code>')

    def strong(self, doc, level, output):
        output('<strong>')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('</strong>')
     
    def underline(self, doc, level, output):
        output("<u>")
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output("</u>")
          
    def innerLink(self, doc, level, output):
        output('<a href="#');
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('">[')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output(']</a>')
    
    def namedLink(self, doc, level, output):
        output('<a name="')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output('">[')
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
        output(']</a>')
    
    def sgml(self,doc,level,output):
        for c in doc.getChildNodes():
            getattr(self, self.element_types[c.getNodeName()])(c, level, output)
    
    def table(self,doc,level,output):
        """
        A StructuredTextTable holds StructuredTextRow(s) which
        holds StructuredTextColumn(s). A StructuredTextColumn
        is a type of StructuredTextParagraph and thus holds
        the actual data.
        """
        output("<table border=1 cellpadding=2>\n")
        for row in doc.getRows()[0]:
            output("<tr>\n")
            for column in row.getColumns()[0]:
                str = "<td colspan=%s>" % column.getSpan()
                output(str)
                #for c in doc.getChildNodes():
                #    getattr(self, self.element_types[c.getNodeName()])(c, level, output)
                for c in column.getChildNodes():
                    getattr(self, self.element_types[c.getNodeName()])(c, level, output)
                output("</td>\n")
            output("</tr>\n")
        output("</table>\n")
          
def html_quote(v, name='(Unknown name)', md={},
                    character_entities=(
                              (('&'),     '&amp;'),
                              (('<'),     '&lt;' ),
                              (('>'),     '&gt;' ),
                              (('\213'), '&lt;' ),
                              (('\233'), '&gt;' ),
                              (('"'),     '&quot;'))): #"
          text=str(v)
          for re,name in character_entities:
                if find(text, re) >= 0: text=join(split(text,re),name)
          return text





