//Atomic TypeScript Definitions


declare module Editor {


   // enum EditMode
   export type EditMode = number;
   export var EDIT_SELECT: EditMode;
   export var EDIT_MOVE: EditMode;
   export var EDIT_ROTATE: EditMode;
   export var EDIT_SCALE: EditMode;


   // enum AxisMode
   export type AxisMode = number;
   export var AXIS_WORLD: AxisMode;
   export var AXIS_LOCAL: AxisMode;


//----------------------------------------------------
// MODULE: Editor
//----------------------------------------------------


   export class MyJSClass extends Atomic.AObject {

      aha: string;

      // Construct.
      constructor();

      getAha(): string;

   }

   export class JSResourceEditor extends ResourceEditor {

      constructor(fullpath: string, container: UITabContainer);

      findText(findText: string, flags: number): boolean;
      findTextClose(): void;
      gotoTokenPos(tokenPos: number): void;
      gotoLineNumber(lineNumber: number): void;
      setFocus(): void;
      hasUnsavedModifications(): boolean;

   }

   export class ResourceEditor extends Atomic.AObject {

      button: UIButton;
      fullPath: string;
      rootContentWidget: UIWidget;

      constructor(fullpath: string, container: UITabContainer);

      getButton(): UIButton;
      hasUnsavedModifications(): boolean;
      setFocus(): void;
      close(navigateToAvailableResource?: boolean): void;
      findText(text: string, flags: number): boolean;
      findTextClose(): void;
      requiresInspector(): boolean;
      getFullPath(): string;
      save(): boolean;
      getRootContentWidget(): UIWidget;

   }

   export class SceneEditor3D extends ResourceEditor {

      scene: Scene;

      constructor(fullpath: string, container: UITabContainer);

      selectNode(node: Node): void;
      getScene(): Scene;
      setFocus(): void;
      requiresInspector(): boolean;
      save(): boolean;

   }

   export class SceneView3D extends Atomic.UISceneView {

      pitch: number;
      yaw: number;

      constructor(sceneEditor: SceneEditor3D);

      selectNode(node: Node): void;
      setPitch(pitch: number): void;
      setYaw(yaw: number): void;
      enable(): void;
      disable(): void;
      isEnabled(): boolean;

   }

   export class InspectorFrame extends Atomic.UIWidget {

      // Construct.
      constructor();


   }



}
