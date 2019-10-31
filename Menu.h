#include "audioInfra.h"
#ifndef _MENU_H_
#define _MENU_H_

#define mS0 27
#define mS1 28
#define mS2 29

#include <vector>
#include "instrumentInterface.h"

class Screen
{
public:
  Screen(const char *t)
  {
    title = t;
  }
  const char *title;
  // std::function<void(AudioInfra *infra)> createParameter = nullptr; // a screen cria seu *parameter ou usando a infra ou sendo um parametro interno
  std::function<const char *(void)> onClick = nullptr; // quando o encoder clica alguma açao é performada especifica de cada screen e retorna a proxima pagina
  Value *parameter;                                    // pode ser um parametro do infra como freuquencia do filtro ou indice no array de strings
  std::vector<char *> strings;                         // se o parametro é interno aqui tem os valores que essa screen tem "LPF, HPF, BPF, VOLTAR"
  std::function<const char *(void)> render = nullptr;  //retorna o float ou string que deve ser colocado no display, roda quando o encoder mudar.
};

class Menu
{
public:
  Encoder encoder;
  int selectedScreenIndex = 0;
  DisplayDriver *disp;
  Menu(DisplayDriver *_disp)
  {
    disp = _disp;
    pinMode(mS0, INPUT);
    pinMode(mS1, INPUT);
    pinMode(mS2, INPUT);
    encoder.onClick = [this]() -> void {
      const char *newScreen = screens[selectedScreenIndex].onClick();
      changeScreen(newScreen);
    };

    Screen menuScreen("MENU");
    menuScreen.strings.push_back("DISTORCAO");
    menuScreen.strings.push_back("FILTRO");
    menuScreen.strings.push_back("INT CLOCK");
    menuScreen.strings.push_back("SESSAO");

    menuScreen.parameter = new Value(0.0, 3.0, 1.0, "", 3);
    menuScreen.onClick = [menuScreen]() -> const char * {
      // return "FILTRO";
      return menuScreen.strings[(int)(menuScreen.parameter->value)];
    };

    menuScreen.render = [menuScreen]() -> char * {
      return menuScreen.strings[(int)(menuScreen.parameter->value)];
    };
    screens.push_back(menuScreen);

    Screen filterScreen("FILTRO");
    filterScreen.strings.push_back("DESLIGADO");
    filterScreen.strings.push_back("LPF");
    filterScreen.strings.push_back("HPF");
    filterScreen.strings.push_back("BPF");
    filterScreen.strings.push_back("VOLTAR");

    filterScreen.parameter = new Value(0.0, (float)filterScreen.strings.size() - 1, 1.0, "", (float)filterScreen.strings.size() - 1);
    filterScreen.onClick = [filterScreen]() -> const char * {
      return strcmp(filterScreen.strings[(int)filterScreen.parameter->value], "VOLTAR") == 0 ? "MENU" : "FREQUENCIA";
    };
    filterScreen.render = [filterScreen]() -> char * {
      return filterScreen.strings[(int)(filterScreen.parameter->value)];
    };
    screens.push_back(filterScreen);

    Screen filterFrequencyScreen("FREQUENCIA");
    filterFrequencyScreen.onClick = [filterFrequencyScreen]() -> const char * {
      return "FILTRO";
    };

    filterFrequencyScreen.parameter = new Value(200.0, 4000.0, 1.0, "", 100);
    filterFrequencyScreen.render = [filterFrequencyScreen]() -> const char * {
      return filterFrequencyScreen.parameter->toString().c_str();
    };
    screens.push_back(filterFrequencyScreen);

    Screen distortionScreen("DISTORCAO");
    distortionScreen.onClick = [distortionScreen]() -> const char * {
      return "MENU";
    };

    distortionScreen.parameter = new Value(0.0, 10.0, 1.0, "", 100);
    distortionScreen.render = [distortionScreen]() -> const char * {
      return distortionScreen.parameter->toString().c_str();
    };
    screens.push_back(distortionScreen);

    Screen clockScreen("INT CLOCK");
    clockScreen.strings.push_back("ATIVADO");
    clockScreen.strings.push_back("BPM");
    clockScreen.strings.push_back("VOLTAR");
    clockScreen.parameter = new Value(0.0, 2.0, 1.0, "", 2);
    clockScreen.onClick = [clockScreen]() -> const char * {
      const char *a = clockScreen.strings[(int)(clockScreen.parameter->value)];
      return strcmp(a, "VOLTAR") == 0 ? "MENU" : a;
    };

    clockScreen.render = [clockScreen]() -> const char * {
      return clockScreen.strings[(int)(clockScreen.parameter->value)];
    };
    screens.push_back(clockScreen);

    Screen activeClockScreen("ATIVADO");
    activeClockScreen.strings.push_back("SIM");
    activeClockScreen.strings.push_back("NAO");
    activeClockScreen.parameter = new Value(0.0, 1.0, 1.0, "", 1);
    activeClockScreen.onClick = [activeClockScreen]() -> const char * {
      return "INT CLOCK";
    };

    activeClockScreen.render = [activeClockScreen]() -> const char * {
      return activeClockScreen.strings[(int)(activeClockScreen.parameter->value)];
    };
    screens.push_back(activeClockScreen);

    Screen clockBPMScreen("BPM");
    clockBPMScreen.parameter = new Value(60.0, 220.0, 120.0, "", 160);
    clockBPMScreen.onClick = [clockBPMScreen]() -> const char * {
      return "INT CLOCK";
    };

    clockBPMScreen.render = [clockBPMScreen]() -> const char * {
      return clockBPMScreen.parameter->toString().c_str();
    };
    screens.push_back(clockBPMScreen);

    Screen sessionScreen("SESSAO");
    sessionScreen.strings.push_back("SALVAR");
    sessionScreen.strings.push_back("RECUPERAR");
    sessionScreen.parameter = new Value(0.0, 1.0, 1.0, "", 1);
    sessionScreen.onClick = [sessionScreen]() -> const char * {
      if ((int)(sessionScreen.parameter->value))
      {
        audioInfra->recoverAudioState();
      }
      else
      {
        audioInfra->saveAudioState();
      }
      return "MENU";
    };

    sessionScreen.render = [sessionScreen]() -> const char * {
      return sessionScreen.strings[(int)(sessionScreen.parameter->value)];
    };
    screens.push_back(sessionScreen);

    encoder.setParameterPointer(screens[0].parameter);
  }
  std::vector<Screen> screens = {};

  void update()
  {
    bool changed = encoder.setReading(digitalReadFast(mS0), digitalReadFast(mS1), digitalReadFast(mS2));
    if (changed)
    {
      const char *a = screens[selectedScreenIndex].render();
      disp->putScreen(screens[selectedScreenIndex].title, a);
    }
  }

  void changeScreen(const char *newScreen)
  {
    char i = 0;
    char pos = 0xFF;
    while (i < screens.size())
    {
      if (strcmp(screens[i].title, newScreen) == 0)
      {
        pos = i;
        break;
      }
      ++i;
    }
    if (pos != 0xFF)
    {
      selectedScreenIndex = pos;
      const char *a = screens[selectedScreenIndex].render();
      disp->putScreen(screens[selectedScreenIndex].title, a);
      encoder.setParameterPointer(screens[selectedScreenIndex].parameter);
    }
  }
};

#endif
