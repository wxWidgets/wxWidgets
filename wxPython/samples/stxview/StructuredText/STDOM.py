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
"""
DOM implementation in StructuredText : Read-Only methods

All standard Zope objects support DOM to a limited extent.
"""
import string


# Node type codes
# ---------------

ELEMENT_NODE                  = 1
ATTRIBUTE_NODE                = 2
TEXT_NODE                     = 3
CDATA_SECTION_NODE            = 4
ENTITY_REFERENCE_NODE         = 5
ENTITY_NODE                   = 6
PROCESSING_INSTRUCTION_NODE   = 7
COMMENT_NODE                  = 8
DOCUMENT_NODE                 = 9
DOCUMENT_TYPE_NODE            = 10
DOCUMENT_FRAGMENT_NODE        = 11
NOTATION_NODE                 = 12

# Exception codes
# ---------------

INDEX_SIZE_ERR                = 1
DOMSTRING_SIZE_ERR            = 2
HIERARCHY_REQUEST_ERR         = 3
WRONG_DOCUMENT_ERR            = 4
INVALID_CHARACTER_ERR         = 5
NO_DATA_ALLOWED_ERR           = 6
NO_MODIFICATION_ALLOWED_ERR   = 7
NOT_FOUND_ERR                 = 8
NOT_SUPPORTED_ERR             = 9
INUSE_ATTRIBUTE_ERR           = 10

# Exceptions
# ----------

class DOMException(Exception):
    pass
class IndexSizeException(DOMException):
    code = INDEX_SIZE_ERR
class DOMStringSizeException(DOMException):
    code = DOMSTRING_SIZE_ERR
class HierarchyRequestException(DOMException):
    code = HIERARCHY_REQUEST_ERR
class WrongDocumentException(DOMException):
    code = WRONG_DOCUMENT_ERR
class InvalidCharacterException(DOMException):
    code = INVALID_CHARACTER_ERR
class NoDataAllowedException(DOMException):
    code = NO_DATA_ALLOWED_ERR
class NoModificationAllowedException(DOMException):
    code = NO_MODIFICATION_ALLOWED_ERR
class NotFoundException(DOMException):
    code = NOT_FOUND_ERR
class NotSupportedException(DOMException):
    code = NOT_SUPPORTED_ERR
class InUseAttributeException(DOMException):
    code = INUSE_ATTRIBUTE_ERR

# Node classes
# ------------

class ParentNode:
   """
   A node that can have children, or, more precisely, that implements
   the child access methods of the DOM.
   """

   def getChildNodes(self, type=type, st=type('')):
      """
      Returns a NodeList that contains all children of this node.
      If there are no children, this is a empty NodeList
      """
      
      r=[]
      for n in self.getChildren():
         if type(n) is st: n=TextNode(n)
         r.append(n.__of__(self))
      
      return  NodeList(r)
   
   def getFirstChild(self, type=type, st=type('')):
      """
      The first child of this node. If there is no such node
      this returns None
      """
      children = self.getChildren()

      if not children:
         return None
         
      n=chidren[0]

      if type(n) is st:
         n=TextNode(n)
         
      return n.__of__(self)

   def getLastChild(self, type=type, st=type('')):
      """
      The last child of this node.  If there is no such node
      this returns None.
      """
      children = self.getChildren()
      if not children: return None
      n=chidren[-1]
      if type(n) is st: n=TextNode(n)
      return n.__of__(self)

   """
   create aliases for all above functions in the pythony way.
   """
   
   def _get_ChildNodes(self, type=type, st=type('')):
      return self.getChildNodes(type,st)
   
   def _get_FirstChild(self, type=type, st=type('')):
      return self.getFirstChild(type,st)

   def _get_LastChild(self, type=type, st=type('')):
      return self.getLastChild(type,st)
  
