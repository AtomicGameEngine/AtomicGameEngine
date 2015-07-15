
export const Quit = "EditorEventQuit";

export const EditResource = "EditorEditResource";

// data
export interface EditResourceEvent {

  // The full path to the resource to edit
  path: string;

}
