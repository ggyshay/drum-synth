#include <stdlib.h>
#include <functional>

class Encoder
{
public:
  int getValue()
  {
    return value >> 1;
  }
  std::function<void(void)> onClick = nullptr;
  bool setReading(bool A, bool B, bool C)
  {
    bool changed = false;
    if (C != lastClickState)
    {
      if (C)
      {
        if (onClick != nullptr)
        {
          onClick();
        }
      }
      lastClickState = C;
    }
    state = A;
    if (state != lastState)
    {
      if (B != state)
      {
        value++;
        if (parameter != nullptr && (value & 1))
          parameter->increment();
      }
      else
      {
        value--;
        if (parameter != nullptr && (value & 1))
          parameter->decrement();
      }
      changed = true;
    }
    lastState = state;
    return changed;
  }

  void setParameterPointer(Value *p)
  {
    parameter = p;
  }

private:
  int value = 0;
  bool lastState = false;
  bool state = false;
  bool lastClickState = false;
  Value *parameter = nullptr;
};
