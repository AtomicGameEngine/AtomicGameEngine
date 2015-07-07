
export const MessageModalEvent = "MessageModalEvent";

export const EditResource = "EditResource";

export interface EditorResourceEvent {

  // The full path to the resource to edit
  path: string;

}

export const CloseResourceEditor = "CloseResourceEditor";
export const ResourceEditorChanged = "ResourceEditorChanged";
