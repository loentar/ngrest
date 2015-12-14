#include <stdlib.h>
#include <string.h>
//#include "StackTracer.h"
#include "Exception.h"

namespace ngrest
{

Exception::Exception(const char* fileLine, const char* function, const char* description_):
    description(fileLine)
{
    description.append(function).append(": ").append(description_);
}

Exception::Exception(const char* fileLine, const char* function, const std::string& description_):
    description(fileLine)
{
    description.append(function).append(": ").append(description_);

    //    static short nEnableStackTracing = -1;
    //    if (nEnableStackTracing == -1)
    //    {
    //      char* szStackTracingEnv = NULL;
    //#if defined WIN32 && !defined __MINGW32__
    //      _dupenv_s(&szStackTracingEnv, NULL, "NGREST_EXCEPTION_STACKTRACING");
    //#else
    //      szStackTracingEnv = getenv("NGREST_EXCEPTION_STACKTRACING");
    //#endif
    //      nEnableStackTracing =
    //          (szStackTracingEnv && ((!strcmp(szStackTracingEnv, "1") || !strcmp(szStackTracingEnv, "TRUE"))))
    //          ? 1 : 0;

    //#ifdef WIN32
    //      if (szStackTracingEnv)
    //      {
    //        free(szStackTracingEnv);
    //      }
    //#endif
    //    }

    //    if (nEnableStackTracing == 1)
    //    {
    //      std::string sStack;
    //      StackTracer::GetStackTraceStr(sStack);
    //      m_sDescr.append("\nTraced stack:\n").append(sStack);
    //    }

}

}

