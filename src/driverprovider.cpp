#include "driverprovider.h"

driverProvider::driverProvider(driverProvider::protocolType protocol, QString socketPath, QString name) : QObject()
{
  this->m_protocol = protocol;
  this->m_socketPath = socketPath;
  this->m_name = name;
}

driverProvider::driverProvider() : QObject(),
m_protocol(driverProvider::PROTO_DRV_SUMMA),
m_socketPath(""),
m_name("")
{
}

driverProvider::~driverProvider()
{
}

driverProvider& driverProvider::operator=(const driverProvider& other)
{
  this->m_name = other.name();
  this->m_protocol = other.m_protocol;
  this->m_socketPath = other.m_socketPath;
  
  return *this;
}
driverProvider::driverProvider(const driverProvider& other) : QObject()
{
  this->m_name = other.name();
  this->m_protocol = other.m_protocol;
  this->m_socketPath = other.m_socketPath;
}

driverProvider::protocolType driverProvider::protocol() const
{
  return (driverProvider::protocolType)this->m_protocol;
}

QString driverProvider::socketPath() const
{
  return m_socketPath;
}

// void driverProvider::setProtocol(const driverProvider::protocolType& protocol)
// {
//   m_protocol = protocol;
// }
// 
// void driverProvider::setSocketPath(const QString& socketPath)
// {
//   m_socketPath = socketPath;
// }
// 
// void driverProvider::setName(const QString& name)
// {
//   m_name = name;
// }

QString driverProvider::name() const
{
  return m_name;
}

bool driverProvider::absolute()
{

}
void driverProvider::setAbsolute(bool absolute)
{

}
bool driverProvider::hasAbsolute()
{
  return false;
}
int driverProvider::dpi()
{

}
void driverProvider::setDpi(int dpi)
{

}
QList< int > driverProvider::dpiChoices()
{

}
bool driverProvider::hasDpi()
{
  return false;
}
int driverProvider::relativeMovement()
{

}
void driverProvider::setRelativeMovement(int relativeMovement)
{

}
bool driverProvider::hasRelativeMovement()
{
  return false;
}
bool driverProvider::smartButtons()
{

}
void driverProvider::setSmartButtons(bool smartButtons)
{

}
bool driverProvider::hasSmartButtons()
{
  return false;
}

