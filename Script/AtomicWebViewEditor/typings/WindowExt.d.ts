//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

/**
 * Defines the interface to what is available for the host to call or for the client to call on the window object
 */
interface Window {
    atomicQuery: any;
    HOST_loadCode: (codeUrl) => void;
    HOST_saveCode: () => void;

    HOST_projectUnloaded: () => void;
    HOST_resourceRenamed: (path: string, newPath: string) => void;
    HOST_resourceDeleted: (path: string) => void;
}
