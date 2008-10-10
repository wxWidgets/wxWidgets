///////////////////////////////////////////////////////////////////////////////
// Name:        tests/xml/xmltest.cpp
// Purpose:     XML classes unit test
// Author:      Vaclav Slavik
// Created:     2008-03-29
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vaclav Slavik
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/xml/xml.h"
#include "wx/sstream.h"

#include <stdarg.h>

// ----------------------------------------------------------------------------
// helpers for testing XML tree
// ----------------------------------------------------------------------------

namespace
{

void CheckXml(wxXmlNode *n, ...)
{
    va_list args;
    va_start(args, n);

    wxXmlNode *child = n->GetChildren();

    for (;;)
    {
        const char *childName = va_arg(args, char*);
        if ( childName == NULL )
            break;

        CPPUNIT_ASSERT( child );
        CPPUNIT_ASSERT_EQUAL( childName, child->GetName() );
        CPPUNIT_ASSERT( child->GetChildren() == NULL );
        CPPUNIT_ASSERT( child->GetParent() == n );

        child = child->GetNext();
    }

    va_end(args);

    CPPUNIT_ASSERT( child == NULL ); // no more children
}

} // anon namespace

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class XmlTestCase : public CppUnit::TestCase
{
public:
    XmlTestCase() {}

private:
    CPPUNIT_TEST_SUITE( XmlTestCase );
        CPPUNIT_TEST( InsertChild );
        CPPUNIT_TEST( InsertChildAfter );
        CPPUNIT_TEST( LoadSave );
    CPPUNIT_TEST_SUITE_END();

    void InsertChild();
    void InsertChildAfter();
    void LoadSave();

    DECLARE_NO_COPY_CLASS(XmlTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( XmlTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( XmlTestCase, "XmlTestCase" );

void XmlTestCase::InsertChild()
{
    wxXmlNode *root = new wxXmlNode(wxXML_ELEMENT_NODE, "root");
    root->AddChild(new wxXmlNode(wxXML_ELEMENT_NODE, "1"));
    wxXmlNode *two = new wxXmlNode(wxXML_ELEMENT_NODE, "2");
    root->AddChild(two);
    root->AddChild(new wxXmlNode(wxXML_ELEMENT_NODE, "3"));
    CheckXml(root, "1", "2", "3", NULL);

    // check inserting in front:
    root->InsertChild(new wxXmlNode(wxXML_ELEMENT_NODE, "A"), NULL);
    CheckXml(root, "A", "1", "2", "3", NULL);
    root->InsertChild(new wxXmlNode(wxXML_ELEMENT_NODE, "B"), root->GetChildren());
    CheckXml(root, "B", "A", "1", "2", "3", NULL);

    // and in the middle:
    root->InsertChild(new wxXmlNode(wxXML_ELEMENT_NODE, "C"), two);
    CheckXml(root, "B", "A", "1", "C", "2", "3", NULL);
}

void XmlTestCase::InsertChildAfter()
{
    wxXmlNode *root = new wxXmlNode(wxXML_ELEMENT_NODE, "root");

    root->InsertChildAfter(new wxXmlNode(wxXML_ELEMENT_NODE, "1"), NULL);
    CheckXml(root, "1", NULL);

    wxXmlNode *two = new wxXmlNode(wxXML_ELEMENT_NODE, "2");
    root->AddChild(two);
    wxXmlNode *three = new wxXmlNode(wxXML_ELEMENT_NODE, "3");
    root->AddChild(three);
    CheckXml(root, "1", "2", "3", NULL);

    // check inserting in the middle:
    root->InsertChildAfter(new wxXmlNode(wxXML_ELEMENT_NODE, "A"), root->GetChildren());
    CheckXml(root, "1", "A", "2", "3", NULL);
    root->InsertChildAfter(new wxXmlNode(wxXML_ELEMENT_NODE, "B"), two);
    CheckXml(root, "1", "A", "2", "B", "3", NULL);

    // and at the end:
    root->InsertChildAfter(new wxXmlNode(wxXML_ELEMENT_NODE, "C"), three);
    CheckXml(root, "1", "A", "2", "B", "3", "C", NULL);
}

void XmlTestCase::LoadSave()
{
    // NB: this is not real XRC but rather some XRC-like XML fragment which
    //     exercises different XML constructs to check that they're saved back
    //     correctly
    //
    // Also note that there should be no blank lines here as they disappear
    // after saving.
    const char *xmlText =
"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
"<resource xmlns=\"http://www.wxwidgets.org/wxxrc\" version=\"2.3.0.1\">\n"
"  <object class=\"wxDialog\" name=\"my_dialog\">\n"
"    <children>\n"
"      <grandchild id=\"1\"/>\n"
"    </children>\n"
"    <subobject/>\n"
"  </object>\n"
"</resource>\n"
    ;

    wxStringInputStream sis(xmlText);

    wxXmlDocument doc;
    CPPUNIT_ASSERT( doc.Load(sis) );

    wxStringOutputStream sos;
    CPPUNIT_ASSERT( doc.Save(sos) );

    CPPUNIT_ASSERT_EQUAL( xmlText, sos.GetString() );
}

