
#pragma once

#include <Atomic/Core/Context.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Engine/Engine.h>

using namespace Atomic;

class JSBind
{
public:

    static void Initialize();

    static SharedPtr<Context> context_;
    static SharedPtr<FileSystem> fileSystem_;
    static SharedPtr<Engine> engine_;

    static String ROOT_FOLDER;
};

