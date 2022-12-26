///////////////////////////////////////////////////////////////////////////////
// Name:        tests/xml/xmltest.cpp
// Purpose:     XML classes unit test
// Author:      Vaclav Slavik
// Created:     2008-03-29
// Copyright:   (c) 2008 Vaclav Slavik
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


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

void CheckXml(const wxXmlNode *n, ...)
{
    va_list args;
    va_start(args, n);

    wxXmlNode *child = n->GetChildren();

    for (;;)
    {
        const char *childName = va_arg(args, char*);
        if ( childName == nullptr )
            break;

        CPPUNIT_ASSERT( child );
        CPPUNIT_ASSERT_EQUAL( childName, child->GetName() );
        CPPUNIT_ASSERT( child->GetChildren() == nullptr );
        CPPUNIT_ASSERT( child->GetParent() == n );

        child = child->GetNext();
    }

    va_end(args);

    CPPUNIT_ASSERT( child == nullptr ); // no more children
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
        CPPUNIT_TEST( PI );
        CPPUNIT_TEST( Escaping );
        CPPUNIT_TEST( DetachRoot );
        CPPUNIT_TEST( AppendToProlog );
        CPPUNIT_TEST( SetRoot );
        CPPUNIT_TEST( CopyNode );
        CPPUNIT_TEST( CopyDocument );
        CPPUNIT_TEST( Doctype );
    CPPUNIT_TEST_SUITE_END();

    void InsertChild();
    void InsertChildAfter();
    void LoadSave();
    void CDATA();
    void PI();
    void Escaping();
    void DetachRoot();
    void AppendToProlog();
    void SetRoot();
    void CopyNode();
    void CopyDocument();
    void Doctype();

    wxDECLARE_NO_COPY_CLASS(XmlTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( XmlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( XmlTestCase, "XmlTestCase" );

void XmlTestCase::InsertChild()
{
    wxScopedPtr<wxXmlNode> root(new wxXmlNode(wxXML_ELEMENT_NODE, "root"));
    root->AddChild(new wxXmlNode(wxXML_ELEMENT_NODE, "1"));
    wxXmlNode *two = new wxXmlNode(wxXML_ELEMENT_NODE, "2");
    root->AddChild(two);
    root->AddChild(new wxXmlNode(wxXML_ELEMENT_NODE, "3"));
    CheckXml(root.get(), "1", "2", "3", nullptr);

    // check inserting in front:
    root->InsertChild(new wxXmlNode(wxXML_ELEMENT_NODE, "A"), nullptr);
    CheckXml(root.get(), "A", "1", "2", "3", nullptr);
    root->InsertChild(new wxXmlNode(wxXML_ELEMENT_NODE, "B"), root->GetChildren());
    CheckXml(root.get(), "B", "A", "1", "2", "3", nullptr);

    // and in the middle:
    root->InsertChild(new wxXmlNode(wxXML_ELEMENT_NODE, "C"), two);
    CheckXml(root.get(), "B", "A", "1", "C", "2", "3", nullptr);
}

void XmlTestCase::InsertChildAfter()
{
    wxScopedPtr<wxXmlNode> root(new wxXmlNode(wxXML_ELEMENT_NODE, "root"));

    root->InsertChildAfter(new wxXmlNode(wxXML_ELEMENT_NODE, "1"), nullptr);
    CheckXml(root.get(), "1", nullptr);

    wxXmlNode *two = new wxXmlNode(wxXML_ELEMENT_NODE, "2");
    root->AddChild(two);
    wxXmlNode *three = new wxXmlNode(wxXML_ELEMENT_NODE, "3");
    root->AddChild(three);
    CheckXml(root.get(), "1", "2", "3", nullptr);

    // check inserting in the middle:
    root->InsertChildAfter(new wxXmlNode(wxXML_ELEMENT_NODE, "A"), root->GetChildren());
    CheckXml(root.get(), "1", "A", "2", "3", nullptr);
    root->InsertChildAfter(new wxXmlNode(wxXML_ELEMENT_NODE, "B"), two);
    CheckXml(root.get(), "1", "A", "2", "B", "3", nullptr);

    // and at the end:
    root->InsertChildAfter(new wxXmlNode(wxXML_ELEMENT_NODE, "C"), three);
    CheckXml(root.get(), "1", "A", "2", "B", "3", "C", nullptr);
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
"  <!-- Test comment -->\n"
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

    const char *xmlTextProlog =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!DOCTYPE resource PUBLIC \"Public-ID\" 'System\"ID\"'>\n"
"<!-- Prolog comment -->\n"
"<?xml-stylesheet href=\"style.css\" type=\"text/css\"?>\n"
"<resource xmlns=\"http://www.wxwidgets.org/wxxrc\" version=\"2.3.0.1\">\n"
"  <!-- Test comment -->\n"
"  <object class=\"wxDialog\" name=\"my_dialog\">\n"
"    <children>\n"
"      <grandchild id=\"1\"/>\n"
"    </children>\n"
"    <subobject/>\n"
"  </object>\n"
"</resource>\n"
"<!-- Trailing comment -->\n"
    ;

    wxStringInputStream sisp(xmlTextProlog);
    CPPUNIT_ASSERT( doc.Load(sisp, "UTF-8") );

    wxStringOutputStream sosp;
    CPPUNIT_ASSERT( doc.Save(sosp) );

    CPPUNIT_ASSERT_EQUAL( xmlTextProlog, sosp.GetString() );
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

void XmlTestCase::PI()
{
    const char *xmlText =
        "<?xml version=\"1.0\" encoding=\"windows-1252\"?>\n"
        "<root>\n"
        "  <?robot index=\"no\" follow=\"no\"?>\n"
        "</root>\n"
    ;

    wxStringInputStream sis(xmlText);
    wxXmlDocument doc;
    CPPUNIT_ASSERT( doc.Load(sis) );

    wxXmlNode *n = doc.GetRoot();
    CPPUNIT_ASSERT( n );

    n = n->GetChildren();
    CPPUNIT_ASSERT( n );

    CPPUNIT_ASSERT_EQUAL( "index=\"no\" follow=\"no\"", n->GetContent() );
}

void XmlTestCase::Escaping()
{
    // Verify that attribute values are escaped correctly, see
    // https://github.com/wxWidgets/wxWidgets/issues/12275

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

void XmlTestCase::DetachRoot()
{
    const char *xmlTextProlog =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!-- Prolog comment -->\n"
"<?xml-stylesheet href=\"style.css\" type=\"text/css\"?>\n"
"<resource xmlns=\"http://www.wxwidgets.org/wxxrc\" version=\"2.3.0.1\">\n"
"  <!-- Test comment -->\n"
"  <object class=\"wxDialog\" name=\"my_dialog\">\n"
"    <children>\n"
"      <grandchild id=\"1\"/>\n"
"    </children>\n"
"    <subobject/>\n"
"  </object>\n"
"</resource>\n"
"<!-- Trailing comment -->\n"
    ;
    const char *xmlTextHtm =
"<?xml version=\"1.0\" encoding=\"windows-1252\"?>\n"
"<html>\n"
"  <head>\n"
"    <title>Testing wxXml</title>\n"
"  </head>\n"
"  <body>\n"
"    <p>Some body text</p>\n"
"  </body>\n"
"</html>\n"
    ;
    wxXmlDocument doc;

    wxStringInputStream sish(xmlTextHtm);
    CPPUNIT_ASSERT( doc.Load(sish) );

    wxXmlNode *root = doc.DetachRoot();

    wxStringInputStream sisp(xmlTextProlog);
    CPPUNIT_ASSERT( doc.Load(sisp) );

    doc.SetRoot(root);

    wxStringOutputStream sos;
    CPPUNIT_ASSERT( doc.Save(sos) );

    const char *xmlTextResult1 =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!-- Prolog comment -->\n"
"<?xml-stylesheet href=\"style.css\" type=\"text/css\"?>\n"
"<html>\n"
"  <head>\n"
"    <title>Testing wxXml</title>\n"
"  </head>\n"
"  <body>\n"
"    <p>Some body text</p>\n"
"  </body>\n"
"</html>\n"
"<!-- Trailing comment -->\n"
    ;
    CPPUNIT_ASSERT_EQUAL( xmlTextResult1, sos.GetString() );

    wxStringInputStream sisp2(xmlTextProlog);
    CPPUNIT_ASSERT( doc.Load(sisp2) );

    root = doc.DetachRoot();

    wxStringInputStream sish2(xmlTextHtm);
    CPPUNIT_ASSERT( doc.Load(sish2) );

    doc.SetRoot(root);

    wxStringOutputStream sos2;
    CPPUNIT_ASSERT( doc.Save(sos2) );

    const char *xmlTextResult2 =
"<?xml version=\"1.0\" encoding=\"windows-1252\"?>\n"
"<resource xmlns=\"http://www.wxwidgets.org/wxxrc\" version=\"2.3.0.1\">\n"
"  <!-- Test comment -->\n"
"  <object class=\"wxDialog\" name=\"my_dialog\">\n"
"    <children>\n"
"      <grandchild id=\"1\"/>\n"
"    </children>\n"
"    <subobject/>\n"
"  </object>\n"
"</resource>\n"
    ;
    CPPUNIT_ASSERT_EQUAL( xmlTextResult2, sos2.GetString() );
}

void XmlTestCase::AppendToProlog()
{
    const char *xmlText =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<root>\n"
"  <p>Some text</p>\n"
"</root>\n"
    ;
    wxXmlDocument rootdoc;
    wxStringInputStream sis(xmlText);
    CPPUNIT_ASSERT( rootdoc.Load(sis) );
    wxXmlNode *root = rootdoc.DetachRoot();

    wxXmlNode *comment1 = new wxXmlNode(wxXML_COMMENT_NODE, "comment",
        " 1st prolog entry ");
    wxXmlNode *pi = new wxXmlNode(wxXML_PI_NODE, "xml-stylesheet",
        "href=\"style.css\" type=\"text/css\"");
    wxXmlNode *comment2 = new wxXmlNode(wxXML_COMMENT_NODE, "comment",
        " 3rd prolog entry ");

    wxXmlDocument doc;
    doc.AppendToProlog( comment1 );
    doc.AppendToProlog( pi );
    doc.SetRoot( root );
    doc.AppendToProlog( comment2 );

    wxStringOutputStream sos;
    CPPUNIT_ASSERT( doc.Save(sos) );

    const char *xmlTextResult =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!-- 1st prolog entry -->\n"
"<?xml-stylesheet href=\"style.css\" type=\"text/css\"?>\n"
"<!-- 3rd prolog entry -->\n"
"<root>\n"
"  <p>Some text</p>\n"
"</root>\n"
    ;
    CPPUNIT_ASSERT_EQUAL( xmlTextResult, sos.GetString() );
}

void XmlTestCase::SetRoot()
{
    wxXmlDocument doc;
    CPPUNIT_ASSERT( !doc.IsOk() );
    wxXmlNode *root = new wxXmlNode(wxXML_ELEMENT_NODE, "root");

    // Test for the problem of https://github.com/wxWidgets/wxWidgets/issues/13135
    doc.SetRoot( root );
    wxXmlNode *docNode = doc.GetDocumentNode();
    CPPUNIT_ASSERT( docNode );
    CPPUNIT_ASSERT( root == docNode->GetChildren() );
    CPPUNIT_ASSERT( doc.IsOk() );

    // Other tests.
    CPPUNIT_ASSERT( docNode == root->GetParent() );
    doc.SetRoot(nullptr); // Removes from doc but dosn't free mem, doc node left.
    CPPUNIT_ASSERT( !doc.IsOk() );

    wxXmlNode *comment = new wxXmlNode(wxXML_COMMENT_NODE, "comment", "Prolog Comment");
    wxXmlNode *pi = new wxXmlNode(wxXML_PI_NODE, "target", "PI instructions");
    doc.AppendToProlog(comment);
    doc.SetRoot( root );
    doc.AppendToProlog(pi);
    CPPUNIT_ASSERT( doc.IsOk() );
    wxXmlNode *node = docNode->GetChildren();
    CPPUNIT_ASSERT( node );
    CPPUNIT_ASSERT( node->GetType() == wxXML_COMMENT_NODE );
    CPPUNIT_ASSERT( node->GetParent() == docNode );
    node = node->GetNext();
    CPPUNIT_ASSERT( node );
    CPPUNIT_ASSERT( node->GetType() == wxXML_PI_NODE );
    CPPUNIT_ASSERT( node->GetParent() == docNode );
    node = node->GetNext();
    CPPUNIT_ASSERT( node );
    CPPUNIT_ASSERT( node->GetType() == wxXML_ELEMENT_NODE );
    CPPUNIT_ASSERT( node->GetParent() == docNode );
    node = node->GetNext();
    CPPUNIT_ASSERT( !node );
    doc.SetRoot(nullptr);
    CPPUNIT_ASSERT( !doc.IsOk() );
    doc.SetRoot(root);
    CPPUNIT_ASSERT( doc.IsOk() );
}

void XmlTestCase::CopyNode()
{
    const char *xmlText =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<root>\n"
"  <first><sub1/><sub2/><sub3/></first>\n"
"  <second/>\n"
"</root>\n"
    ;
    wxXmlDocument doc;
    wxStringInputStream sis(xmlText);
    CPPUNIT_ASSERT( doc.Load(sis) );

    wxXmlNode* const root = doc.GetRoot();
    CPPUNIT_ASSERT( root );

    wxXmlNode* const first = root->GetChildren();
    CPPUNIT_ASSERT( first );

    wxXmlNode* const second = first->GetNext();
    CPPUNIT_ASSERT( second );

    *first = *second;

    wxStringOutputStream sos;
    CPPUNIT_ASSERT( doc.Save(sos) );

    const char *xmlTextResult =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<root>\n"
"  <second/>\n"
"  <second/>\n"
"</root>\n"
    ;
    CPPUNIT_ASSERT_EQUAL( xmlTextResult, sos.GetString() );
}

void XmlTestCase::CopyDocument()
{
    const char *xmlText =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!DOCTYPE resource PUBLIC \"Public-ID\" \"System'ID'\">\n"
"<!-- 1st prolog entry -->\n"
"<root>\n"
"  <first>Text</first>\n"
"  <second/>\n"
"</root>\n"
    ;
    wxXmlDocument doc1;
    wxStringInputStream sis(xmlText);
    CPPUNIT_ASSERT( doc1.Load(sis) );

    wxXmlDocument doc2 = doc1;

    wxStringOutputStream sos;
    CPPUNIT_ASSERT(doc2.Save(sos));

    CPPUNIT_ASSERT_EQUAL( xmlText, sos.GetString() );
}

void XmlTestCase::Doctype()
{
    const char *xmlText =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<!DOCTYPE root PUBLIC \"Public-ID\" 'System\"ID\"'>\n"
        "<root>\n"
        "  <content/>\n"
        "</root>\n"
    ;

    wxStringInputStream sis(xmlText);
    wxXmlDocument doc;
    CPPUNIT_ASSERT( doc.Load(sis) );

    wxXmlDoctype dt = doc.GetDoctype();

    CPPUNIT_ASSERT_EQUAL( "root", dt.GetRootName() );
    CPPUNIT_ASSERT_EQUAL( "System\"ID\"", dt.GetSystemId() );
    CPPUNIT_ASSERT_EQUAL( "Public-ID", dt.GetPublicId() );

    CPPUNIT_ASSERT( dt.IsValid() );
    CPPUNIT_ASSERT_EQUAL( "root PUBLIC \"Public-ID\" 'System\"ID\"'", dt.GetFullString() );
    dt = wxXmlDoctype( dt.GetRootName(), dt.GetSystemId() );
    CPPUNIT_ASSERT( dt.IsValid() );
    CPPUNIT_ASSERT_EQUAL( "root SYSTEM 'System\"ID\"'", dt.GetFullString() );
    dt = wxXmlDoctype( dt.GetRootName() );
    CPPUNIT_ASSERT( dt.IsValid() );
    CPPUNIT_ASSERT_EQUAL( "root", dt.GetFullString() );

    doc.SetDoctype(dt);
    wxStringOutputStream sos;
    CPPUNIT_ASSERT(doc.Save(sos));
    const char *xmlText1 =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<!DOCTYPE root>\n"
        "<root>\n"
        "  <content/>\n"
        "</root>\n"
    ;
    CPPUNIT_ASSERT_EQUAL( xmlText1, sos.GetString() );

    doc.SetDoctype(wxXmlDoctype());
    wxStringOutputStream sos2;
    CPPUNIT_ASSERT(doc.Save(sos2));
    const char *xmlText2 =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<root>\n"
        "  <content/>\n"
        "</root>\n"
    ;
    CPPUNIT_ASSERT_EQUAL( xmlText2, sos2.GetString() );

    doc.SetDoctype(wxXmlDoctype("root", "Sys'id"));
    wxStringOutputStream sos3;
    CPPUNIT_ASSERT(doc.Save(sos3));
    const char *xmlText3 =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<!DOCTYPE root SYSTEM \"Sys'id\">\n"
        "<root>\n"
        "  <content/>\n"
        "</root>\n"
    ;
    CPPUNIT_ASSERT_EQUAL( xmlText3, sos3.GetString() );

    dt = wxXmlDoctype( "", "System\"ID\"", "Public-ID" );
    CPPUNIT_ASSERT( !dt.IsValid() );
    CPPUNIT_ASSERT_EQUAL( "", dt.GetFullString() );
    // Strictly speaking, this is illegal for XML but is legal for SGML.
    dt = wxXmlDoctype( "root", "", "Public-ID" );
    CPPUNIT_ASSERT( dt.IsValid() );
    CPPUNIT_ASSERT_EQUAL( "root PUBLIC \"Public-ID\"", dt.GetFullString() );

    // Using both single and double quotes in system ID is not allowed.
    dt = wxXmlDoctype( "root", "O'Reilly (\"editor\")", "Public-ID" );
    CPPUNIT_ASSERT( !dt.IsValid() );
}

// This test is disabled by default as it requires the environment variable
// below to be defined to point to a XML file to load.
TEST_CASE("XML::Load", "[xml][.]")
{
    wxString file;
    REQUIRE( wxGetEnv("WX_TEST_XML_FILE", &file) );

    wxXmlDocument doc;
    REQUIRE( doc.Load(file) );
    CHECK( doc.IsOk() );

    wxStringOutputStream sos;
    REQUIRE( doc.Save(sos) );

    WARN("Dump of " << file << ":\n" << sos.GetString());
}
