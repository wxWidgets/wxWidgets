/////////////////////////////////////////////////////////////////////////////
// Name:        tests/benchmarks/bench.h
// Purpose:     Main header of the benchmarking suite
// Author:      Vadim Zeitlin
// Created:     2008-07-19
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_BENCHMARKS_BENCH_H_
#define _WX_TESTS_BENCHMARKS_BENCH_H_

#include "wx/cpp.h"
#include "wx/defs.h"

namespace Bench
{

/**
    Utility object used to register the benchmark functions.

    @see BENCHMARK_FUNC
 */
class Function
{
public:
    typedef bool (*Type)();

    /// Ctor is used implicitly by BENCHMARK_FUNC().
    Function(const char *name, Type func)
        : m_name(name),
          m_func(func),
          m_next(ms_head)
    {
        ms_head = this;
    }

    /// Get the name of this function
    const char *GetName() const { return m_name; }

    /// Run the function, return its return value.
    bool Run() { return (*m_func)(); }


    /// Get the head of the linked list of benchmark objects
    static Function *GetFirst() { return ms_head; }

    /// Get the next object in the linked list or NULL
    Function *GetNext() const { return m_next; }

private:
    // the head of the linked list of Bench::Function objects
    static Function *ms_head;

    // name of and pointer to the function, as passed to the ctor
    const char * const m_name;
    const Type m_func;

    // pointer to the next object in the linked list or NULL
    Function * const m_next;

    DECLARE_NO_COPY_CLASS(Function)
};

/**
    Get the numeric parameter.

    Tests may use this parameter in whatever way they see fit, by default it is
    1 but can be set to a different value by user from the command line.
 */
long GetNumericParameter();

/**
    Get the string parameter.

    Tests may use this parameter in whatever way they see fit, by default it is
    empty but can be set to a different value by user from the command line.
 */
wxString GetStringParameter();

} // namespace Bench

/**
    This macro defines a benchmark function.

    All these functions return a boolean value and take no parameters. The
    return value is needed to prevent the compiler from optimizing the
    benchmark entirely away and so typically will be constructed using the
    results of the benchmark actions, even though normally benchmark should
    always return true.

    Once benchmark is defined, it can be invoked from the command line of the
    main benchmarking program.
 */
#define BENCHMARK_FUNC(name)                                                  \
    static bool name();                                                       \
    static Bench::Function wxMAKE_UNIQUE_NAME(name)(#name, name);             \
    bool name()

#endif // _WX_TESTS_BENCHMARKS_BENCH_H_
