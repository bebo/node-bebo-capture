var Native = require('bindings')('BeboCaptureNative');
var Promise = require('bluebird');

var BeboCapture = {
};
BeboCapture.getCapture = Promise.promisify(Native.getCapture);

module.exports = BeboCapture;
