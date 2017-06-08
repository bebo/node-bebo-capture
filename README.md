# node-bebo-capture
native module for node to control bebo-capture DLL


## BeboCapture.getDesktop()
```JavaScript
const BeboCapture = require('bebo-capture');

BeboCapture.getDesktop()
  .then(deviceList => console.log(deviceList));
// outputs:
[{
   id: "desktop:0",
   label: "Screen 0",
   type: "desktop"
 },{
   id: "desktop:1",
   label: "Screen 1",
   type: "desktop"
}]
```

## BeboCapture.setCapture()

### Capture Desktop
```JavaScript
const BeboCapture = require('bebo-capture');

let options = {
  id: "desktop:0",
  type: "desktop"
};

BeboCapture.setCapture(options)
  .then(() => console.log("capturing screen 0"));
```

### Capture Game (inject)
```JavaScript
const BeboCapture = require('bebo-capture');

let options = {
  type: "inject",
  "CaptureWindowClassName": "TankWindow",
  "CaptureWindowName": "",
  "CaptureAntiCheat": 0
};

BeboCapture.setCapture(options)
  .then(() => console.log("capturing Overwatch"));
```





