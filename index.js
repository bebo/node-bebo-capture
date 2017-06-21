var Native = require('bindings')('BeboCaptureNative');
const util = require('util');
var promisify = util.promisify;

if (promisify == null) {
  let Promise = require('bluebird');
  promisify = Promise.promisify;
}

var BeboCapture = {
};

BeboCapture.getCapture = promisify(Native.getCapture);
BeboCapture.getDesktops = promisify(Native.getDesktops);
BeboCapture.getDesktopList = promisify(Native.getDesktops);
BeboCapture.getWindowList = promisify(Native.getWindowList);

var setCapture = promisify(Native.setCapture);
BeboCapture.setCapture = (options) => {
    if (!typeof(options) === 'object') {
        return new Promise((_, reject) => { reject("invalid object") });
    }
    return setCapture(
      options.type || "",
      options.id || "",
      options.label || "",
      options.windowName || "",
      options.windowClassName || "",
      options.windowHandle || "",
      options.exeFullName || "",
      options.antiCheat || false,
      options.once || false);
}

module.exports = BeboCapture;