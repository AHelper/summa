#ifndef DRIVERMANAGER_H
#define DRIVERMANAGER_H

#include <QtCore/QObject>
#include <QVector>
#include "driverinstance.h"
#include "driverprovider.h"

class driverManager : public QObject
{
  Q_OBJECT

public:
  driverManager();
  ~driverManager();
  
  QList< driverProvider > getProviders();
  QList< driverInstance > getInstances();
  
  void addInstance(driverInstance& inst);
  void removeInstance(driverInstance& inst);
  
private:
  
};

#endif // DRIVERMANAGER_H
