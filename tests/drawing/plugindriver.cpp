///////////////////////////////////////////////////////////////////////////////
// Name:        tests/drawing/plugindriver.cpp
// Purpose:     Plugin management for the drawing tests
// Author:      Armel Asselin
// Created:     2014-02-28
// Copyright:   (c) 2014 Ellié Computing <opensource@elliecomputing.com>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "drawing.h"

#if wxUSE_TEST_GC_DRAWING
#include "wx/dynlib.h"

void GraphicsContextDrawingTestCase::RunPluginsDrawingCase (
    const DrawingTestCase & testCase)
{
    if (!m_drawingPluginsLoaded)
    {
        m_drawingPluginsLoaded = true;

        wxString pluginsListStr;
        if (!wxGetEnv ("WX_TEST_SUITE_GC_DRAWING_PLUGINS", &pluginsListStr))
            return; // no plugins

        wxArrayString pluginsNameArray = wxSplit (pluginsListStr, ',', '\0');
        m_drawingPlugins.resize (pluginsNameArray.size());

        for (size_t idx=0; idx<pluginsNameArray.size(); ++idx)
        {
            PluginInfo &pluginBeingLoaded = m_drawingPlugins[idx];
            pluginBeingLoaded.library = new wxDynamicLibrary;
            if (!pluginBeingLoaded.library->Load (pluginsNameArray[idx]))
            {
                wxLogFatalError("could not load drawing plugin %s",
                    pluginsNameArray[idx]);
                return;
            }

            wxDYNLIB_FUNCTION(CreateDrawingTestLifeCycleFunction,
                CreateDrawingTestLifeCycle, *pluginBeingLoaded.library);
            wxDYNLIB_FUNCTION(DestroyDrawingTestLifeCycleFunction,
                DestroyDrawingTestLifeCycle, *pluginBeingLoaded.library);

            if (!pfnCreateDrawingTestLifeCycle ||
                !pfnDestroyDrawingTestLifeCycle)
            {
                wxLogFatalError("could not find function"
                    " CreateDrawingTestLifeCycle or "
                    "DestroyDrawingTestLifeCycle in %s", pluginsNameArray[idx]);
                return;
            }

            pluginBeingLoaded.destroy = pfnDestroyDrawingTestLifeCycle;
            pluginBeingLoaded.gcFactory = (*pfnCreateDrawingTestLifeCycle)();
            if (!pluginBeingLoaded.gcFactory)
            {
                wxLogFatalError("failed to create life-cycle object in %s",
                    pluginsNameArray[idx]);
                return;
            }
        }
    }

    // now execute the test case for each plugin
    for (size_t idxp=0; idxp<m_drawingPlugins.size(); ++idxp)
    {
        RunIndividualDrawingCase (*m_drawingPlugins[idxp].gcFactory, testCase);
    }
}

///////////////////////////////////////////////////////////////////////////////
// GraphicsContextDrawingTestCase::PluginInfo /////////////////////////////////

GraphicsContextDrawingTestCase::PluginInfo::PluginInfo()
    : library(nullptr), gcFactory(nullptr), destroy(nullptr) {
}

GraphicsContextDrawingTestCase::PluginInfo::~PluginInfo()
{
    if (destroy && gcFactory)
        (*destroy)(gcFactory);

    delete library;
}

#endif // wxUSE_TEST_GC_DRAWING
