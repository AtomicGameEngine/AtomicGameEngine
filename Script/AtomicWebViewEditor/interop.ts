// This is the interop file, exposing functions that can be called by the host game engine
import editor from "./editor/editor";
import * as editorConfig from "./editor/editorConfig";

export function saveCode() {

  const data = {
    message: "saveCode",
    payload: editor.session.getValue()
  };

  window.atomicQuery({
    request: JSON.stringify(data),
    persistent: false,
    onSuccess: function(response) {/**/ },
    onFailure: function(error_code, error_message) {
      console.log("Error getting code");
    }
  });
}

export function codeLoaded(value: string, fileExt: string) {
  editor.session.setValue(value);
  editor.gotoLine(0);

  editor.getSession().on("change", function(e) {
    window.atomicQuery({
      request: "change",
      persistent: false,
      onSuccess: (response) => {/**/ },
      onFailure: (error_code, error_message) => {
        console.log("Error on change");
      }
    });
  });
}

export function loadCode(codeUrl: string) {
  const fileExt = codeUrl.split(".").pop();

  // go ahead and set the theme prior to pulling the file across
  editorConfig.configure(fileExt);

  const p = new Promise(function(resolve, reject) {
    const xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = () => {
      if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
        resolve(xmlHttp.responseText);
      }
    };
    xmlHttp.open("GET", codeUrl, true); // true for asynchronous
    xmlHttp.send(null);
  }).then(function(src: string) {
    codeLoaded(src, fileExt);
  });
}

// Set up the window object so the host can call into it
window.loadCode = loadCode;
window.saveCode = saveCode;
