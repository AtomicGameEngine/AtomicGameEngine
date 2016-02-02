// Instantiate the editor
const editor = ace.edit("editor");

// and set some default options
editor.setOptions({
    enableBasicAutocompletion: true,
    enableSnippets: true,
    enableLiveAutocompletion: true,
    theme: "ace/theme/monokai"
});

export default editor;
