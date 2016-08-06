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

#include <Atomic/Container/HashMap.h>
#include <Atomic/Container/List.h>

#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Graphics/Technique.h>
#include <Atomic/Graphics/Texture2D.h>
#include <Atomic/Graphics/Material.h>
#include <Atomic/Graphics/Model.h>
#include <Atomic/Scene/Scene.h>

using namespace Atomic;

namespace ToolCore
{

class JSONSceneImporter;
class JSONNode;
class JSONTransform;
class JSONMeshRenderer;
class JSONSkinnedMeshRenderer;
class JSONAnimation;
class JSONTerrain;
class JSONBoxCollider;
class JSONMeshCollider;
class JSONRigidBody;
class JSONLight;
class JSONTimeOfDay;

class JSONSceneProcess : public Object
{

    ATOMIC_OBJECT(JSONSceneProcess, Object);

public:

    JSONSceneProcess(Context* context, JSONSceneImporter* importer) : Object(context)
    {
        importer_ = importer;
    }

    bool Process(const String& resourcePath);
    bool Write();

private:

    String resourcePath_;

    HashMap<String, SharedPtr<Material> > materials_;
    HashMap<String, SharedPtr<Model> > models_;

    SharedPtr<Scene> scene_ ;
    List<SharedPtr<Node> > hierarchy_;

    bool WriteMaterials();
    bool WriteModels();
    bool WriteHierarchy(Scene *scene);

    bool ProcessTextures();
    bool ProcessLightmaps();
    bool ProcessMaterials();
    bool ProcessModels();

    bool ProcessComponent(Node* node, const JSONTransform* jtransform );
    bool ProcessComponent(Node* node, const JSONMeshRenderer* jmeshrenderer );
    bool ProcessComponent(Node* node, const JSONSkinnedMeshRenderer* jmeshrenderer );
    bool ProcessComponent(Node* node, const JSONAnimation* janim );
    bool ProcessComponent(Node* node, const JSONTerrain* jterrain );
    bool ProcessComponent(Node* node, const JSONBoxCollider* jbox );
    bool ProcessComponent(Node* node, const JSONMeshCollider* jmesh );
    bool ProcessComponent(Node* node, const JSONLight* jlight );
    bool ProcessComponent(Node* node, const JSONRigidBody* jbody );
    bool ProcessComponent(Node* node, const JSONTimeOfDay* jtime );
    Node* ProcessNode(const JSONNode* jnode, Node *parent);

    bool ProcessHierarchy(Scene *scene);

    SharedPtr<JSONSceneImporter> importer_;

};

}
