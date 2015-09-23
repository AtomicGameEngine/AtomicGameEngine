
#include "AtomicSharp.h"


namespace Atomic
{

WeakPtr<Context> AtomicSharp::csContext_;
WeakPtr<AtomicSharp> AtomicSharp::instance_;

AtomicSharp::AtomicSharp(Context* context) :
    Object(context)
{
    assert(!instance_);
    instance_ = this;
    csContext_ = context;
}

AtomicSharp::~AtomicSharp()
{
    instance_ = NULL;
}


}

