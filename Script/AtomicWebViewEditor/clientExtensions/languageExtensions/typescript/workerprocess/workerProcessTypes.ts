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

export interface WorkerProcessMessage<T> extends MessageEvent {
    data: T;
}

export interface WorkerProcessMessageData {
    command: string;
}

export interface SaveMessageData extends WorkerProcessMessageData, Editor.EditorEvents.CodeSavedEvent { }

export interface DeleteMessageData extends WorkerProcessMessageData, Editor.EditorEvents.DeleteResourceEvent { }
export interface RenameMessageData extends WorkerProcessMessageData, Editor.EditorEvents.RenameResourceEvent { }

export const SetPreferences = "SET_PREFERENCES";
export interface SetPreferencesMessageData extends WorkerProcessMessageData {
    preferences: {
        compileOnSave?: boolean;
    };
}

export const GetAnnotations = "ANNOTATIONS";
export const AnnotationsUpdated = "ANNOTATIONS_RESPONSE";
export interface GetAnnotationsMessageData extends SaveMessageData { };
export interface GetAnnotationsResponseMessageData extends WorkerProcessMessageData {
    annotations: any[];
}

export const DoFullCompile = "DO_FULL_COMPILE";
export interface FullCompileMessageData extends WorkerProcessMessageData {
    tsConfig: any;
};

export const DisplayFullCompileResults = "DISPLAY_FULL_COMPILE_RESULTS";
export interface FullCompileResultsMessageData extends GetAnnotationsResponseMessageData {
    compilerOptions: any;
    duration: number;
}

export const SaveFile = "SAVE_FILE";

export const Connect = "HELO";
export const Disconnect = "CLOSE";
export const Message = "MESSAGE";
export const Alert = "ALERT";


/* New Monaco messages */
export interface MonacoWordCompletion extends monaco.languages.CompletionItem {
    uri: string;
    positionOffset: number;
    /**
     * Completion kind from the TypescriptLanguage Service
     * @type {string}
     */
    completionKind: string;
}

export const MonacoProvideCompletionItems = "MONACO_PROVIDE_COMPLETION_ITEMS";
export const MonacoProvideCompletionItemsResponse = "MONACO_PROVIDE_COMPLETION_ITEMS_RESPONS";
export interface MonacoProvideCompletionItemsMessageData extends WorkerProcessMessageData {
    uri: string;
    source: string;
    positionOffset: number;
}

export interface MonacoProvideCompletionItemsResponseMessageData extends WorkerProcessMessageData {
    completions: MonacoWordCompletion[];
}

export const MonacoResolveCompletionItem = "MONACO_RESOLVE_COMPLETION_ITEM";
export const MonacoResolveCompletionItemResponse = "MONACO_RESOLVE_COMPLETION_ITEM_RESPONSE";
export interface MonacoResolveCompletionItemMessageData extends WorkerProcessMessageData {
    item: MonacoWordCompletion;
}

export interface MonacoResolveCompletionItemResponseMessageData extends WorkerProcessMessageData, monaco.languages.CompletionItem { }

export const MonacoGetQuickInfo = "QUICK_INFO";
export const MonacoGetQuickInfoResponse = "QUICK_INFO_RESPONSE";
export interface MonacoGetQuickInfoMessageData extends WorkerProcessMessageData {
    source: string;
    uri: string;
    positionOffset: number;
}

export interface MonacoGetQuickInfoResponseMessageData extends WorkerProcessMessageData {
    contents?: string;
    textSpan?: {
        start: number,
        length: number
    };
}

export const MonacoGetSignature = "SIGNATURE";
export const MonacoGetSignatureResponse = "SIGNATURE_RESPONSE";
export interface MonacoGetSignatureMessageData extends WorkerProcessMessageData {
    source: string;
    uri: string;
    positionOffset: number;
}

export interface MonacoGetSignatureMessageDataResponse extends WorkerProcessMessageData {
    selectedItemIndex?: number;
    argumentIndex?: number;
    signatures?;
}
