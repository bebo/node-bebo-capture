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
BeboCapture.getConstraints = promisify(Native.getConstraints);

var setCapture = promisify(Native.setCapture);
BeboCapture.setCapture = (options) => {
  if (!typeof (options) === 'object') {
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

var setConstraints = promisify(Native.setConstraints);
BeboCapture.setConstraints = (options) => {
  if (!typeof (options) === 'object') {
    return new Promise((_, reject) => { reject("invalid object") });
  }

  if (!options.width && !options.height && !options.fps) {
    return Promise.reject("require params: width, height OR fps");
  }

  if ((options.width && typeof (options.width) !== 'number') ||
    (options.height && typeof (options.height) !== 'number') ||
    (options.fps && typeof (options.fps) !== 'number')) {
    return Promise.reject("width, height, fps are not number");
  }

  if ((!options.width && options.width < 0) ||
    (!options.height && options.height < 0) ||
    (!options.fps && options.fps < 0)) {
    return Promise.reject("invalid params value: width, height, fps need to be non negative");
  }

  return setConstraints(
    options.width || 0,
    options.height || 0,
    options.fps || 0);
}

module.exports = BeboCapture;