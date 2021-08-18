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

#include "wx/module.h"
#include "wx/wxcrt.h"       // for wxStrcat()

static bool gs_wasInitialized = wxModule::AreInitialized();

// ----------------------------------------------------------------------------
// test classes derived from wxModule
// ----------------------------------------------------------------------------

wxString g_strLoadOrder;

class Module : public wxModule
{
protected:
    virtual bool OnInit() wxOVERRIDE { g_strLoadOrder += GetClassInfo()->GetClassName(); return true; }
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
// tests themselves
// ----------------------------------------------------------------------------

TEST_CASE("wxModule::Initialized", "[module]")
{
    CHECK( !gs_wasInitialized );
    CHECK( wxModule::AreInitialized() );
}

TEST_CASE("wxModule::LoadOrder", "[module]")
{
    // module D is the only one with no dependencies and so should load as first (and so on):
    CHECK( g_strLoadOrder == "ModuleDModuleCModuleBModuleA" );
}
