#include "summadeviceconfig.h"
#include "ui_summadeviceconfig.h"
#include "driverinstance.h"
#include "drivermanager.h"
#include <iostream>

using namespace std;

summaDeviceConfig::summaDeviceConfig(QWidget* parent) : QDialog(parent)
{
  ui = new Ui::summaDeviceConfig;
  ui->setupUi(this);
  
  driverManager mgr;
  
  auto providers = mgr.getProviders();
  
  for(auto provider : providers)
  {
    this->ui->deviceTypeCombo->addItem(provider.name(), QVariant::fromValue<driverProvider>(provider));
  }
  
  auto instances = mgr.getInstances();
  
  for(auto instance : instances)
  {
    QTableWidgetItem* providerItem = new QTableWidgetItem(instance.provider().name());
    QTableWidgetItem* pathItem = new QTableWidgetItem(instance.path());
    
    this->ui->deviceTable->insertRow(this->ui->deviceTable->rowCount());
    this->ui->deviceTable->setItem(this->ui->deviceTable->rowCount(), 0, providerItem);
    this->ui->deviceTable->setItem(this->ui->deviceTable->rowCount(), 1, pathItem);
  }
}

summaDeviceConfig::~summaDeviceConfig()
{
  delete ui;
}

void summaDeviceConfig::newDevice()
{
  this->isCreatingNew = true;
  
  this->ui->deviceTypeCombo->setEnabled(true);
  this->ui->devicePathEdit->setEnabled(true);
  this->ui->deviceSaveButton->setEnabled(true);
  
  this->ui->deviceTypeCombo->setCurrentIndex(0);
  this->ui->devicePathEdit->setText("");
}

void summaDeviceConfig::deleteDevice()
{
  if(!this->ui->deviceTable->selectedItems().isEmpty())
  {
    driverManager mgr;
    driverInstance inst = this->ui->deviceTable->selectedItems()[0]->data(Qt::UserRole).value<driverInstance>();
    
    mgr.removeInstance(inst);
  }
}

void summaDeviceConfig::saveDevice()
{
  if(this->isCreatingNew)
  {
    driverManager mgr;
    driverProvider provider = this->ui->deviceTypeCombo->itemData(this->ui->deviceTypeCombo->currentIndex()).value<driverProvider>();
    driverInstance inst(provider, this->ui->devicePathEdit->text());
    
    cout << "provider: " << provider.name().toStdString() << endl;
    
    mgr.addInstance(inst);
    
    QTableWidgetItem* providerItem = new QTableWidgetItem(provider.name());
    QTableWidgetItem* pathItem = new QTableWidgetItem(inst.path());
    
    providerItem->setData(Qt::UserRole, QVariant::fromValue<driverInstance>(inst));
    
    this->ui->deviceTable->insertRow(this->ui->deviceTable->rowCount());
    this->ui->deviceTable->setItem(this->ui->deviceTable->rowCount()-1, 0, providerItem);
    this->ui->deviceTable->setItem(this->ui->deviceTable->rowCount()-1, 1, pathItem);
  } else {
    int row = this->ui->deviceTable->currentRow();
    driverInstance inst = this->ui->deviceTable->item(row, 0)->data(Qt::UserRole).value<driverInstance>();
    driverManager mgr;
    driverProvider provider = this->ui->deviceTypeCombo->itemData(this->ui->deviceTypeCombo->currentIndex()).value<driverProvider>();
    
    mgr.removeInstance(inst);
    inst = driverInstance(provider, this->ui->devicePathEdit->text());
    mgr.addInstance(inst);
    
    this->ui->deviceTable->item(row, 0)->setText(provider.name());
    this->ui->deviceTable->item(row, 1)->setText(inst.path());
  }
  
  this->ui->deviceTypeCombo->setEnabled(false);
  this->ui->devicePathEdit->setEnabled(false);
  this->ui->devicePathEdit->setText("");
  this->ui->deviceSaveButton->setEnabled(false);
}

void summaDeviceConfig::deviceClicked(int row, int col)
{
  this->ui->deviceTypeCombo->setEnabled(true);
  this->ui->devicePathEdit->setEnabled(true);
  this->ui->deviceSaveButton->setEnabled(true);
  
  this->ui->deviceTypeCombo->setCurrentIndex(this->ui->deviceTypeCombo->findText(this->ui->deviceTable->item(row, 0)->text()));
  this->ui->devicePathEdit->setText(this->ui->deviceTable->item(row, 1)->text());
}
