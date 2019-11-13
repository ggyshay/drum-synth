#include <vector>
#include <functional>
#ifndef INSTRUMENTINTERFACE_H
#define INSTRUMENTINTERFACE_H

class Value
{
public:
  Value(float _m, float _M, float initialValue, const char *_nameTag, int steps = 40, bool _isExp = false)
  {
    isExp = _isExp;
    if (isExp)
    {
      float lambda = -log(_M / _m) / ((float)steps);
      delta = 1 - lambda;
    }
    else
    {
      delta = (_M - _m) / ((float)steps);
    }
    _min = _m;
    _max = _M;
    value = initialValue;
    nameTag = _nameTag;
  }
  float value;
  const char *nameTag;
  //    float setValue(int input) { //0 a 40
  //      return value = min(max(angular * input + linear, _min), _max);
  //    }

  void increment()
  {
    if (isExp)
    {
      value *= delta;
    }
    else
    {
      value += delta;
    }
    value = min(value, _max);
  }

  void decrement()
  {
    if (isExp)
    {
      value /= delta;
    }
    else
    {
      value -= delta;
    }
    value = max(value, _min);
  }

  // const char *toString()
  // {
  //   char output[9];
  //   snprintf(output, 9, "%.2f", value);
  //   // String s = nameTag;
  //   // s += String(" ");
  //   // s += value;
  //   // s += "\0";
  //   return output;
  //   // return s;
  // }

  //
  //    void setBoundaries(float _min, float _max) {
  //      linear = _min;
  //      angular = (_max - _min) / 40.0f;
  //    }
  //    std::function<void(float)> onChange = nullptr;

private:
  float delta;
  float _min;
  float _max;
  bool isExp;
};

class InstrumentI
{
public:
  InstrumentI()
  {
    outPatch = new AudioConnection(outMixer, s_out);
  }
  std::vector<Value> values = {};
  AudioMixer4 outMixer;
  AudioAmplifier s_out;
  virtual void noteOn(byte) = 0;
  virtual void setupParams() = 0;
  Value *getNextValue()
  {
    selectedIndex = (selectedIndex + 1) % values.size();
    return &(values[selectedIndex]);
  }

  Value *getCurrentValue()
  {
    return &(values[selectedIndex]);
  }

protected:
  char fileName[20];

private:
  int selectedIndex = 0;
  AudioConnection *outPatch;
};

#endif
