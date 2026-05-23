#include "Renderer.h"
#include "Gui.h"




int main(){
  CW::Renderer::Renderer window;

  while (!window.getWindowData()->should_close){
    window.beginFrame();
    window.swapBuffer();

    window.windowEvents();
  }
};


