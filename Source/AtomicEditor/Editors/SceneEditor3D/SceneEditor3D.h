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

#include <TurboBadger/tb_widgets_common.h>

#include "../ResourceEditor.h"
#include "SceneView3D.h"
#include "Gizmo3D.h"

using namespace Atomic;
using namespace tb;

namespace Atomic
{
class Scene;
class Node;
class View3D;
class Camera;
class DebugRenderer;
class Octree;

}

namespace ToolCore
{
    class ProjectUserPrefs;
    class SceneImporter;
}

using namespace ToolCore;

namespace AtomicEditor
{

class SceneSelection;
class SceneEditHistory;

class SceneEditor3D: public ResourceEditor
{
    ATOMIC_OBJECT(SceneEditor3D, ResourceEditor);

public:

    SceneEditor3D(Context* context, const String& fullpath, UITabContainer* container);

    virtual ~SceneEditor3D();

    bool OnEvent(const TBWidgetEvent &ev);

    SceneSelection* GetSelection() { return selection_; }
    SceneEditHistory* GetEditHistory() { return editHistory_; }
    SceneView3D* GetSceneView3D() { return sceneView_; }

    void RegisterNode(Node* node);
    void RegisterNodes(const PODVector<Node*>& nodes);

    void ReparentNode(Node* node, Node* newParent);

    Scene* GetScene() { return scene_; }
    Gizmo3D* GetGizmo() { return gizmo3D_; }

    void SetFocus();

    virtual bool RequiresInspector() { return true; }

    void Close(bool navigateToAvailableResource = true);
    bool Save();

    void Undo();
    void Redo();
    void Cut();
    void Copy();
    void Paste();

    ProjectUserPrefs* GetUserPrefs();

    void InvokeShortcut(const String& shortcut);

    static SceneEditor3D* GetSceneEditor(Scene* scene);

private:

    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandlePlayStarted(StringHash eventType, VariantMap& eventData);
    void HandlePlayStopped(StringHash eventType, VariantMap& eventData);
    void HandleGizmoEditModeChanged(StringHash eventType, VariantMap& eventData);
    void HandleGizmoAxisModeChanged(StringHash eventType, VariantMap& eventData);

    void HandleUserPrefSaved(StringHash eventType, VariantMap& eventData);

    void HandleSceneEditSceneModified(StringHash eventType, VariantMap& eventData);
    void HandleSceneEditNodeCreated(StringHash eventType, VariantMap& eventData);

    void HandleCubemapRenderBegin(StringHash eventType, VariantMap& eventData);
    void HandleCubemapRenderEnd(StringHash eventType, VariantMap& eventData);

    void UpdateGizmoSnapSettings();

    SharedPtr<Scene> scene_;

    // TODO: multiple views
    SharedPtr<SceneView3D> sceneView_;

    SharedPtr<Gizmo3D> gizmo3D_;

    SharedPtr<SceneSelection> selection_;
    SharedPtr<SceneEditHistory> editHistory_;
    SharedPtr<SceneImporter> sceneImporter_;

    SharedPtr<Node> clipboardNode_;

    WeakPtr<ProjectUserPrefs> userPrefs_;

    void RegisterSceneEditor();

    static Vector<WeakPtr<SceneEditor3D>> sceneEditors_;

    int cubemapRenderCount_;

};

}
