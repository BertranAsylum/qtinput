# Qt-input-tools
Provides friendly user input into Qt widgets inherited from QLineEdit (like a QSpinBox, QDoubleSpinBox, QTimeEdit)
* <b>drag-controller</b>: You can change editor's value by dragging it linear or circular gesture.
Try it with:
``` c++
#include <qt-input-tools/drag-controller.h>
using namespace QtInputTools;

void func() {
  DragController::attach(/*editor*/spinBox, /*mode*/DragController::LINEAR, /*sensitivity*/0.5);
}
```
<div>
<span>
<img src="https://raw.github.com/twister9893/qt-input-tools/master/example/screenshot-1.png" width="390" height="250" hspace="5"/>
</span>
<span>
<img src="https://raw.github.com/twister9893/qt-input-tools/master/example/screenshot-2.png" width="390" height="250" hspace="5"/>
</span>
</div>

## Installation
1. Download zip or clone with ```git clone git@github.com:twister9893/qt-input-tools.git```
2. Execute in project dir:
  ```
  qmake
  make
  sudo make install
  ```
## Example installation
Execute in project dir:
```
cd example
qmake
make
cd ../exe
./example
```
