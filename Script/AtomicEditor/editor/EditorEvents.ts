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

export const PlayerLog = "EditorPlayerLog";
export function PlayerLogEvent(callback: Atomic.EventCallback<PlayerLogEvent>) {
    return Atomic.ScriptEvent(PlayerLog, callback);
}
export interface PlayerLogEvent extends Atomic.EventMetaData {

  message: string;
  level: number;

}

export const ProjectUnloadedNotification = "ProjectUnloadedNotification";
export function ProjectUnloadedNotificationEvent(callback: Atomic.EventCallback<null>) {
    return Atomic.ScriptEvent(ProjectUnloadedNotification, callback);
}

export const RequestProjectLoad = "RequestProjectLoad";
export function RequestProjectLoadEvent(callback: Atomic.EventCallback<RequestProjectLoadEvent>) {
    return Atomic.ScriptEvent(RequestProjectLoad, callback);
}
export interface RequestProjectLoadEvent extends Atomic.EventMetaData {

  // The full path to the .atomic file
  path: string;

}

export const ProjectLoaded = "ProjectLoaded";
export function ProjectLoadedEvent(callback: Atomic.EventCallback<null>) {
    return Atomic.ScriptEvent(ProjectLoaded, callback);
}

export const LoadProjectNotification = "EditorLoadProjectNotification";
export function LoadProjectNotificationEvent(callback: Atomic.EventCallback<LoadProjectEvent>) {
    return Atomic.ScriptEvent(LoadProjectNotification, callback);
}
export interface LoadProjectEvent extends Atomic.EventMetaData {

  // The full path to the .atomic file
  path: string;

}
