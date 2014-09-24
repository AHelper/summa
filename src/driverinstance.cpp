#include "driverinstance.h"

driverInstance::~driverInstance()
{
}

driverInstance::driverInstance(): QObject()
{

}

driverInstance::driverInstance(const driverInstance& other): QObject()
{
  this->m_provider = other.m_provider;
  this->m_path = other.m_path;
}

driverInstance::driverInstance(driverProvider provider, QString path): QObject()
{
  this->m_provider = provider;
  this->m_path = path;
}

driverProvider driverInstance::provider()
{
  return this->m_provider;
}

QString driverInstance::path()
{
  return this->m_path;
}

driverInstance& driverInstance::operator=(const driverInstance& other)
{
  this->m_provider = other.m_provider;
  this->m_path = other.m_path;
}

driverInstance::operator QVariant()
{
  return QVariant::fromValue<driverInstance>(*this);
}

QDataStream& operator<<(QDataStream& stream, const driverInstance& other)
{
  stream << other.m_provider << other.m_path;
  
  return stream;
}

QDataStream& operator>>(QDataStream& stream, driverInstance& other)
{
  stream >> other.m_provider >> other.m_path;
  
  return stream;
}

QDataStream& operator<<(QDataStream& stream, const QList<driverInstance>& other)
{
  stream << other.size();
  
  for(driverInstance inst : other)
    stream << inst;
  
  return stream;
}

QDataStream& operator>>(QDataStream& stream, QList<driverInstance>& other)
{
  int size;
  
  stream >> size;
  
  while(size--)
  {
    driverInstance inst;
    
    stream >> inst;
    
    other.push_back(inst);
  }
  
  return stream;
}
