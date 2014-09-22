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