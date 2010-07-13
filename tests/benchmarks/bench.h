/////////////////////////////////////////////////////////////////////////////
// Name:        tests/benchmarks/bench.h
// Purpose:     Main header of the benchmarking suite
// Author:      Vadim Zeitlin
// Created:     2008-07-19
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
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
    typedef bool (*InitType)();
    typedef bool (*FuncType)();
    typedef void (*DoneType)();

    /// Ctor is used implicitly by BENCHMARK_FUNC().
    Function(const char *name,
             FuncType func,
             InitType init = NULL,
             DoneType done = NULL)
        : m_name(name),
          m_func(func),
          m_init(init),
          m_done(done),
          m_next(ms_head)
    {
        ms_head = this;
    }

    /// Get the name of this function
    const char *GetName() const { return m_name; }

    /// Perform once-only initialization prior to Run().
    bool Init() { return m_init ? (*m_init)() : true; }

    /// Run the function, return its return value.
    bool Run() { return (*m_func)(); }

    /// Clean up after performing the benchmark.
    void Done() { if ( m_done ) (*m_done)(); }

    /// Get the head of the linked list of benchmark objects
    static Function *GetFirst() { return ms_head; }

    /// Get the next object in the linked list or NULL
    Function *GetNext() const { return m_next; }

private:
    // the head of the linked list of Bench::Function objects
    static Function *ms_head;

    // name of and pointer to the function, as passed to the ctor
    const char * const m_name;
    const FuncType m_func;
    const InitType m_init;
    const DoneType m_done;

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

/**
    Define a benchmark function requiring initialization and shutdown.

    This macro is similar to BENCHMARK_FUNC() but ensures that @a init is
    called before the benchmark is ran and @a done afterwards.
 */
#define BENCHMARK_FUNC_WITH_INIT(name, init, done)                            \
    static bool name();                                                       \
    static Bench::Function wxMAKE_UNIQUE_NAME(name)(#name, name, init, done); \
    bool name()

#endif // _WX_TESTS_BENCHMARKS_BENCH_H_
