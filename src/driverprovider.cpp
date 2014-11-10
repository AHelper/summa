#include "driverprovider.h"
#include "comm_summa.pb.h"
#include <iostream>

driverProvider::driverProvider(driverProvider::protocolType protocol, QString socketPath, QString name) : QObject(),
  m_protocol(protocol),
  m_socketPath(socketPath),
  m_name(name)
{
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
  comm_summa_request req;
  req.set_query(true);
  
  comm_summa_response* response = getResponse(&req);
  
  if(response)
    if(response->has_query())
      if(response->query().set().has_dpi())
        return response->query().set().dpi();
      else
        qDebug() << "No" << __LINE__;
    else
      qDebug() << "No" << __LINE__;
  else
    qDebug() << "No" << __LINE__;
  return -1;
  /*
  if(!connectSocket())
    return -1;
  
  comm_summa_request req;
  int remaining;
  
  req.set_query(true);
  
  char buffer[req.ByteSize()+4];
  
  qDebug() << req.SerializeToArray(buffer+4, req.ByteSize());
  *(int*)buffer = req.ByteSize();
  
  comm_summa_request req2;
  
  qDebug() << req2.ParseFromArray(buffer + 4, req.ByteSize());
  qDebug() << req2.has_query() << req2.query();
  
  for(int pos = 0; pos < req.ByteSize(); pos++)
  {
    printf("%2X", buffer[pos+4]);
  }
  printf("\n");
  
  this->m_socket.write(buffer, req.ByteSize() + 4);
//   this->m_socket.flush();
//   this->m_socket.waitForBytesWritten(1000);
  this->m_socket.flush();
  
  remaining = 4;
  
  while(remaining)
  {
    this->m_socket.waitForReadyRead();
    int read = this->m_socket.read(buffer+4-remaining, remaining);
    
    if(read < 0)
      return -1;
    remaining -= read;
  }
  printf("LINE %d\n", __LINE__);
  
  remaining = *(int*)buffer;
  qDebug() << req.ByteSize() << endl;
  int total = remaining;
  
  char buffer2[total];
  
  while(remaining)
  {
    this->m_socket.waitForReadyRead(1000);
    int len = this->m_socket.read(buffer2+total-remaining, remaining);
    if(len == -1)
      return -1;
    remaining -= len;
    printf("LINE %d\n", __LINE__);
  }
  printf("LINE %d\n", __LINE__);
  
  comm_summa_response response;
  response.ParseFromArray(buffer2, total);
  printf("LINE %d\n", __LINE__);
  
  qDebug() << "Received query data!" << total;
  
  if(response.has_query())
    if(response.query().set().has_dpi())
      return response.query().set().dpi();
  return -1;*/
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

bool driverProvider::connectSocket()
{
  if(this->m_socket.state() != QLocalSocket::ConnectedState)
    this->m_socket.connectToServer("/var/run/drv_summa.socket");
  
  if(!this->m_socket.waitForConnected(500)) {
    qDebug() << this->m_socket.errorString();
    return false;
  }
  return true;
}

QDataStream& operator<<(QDataStream& stream, const driverProvider& p)
{
  stream << (qint32)p.m_protocol << p.m_name << p.m_socketPath;

  return stream;
}

QDataStream& operator>>(QDataStream& stream, driverProvider& p)
{
  stream >> (qint32&)p.m_protocol >> p.m_name >> p.m_socketPath;

  return stream;
}

bool driverProvider::operator==(const driverProvider& other)
{
  return this->m_name == other.m_name && this->m_protocol == other.m_protocol && this->m_socketPath == other.m_socketPath;
}

comm_summa_response* driverProvider::getResponse(comm_summa_request* req)
{
  if(!connectSocket()){qDebug() << "Failure" << __LINE__;
      return 0;
    }
  
  int remaining = 4;
  int total;
  char buffer[req->ByteSize()+4];
  
  req->SerializeToArray(buffer+4, req->ByteSize());
  *(int*)buffer = req->ByteSize();
  
  this->m_socket.write(buffer, req->ByteSize() + 4);
  this->m_socket.flush();
  
  while(remaining)
  {
    this->m_socket.waitForReadyRead();
    int read = this->m_socket.read(buffer+4-remaining, remaining);
    
    if(read < 0){qDebug() << "Failure" << __LINE__;
      return 0;
    }
    remaining -= read;
  }
  
  total = remaining = *(int*)buffer;
  
  char buffer2[total];
  
  while(remaining)
  {
    this->m_socket.waitForReadyRead(1000);
    int len = this->m_socket.read(buffer2+total-remaining, remaining);
    
    if(len == -1){qDebug() << "Failure" << __LINE__;
      return 0;
    }
    
    remaining -= len;
  }
  
  comm_summa_response* response = new comm_summa_response;
  response->ParseFromArray(buffer2, total);
  
  return response;
}

