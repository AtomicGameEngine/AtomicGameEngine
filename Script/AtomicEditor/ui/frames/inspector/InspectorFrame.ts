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

import EditorEvents = require("editor/EditorEvents");
import ScriptWidget = require("ui/ScriptWidget");

// inspectors

import MaterialInspector = require("./MaterialInspector");
import ModelInspector = require("./ModelInspector");
import PrefabInspector = require("./PrefabInspector");
import TextureInspector = require("./TextureInspector");
import AssemblyInspector = require("./AssemblyInspector");

import SelectionInspector = require("./SelectionInspector");
// make sure these are hooked in
import "./SelectionEditTypes";
import "./SelectionSectionCoreUI";


class InspectorFrame extends ScriptWidget {

    scene: Atomic.Scene = null;
    sceneEditor: Editor.SceneEditor3D;

    selectionInspector: SelectionInspector;

    constructor() {

        super();

        this.gravity = Atomic.UI_GRAVITY_TOP_BOTTOM;

        this.load("AtomicEditor/editor/ui/inspectorframe.tb.txt");

        var container = this.getWidget("inspectorcontainer");

        this.subscribeToEvent(EditorEvents.EditResource, (data) => this.handleEditResource(data));
        this.subscribeToEvent(EditorEvents.ProjectUnloadedNotification, (data) => this.handleProjectUnloaded(data));

        this.subscribeToEvent(EditorEvents.ActiveSceneEditorChange, (data) => this.handleActiveSceneEditorChanged(data));

    }

    handleActiveSceneEditorChanged(event: EditorEvents.ActiveSceneEditorChangeEvent) {

        if (this.sceneEditor == event.sceneEditor) {
            return;
        }

        if (this.scene)
            this.unsubscribeFromEvents(this.scene);

        this.closeSelectionInspector();

        this.sceneEditor = null;
        this.scene = null;

        if (!event.sceneEditor)
          return;

        this.sceneEditor = event.sceneEditor;
        this.scene = event.sceneEditor.scene;

        if (this.scene) {

            this.subscribeToEvent(this.scene, "SceneNodeSelected", (event: Editor.SceneNodeSelectedEvent) => this.handleSceneNodeSelected(event));

            // add the current selection
            var selection = this.sceneEditor.selection;

            for (var i = 0; i < selection.getSelectedNodeCount(); i++) {

                this.handleSceneNodeSelected( { node: selection.getSelectedNode(i),  scene: this.scene, selected: true, quiet: true} );

            }

            // FIXME: Duktape is leaking selection object without nulling out
            selection = null;
        }

    }

    closeSelectionInspector() {

        if (!this.selectionInspector)
            return;

        var container = this.getWidget("inspectorcontainer");
        container.deleteAllChildren();

        this.selectionInspector = null;

    }

    handleSceneNodeSelected(ev: Editor.SceneNodeSelectedEvent) {

        var selection = this.sceneEditor.selection;

        if (this.selectionInspector) {

            if (ev.selected) {
                this.selectionInspector.addNode(ev.node);
            } else {
                this.selectionInspector.removeNode(ev.node);
            }

        } else if (ev.selected) {

            var container = this.getWidget("inspectorcontainer");
            container.deleteAllChildren();

            var inspector = this.selectionInspector = new SelectionInspector(this.sceneEditor);
            inspector.addNode(ev.node);
            container.addChild(inspector);

        }

        // close last, so state is saved
        if (!selection.selectedNodeCount) {
            this.closeSelectionInspector();
        }

    }


    handleProjectUnloaded(data) {

        this.closeSelectionInspector();
        var container = this.getWidget("inspectorcontainer");
        container.deleteAllChildren();
    }


    handleEditResource(ev: EditorEvents.EditResourceEvent) {

        var path = ev.path;

        var db = ToolCore.getAssetDatabase();
        var asset = db.getAssetByPath(path);

        if (asset) {

            this.inspectAsset(asset);

        }

    }


    inspectAsset(asset: ToolCore.Asset) {

        this.closeSelectionInspector();

        var container = this.getWidget("inspectorcontainer");
        container.deleteAllChildren();

        if (asset.importerTypeName == "ModelImporter") {

            var inspector = new ModelInspector();
            container.addChild(inspector);

            inspector.inspect(asset);

        }

        if (asset.importerTypeName == "MaterialImporter") {

            var cache = Atomic.getResourceCache();

            var material = <Atomic.Material>cache.getResource("Material", asset.path);

            if (!material) {
                return;
            }

            var materialInspector = new MaterialInspector();
            container.addChild(materialInspector);

            materialInspector.inspect(asset, material);
        }

        if (asset.importerTypeName == "PrefabImporter") {

            var prefabInspector = new PrefabInspector();
            container.addChild(prefabInspector);

            prefabInspector.inspect(asset);
        }

        if (asset.importerTypeName == "TextureImporter") {

            var thumbnail = asset.cachePath + "_thumbnail.png";
            var cache = Atomic.getResourceCache();
            var texture = <Atomic.Texture2D>cache.getResource("Texture2D", thumbnail);

            if (!texture) {
                return;
            }

            var textureInspector = new TextureInspector();
            container.addChild(textureInspector);

            textureInspector.inspect(texture, asset);
        }

        if (asset.importerTypeName == "NETAssemblyImporter") {

          var assemblyInspector = new AssemblyInspector();
          container.addChild(assemblyInspector);
          assemblyInspector.inspect(asset);

        }

    }

}

export = InspectorFrame;
