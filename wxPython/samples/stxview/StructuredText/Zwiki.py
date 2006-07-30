#!/usr/bin/python
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

from Html   import HTML
from string import split
from ST     import DOC
import re

"""
This is the new structured text type.
"""

class Zwiki_Title:
   def __init__(self,str=''):
      self.expr1   = re.compile('([A-Z]+[A-Z]+[a-zA-Z]*)').search
      self.expr2   = re.compile('([A-Z]+[a-z]+[A-Z]+[a-zA-Z]*)').search
      self.str    = [str]
      self.typ    = "Zwiki_Title"

   def type(self):
      return '%s' % self.typ

   def string(self):
      return self.str

   def __getitem__(self,index):
      return self.str[index]

   def __call__(self,raw_string,subs):

      """
      The raw_string is checked to see if it matches the rules
      for this structured text expression. If the raw_string does,
      it is parsed for the sub-string which matches and a doc_inner_link
      instance is returned whose string is the matching substring.
      If raw_string does not match, nothing is returned.
      """

      if self.expr1(raw_string):
         start,end               = self.expr1(raw_string).span()
         result                  = Zwiki_Title(raw_string[start:end])
         result.start,result.end = self.expr1(raw_string).span()
         return result
      elif self.expr2(raw_string):
         start,end               = self.expr2(raw_string).span()
         result                  = Zwiki_Title(raw_string[start:end])
         result.start,result.end = self.expr2(raw_string).span()
         return result
      else:
         return None

   def span(self):
      return self.start,self.end

class Zwiki_doc(DOC):

   def __init__(self):
      DOC.__init__(self)
      """
      Add the new type to self.types
      """
      self.types.append(Zwiki_Title())

class Zwiki_parser(HTML):
   def __init__(self):
      HTML.__init__(self)
      self.types["Zwiki_Title"] = self.zwiki_title

   def zwiki_title(self,object):
      result = ""
      for x in object.string():
         result = result + x
      result = "<a href=%s>%s</a>" % (result,result)
      #result = "<dtml-wikiname %s>" % result
      self.string = self.string + result
