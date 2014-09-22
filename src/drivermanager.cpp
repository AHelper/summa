#include "drivermanager.h"
#include <QtCore/QSettings>
#include <iostream>
#include <QtCore/QDebug>

using namespace std;

driverManager::driverManager()
{
}

driverManager::~driverManager()
{
  
}

QList< driverInstance > driverManager::getInstances()
{
  QSettings settings("AHelper", "summactl");

  return settings.value("instances").value<QList<driverInstance> >();
}

QList< driverProvider > driverManager::getProviders()
{
  static QList<driverProvider> providers;
  
  if(providers.isEmpty())
  {
    providers.push_back(driverProvider(driverProvider::PROTO_DRV_SUMMA, "/var/run/drv_summa.socket", "SummaSketch II"));
  }
  
  return providers;
}

void driverManager::addInstance(driverInstance& inst)
{
  QSettings settings("AHelper", "summactl");
  
  QList<driverInstance> instances = getInstances();
  
  instances.push_back(inst);
  cout << instances.size() << endl;
  
  settings.setValue("instances", QVariant::fromValue<QList<driverInstance> >(instances));
  instances = getInstances();
  cout << instances[0].path().toStdString() << " " << instances[0].provider().name().toStdString() << endl;
  cout << QVariant::fromValue<QList<driverInstance>>(instances).userType() << endl;
}

void driverManager::removeInstance(driverInstance& inst)
{

}