class NodeWrapper(ParentNode):
   """
   This is an acquisition-like wrapper that provides parent access for 
   DOM sans circular references!
   """

   def __init__(self, aq_self, aq_parent):
      self.aq_self=aq_self
      self.aq_parent=aq_parent

   def __getattr__(self, name):
      return getattr(self.aq_self, name)
    
   def getParentNode(self):
      """
      The parent of this node.  All nodes except Document
      DocumentFragment and Attr may have a parent
      """
      return self.aq_parent

   def _getDOMIndex(self, children, getattr=getattr):
      i=0
      self=self.aq_self
      for child in children:
         if getattr(child, 'aq_self', child) is self:
            self._DOMIndex=i
            return i
         i=i+1
      return None

   def getPreviousSibling(self,
                          type=type,
                          st=type(''),
                          getattr=getattr,
                          None=None):

      """
      The node immediately preceding this node.  If
      there is no such node, this returns None.
      """
      
      children = self.aq_parent.getChildren()
      if not children:
         return None

      index=getattr(self, '_DOMIndex', None)
      if index is None:
         index=self._getDOMIndex(children)
         if index is None: return None

      index=index-1
      if index < 0: return None
      try: n=children[index]
      except IndexError: return None
      else:
         if type(n) is st:
            n=TextNode(n)
         n._DOMIndex=index
         return n.__of__(self)


   def getNextSibling(self, type=type, st=type('')):
      """
      The node immediately preceding this node.  If
      there is no such node, this returns None.
      """
      children = self.aq_parent.getChildren()
      if not children:
         return None

      index=getattr(self, '_DOMIndex', None)
      if index is None:
         index=self._getDOMIndex(children)
         if index is None:
            return None

      index=index+1
      try: n=children[index]
      except IndexError:
         return None
      else:
         if type(n) is st:
            n=TextNode(n)
         n._DOMIndex=index
         return n.__of__(self)

   def getOwnerDocument(self):
      """
      The Document object associated with this node, if any.
      """
      return self.aq_parent.getOwnerDocument()

   """
   create aliases for all above functions in the pythony way.   
   """
   
   def _get_ParentNode(self):
      return self.getParentNode()

   def _get_DOMIndex(self, children, getattr=getattr):
      return self._getDOMIndex(children,getattr)
      
   def _get_PreviousSibling(self,
                          type=type,
                          st=type(''),
                          getattr=getattr,
                          None=None):

      return self.getPreviousSibling(type,st,getattr,None)
      
   def _get_NextSibling(self, type=type, st=type('')):
      return self.getNextSibling(type,st)
      
   def _get_OwnerDocument(self):
      return self.getOwnerDocument()
      
class Node(ParentNode):
   """
   Node Interface
   """

   # Get a DOM wrapper with a parent link
   def __of__(self, parent):
      return NodeWrapper(self, parent)

   # DOM attributes    
   # --------------
    
   def getNodeName(self):
      """
      The name of this node, depending on its type
      """

   def getNodeValue(self):
      """
      The value of this node, depending on its type
      """
      return None

   def getParentNode(self):
      """
      The parent of this node.  All nodes except Document
      DocumentFragment and Attr may have a parent
      """

   def getChildren(self):
      """
      Get a Python sequence of children
      """
      return ()

   def getPreviousSibling(self,
                          type=type,
                          st=type(''),
                          getattr=getattr,
                          None=None):
      """
      The node immediately preceding this node.  If
      there is no such node, this returns None.
      """

   def getNextSibling(self, type=type, st=type('')):
      """
      The node immediately preceding this node.  If
      there is no such node, this returns None.
      """

   def getAttributes(self):
      """
      Returns a NamedNodeMap containing the attributes
      of this node (if it is an element) or None otherwise.
      """
      return None

   def getOwnerDocument(self):
      """
      The Document object associated with this node, if any.
      """
        
    # DOM Methods    
    # -----------
    
   def hasChildNodes(self):
      """
      Returns true if the node has any children, false
      if it doesn't.
      """
      return len(self.getChildren())

   """
   create aliases for all above functions in the pythony way.
   """

   def _get_NodeName(self):
      return self.getNodeName()
      
   def _get_NodeValue(self):
      return self.getNodeValue()
      
   def _get_ParentNode(self):
      return self.getParentNode()

   def _get_Children(self):
      return self.getChildren()

   def _get_PreviousSibling(self,
                          type=type,
                          st=type(''),
                          getattr=getattr,
                          None=None):
      
      return self.getPreviousSibling(type,st,getattr,None)
      
   def _get_NextSibling(self, type=type, st=type('')):
      return self.getNextSibling()

   def _get_Attributes(self):
      return self.getAttributes()

   def _get_OwnerDocument(self):
      return self.getOwnerDocument()
    
   def _has_ChildNodes(self):
      return self.hasChildNodes()
      
         
class TextNode(Node):

   def __init__(self, str): self._value=str
      
   def getNodeType(self):
      return TEXT_NODE
      
   def getNodeName(self):
      return '#text'

   def getNodeValue(self):
      return self._value
   
   """
   create aliases for all above functions in the pythony way.
   """
   
   def _get_NodeType(self):
      return self.getNodeType()
      
   def _get_NodeName(self):
      return self.getNodeName()

   def _get_NodeValue(self):
      return self.getNodeValue()

