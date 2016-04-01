//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

/**
 * All of the publicly available events that client extensions listen to
 */
export default class ClientExtensionEventNames {
    static CodeLoadedEvent = "CodeLoadedEvent";
    static ConfigureEditorEvent = "ConfigureEditorEvent";
    static ResourceSavedEvent = "ResourceSavedEvent";
    static ResourceRenamedEvent = "ResourceRenamedEvent";
    static ResourceDeletedEvent = "ResourceDeletedEvent";
    static ProjectUnloadedEvent = "ProjectUnloadedEvent";
}
