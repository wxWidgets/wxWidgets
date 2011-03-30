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
#include "wx/scopedptr.h"
#include "wx/sstream.h"

#include <stdarg.h>

// ----------------------------------------------------------------------------
// helpers for testing XML tree
// ----------------------------------------------------------------------------

namespace
{

void CheckXml(const wxScopedPtr<wxXmlNode>& n, ...)
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
        CPPUNIT_ASSERT( child->GetParent() == n.get() );

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
        CPPUNIT_TEST( CDATA );
        CPPUNIT_TEST( Escaping );
    CPPUNIT_TEST_SUITE_END();

    void InsertChild();
    void InsertChildAfter();
    void LoadSave();
    void CDATA();
    void Escaping();

    DECLARE_NO_COPY_CLASS(XmlTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( XmlTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( XmlTestCase, "XmlTestCase" );

void XmlTestCase::InsertChild()
{
    wxScopedPtr<wxXmlNode> root(new wxXmlNode(wxXML_ELEMENT_NODE, "root"));
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
    wxScopedPtr<wxXmlNode> root(new wxXmlNode(wxXML_ELEMENT_NODE, "root"));

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


#if wxUSE_UNICODE
    const char *utf8xmlText =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<word>\n"
"  <lang name=\"fr\">\xc3\xa9t\xc3\xa9</lang>\n"
"  <lang name=\"ru\">\xd0\xbb\xd0\xb5\xd1\x82\xd0\xbe</lang>\n"
"</word>\n"
    ;

    wxStringInputStream sis8(wxString::FromUTF8(utf8xmlText));
    CPPUNIT_ASSERT( doc.Load(sis8) );

    // this contents can't be represented in Latin-1 as it contains Cyrillic
    // letters
    doc.SetFileEncoding("ISO-8859-1");
    CPPUNIT_ASSERT( !doc.Save(sos) );

    // but it should work in UTF-8
    wxStringOutputStream sos8;
    doc.SetFileEncoding("UTF-8");
    CPPUNIT_ASSERT( doc.Save(sos8) );
    CPPUNIT_ASSERT_EQUAL( wxString(utf8xmlText),
                          wxString(sos8.GetString().ToUTF8()) );
#endif // wxUSE_UNICODE
}

void XmlTestCase::CDATA()
{
    const char *xmlText =
        "<?xml version=\"1.0\" encoding=\"windows-1252\"?>\n"
        "<name>\n"
        "  <![CDATA[Giovanni Mittone]]>\n"
        "</name>\n"
    ;

    wxStringInputStream sis(xmlText);
    wxXmlDocument doc;
    CPPUNIT_ASSERT( doc.Load(sis) );

    wxXmlNode *n = doc.GetRoot();
    CPPUNIT_ASSERT( n );

    n = n->GetChildren();
    CPPUNIT_ASSERT( n );

    // check that both leading ("  ") and trailing white space is not part of
    // the node contents when CDATA is used and wxXMLDOC_KEEP_WHITESPACE_NODES
    // is not
    CPPUNIT_ASSERT_EQUAL( "Giovanni Mittone", n->GetContent() );
}

void XmlTestCase::Escaping()
{
    // Verify that attribute values are escaped correctly, see
    // http://trac.wxwidgets.org/ticket/12275

    const char *xmlText =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<root text=\"hello&#xD;&#xA;this is a new line\">\n"
"  <x/>\n"
"</root>\n"
    ;

    wxStringInputStream sis(xmlText);

    wxXmlDocument doc;
    CPPUNIT_ASSERT( doc.Load(sis) );

    wxStringOutputStream sos;
    CPPUNIT_ASSERT( doc.Save(sos) );

    CPPUNIT_ASSERT_EQUAL( xmlText, sos.GetString() );
}
