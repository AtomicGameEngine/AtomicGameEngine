//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorEvents = require("editor/EditorEvents");
import ScriptWidget = require("ui/ScriptWidget");

// inspectors

import MaterialInspector = require("./MaterialInspector");
import ModelInspector = require("./ModelInspector");
import AssemblyInspector = require("./AssemblyInspector");
import PrefabInspector = require("./PrefabInspector");

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
        this.subscribeToEvent("ProjectUnloaded", (data) => this.handleProjectUnloaded(data));

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

        if (asset.importerTypeName == "NETAssemblyImporter") {

            var assemblyInspector = new AssemblyInspector();
            container.addChild(assemblyInspector);

            assemblyInspector.inspect(asset);

        }

        if (asset.importerTypeName == "PrefabImporter") {

            var prefabInspector = new PrefabInspector();
            container.addChild(prefabInspector);

            prefabInspector.inspect(asset);
        }

    }

}

export = InspectorFrame;
