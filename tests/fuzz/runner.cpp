///////////////////////////////////////////////////////////////////////////////
// Name:        tests/fuzz/runner.cpp
// Purpose:     Main function for running fuzzers with a single input file
// Author:      Vadim Zeitlin
// Created:     2017-10-28
// Copyright:   (c) 2017 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// Normally fuzzers are run by libFuzzer, which executes the entry function
// LLVMFuzzerTestOneInput() with many different inputs, but it can be useful to
// run them with just a single input to check a particular problem found during
// fuzzing. To do this, link the fuzzer code with this file and run it with the
// file name containing the test data. E.g. an example use:
//
//  $ g++ -g -fsanitize=undefined tests/fuzz/{zip,runner}.cpp `wx-config --cxxflags --libs base`
//  $ ./a.out testcase-found-by-libfuzzer

#include "wx/buffer.h"
#include "wx/crt.h"
#include "wx/ffile.h"
#include "wx/init.h"

// The fuzzer entry function.
extern "C" int LLVMFuzzerTestOneInput(const wxUint8 *data, size_t size);

int main(int argc, char** argv)
{
    wxInitializer init(argc, argv);
    if ( !init.IsOk() )
    {
        wxPrintf("Initializing wxWidgets failed.\n");
        return 1;
    }

    if ( argc != 2 )
    {
        wxPrintf("Usage: %s <input-file>\n", argv[0]);
        return 2;
    }

    wxFFile file(argv[1], "rb");
    if ( !file.IsOpened() )
    {
        wxPrintf("Failed to open the input file \"%s\".\n", argv[1]);
        return 3;
    }

    const wxFileOffset ofs = file.Length();
    if ( ofs < 0 )
    {
        wxPrintf("Failed to get the input file \"%s\" size.\n", argv[1]);
        return 3;
    }

    const size_t len = ofs;
    wxMemoryBuffer buf(len);
    if ( file.Read(buf.GetData(), len) != len )
    {
        wxPrintf("Failed to read from the input file \"%s\".\n", argv[1]);
        return 3;
    }

    return LLVMFuzzerTestOneInput(static_cast<wxUint8*>(buf.GetData()), len);
}