class Element(Node):
   """
   Element interface
   """
    
   # Element Attributes
   # ------------------
    
   def getTagName(self):
      """The name of the element"""
      return self.__class__.__name__
    
   def getNodeName(self):
      """The name of this node, depending on its type"""
      return self.__class__.__name__
      
   def getNodeType(self):
      """A code representing the type of the node."""
      return ELEMENT_NODE

   def getNodeValue(self, type=type, st=type('')):
      r=[]
      for c in self.getChildren():
         if type(c) is not st:
            c=c.getNodeValue()
         r.append(c)
      return string.join(r,'')
    
   def getParentNode(self):
      """
      The parent of this node.  All nodes except Document
      DocumentFragment and Attr may have a parent
      """
      
   # Element Methods
   # ---------------
    
   _attributes=()

   def getAttribute(self, name): return getattr(self, name, None)
   def getAttributeNode(self, name):
      if hasattr(self, name):
         return Attr(name, getattr(self, name))

   def getAttributes(self):
      d={}
      for a in self._attributes:
         d[a]=getattr(self, a, '')
      return NamedNodeMap(d)

   def getAttribute(self, name):
      """Retrieves an attribute value by name."""
      return None

   def getAttributeNode(self, name):
      """ Retrieves an Attr node by name or None if
      there is no such attribute. """
      return None

   def getElementsByTagName(self, tagname):
      """
      Returns a NodeList of all the Elements with a given tag
      name in the order in which they would be encountered in a
      preorder traversal of the Document tree.  Parameter: tagname
      The name of the tag to match (* = all tags). Return Value: A new
      NodeList object containing all the matched Elements.
      """
      nodeList = []
      for child in self.getChildren():
         if (child.getNodeType()==ELEMENT_NODE and \
             child.getTagName()==tagname or tagname== '*'):
                
            nodeList.append(child)
               
         if hasattr(child, 'getElementsByTagName'):
            n1       = child.getElementsByTagName(tagname)
            nodeList = nodeList + n1._data
      return NodeList(nodeList)
   
   """
   create aliases for all above functions in the pythony way.
   """

   def _get_TagName(self):
      return self.getTagName()
      
   def _get_NodeName(self):
      return self.getNodeName()
      
   def _get_NodeType(self):
      return self.getNodeType()
      
   def _get_NodeValue(self, type=type, st=type('')):
      return self.GetNodeValue(type,st)
      
   def _get_ParentNode(self):
      return self.getParentNode()
      
   def _get_Attribute(self, name):
      return self.getAttribute(name)
   
   def _get_AttributeNode(self, name):
      return self.getAttributeNode(name)
      
   def _get_Attributes(self):
      return self.getAttributes()
      
   def _get_Attribute(self, name):
      return self.getAttribute(name)
      
   def _get_AttributeNode(self, name):
      return self.getAttributeNode(name)
      
   def _get_ElementsByTagName(self, tagname):
      return self.getElementsByTagName(tagname)
      
         
class NodeList:
   """
   NodeList interface - Provides the abstraction of an ordered
   collection of nodes.
    
   Python extensions: can use sequence-style 'len', 'getitem', and
   'for..in' constructs.
   """
    
   def __init__(self,list=None):
      self._data = list or []
    
   def __getitem__(self, index, type=type, st=type('')):
      return self._data[index]

   def __getslice__(self, i, j):
      return self._data[i:j]
    
   def item(self, index):
      """
      Returns the index-th item in the collection
      """
      try:  return self._data[index]    
      except IndexError: return None
         
   def getLength(self):
      """
      The length of the NodeList
      """
      return len(self._data)
    
   __len__=getLength
   
   """
   create aliases for all above functions in the pythony way.
   """
   
   def _get_Length(self):
      return self.getLength()
 
class NamedNodeMap:
   """
   NamedNodeMap interface - Is used to represent collections
   of nodes that can be accessed by name.  NamedNodeMaps are not
   maintained in any particular order.
    
   Python extensions: can use sequence-style 'len', 'getitem', and
   'for..in' constructs, and mapping-style 'getitem'.
   """
    
   def __init__(self, data=None):
      if data is None:
         data = {}
      self._data = data

   def item(self, index):
      """
      Returns the index-th item in the map
      """
      try: return self._data.values()[index]
      except IndexError: return None
        
   def __getitem__(self, key):
      if type(key)==type(1):
         return self._data.values()[key]
      else:
         return self._data[key]
            
   def getLength(self):
      """
      The length of the NodeList
      """
      return len(self._data)
    
   __len__ = getLength
    
   def getNamedItem(self, name):
      """
      Retrieves a node specified by name. Parameters:
      name Name of a node to retrieve. Return Value A Node (of any
      type) with the specified name, or None if the specified name
      did not identify any node in the map.
      """
      if self._data.has_key(name): 
         return self._data[name]
      return None

   """
   create aliases for all above functions in the pythony way.
   """
   def _get_Length(self):
      return self.getLength()
      
   def _get_NamedItem(self, name):
      return self.getNamedItem(name)
      
class Attr(Node):
   """
   Attr interface - The Attr interface represents an attriubte in an
   Element object. Attr objects inherit the Node Interface
   """

   def __init__(self, name, value, specified=1):
      self.name = name
      self.value = value
      self.specified = specified
        
   def getNodeName(self):
      """
      The name of this node, depending on its type
      """
      return self.name

   def getName(self):
      """
      Returns the name of this attribute.
      """
      return self.name
    
   def getNodeValue(self):
      """
      The value of this node, depending on its type
      """
      return self.value

   def getNodeType(self):
      """
      A code representing the type of the node.
      """
      return ATTRIBUTE_NODE

   def getSpecified(self):
      """
      If this attribute was explicitly given a value in the
      original document, this is true; otherwise, it is false.
      """
      return self.specified
        
   """
   create aliases for all above functions in the pythony way.
   """
   
   def _get_NodeName(self):
      return self.getNodeName()

   def _get_Name(self):
      return self.getName()
    
   def _get_NodeValue(self):
      return self.getNodeValue()

   def _get_NodeType(self):
      return self.getNodeType()

   def _get_Specified(self):
      return self.getSpecified()
