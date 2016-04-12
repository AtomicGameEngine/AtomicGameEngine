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
export interface DeleteMessageData extends WorkerProcessMessageData, Editor.EditorEvents.DeleteResourceEvent {}
export interface RenameMessageData extends WorkerProcessMessageData, Editor.EditorEvents.RenameResourceEvent {}

export const GetCompletions = "COMPLETIONS";
export const CompletionResponse = "COMPLETION_RESPONSE";
export interface WordCompletion {
    caption: string;
    meta: string;
    score: number;
    value: string;
    pos: number;
    snippet?: string;
    docHTML?: string;
    docText?: string;
}

export interface GetCompletionsMessageData extends WorkerProcessMessageData {
    sourceText: string;
    filename: string;
    pos: { row: number, column: number };
    prefix: string;
}

export interface GetCompletionsResponseMessageData extends WorkerProcessMessageData {
    completions: Array<WordCompletion>;
}

export const GetDocTooltip = "DOC_TOOLTIP";
export const DocTooltipResponse = "DOC_TOOLTIP_RESPONSE";
export interface GetDocTooltipMessageData extends WorkerProcessMessageData {
    completionItem: WordCompletion;
    filename: string;
    pos: number;
}

export interface GetDocTooltipResponseMessageData extends WorkerProcessMessageData {
    docText?: string;
    docHTML?: string;
}

export const GetAnnotations = "ANNOTATIONS";
export const AnnotationsUpdated = "ANNOTATIONS_RESPONSE";
export interface GetAnnotationsMessageData extends SaveMessageData {};
export interface GetAnnotationsResponseMessageData extends WorkerProcessMessageData {
    annotations: any[];
}

export const Connect = "HELO";
export const Disconnect = "CLOSE";
export const Message = "MESSAGE";
export const Alert = "ALERT";
