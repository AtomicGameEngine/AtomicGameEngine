// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Container/HashMap.h>
#include <Atomic/Container/List.h>


#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Graphics/Technique.h>
#include <Atomic/Graphics/Texture2D.h>
#include <Atomic/Graphics/Material.h>
#include <Atomic/Atomic3D/Model.h>
#include <Atomic/Scene/Scene.h>

using namespace Atomic;

namespace AtomicEditor
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

    OBJECT(JSONSceneProcess);

public:

    JSONSceneProcess(Context* context, JSONSceneImporter* importer) : Object(context)
    {
        importer_ = importer;
    }

    bool Process();
    bool Write();

private:

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
