# node-bebo-capture
native module for node to control bebo-capture DLL


## BeboCapture.getDesktops()
```JavaScript
const BeboCapture = require('bebo-capture');

BeboCapture.getDesktops()
  .then(deviceList => console.log(deviceList));
// outputs:
[{
   id: "desktop:0:0",
   label: "Screen 0",
   type: "desktop"
 },{
   id: "desktop:0:1",
   label: "Screen 1",
   type: "desktop"
}]
```

## BeboCapture.setCapture()

### Capture Desktop
```JavaScript
let options = {
  id: "desktop:0",
  type: "desktop",
  label: "Screen 0"
};

BeboCapture.setCapture(options)
  .then((capture) => console.log("capturing screen 1", capture));
```

### Capture Game (inject)
```JavaScript
let options = {
  type: "inject",
  windowClassName: "TankWindow",
  windowName: "Overwatch",
  antiCheat: true 
};

BeboCapture.setCapture(options)
  .then(() => console.log("capturing Overwatch"));
```

## BeboCapture.getCapture()

What does Bebo Capture API think it is doing?

```JavaScript
BeboCapture.getCapture()
  .then(currentSettings => console.log(currentSettings));
// outputs e.g. for desktop
{
  id: "desktop:0",
  type: "desktop",
  label: "Screen 0"
}
```




