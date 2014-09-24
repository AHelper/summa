#include <QApplication>
#include "summacontrolpanel.h"
#include "driverinstance.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  qRegisterMetaTypeStreamOperators<driverProvider>("driverProvider");
  qRegisterMetaTypeStreamOperators<driverInstance>("driverInstance");
  qRegisterMetaTypeStreamOperators<QList<driverInstance> >("QList<driverInstance>");
  summaControlPanel window;
  
  window.show();
  
  app.exec();
}