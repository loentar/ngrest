//#include <stdlib.h>
//#include <string.h>
//#include "StackTracer.h"
#include "Exception.h"

namespace ngrest {

Exception::Exception(const char* fileLine_, const char* function_, const std::string& description_):
    fileLine(fileLine_),
    function(function_),
    description(description_)
{
    //    static short enableStackTracing = -1;
    //    if (enableStackTracing == -1)
    //    {
    //      char* szStackTracingEnv = nullptr;
    //#if defined WIN32 && !defined __MINGW32__
    //      _dupenv_s(&szStackTracingEnv, nullptr, "NGREST_EXCEPTION_STACKTRACING");
    //#else
    //      szStackTracingEnv = getenv("NGREST_EXCEPTION_STACKTRACING");
    //#endif
    //      enableStackTracing =
    //          (szStackTracingEnv && ((!strcmp(szStackTracingEnv, "1") || !strcmp(szStackTracingEnv, "TRUE"))))
    //          ? 1 : 0;

    //#ifdef WIN32
    //      if (szStackTracingEnv)
    //      {
    //        free(szStackTracingEnv);
    //      }
    //#endif
    //    }

    //    if (enableStackTracing == 1)
    //    {
    //      std::string stack;
    //      StackTracer::GetStackTraceStr(stack);
    //      descr.append("\nTraced stack:\n").append(stack);
    //    }
}

}

