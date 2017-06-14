var BeboCapture = require('../');
var assert = require('assert');

describe('bebo capture extension', function() {
  it('getCapture should return the current settings', function () {
    return BeboCapture.getCapture()
      .then(function (data) {
        assert(data != null, "capture settings should exist");
        assert(data.type != null, "capture type should exist");
      }).catch(function(err) {
        console.log(err);
        throw err;
      });
  });
  it('setCaptureVerify ', function () {
    let testValues = {
      id: "test_id",
      type: "inject",
      windowClassName: "TankWindow",
      windowName: "",
      antiCheat: true 
    };
    let expected = Object.assign({label: ""}, testValues);
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
  it('setCapture should set the current settings', function () {
    let testValues = {
        id: "test_id",
        type: "inject",
        windowClassName: "TankWindow",
        windowName: "",
        antiCheat: false 
    };

    let expected = Object.assign({label: ""}, testValues);
    return BeboCapture.setCapture(testValues)
      .then(function (result) {
          assert.deepEqual(result, expected);
      }).catch(function(err) {
        console.log(err);
        throw err;
      });
  });
  it('getDesktops should return the current settings', function () {
      return BeboCapture.getDesktops()
        .then(function (data) {
            assert(data.length > 0, "expect one or more desktops");
            assert(data[0].type === 'desktop', "expect capture type desktop");
            assert(data[0].id === 'desktop:0', "capture id should be desktop:0");
            assert(data[0].label === 'Screen 1', "capture id should be desktop:0");
            console.log("desktops", data);
      }).catch(function(err) {
        console.log(err);
        throw err;
        });
  });
});
