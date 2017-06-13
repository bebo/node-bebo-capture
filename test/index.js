var BeboCapture = require('../');
var assert = require('assert');

describe('bebo capture extension', function() {
  it('getCapture should return the current settings', function () {
    return BeboCapture.getCapture()
      .then(function (data) {
        console.log('data', data);
        assert(data != null, "capture settings should exist");
        assert(data.type != null, "capture type should exist");
    });
  });
  it('getCapture B should return the current settings', function () {
    return BeboCapture.getCapture()
      .then(function (data) {
        console.log('data', data);
        assert(data != null, "capture settings should exist");
        assert(data.type != null, "capture type should exist");
    });
  });
  it('getCapture C should return the current settings', function () {
    return BeboCapture.getCapture()
      .then(function (data) {
        console.log('data', data);
        assert(data != null, "capture settings should exist");
        assert(data.type != null, "capture type should exist");
    });
  });
  it('setCaptureVerify ', function () {
    let testValues = {
      id: "test_id",
      type: "inject",
      windowClassName: "TankWindow",
      windowName: "",
      //  antiCheat: true 
    };
    let expected = Object.assign({label: ""}, testValues);
    return BeboCapture.setCapture(testValues)
      .then(function (result) {
        console.log('setCapture result:', result);
        assert.deepEqual(result, expected);
      }).then(function verify() {
        console.log("run get now");
        return BeboCapture.getCapture();
      }).then(function (verifyData) {
        console.log('getData result:', verifyData);
        assert(verifyData != null, "capture settings should exist");
        assert.deepEqual(verifyData, expected);
        console.log("OOOKKK");
      });
  });
  it('getCapture should return the current settings', function () {
    return BeboCapture.getCapture()
      .then(function (data) {
        console.log('data', data);
        assert(data != null, "capture settings should exist");
        assert(data.type != null, "capture type should exist");
    });
  });
  it('setCapture should set the current settings', function () {
      let testValues = {
          id: "test_id",
          type: "inject",
          windowClassName: "TankWindow",
          windowName: "",
        //  antiCheat: true 
      };

      let expected = Object.assign({label: ""}, testValues);
      return BeboCapture.setCapture(testValues)
          .then(function (result) {
              console.log('setCapture result:', result);
              assert.deepEqual(result, expected);
          })
          /*
          .then(function verify() {

              return BeboCapture.getCapture()
                .then(function (verifyData) {
                        console.log('getData result:', verifyData);
                    try ()
                        assert(verifyData != null, "capture settings should exist");
                        assert.deepEqual(verifyData, expected);
                        console.log("OOOKKK");
                });
          });
          */
  });
  it('getDesktops should return the current settings', function () {
      return BeboCapture.getDesktops()
        .then(function (data) {
            assert(data.length > 0, "expect one or more desktops");
            assert(data[0].type === 'desktop', "expect capture type desktop");
            assert(data[0].id === 'desktop:0', "capture id should be desktop:0");
            assert(data[0].label === 'Screen 1', "capture id should be desktop:0");
        });
  });
});
