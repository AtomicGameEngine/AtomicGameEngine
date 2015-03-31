

// ln -sf /Users/josh/Dev/atomic/AtomicGameEngine/Artifacts/MacOSX_Package/AtomicEditor.app ./AtomicEditor.app

atomic_data = require('atomic-cli-mac-data');

module.exports = {
  "ATOMICTOOL_BIN" : atomic_data.ATOMICTOOL_BIN,
  "ATOMICEDITOR_APP" : atomic_data.ATOMICEDITOR_APP,
  "ATOMICEDITOR_DATADIR" : atomic_data.ATOMICEDITOR_DATADIR

}
