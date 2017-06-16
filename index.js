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
BeboCapture.getWindowList = promisify(Native.getWindowList);

/*
BeboCapture.getDesktops = () => {
    // TODO FAKE FOR NOW
    return new Promise((resolve) => {
        let result = [{
            id: "desktop:0",
            label: "Screen 1",
            type: "desktop"
        },{
            id: "desktop:1",
            label: "Screen 2",
            type: "desktop"
        }];
        return resolve(result);
    });
}
*/

var setCapture = promisify(Native.setCapture);
BeboCapture.setCapture = (options) => {
    if (!typeof(options) === 'object') {
        return new Promise((_, reject) => { reject("invalid object") });
    }
    return setCapture(options.type || "", options.id || "", options.label || "", options.windowName || "", options.windowClassName || "", options.antiCheat || false);
}

module.exports = BeboCapture;