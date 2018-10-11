///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/module.cpp
// Purpose:     Test wxModule
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-06-02
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#include "wx/module.h"
#include "wx/wxcrt.h"       // for wxStrcat()

// ----------------------------------------------------------------------------
// test classes derived from wxModule
// ----------------------------------------------------------------------------

char g_strLoadOrder[256] = "\0";

class Module : public wxModule
{
protected:
    virtual bool OnInit() wxOVERRIDE { wxStrcat(g_strLoadOrder, GetClassInfo()->GetClassName()); return true; }
    virtual void OnExit() wxOVERRIDE { }
};

class ModuleA : public Module
{
public:
    ModuleA();
private:
    wxDECLARE_DYNAMIC_CLASS(ModuleA);
};

class ModuleB : public Module
{
public:
    ModuleB();
private:
    wxDECLARE_DYNAMIC_CLASS(ModuleB);
};

class ModuleC : public Module
{
public:
    ModuleC();
private:
    wxDECLARE_DYNAMIC_CLASS(ModuleC);
};

class ModuleD : public Module
{
public:
    ModuleD();
private:
    wxDECLARE_DYNAMIC_CLASS(ModuleD);
};

wxIMPLEMENT_DYNAMIC_CLASS(ModuleA, wxModule);
ModuleA::ModuleA()
{
    AddDependency(CLASSINFO(ModuleB));
    AddDependency(CLASSINFO(ModuleD));
}

wxIMPLEMENT_DYNAMIC_CLASS(ModuleB, wxModule);
ModuleB::ModuleB()
{
    AddDependency(CLASSINFO(ModuleC));
    AddDependency(CLASSINFO(ModuleD));
}

wxIMPLEMENT_DYNAMIC_CLASS(ModuleC, wxModule);
ModuleC::ModuleC()
{
    AddDependency(CLASSINFO(ModuleD));
}

wxIMPLEMENT_DYNAMIC_CLASS(ModuleD, wxModule);
ModuleD::ModuleD()
{
}

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ModuleTestCase : public CppUnit::TestCase
{
public:
    ModuleTestCase() { }

private:
    CPPUNIT_TEST_SUITE( ModuleTestCase );
        CPPUNIT_TEST( LoadOrder );
    CPPUNIT_TEST_SUITE_END();

    void LoadOrder();
    wxDECLARE_NO_COPY_CLASS(ModuleTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ModuleTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ModuleTestCase, "ModuleTestCase" );

void ModuleTestCase::LoadOrder()
{
    // module D is the only one with no dependencies and so should load as first (and so on):
    CPPUNIT_ASSERT_EQUAL( std::string("ModuleDModuleCModuleBModuleA"),
                          g_strLoadOrder );
}
