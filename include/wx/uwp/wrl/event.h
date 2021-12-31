///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/wrl/event.h
// Purpose:     WRL event callback implementation
// Author:      nns52k
// Created:     2021-02-25
// Copyright:   (c) 2021 wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_WRL_H_
#define _WX_MSW_PRIVATE_WRL_H_

#include <atomic>

template <typename baseT, typename ...argTs>
class CInvokable : public baseT
{
public:
    CInvokable() : m_nRefCount(0) {}
    virtual ~CInvokable() {}
    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObj) override
    {
        if (riid == __uuidof(baseT) || riid == IID_IUnknown)
        {
            *ppvObj = this;
            AddRef();
            return S_OK;
        }

        *ppvObj = NULL;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override
    {
        return ++m_nRefCount;
    }
    ULONG STDMETHODCALLTYPE Release() override
    {
        size_t ret = --m_nRefCount;
        if (ret == 0)
            delete this;
        return ret;
    }
private:
    std::atomic<size_t> m_nRefCount;
};

template <typename baseT, typename ...argTs>
class CInvokableLambda : public CInvokable<baseT, argTs...>
{
public:
    CInvokableLambda(std::function<HRESULT(argTs...)> lambda)
        : m_lambda(lambda)
    {}
    // baseT method
    HRESULT STDMETHODCALLTYPE Invoke(argTs ...args) override
    {
        return m_lambda(args...);
    }
private:
    std::function<HRESULT(argTs...)> m_lambda;
};

template <typename baseT, typename contextT, typename ...argTs>
class CInvokableMethod : public CInvokable<baseT, argTs...>
{
public:
    CInvokableMethod(contextT* ctx, HRESULT(contextT::* mthd)(argTs...))
        : m_ctx(ctx), m_mthd(mthd)
    {}
    // baseT method
    HRESULT STDMETHODCALLTYPE Invoke(argTs ...args) override
    {
        return (m_ctx->*m_mthd)(args...);
    }
private:
    contextT* m_ctx;
    HRESULT(contextT::* m_mthd)(argTs...);
};

// the function templates to generate concrete classes from above class templates
template <
    typename baseT, typename lambdaT, // base type & lambda type
    typename LT, typename ...argTs // for capturing argument types of lambda
>
wxCOMPtr<baseT> Callback_impl(lambdaT&& lambda, HRESULT(LT::*)(argTs...) const)
{
    return wxCOMPtr<baseT>(new CInvokableLambda<baseT, argTs...>(lambda));
}

template <typename baseT, typename lambdaT>
wxCOMPtr<baseT> Callback(lambdaT&& lambda)
{
    return Callback_impl<baseT>(std::move(lambda), &lambdaT::operator());
}

template <typename baseT, typename contextT, typename ...argTs>
wxCOMPtr<baseT> Callback(contextT* ctx, HRESULT(contextT::* mthd)(argTs...))
{
    return wxCOMPtr<baseT>(new CInvokableMethod<baseT, contextT, argTs...>(ctx, mthd));
}

#endif // _WX_MSW_PRIVATE_WRL_H_
