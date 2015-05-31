
export enum StringID {

    RevealInFinder,
    ShortcutUndo,
    ShortcutRedo,
    ShortcutCut,
    ShortcutCopy,
    ShortcutPaste,
    ShortcutSelectAll,
    ShortcutFind,
    ShortcutFindNext,
    ShortcutFindPrev,
    ShortcutBeautify,
    ShortcutCloseFile,
    ShortcutSaveFile,
    ShortcutPlay,
    ShortcutBuild,
    ShortcutBuildSettings
}

export class EditorString {

    public static GetString(id: StringID): string {

        return EditorString.lookup[id];

    }

    private static lookup: { [id: number]: string; } = {};

    private static Ctor = (() => {

        var lookup = EditorString.lookup;

        var shortcutKey = "⌘";

        lookup[StringID.RevealInFinder] = "Reveal in Finder";

        // Mac
        lookup[StringID.ShortcutRedo] = "⇧⌘Z";
        lookup[StringID.ShortcutFindNext] = "⌘G";
        lookup[StringID.ShortcutFindPrev] = "⇧⌘G";
        lookup[StringID.ShortcutBuildSettings] = "⇧⌘B";

        // General
        lookup[StringID.ShortcutUndo] = shortcutKey + "Z";

        lookup[StringID.ShortcutCut] = shortcutKey + "X";
        lookup[StringID.ShortcutCopy] = shortcutKey + "C";
        lookup[StringID.ShortcutPaste] = shortcutKey + "V";
        lookup[StringID.ShortcutSelectAll] = shortcutKey + "A";
        lookup[StringID.ShortcutFind] = shortcutKey + "F";

        lookup[StringID.ShortcutBeautify] = shortcutKey + "I";

        lookup[StringID.ShortcutSaveFile] = shortcutKey + "S";
        lookup[StringID.ShortcutCloseFile] = shortcutKey + "W";

        lookup[StringID.ShortcutPlay] = shortcutKey + "P";

        lookup[StringID.ShortcutBuild] = shortcutKey + "B";

    })();

}
