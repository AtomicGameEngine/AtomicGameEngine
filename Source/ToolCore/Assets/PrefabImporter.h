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

#pragma once

#include "AssetImporter.h"

namespace Atomic
{
class Scene;
}

namespace ToolCore
{

class PrefabImporter : public AssetImporter
{
    ATOMIC_OBJECT(PrefabImporter, AssetImporter);

public:
    /// Construct.
    PrefabImporter(Context* context, Asset* asset);
    virtual ~PrefabImporter();

    virtual void SetDefaults();

    virtual bool Preload();

    /// Instantiate a node from the asset
    Node* InstantiateNode(Node* parent, const String& name);

protected:

    bool Import();

    virtual bool LoadSettingsInternal(JSONValue& jsonRoot);
    virtual bool SaveSettingsInternal(JSONValue& jsonRoot);

    /// Handle notifying any objects that might need to update after the prefab file changes, such as ResourceCache or any scene components
    virtual void OnPrefabFileChanged();

private:

    void HandlePrefabSave(StringHash eventType, VariantMap& eventData);

   SharedPtr<Atomic::Scene> preloadResourceScene_;

};

}
