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
    ShortcutPause,
    ShortcutStep,
    ShortcutPlayDebug,
    ShortcutBuild,
    ShortcutBuildSettings,
    ShortcutFrameSelected
}

export class EditorString {

    public static GetString(id: StringID): string {

        return EditorString.lookup[id];

    }

    private static lookup: { [id: number]: string; } = {};

    private static Ctor = (() => {

        var lookup = EditorString.lookup;

        var shortcutKey;
        if (Atomic.platform == "MacOSX") {
            shortcutKey = "⌘";
        } else { // Windows and Linux
            shortcutKey = "CTRL + ";
        }

        lookup[StringID.RevealInFinder] = "Reveal in Finder";

        // Mac, Windows and Linux
        lookup[StringID.ShortcutRedo] = "⇧" + shortcutKey + "Z";
        lookup[StringID.ShortcutFindNext] = shortcutKey + "G";
        lookup[StringID.ShortcutFindPrev] = "⇧" + shortcutKey + "G";
        lookup[StringID.ShortcutBuildSettings] = "⇧" + shortcutKey + "B";

        // General
        lookup[StringID.ShortcutUndo] = shortcutKey + "Z";

        lookup[StringID.ShortcutCut] = shortcutKey + "X";
        lookup[StringID.ShortcutCopy] = shortcutKey + "C";
        lookup[StringID.ShortcutPaste] = shortcutKey + "V";
        lookup[StringID.ShortcutSelectAll] = shortcutKey + "A";

        lookup[StringID.ShortcutFind] = shortcutKey + "F";

        lookup[StringID.ShortcutFrameSelected] = "F";

        lookup[StringID.ShortcutBeautify] = shortcutKey + "I";

        lookup[StringID.ShortcutSaveFile] = shortcutKey + "S";
        lookup[StringID.ShortcutCloseFile] = shortcutKey + "W";

        lookup[StringID.ShortcutPlay] = shortcutKey + "P";
        lookup[StringID.ShortcutPause] = shortcutKey + "U";
        lookup[StringID.ShortcutStep] = "⇧" + shortcutKey + "U";
        lookup[StringID.ShortcutPlayDebug] = "⇧" + shortcutKey + "P";

        lookup[StringID.ShortcutBuild] = shortcutKey + "B";

    })();

}
