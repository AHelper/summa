#ifndef SUMMADEVICECONFIG_H
#define SUMMADEVICECONFIG_H

#include <Qt/QtGui>
#include "driverinstance.h"

namespace Ui
{
class summaDeviceConfig;
}

class summaDeviceConfig : public QDialog
{
  Q_OBJECT
public:
  summaDeviceConfig(QWidget *parent);
  ~summaDeviceConfig();
  
  driverInstance getInstance();

private:
  Ui::summaDeviceConfig* ui;
  bool isCreatingNew = false;
  
private slots:
  // newButton
  void newDevice();
  // deleteButton
  void deleteDevice();
  // deviceTable
  void deviceClicked(int row, int col);
  // deviceSaveButton
  void saveDevice();
};

#endif // SUMMADEVICECONFIG_H
