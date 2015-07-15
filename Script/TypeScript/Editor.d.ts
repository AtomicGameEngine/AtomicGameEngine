//Atomic TypeScript Definitions


/// <reference path="Atomic.d.ts" />

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


   export class FileUtils extends Atomic.AObject {

      // Construct.
      constructor();

      createDirs(folder: string): boolean;
      openProjectFileDialog(): void;
      newProjectFileDialog(): string;
      revealInFinder(fullpath: string): void;

   }

   export class JSResourceEditor extends ResourceEditor {

      constructor(fullpath: string, container: Atomic.UITabContainer);

      findText(findText: string, flags: number): boolean;
      findTextClose(): void;
      gotoTokenPos(tokenPos: number): void;
      gotoLineNumber(lineNumber: number): void;
      setFocus(): void;
      hasUnsavedModifications(): boolean;

   }

   export class ResourceEditor extends Atomic.AObject {

      button: Atomic.UIButton;
      fullPath: string;
      rootContentWidget: Atomic.UIWidget;

      constructor(fullpath: string, container: Atomic.UITabContainer);

      getButton(): Atomic.UIButton;
      hasUnsavedModifications(): boolean;
      setFocus(): void;
      close(navigateToAvailableResource?: boolean): void;
      findText(text: string, flags: number): boolean;
      findTextClose(): void;
      requiresInspector(): boolean;
      getFullPath(): string;
      save(): boolean;
      getRootContentWidget(): Atomic.UIWidget;

   }

   export class SceneEditor3D extends ResourceEditor {

      scene: Atomic.Scene;

      constructor(fullpath: string, container: Atomic.UITabContainer);

      selectNode(node: Atomic.Node): void;
      getScene(): Atomic.Scene;
      setFocus(): void;
      requiresInspector(): boolean;
      save(): boolean;

   }

   export class SceneView3D extends Atomic.UISceneView {

      pitch: number;
      yaw: number;

      constructor(sceneEditor: SceneEditor3D);

      selectNode(node: Atomic.Node): void;
      setPitch(pitch: number): void;
      setYaw(yaw: number): void;
      enable(): void;
      disable(): void;
      isEnabled(): boolean;

   }



}
