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
});
