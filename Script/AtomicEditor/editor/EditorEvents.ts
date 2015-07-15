
export const Quit = "EditorEventQuit";

export const LoadProject = "EditorLoadProject";
export interface LoadProjectEvent {

  // The full path to the .atomic file
  path: string;

}

export const EditResource = "EditorEditResource";
export interface EditResourceEvent {

  // The full path to the resource to edit
  path: string;

}
