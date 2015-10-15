
#include "NETHost.h"

namespace Atomic
{

String NETHost::coreCLRFilesAbsPath_;
String NETHost::coreCLRAssemblyLoadPaths_;
String NETHost::coreCLRTPAPaths_;

NETHost::NETHost(Context* context) :
    Object(context)
{

}

NETHost::~NETHost()
{

}

}
