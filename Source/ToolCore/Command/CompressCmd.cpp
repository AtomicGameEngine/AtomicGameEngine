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
        this->SetRequiresProjectLoad(false);

        return true;
    }



    void CompressCmd::Run()
    {
        db_ = GetSubsystem<AssetDatabase>();
        cache_ = GetSubsystem<ResourceCache>();
        cache_->AddResourceDir(compressDirectory_);

        cachePath_ = db_->GetCachePath();

        fileSystem_ = GetSubsystem<FileSystem>();

        Vector<String> pngFiles;
        fileSystem_->ScanDir(pngFiles, compressDirectory_, ".png", SCAN_FILES, true);
        Vector<String> jpgFiles;
        fileSystem_->ScanDir(jpgFiles, compressDirectory_, ".jpg", SCAN_FILES, true);
        Vector<String> tgaFiles;
        fileSystem_->ScanDir(tgaFiles, compressDirectory_, ".tga", SCAN_FILES, true);

        for (int i = 0; i < pngFiles.Size(); i++)
        {
            CompressFile(cache_->GetFile(pngFiles[i], false));
        }
        for (int i = 0; i < jpgFiles.Size(); i++)
        {
            CompressFile(cache_->GetFile(jpgFiles[i], false));
        }
        for (int i = 0; i < tgaFiles.Size(); i++)
        {
            CompressFile(cache_->GetFile(tgaFiles[i], false));
        }

        ATOMIC_LOGDEBUG("Texture Compression Complete:" + compressDirectory_);
    }

    void CompressCmd::CompressFile(SharedPtr<File> file)
    {
        asset_ = new Asset(context_);
        asset_->SetPath(file->GetName());

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

        image->SavePNG(compressDirectory_ +"/Cache/" + asset_->GetGUID() + "_thumbnail.png");
    }
}
