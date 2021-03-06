// Copyright © 2010-2015 The CefSharp Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found in the LICENSE file.

#pragma once

#include "include/cef_app.h"
#include "CefSettings.h"

using namespace CefSharp::Internals;

namespace CefSharp
{
    private class CefSharpApp : public CefApp,
        public CefBrowserProcessHandler
    {
        gcroot<CefSettings^> _cefSettings;
        gcroot<Action^> _onContextInitialized;

    public:
        CefSharpApp(CefSettings^ cefSettings, Action^ onContextInitialized) :
            _cefSettings(cefSettings),
            _onContextInitialized(onContextInitialized)
        {
        }

        ~CefSharpApp()
        {
            _cefSettings = nullptr;
            _onContextInitialized = nullptr;
        }

        virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE
        {
            return this;
        }

        virtual void OnContextInitialized() OVERRIDE
        {
            if (static_cast<Action^>(_onContextInitialized) != nullptr)
            {
                _onContextInitialized->Invoke();
            }
        }

        virtual void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) OVERRIDE
        {
            if (_cefSettings->WcfEnabled)
            {
                command_line->AppendArgument(StringUtils::ToNative(CefSharpArguments::WcfEnabledArgument));
            }
        }
        
        virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) OVERRIDE
        {
            if(_cefSettings->CefCommandLineArgs->Count == 0)
                return;

            auto commandLine = command_line.get();

            // Not clear what should happen if we 
            // * already have some command line flags given (is this possible? Perhaps from globalCommandLine)
            // * have no flags given (-> call SetProgramm() with first argument?)

            for each(KeyValuePair<String^, String^>^ kvp in _cefSettings->CefCommandLineArgs)
            {
                CefString name = StringUtils::ToNative(kvp->Key);
                CefString value = StringUtils::ToNative(kvp->Value);

                // Right now the command line args handed to the application (global command line) have higher
                // precedence than command line args provided by the app
                if(!commandLine->HasSwitch(name))
                    commandLine->AppendSwitchWithValue(name, value);
            }
        }

        virtual void OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) OVERRIDE
        {
            for each (CefCustomScheme^ cefCustomScheme in _cefSettings->CefCustomSchemes)
            {
                // TODO: Causes an "assertion failed" error here: DCHECK_EQ(CefCallbackCToCpp::DebugObjCt, 0)
                // when the process is shutting down.

                // TOOD: Consider adding error handling here. But where do we report any errors that may have occurred?
                registrar->AddCustomScheme(StringUtils::ToNative(cefCustomScheme->SchemeName), cefCustomScheme->IsStandard, cefCustomScheme->IsLocal, cefCustomScheme->IsDisplayIsolated);
            }
        };

        IMPLEMENT_REFCOUNTING(CefSharpApp)
    };
}
