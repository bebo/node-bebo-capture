/*eslint eqeqeq: ["error", "smart"]*/

var BeboCapture = require('../');
var assert = require('assert');

const emptyValues = {
  id: "",
  label: "",
  type: "inject",
  windowName: "",
  windowClassName: "",
  antiCheat: false 
}

describe('bebo capture extension', function() {
  it('getCapture should return the current settings', function () {
    return BeboCapture.getCapture()
      .then(function (data) {
        assert(data != null, "capture settings should exist");
      }).catch(function(err) {
        console.log(err);
        throw err;
      });
  });
  it('setCapture clear ', function () {
    let testValues = {
    };
    let expected = Object.assign({}, emptyValues, testValues);
    return BeboCapture.setCapture(testValues)
      .then(function (result) {
        assert.deepEqual(result, expected);
      }).then(function verify() {
        return BeboCapture.getCapture();
      }).then(function (verifyData) {
        assert(verifyData != null, "capture settings should exist");
        assert.deepEqual(verifyData, expected);
      }).catch(function(err) {
        console.log(err);
        throw err;
      });
  });
  it('setCapture desktop and verify', function () {
    let testValues = {
      id: "desktop:0:0",
      type: "desktop",
      label: "Screen 0"
    };
    let expected = Object.assign({}, emptyValues, testValues);
    return BeboCapture.setCapture(testValues)
      .then(function (result) {
        assert.deepEqual(result, expected);
      }).then(function verify() {
        return BeboCapture.getCapture();
      }).then(function (verifyData) {
        assert(verifyData != null, "capture settings should exist");
        assert.deepEqual(verifyData, expected);
      }).catch(function(err) {
        console.log(err);
        throw err;
      });
  });
  it('setCapture window and verify', function () {
    let testValues = {
      id: "gdi",
      type: "gdi",
      label: "My Favorite window",
      windowHandle: "0xdeadbeefdeadbeef",
      windowClassName: "TankWindow",
      windowName: "Overwatch"
    };
    let expected = Object.assign({}, emptyValues, testValues);
    return BeboCapture.setCapture(testValues)
      .then(function (result) {
        assert.deepEqual(result, expected);
      }).then(function verify() {
        return BeboCapture.getCapture();
      }).then(function (verifyData) {
        assert(verifyData != null, "capture settings should exist");
        assert.deepEqual(verifyData, expected);
      }).catch(function(err) {
        console.log(err);
        throw err;
      });
  });
  it('setCapture inject and verify', function () {
    let testValues = {
        id: "",
        type: "inject",
        windowClassName: "TankWindow",
        windowName: "Overwatch",
        antiCheat: true 
    };

    let expected = Object.assign({}, emptyValues, testValues);
    return BeboCapture.setCapture(testValues)
      .then(function (result) {
          assert.deepEqual(result, expected);
      }).catch(function(err) {
        console.log(err);
        throw err;
      });
  });
  it('getDesktopList should return the current settings', function () {
    return BeboCapture.getDesktopList()
        .then(function (data) {
            console.log("desktops", data);
            assert(data.length > 0, "expect one or more desktops");
            assert(data[0].type === 'desktop', "expect capture type desktop");
            assert(data[0].id === 'desktop:0:0', "capture id should be desktop:0:0");
            assert(data[0].label === 'Screen 1', "capture label should be Screen 1");
            assert(Array.isArray(data), "expect array");
      }).catch(function(err) {
        console.log(err);
        throw err;
        });
  });
  it('getWindowList return list of windows', function () {
      return BeboCapture.getWindowList()
        .then(function (data) {
          console.log("windows", data);
          assert(Array.isArray(data), "expect array");
          /*
            assert(data.length > 0, "expect one or more desktops");
            assert(data[0].type === 'desktop', "expect capture type desktop");
            assert(data[0].id === 'desktop:0:0', "capture id should be desktop:0:0");
            assert(data[0].label === 'Screen 1', "capture label should be Screen 1");
            */
      }).catch(function(err) {
        console.log(err);
        throw err;
        });
  });
});
