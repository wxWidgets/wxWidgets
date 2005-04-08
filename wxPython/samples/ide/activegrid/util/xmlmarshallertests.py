#----------------------------------------------------------------------------
# Name:         xmlmarshallertests.py
# Purpose:
#
# Author:       John Spurling
#
# Created:      8/16/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import unittest
import xmlmarshaller
from xmlprettyprinter import xmlprettyprint

marshalledPersonObject = """
<person objtype="Person">
  <firstName>Albert</firstName>
  <lastName>Camus</lastName>
  <address>23 Absurd St.</address>
  <city>Ennui</city>
  <state>MO</state>
  <zip>54321</zip>
  <_phoneNumber>808-303-2323</_phoneNumber>
  <favoriteWords objtype="list">
    <item>angst</item>
    <item>ennui</item>
    <item>existence</item>
  </favoriteWords>
  <weight objtype="float">150</weight>
</person>
"""

marshalledint = '''
<item objtype="int">23</item>
'''

marshalledlist = '''
<mylist objtype="list">
    <item>foo</item>
    <item>bar</item>
</mylist>
'''

## a dummy class taken from the old XmlMarshaller module.
## class Person:
##     def __init__(self):
##         # These are not necessary but are nice if you want to tailor
##         # the Python object <-> XML binding

##         # The xml element name to use for this object, otherwise it
##         # will use a fully qualified Python name like __main__.Person
##         # which can be ugly.
##         self.__xmlname__ = "person"
##         self.firstName = None
##         self.lastName = None
##         self.addressLine1 = None
##         self.addressLine2 = None
##         self.city = None
##         self.state = None
##         self.zip = None
##         self._phoneNumber = None
##         self.favoriteWords = None
##         self.weight = None
class Person:
    __xmlflattensequence__ = {'asequence': ('the_earth_is_flat',)}

class XmlMarshallerTestFunctions(unittest.TestCase):

    def setUp(self):
        '''common setup code goes here.'''
        pass

    def testInt(self):
        xml = xmlmarshaller.marshal(1)
        print "\n#########################################"
        print  "# testString test case                  #"
        print  "#########################################"
        print "marshalled int object:\n"
        print xmlprettyprint(xml)

    def testDict(self):
        xml = xmlmarshaller.marshal({'one': 1,
                                     'two': 2,
                                     'three': 3})
        print "\n#########################################"
        print  "# testString test case                  #"
        print  "#########################################"
        print "marshalled dict object:\n"
        print xmlprettyprint(xml)

    def testBool(self):
        xmltrue = xmlmarshaller.marshal(True)
        xmlfalse = xmlmarshaller.marshal(False)
        print "\n#########################################"
        print  "# testBool test case                    #"
        print  "#########################################"
        print "marshalled boolean true object:\n"
        print xmlprettyprint(xmltrue)
        print "\nmarshalled boolean false object:\n"
        print xmlprettyprint(xmlfalse)
        pytrue = xmlmarshaller.unmarshal(xmltrue)
        assert pytrue is True
        pyfalse = xmlmarshaller.unmarshal(xmlfalse)
        assert pyfalse is False

    def testString(self):
        xml = xmlmarshaller.marshal(
            "all your marshalled objects are belong to us")
        print "\n#########################################"
        print  "# testString test case                  #"
        print  "#########################################"
        print xmlprettyprint(xml)

    def testEmptyElement(self):
        person = Person()
        person.firstName = "Albert"
        person.__xmlattributes__ = ('firstName',)
        xml = xmlmarshaller.marshal(person, 'person')
        print "\n#########################################"
        print  "# testEmptyElement test case            #"
        print  "#########################################"
        print xml
        assert (xml == """<person objtype="__main__.Person" firstName="Albert"/>""")

    def testXMLFlattenSequence(self):
        person = Person()
        person.asequence = ('one', 'two')
        xml = xmlmarshaller.marshal(person, 'person')
        print "\n#########################################"
        print  "# testXMLFlattenSequence test case      #"
        print  "#########################################"
        print xml
        assert (xml == """<person objtype="__main__.Person"><the_earth_is_flat>one</the_earth_is_flat><the_earth_is_flat>two</the_earth_is_flat></person>""")
        unmarshalledperson = xmlmarshaller.unmarshal(xml)
        assert(hasattr(unmarshalledperson, 'asequence'))
        assert(len(unmarshalledperson.asequence) == 2)

    def testInstance(self):
        print "\n#########################################"
        print  "# testInstance test case                #"
        print  "#########################################"
        class Foo:
            def __init__(self):
                self.alist = [1,2]
                self.astring = 'f00'
        f = Foo()
        xml = xmlmarshaller.marshal(f, 'foo')
        print xml

    def testPerson(self):
        person = Person()
        person.firstName = "Albert"
        person.lastName = "Camus"
        person.addressLine1 = "23 Absurd St."
        person.city = "Ennui"
        person.state = "MO"
        person.zip = "54321"
        person._phoneNumber = "808-303-2323"
        person.favoriteWords = ['angst', 'ennui', 'existence']
        person.weight = 150
#        __xmlattributes__ = ('fabulousness',)
        person.fabulousness = "tres tres"
        xml = xmlmarshaller.marshal(person)
        print "\n#########################################"
        print  "# testPerson test case                  #"
        print  "#########################################"
        print "Person object marshalled into XML:\n"
        print xml
        # When encountering a "person" element, use the Person class
##         elementMappings = { "person" : Person }
##         obj = unmarshal(xml, elementMappings = elementMappings)
##         print "Person object recreated from XML with attribute types indicated:"
##         print obj.person.__class__
##         for (attr, value) in obj.person.__dict__.items():
##            if not attr.startswith("__"):
##                print attr, "=", value, type(value)
##         print


if __name__ == "__main__":
    unittest.main()
