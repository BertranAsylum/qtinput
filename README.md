# qt-input-tools
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
