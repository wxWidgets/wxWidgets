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
'''
$Id$'''

from StructuredText import * # :-)
        
def ctag(s):
    # Blech, wish we could use character tags
    if s is None: s=''
    s=gsub(strong,'\\1<bold>\\2<plain>\\3',s)
    s=gsub(code,  '\\1<family Courier>\\2<family Times>\\3',s)
    s=gsub(em,    '\\1<italic>\\2<plain>\\3',s)
    return join(map(strip,split(s,'\n')),'\n')

class MML(StructuredText):

    '''\
    An MML structured text formatter.
    '''\

    def __str__(self,
                ):
        '''\
        Return an HTML string representation of the structured text data.

        '''
        s=self._str(self.structure,self.level)
        return s

    def ul(self, before, p, after):
        return ("%s\n\n<Bulleted>\n%s%s"
                % (before, ctag(p), after))

    def ol(self, before, p, after):
        return ("%s\n\n<Numbered>\n%s%s"
                % (before, ctag(p), after))

    def dl(self, before, t, d, after):
        return ("%s\n\n<Term>\n%s\n\n<Definition>\n%s%s" 
                % (before,ctag(t),ctag(d),after))

    def head(self, before, t, level, d):
        return ("%s\n\n<Heading%d>\n%s%s"
                % (before,level,ctag(t),d))

    def normal(self,before,p,after):
        return "%s\n\n<Body>\n%s%s" % (before, ctag(p), after)

    def pre(self,structure,r=None):
        if r is None: r=['']
        for s in structure:
            for line in split(s[0],'\n'):
                r.append('\n<PRE>')
                r.append(line)
            self.pre(s[1],r)
        return join(r,'\n')

    def _str(self,structure,level):
        r=''
        for s in structure:
            # print s[0],'\n', len(s[1]), '\n\n'
            if bullet.match(s[0]) >= 0:
                p=bullet.group(1)
                r=self.ul(r,p,self._str(s[1],level))
            elif ol.match(s[0]) >= 0:
                p=ol.group(3)
                r=self.ol(r,p,self._str(s[1],level))
            elif olp.match(s[0]) >= 0:
                p=olp.group(1)
                r=self.ol(r,p,self._str(s[1],level))
            elif dl.match(s[0]) >= 0:
                t,d=dl.group(1,2)
                r=self.dl(r,t,d,self._str(s[1],level))
            elif example.search(s[0]) >= 0 and s[1]:
                # Introduce an example, using pre tags:
                r=self.normal(r,s[0],self.pre(s[1]))
            elif s[0][-2:]=='::' and s[1]:
                # Introduce an example, using pre tags:
                r=self.normal(r,s[0][:-1],self.pre(s[1]))
            elif nl.search(s[0]) < 0 and s[1] and s[0][-1:] != ':':
                # Treat as a heading
                t=s[0]
                r=self.head(r,t,level,
                            self._str(s[1],level and level+1))
            else:
                r=self.normal(r,s[0],self._str(s[1],level))
        return r        
