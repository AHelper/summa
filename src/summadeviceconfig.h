#ifndef SUMMADEVICECONFIG_H
#define SUMMADEVICECONFIG_H

#include <Qt/QtGui>


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

private:
  Ui::summaDeviceConfig* ui;
  bool isCreatingNew = false;
  
private slots:
  // newButton
  void newDevice();
  // deleteButton
  void deleteDevice();
  // deviceTable
  void deviceClicked(QTableWidgetItem* item);
  // deviceSaveButton
  void saveDevice();
};

#endif // SUMMADEVICECONFIG_H
