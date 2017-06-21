# node-bebo-capture
native module for node to control bebo-capture DLL


## BeboCapture.getDesktopList()
```JavaScript
const BeboCapture = require('bebo-capture');

BeboCapture.getDesktopList()
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

## BeboCapture.getWindowList()
```JavaScript
const BeboCapture = require('bebo-capture');

BeboCapture.getWindowList()
  .then(windowList => console.log(windowList));
// outputs:
[{ 
    windowName: 'OpenProcess function (Windows) - Google Chrome',
    windowClassName: 'Chrome_WidgetWin_1',
    windowHandle: '0x00000000000f00aa',
    exeFullName: 'C:\\Users\\fpn\\AppData\\Local\\Google\\Chrome SxS\\Application\\chrome.exe'
  }, {
    windowName: 'Slack - monkey inferno',
    windowClassName: 'Chrome_WidgetWin_1',
    windowHandle: '0x00000000000900ce',
    exeFullName: 'C:\\Users\\fpn\\AppData\\Local\\slack\\app-2.6.3\\slack.exe'
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

### Capture window using GDI
```JavaScript
let options = {
  type: "gdi",
  label: "My Favorite window",
  windowHandle: "0xdeadbeefdeadbeef",
  windowClassName: "TankWindow",
  windowName: "Overwatch",
  exeFullName: "C:\\Program Files(x86)\\whatever\\overwatch.exe",
  once: false
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


# How to build

Have Visual Studio 2015 SP3 installed

If you don't have visual studio in your env you may need to run this in the Visual Studio Command Prompt.

Also run in cmd not Ubuntu bash,... (yes you know who I am talking to)

```
npm install -g node-gyp
npm install -g mocha

npm config set msvs_version "2015"
node-gyp configure

# if you want debug
# node-gyp configure --debug

npm run build
npm test
