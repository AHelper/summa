#include <QApplication>
#include "summacontrolpanel.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  summaControlPanel window;
  
  window.show();
  
  app.exec();
}