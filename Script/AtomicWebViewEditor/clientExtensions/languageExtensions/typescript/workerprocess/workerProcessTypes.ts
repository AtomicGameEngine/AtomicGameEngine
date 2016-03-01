//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

export interface WorkerProcessMessage<T> extends MessageEvent {
    data: T;
}

export interface WorkerProcessMessageData {
    command: string;
}

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

export const Connect = "HELO";
export const Disconnect = "CLOSE";
export const Message = "MESSAGE";
export const Alert = "ALERT";

export interface SaveMessageData extends WorkerProcessMessageData, Editor.EditorEvents.SaveResourceEvent { }
export interface DeleteMessageData extends WorkerProcessMessageData, Editor.EditorEvents.DeleteResourceEvent {}
export interface RenameMessageData extends WorkerProcessMessageData, Editor.EditorEvents.RenameResourceEvent {}
