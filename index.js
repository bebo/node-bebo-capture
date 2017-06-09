var Native = require('bindings')('BeboCaptureNative');
var Promise = require('bluebird');

var BeboCapture = {
};
BeboCapture.getCapture = Promise.promisify(Native.getCapture);

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

BeboCapture.setCapture = (options) => {
    return Promise.resolve();
}

module.exports = BeboCapture;