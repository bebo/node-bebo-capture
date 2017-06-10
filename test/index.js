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
  it('getDesktops should return the current settings', function () {
    return BeboCapture.getDesktops()
      .then(function (data) {
        assert(data.length > 0, "expect one or more desktops");
        assert(data[0].type === 'desktop', "expect capture type desktop");
        assert(data[0].id === 'desktop:0', "capture id should be desktop:0");
        assert(data[0].label === 'Screen 1', "capture id should be desktop:0");
    });
  });
  it('setCapture should set the current settings', function () {
      let testValues = {
          id: "test_id",
          type: "inject",
          CaptureWindowClassName: "TankWindow",
          CaptureWindowName: "",
          CaptureAntiCheat: true 
      };

    return BeboCapture.setCapture(testValues)
      .then(function () {
        return BeboCapture.getCapture()
          .then(function (data) {
            console.log('data', data);
            assert(data != null, "capture settings should exist");
            assert(data.type == testValues.type, "capture type should exist");
        });
    });
  });
});
