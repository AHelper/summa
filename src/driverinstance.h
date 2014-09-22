#ifndef DRIVERINSTANCE_H
#define DRIVERINSTANCE_H

#include <QtCore/QObject>
#include <QtCore/QMetaType>
#include "driverprovider.h"

class driverInstance : public QObject
{
  Q_OBJECT
  Q_PROPERTY(driverProvider provider READ provider)
  Q_PROPERTY(QString path READ path);
  
public:
  driverInstance();
  driverInstance(const driverInstance& other);
  driverInstance(driverProvider provider, QString path);
  ~driverInstance();
  
  driverInstance& operator=(const driverInstance& other);
  
  driverProvider provider();
  QString path();

private:
  driverProvider m_provider;
  QString m_path;
};
  
Q_DECLARE_METATYPE(QList<driverInstance>)
Q_DECLARE_METATYPE(driverInstance)

#endif // DRIVERINSTANCE_H