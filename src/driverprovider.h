#ifndef DRIVERPROVIDER_H
#define DRIVERPROVIDER_H

#include <QtCore/QObject>
#include <QtCore/QMetaType>
#include <QtNetwork/QLocalSocket>

class comm_summa_request;
class comm_summa_response;
class driverProvider : public QObject
{
  Q_OBJECT
  Q_PROPERTY(protocolType protocol READ protocol)
  Q_PROPERTY(QString socketPath READ socketPath)
  Q_PROPERTY(QString name READ name)
  Q_PROPERTY(int dpi READ dpi WRITE setDpi)
  Q_PROPERTY(bool absolute READ absolute WRITE setAbsolute)
  Q_PROPERTY(int relativeMovement READ relativeMovement WRITE setRelativeMovement)
  Q_PROPERTY(bool smartButtons READ smartButtons WRITE setSmartButtons)

  friend class driverManager;
  friend class driverInstance;
  friend QDataStream& operator<<(QDataStream& stream, const driverProvider& p);
  friend QDataStream& operator>>(QDataStream& stream, driverProvider& p);
  
public:
  typedef enum {
    PROTO_DRV_SUMMA = 0
  } protocolType;
  
private:
  driverProvider(protocolType protocol, QString socketPath, QString name);
  
public:
  driverProvider();
  driverProvider(const driverProvider& other);
  ~driverProvider();
  
  driverProvider& operator=(const driverProvider& other);
  bool operator==(const driverProvider& other);

  protocolType protocol() const;
  QString socketPath() const;
  QString name() const;
  
//   void setProtocol(const protocolType& protocol);
//   void setSocketPath(const QString& socketPath);
//   void setName(const QString& name);
  
  int dpi();
  void setDpi(int dpi);
  QList<int> dpiChoices();
  bool hasDpi();
  
  bool absolute();
  void setAbsolute(bool absolute);
  bool hasAbsolute();
  
  int relativeMovement();
  void setRelativeMovement(int relativeMovement);
  bool hasRelativeMovement();
  
  bool smartButtons();
  void setSmartButtons(bool smartButtons);
  bool hasSmartButtons();
  
  bool connectSocket();

private:
  
  protocolType m_protocol;
  QString m_socketPath;
  QString m_name;
  QLocalSocket m_socket;
  
  comm_summa_response* getResponse(comm_summa_request *req);
};

Q_DECLARE_METATYPE(driverProvider)
Q_DECLARE_METATYPE(driverProvider::protocolType)

#endif // DRIVERPROVIDER_H
