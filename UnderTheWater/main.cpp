#include "App.h"



int main(){
  UW::App app;
  while(app.isRunning()) app.run();
  return 0;
};
