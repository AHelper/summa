#include <QApplication>
#include "summacontrolpanel.h"
#include "driverinstance.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  qRegisterMetaType<driverProvider>();
  summaControlPanel window;
  
  window.show();
  
  app.exec();
}