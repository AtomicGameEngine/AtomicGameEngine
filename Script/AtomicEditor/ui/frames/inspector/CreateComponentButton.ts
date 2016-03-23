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

var audioCreateSource = new Atomic.UIMenuItemSource();

audioCreateSource.addItem(new Atomic.UIMenuItem("SoundListener", "SoundListener"));
audioCreateSource.addItem(new Atomic.UIMenuItem("SoundSource", "SoundSource"));
audioCreateSource.addItem(new Atomic.UIMenuItem("SoundSource3D", "SoundSource3D"));

var _2DCreateSource = new Atomic.UIMenuItemSource();
_2DCreateSource.addItem(new Atomic.UIMenuItem("PhysicsWorld2D", "PhysicsWorld2D"));
_2DCreateSource.addItem(new Atomic.UIMenuItem("StaticSprite2D", "StaticSprite2D"));
_2DCreateSource.addItem(new Atomic.UIMenuItem("AnimatedSprite2D", "AnimatedSprite2D"));
_2DCreateSource.addItem(new Atomic.UIMenuItem("ParticleEmitter2D", "ParticleEmitter2D"));
_2DCreateSource.addItem(new Atomic.UIMenuItem("PointLight2D", "PointLight2D"));
_2DCreateSource.addItem(new Atomic.UIMenuItem("DirectionalLight2D", "DirectionalLight2D"));
_2DCreateSource.addItem(new Atomic.UIMenuItem("RigidBody2D", "RigidBody2D"));
_2DCreateSource.addItem(new Atomic.UIMenuItem("CollisionBox2D", "CollisionBox2D"));
_2DCreateSource.addItem(new Atomic.UIMenuItem("CollisionCircle2D", "CollisionCircle2D"));
_2DCreateSource.addItem(new Atomic.UIMenuItem("TileMap2D", "TileMap2D"));

var geometryCreateSource = new Atomic.UIMenuItemSource();

geometryCreateSource.addItem(new Atomic.UIMenuItem("StaticModel", "StaticModel"));
geometryCreateSource.addItem(new Atomic.UIMenuItem("AnimatedModel", "AnimatedModel"));
geometryCreateSource.addItem(new Atomic.UIMenuItem("AnimationController", "AnimationController"));
geometryCreateSource.addItem(new Atomic.UIMenuItem("BillboardSet", "BillboardSet"));
geometryCreateSource.addItem(new Atomic.UIMenuItem("CustomGeometry", "CustomGeometry"));
geometryCreateSource.addItem(new Atomic.UIMenuItem("ParticleEmitter", "ParticleEmitter"));
geometryCreateSource.addItem(new Atomic.UIMenuItem("Skybox", "SkyBox"));
geometryCreateSource.addItem(new Atomic.UIMenuItem("StaticModelGroup", "StaticModelGroup"));
geometryCreateSource.addItem(new Atomic.UIMenuItem("Terrain", "Terrain"));
geometryCreateSource.addItem(new Atomic.UIMenuItem("Text3D", "create component"));
geometryCreateSource.addItem(new Atomic.UIMenuItem("Water", "Water"));

var logicCreateSource = new Atomic.UIMenuItemSource();

logicCreateSource.addItem(new Atomic.UIMenuItem("JSComponent", "JSComponent"));
logicCreateSource.addItem(new Atomic.UIMenuItem("CSComponent", "CSComponent"));
logicCreateSource.addItem(new Atomic.UIMenuItem("AnimationController", "AnimationController"));
logicCreateSource.addItem(new Atomic.UIMenuItem("SplinePath", "SplinePath"));

var navigationCreateSource = new Atomic.UIMenuItemSource();

navigationCreateSource.addItem(new Atomic.UIMenuItem("CrowdAgent", "CrowdAgent"));
navigationCreateSource.addItem(new Atomic.UIMenuItem("CrowdManager", "CrowdManager"));
navigationCreateSource.addItem(new Atomic.UIMenuItem("NavArea", "NavArea"));
navigationCreateSource.addItem(new Atomic.UIMenuItem("Navigable", "Navigable"));
navigationCreateSource.addItem(new Atomic.UIMenuItem("NavigationMesh", "NavigationMesh"));
navigationCreateSource.addItem(new Atomic.UIMenuItem("DynamicNavigationMesh", "DynamicNavigationMesh"));
navigationCreateSource.addItem(new Atomic.UIMenuItem("Obstacle", "Obstacle"));
navigationCreateSource.addItem(new Atomic.UIMenuItem("OffMeshConnection", "OffMeshConnection"));

var networkCreateSource = new Atomic.UIMenuItemSource();

networkCreateSource.addItem(new Atomic.UIMenuItem("Network Priority", "create component"));

var physicsCreateSource = new Atomic.UIMenuItemSource();

physicsCreateSource.addItem(new Atomic.UIMenuItem("CollisionShape", "CollisionShape"));
physicsCreateSource.addItem(new Atomic.UIMenuItem("Constraint", "Constraint"));
physicsCreateSource.addItem(new Atomic.UIMenuItem("RigidBody", "RigidBody"));

var sceneCreateSource = new Atomic.UIMenuItemSource();

sceneCreateSource.addItem(new Atomic.UIMenuItem("Camera", "Camera"));
sceneCreateSource.addItem(new Atomic.UIMenuItem("Light", "Light"));
sceneCreateSource.addItem(new Atomic.UIMenuItem("Zone", "Zone"));

var subsystemCreateSource = new Atomic.UIMenuItemSource();

subsystemCreateSource.addItem(new Atomic.UIMenuItem("DebugRenderer", "create component"));
subsystemCreateSource.addItem(new Atomic.UIMenuItem("Octree", "create component"));
subsystemCreateSource.addItem(new Atomic.UIMenuItem("PhysicsWorld", "create component"));

var editorCreateSource = new Atomic.UIMenuItemSource();

editorCreateSource.addItem(new Atomic.UIMenuItem("CubemapGenerator", "CubemapGenerator"));


var componentCreateSource = new Atomic.UIMenuItemSource();

var sources = {
    Audio: audioCreateSource,
    "2D": _2DCreateSource,
    Geometry: geometryCreateSource,
    Logic: logicCreateSource,
    Navigation: navigationCreateSource,
    Network: networkCreateSource,
    Physics: physicsCreateSource,
    Scene: sceneCreateSource,
    SubSystem: subsystemCreateSource,
    Editor : editorCreateSource
};

for (var sub in sources) {

    var item = new Atomic.UIMenuItem(sub);
    item.subSource = sources[sub];
    componentCreateSource.addItem(item);

}


class CreateComponentButton extends Atomic.UIButton {

    constructor() {

        super();

        this.fd.id = "Vera";
        this.fd.size = 11;

        this.text = "Create Component";

        this.subscribeToEvent("WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

    // note instance method
    onClick = () => {

        var menu = new Atomic.UIMenuWindow(this, "create component popup");
        menu.fontDescription = this.fd;
        menu.show(componentCreateSource);
    };

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type != Atomic.UI_EVENT_TYPE_CLICK)
            return;

        if (ev.target && ev.target.id == "create component popup") {

            this.sendEvent("SelectionCreateComponent", { componentTypeName : ev.refid});

            return true;

        }

    }

    fd: Atomic.UIFontDescription = new Atomic.UIFontDescription();

}

export = CreateComponentButton;
