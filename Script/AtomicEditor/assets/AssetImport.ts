
class AssetImport extends Atomic.ScriptObject {

  constructor() {

    super();

    this.subscribeToEvent("DragAndDrop", (data) => this.handleDropFile(data));

  }

  handleDropFile(data):void {

    Atomic.print("Dropped: ", data.fileName);

  }


}

export = AssetImport;
