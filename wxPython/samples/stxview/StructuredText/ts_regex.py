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
"""Provide a thread-safe interface to regex
"""
import regex, regsub #, Sync
from regex import *
from regsub import split, sub, gsub, splitx, capwords

try: 
    import thread
except:
    class allocate_lock:
        def acquire(*args): pass
        def release(*args): pass

else:
    class SafeFunction:
        _l=thread.allocate_lock()
        _a=_l.acquire
        _r=_l.release

        def __init__(self, f):
            self._f=f

        def __call__(self, *args, **kw):
            self._a()
            try: return apply(self._f, args, kw)
            finally: self._r()

    split=SafeFunction(split)
    sub=SafeFunction(sub)
    gsub=SafeFunction(gsub)
    splitx=SafeFunction(splitx)
    capwords=SafeFunction(capwords)

    allocate_lock=thread.allocate_lock

class compile:

    _r=None
    groupindex=None

    def __init__(self, *args):
        self._r=r=apply(regex.compile,args)
        self._init(r)

    def _init(self, r):
        lock=allocate_lock()
        self.__a=lock.acquire
        self.__r=lock.release
        self.translate=r.translate
        self.givenpat=r.givenpat
        self.realpat=r.realpat

    def match(self, string, pos=0):
        self.__a()
        try: return self._r.match(string, pos)
        finally: self.__r()

    def search(self, string, pos=0):
        self.__a()
        try: return self._r.search(string, pos)
        finally: self.__r()
        
    def search_group(self, str, group, pos=0):
        """Search a string for a pattern.

        If the pattern was not found, then None is returned,
        otherwise, the location where the pattern was found,
        as well as any specified group are returned.
        """
        self.__a()
        try:
            r=self._r
            l=r.search(str, pos)
            if l < 0: return None
            return l, apply(r.group, group)
        finally: self.__r()

    def match_group(self, str, group, pos=0):
        """Match a pattern against a string

        If the string does not match the pattern, then None is
        returned, otherwise, the length of the match, as well
        as any specified group are returned.
        """
        self.__a()
        try:
            r=self._r
            l=r.match(str, pos)
            if l < 0: return None
            return l, apply(r.group, group)
        finally: self.__r()

    def search_regs(self, str, pos=0):
        """Search a string for a pattern.

        If the pattern was not found, then None is returned,
        otherwise, the 'regs' attribute of the expression is
        returned.
        """
        self.__a()
        try:
            r=self._r
            r.search(str, pos)
            return r.regs
        finally: self.__r()

    def match_regs(self, str, pos=0):
        """Match a pattern against a string

        If the string does not match the pattern, then None is
        returned, otherwise, the 'regs' attribute of the expression is
        returned.
        """
        self.__a()
        try:
            r=self._r
            r.match(str, pos)
            return r.regs
        finally: self.__r()

class symcomp(compile):

    def __init__(self, *args):
        self._r=r=apply(regex.symcomp,args)
        self._init(r)
        self.groupindex=r.groupindex




        
