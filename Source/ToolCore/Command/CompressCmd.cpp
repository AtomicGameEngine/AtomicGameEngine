//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Atomic/Resource/Image.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>


#include "CompressCmd.h"

namespace ToolCore
{
    const char* COMPRESS_FILETYPES[] = { ".jpg", ".png", ".tga"};

    CompressCmd::CompressCmd(Context* context) : Command(context)
    {

    }

    CompressCmd::~CompressCmd()
    {

    }

    bool CompressCmd::Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
    {
        String argument = arguments[startIndex].ToLower();
        String value = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1] : String::EMPTY;

        if (argument != "compress")
        {
            errorMsg = "Unable to parse compress command";
            return false;
        }

        if (!value.Length())
        {
            errorMsg = "Unable to parse compress platform";
            return false;
        }

        compressDirectory_ = value.ToLower();

        return true;
    }



    void CompressCmd::Run()
    {
        db_ = GetSubsystem<AssetDatabase>();
        cache_ = GetSubsystem<ResourceCache>();
        cache_->AddResourceDir(compressDirectory_);

        cachePath_ = db_->GetCachePath();

        fileSystem_ = GetSubsystem<FileSystem>();

        for (int i = 0; i < sizeof(COMPRESS_FILETYPES) / sizeof(COMPRESS_FILETYPES[0]); ++i)
        {
            Vector<String> paths;
            fileSystem_->ScanDir(paths, compressDirectory_, COMPRESS_FILETYPES[i], SCAN_FILES, true);
            for (int j = 0; j < paths.Size(); j++)
            {
                CompressFile(cache_->GetFile(paths[j], false));
            }
        }

        ATOMIC_LOGDEBUG("Texture Compression Complete:" + compressDirectory_);

        Finished();
    }

    void CompressCmd::CompressFile(SharedPtr<File> file)
    {
        asset_ = new Asset(context_);
        asset_->SetPath(file->GetName());

        // TODO: Handle other compression formats
        String compressedPath = compressDirectory_ + "/Cache/DDS/" + file->GetName() + ".dds";
        if (fileSystem_->FileExists(compressedPath))
            fileSystem_->Delete(compressedPath);

        SharedPtr<Image> image = cache_->GetTempResource<Image>(asset_->GetPath());

        if (!image->IsCompressed())
        {
            fileSystem_->CreateDirs(compressDirectory_, "/Cache/DDS/" + Atomic::GetPath(file->GetName()));
            image->SaveDDS(compressedPath);
        }

        image->SavePNG(compressDirectory_ + "/Cache/" + asset_->GetGUID());
        image->SavePNG(compressDirectory_ + "/Cache/" + asset_->GetGUID() + "_thumbnail.png");
    }
}
