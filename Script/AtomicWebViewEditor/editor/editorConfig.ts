import editor from "./editor";

/**
 * Set the editor theme and configuration based upon the file extension
 * @param  {string} fileExt
 */
export function configure(fileExt: string) {
    editor.setTheme("ace/theme/monokai");
    switch (fileExt.toLowerCase()) {
        case "ts":
            editor.session.setMode("ace/mode/typescript");
            break;
        case "js":
            editor.session.setMode("ace/mode/javascript");
            break;
        default:
            editor.session.setMode("ace/mode/javascript");
            break;
    }
}
