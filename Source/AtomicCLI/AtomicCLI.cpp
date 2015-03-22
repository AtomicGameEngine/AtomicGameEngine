
#include <Atomic/Core/ProcessUtils.h>

using namespace Atomic;

void Run(const Vector<String>& arguments)
{
    if (arguments.Size() < 2)
    {

    }

}

int main(int argc, char** argv)
{
    Vector<String> arguments;
    
#ifdef ATOMIC_PLATFORM_WINDOWS
    arguments = ParseArguments(GetCommandLineW());
#else
    arguments = ParseArguments(argc, argv);
#endif
    
    Run(arguments);
    return 0;
}
