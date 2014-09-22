/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  HAL <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "summacontrolpanel.h"
#include "ui_summacontrolpanel.h"
#include "summadeviceconfig.h"
#include "drivermanager.h"
#include <iostream>

using namespace std;

summaControlPanel::summaControlPanel(QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f)
{
  ui = new Ui::summaControlPanel;
  ui->setupUi(this);
  
  driverManager mgr;
  auto instances = mgr.getInstances();
  
  // Populate the drivers list
  for(auto instance : instances)
  {
    ui->deviceCombo->addItem(instance.provider().name() + " @ " + instance.path(), QVariant::fromValue<driverInstance>(instance));
  }
  
  ui->deviceCombo->setCurrentIndex(0);
}

summaControlPanel::~summaControlPanel()
{
  delete ui;
}

void summaControlPanel::deviceChanged(int dev)
{
  cout << "TODO: deviceChanged(" << dev << ")\n";
}

void summaControlPanel::deviceConfig()
{
  cout << "TODO: deviceConfig\n";
  summaDeviceConfig cfg(this);

  cfg.show();

  if(cfg.exec() == QDialog::Accepted)
  {
    
  }
}
