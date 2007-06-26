# Name:         attribute.py
# Purpose:      Attribute classes
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      25.06.2007
# RCS-ID:       $Id$

from model import Model

class Attribute:
    '''Base class for defining attributes.'''
    def add(parentNode, attribute, value):
        elem = Model.dom.createElement(attribute)
        text = Model.dom.createTextNode(value)
        elem.appendChild(text)
        parentNode.appendChild(elem)
    add = staticmethod(add)
    def get(node):
        '''Get collected element texts.'''
        t = ''
        for data in [n.data for n in node.childNodes if n.nodeType == node.TEXT_NODE]:
            t += data
        return t
    get = staticmethod(get)

class ContentAttribute:
    '''Content attribute class. Value is a list of strings.'''
    hasName = False
    
    def add(parentNode, attribute, value):
        contentElem = Model.dom.createElement('content')
        for item in value:
            elem = Model.dom.createElement('item')
            text = Model.dom.createTextNode(item)
            elem.appendChild(text)
            contentElem.appendChild(elem)
        parentNode.appendChild(contentElem)
    add = staticmethod(add)
    def get(node):
        value = []
        for n in node.childNodes:
            if n.nodeType == node.ELEMENT_NODE and n.tagName == 'item':
                value.append(Attribute.get(n))
        return value
    get = staticmethod(get)

class FontAttribute:
    '''Font attribute class. Value is a dictionary of font attribtues.'''
    attributes = ['size', 'style', 'weight', 'underlined', 'family', 'face', 'encoding', 
                  'sysfont', 'relativesize']
    def add(parentNode, attribute, value):
        fontElem = Model.dom.createElement('font')
        for a in filter(value.has_key, FontAttribute.attributes):
            elem = Model.dom.createElement(a)
            text = Model.dom.createTextNode(value[a])
            elem.appendChild(text)
            fontElem.appendChild(elem)
        parentNode.appendChild(fontElem)
    add = staticmethod(add)
    def get(node):
        value = {}
        for n in node.childNodes:
            if n.nodeType == node.ELEMENT_NODE:
                value[n.tagName] = Attribute.get(n)
        return value
    get = staticmethod(get)

class MultiAttribute:
    '''Repeated attribute like growablecols.'''
    def add(parentNode, attribute, value):
        for v in value:
            elem = Model.dom.createElement(attribute)
            text = Model.dom.createTextNode(v)
            elem.appendChild(text)
            parentNode.appendChild(elem)
    add = staticmethod(add)
    def get(node):
        tag = node.tagName  # remember tag name
        value = []
        # Look for multiple tags
        while node and node.tagName == tag:
            value.append(Attribute.get(node))
            node = node.nextSibling
        return value
    get = staticmethod(get)
