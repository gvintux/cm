#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //
    this->setWindowIcon(QIcon(":/null.png"));
    ui->setupUi(this);
    loadSets();
    buildGUI();
    makeConnections();
    connectDB();
    updateFieldsCombo();
    updateEditor();
    updatePrefEmployeDialog();
    updatePrefDepartmentDialog();
    showAllRecords();
    updateStatistics();
    timer.start(3000);
}

MainWindow::~MainWindow()
{
    saveSets();
    timer.stop();
    delete ui;
}

QByteArray MainWindow::uploadPictrue(QString path)
{
    QByteArray pic;
    QFile f(path);
    if(f.exists())
    {
        f.open(QFile::ReadOnly);
        if(f.isOpen())
        {
           pic = qCompress(f.readAll(), cmpRatio).toHex();
           f.close();
        }

    }
    return pic;
}

void MainWindow::toLog(int type, QString msg)
{
    QString time = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");
    QIcon *label = NULL;
    QString text = time + " #" + msg;
    QListWidgetItem *lwi = new  QListWidgetItem();
    switch(type)
    {
        case LOG_OK:
        label = new QIcon(QPixmap(":pic/log_ok.png"));
        label->setThemeName("INF");
        lwi->setTextColor(Qt::darkGreen);
        if(ui->tbLog->text()!="E" && ui->tbLog->text()!="W")
        {
            ui->tbLog->setText("O");
            ui->tbLog->setIcon(QIcon(QPixmap(":pic/log_s_ok.png")));
        }
        break;

        case LOG_WARN:
        label = new QIcon(QPixmap(":pic/log_warn.png"));
        label->setThemeName("WRN");
        lwi->setTextColor(Qt::darkYellow);
        if(ui->tbLog->text()!="E")
        {
            ui->tbLog->setText("W");
            ui->tbLog->setIcon(QIcon(QPixmap(":pic/log_s_warn.png")));
        }
        break;

        case LOG_ERR:
        label = new QIcon(QPixmap(":pic/log_error.png"));
        label->setThemeName("ERR");
        lwi->setTextColor(Qt::darkRed);
        ui->tbLog->setText("E");
        ui->tbLog->setIcon(QIcon(QPixmap(":pic/log_s_error.png")));
        break;
    }

    lwi->setText(text);
    lwi->setIcon(*label);
    lwLogList->addItem(lwi);
}

void MainWindow::buildGUI()
{
    rbCartridges = ui->rbSearchCartridges;
    rbRequests = ui->rbSearchRequests;
    cbSearchIn = ui->cbSearchIn;
    swEditor = ui->swRecordInfo;
    tbNewRecord = ui->tbAddRecord;
    tbEditRecord = ui->tbEditRecord;
    tbDeleteRecord = ui->tbDeleteRecord;
    leCartridgeID = ui->leCartridgeID;
    cbCartridgeModel = ui->cbCartridgeModel;
    cbCartridgeVendor = ui->cbCartridgeVendor;
    leCartridgeCycle = ui->leCartridgeCycle;
    cbCartridgeStatus = ui->cbCartridgeStatus;
    pbSearch = ui->pbSearch;
    leSearch = ui->leSearch;
    twTable = ui->twResult;
    twTable->setEditTriggers(twTable->NoEditTriggers);
    loadDialogs();
    statusBar()->hide();
    this->setStyleSheet("QMenuBar{background-color: rgba(0,0,0,200); color: rgb(255,255,255);}");
    ui->gbEditor->setStyleSheet("QGroupBox{background-color: rgba(5,69,104,200); border-radius: 10px;}");
    ui->gbResult->setStyleSheet("QTableWidget{background-color: rgba(225,230,105,200); border-radius: 10px;}"
                                "QGroupBox{background-color: rgba(225,230,105,200); border-radius: 10px; padding-top: 20px;}");
    ui->gbSearch->setStyleSheet("QGroupBox{background-color: rgba(100,200,140,200); border-radius: 10px; padding-top: 20px;}");
    ui->gbStatistics->setStyleSheet("QGroupBox{background-color: rgba(225,175,180,200); border-radius: 10px; padding-top: 20px;}");
}

void MainWindow::makeConnections()
{
    connect(rbCartridges, SIGNAL(clicked()), this, SLOT(updateFieldsCombo()));
    connect(rbRequests, SIGNAL(clicked()), this, SLOT(updateFieldsCombo()));
    //connect(rbRequests, SIGNAL(clicked()), this, SLOT(updateEditor()));
    //connect(rbCartridges, SIGNAL(clicked()), this, SLOT(updateEditor()));
    connect(tbNewRecord, SIGNAL(clicked()), this, SLOT(newRecord()));
    connect(tbEditRecord, SIGNAL(clicked()), this, SLOT(editRecord()));
    connect(tbDeleteRecord, SIGNAL(clicked()), this, SLOT(deleteRecord()));
    connect(cbCartridgeModel, SIGNAL(currentIndexChanged(int)), this, SLOT(updateEditorSelected(int)));
    connect(pbSearch, SIGNAL(clicked()), this, SLOT(doSearch()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateStatistics()));
    connect(twTable, SIGNAL(cellClicked(int,int)), this, SLOT(updateEditorFields(int,int)));
    connect(ui->tbLog, SIGNAL(clicked()), logDialog, SLOT(show()));
    connect(ui->tbClearForm, SIGNAL(clicked()), this, SLOT(clearForm()));
    linkDialogs();
    linkMenu();


}

void MainWindow::connectDB()
{
    database = QSqlDatabase::addDatabase("QMYSQL");
    database.setDatabaseName(db);
    database.setHostName(server);
    database.setPort(port.toInt());
    database.setUserName(user);
    database.setPassword(password);
    if(database.open()) toLog(LOG_OK, tr("Соединение с БД установлено"));
    else toLog(LOG_ERR, tr("Соединение с БД не установлено: ") + database.lastError().text());
}

void MainWindow::updateFieldsCombo()
{
    cbSearchIn->clear();
    if(rbCartridges->isChecked())
    {
        QStringList cartridgeFields;
        cartridgeFields << tr("Номер")  << tr("Модель")  << tr("Производитель")  << tr("Цикл")  << tr("Статус")  << tr("Совместимость");
        cbSearchIn->addItems(cartridgeFields);
        swEditor->setCurrentIndex(1);
        updateEditorSelected(0);
        ui->gbEditor->setTitle(tr("Редактор картриджей"));
    }
    else
    {
        QStringList requestFields;
        requestFields << tr("Номер")  << tr("Время")  << tr("Автор")  << tr("Подразделение")  << tr("Картридж")  << tr("Исполнитель");
        cbSearchIn->addItems(requestFields);
        swEditor->setCurrentIndex(0);
        QDateTimeEdit* dteTime = this->findChild<QDateTimeEdit*>("dtRequestDate");
        dteTime->setDateTime(QDateTime::currentDateTime());
        ui->leRequestEnvoy->setFocus();
        ui->gbEditor->setTitle(tr("Редактор заявок"));
    }
    showAllRecords();
}

void MainWindow::updatePrefEmployeDialog()
{
    QSqlQuery q(database);
    QComboBox *cbEmployes = prefEmployeDialog->findChild<QComboBox*>("cbField");
    cbEmployes->clear();
    QComboBox* cbRequestExecutor = this->findChild<QComboBox*>("cbRequestExecutor");
    cbRequestExecutor->clear();
    if(q.exec("SELECT * FROM employe ORDER BY employe_name"))
    {
        toLog(LOG_OK, tr("Выборка списка сотрудников успешна"));
        while(q.next())
        {
            cbEmployes->addItem(q.value(1).toString(), q.value(0));
            cbRequestExecutor->addItem(q.value(1).toString(), q.value(0));
        }
    }
    else toLog(LOG_ERR, tr("Не удалось запросить список сотрудников: ") + q.lastError().text());


}

void MainWindow::updateVendorDialog()
{
    QSqlQuery q(database);
    QComboBox *cbVendors = vendorDialog->findChild<QComboBox*>("cbField");
    cbVendors->clear();
    if(q.exec("SELECT * FROM vendor ORDER BY vendor_name"))
    {
        toLog(LOG_OK, tr("Выборка списка производителей картриджей успешна"));
        while(q.next()) cbVendors->addItem(q.value(1).toString(), q.value(0));
    }
    else toLog(LOG_ERR, tr("Не удалось запросить список производителей картриджей: ") + q.lastError().text());

}

void MainWindow::updateModelDialog()
{
    QSqlQuery q(database);
    QComboBox* cbVendor = modelDialog->findChild<QComboBox*>("cbVendor");
    cbVendor->clear();
    if(q.exec("SELECT * FROM vendor ORDER BY vendor_name"))
    {
        while(q.next()) cbVendor->addItem(q.value(1).toString(), q.value(0));
        toLog(LOG_OK, tr("Выборка списка моделей картриджей успешна"));
    }
    else toLog(LOG_ERR, tr("Не удалось запросить список моделей картриджей: ") + q.lastError().text());

    QComboBox* cbModel = modelDialog->findChild<QComboBox*>("cbModel");
    cbModel->clear();
    if(q.exec("SELECT * FROM model ORDER BY model_name"))
    {
        while(q.next()) cbModel->addItem(q.value(1).toString(), q.value(0));
        toLog(LOG_OK, tr("Выборка списка моделей картриджей успешна"));
    }
    else toLog(LOG_ERR, tr("Не удалось запросить список моделей картриджей: ") + q.lastError().text());

    if(cbModel->currentText().isEmpty())
    {
        QLabel* lbImage = modelDialog->findChild<QLabel*>("lbImage");
        lbImage->setPixmap(QPixmap(":/null.png").scaled(512, 512, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void MainWindow::updatePrefDepartmentDialog()
{
    QComboBox* prefDepartmentDialog_cbDepartment = prefDepartmentDialog->findChild<QComboBox*>("cbField");
    QSqlQuery q(database);
    prefDepartmentDialog_cbDepartment->clear();
    QComboBox* cbRequestDepartment = this->findChild<QComboBox*>("cbRequestDepartment");
    cbRequestDepartment->clear();
    if(q.exec("SELECT * FROM department ORDER BY department_name"))
    {
        while (q.next())
        {
            prefDepartmentDialog_cbDepartment->addItem(q.value(1).toString(), q.value(0));
            cbRequestDepartment->addItem(q.value(1).toString(), q.value(0));
        }
        prefDepartmentDialog_cbDepartment->setCurrentIndex(0);
        toLog(LOG_OK, tr("Выборка списка подразделений успешна"));
    }
    else toLog(LOG_ERR, tr("Не удалось запросить список подразделений: ") + q.lastError().text());;
}

void MainWindow::updateModelDialogSelected(int current)
{
    QComboBox* cbModel = modelDialog->findChild<QComboBox*>("cbModel");
    QLabel* lbImage = modelDialog->findChild<QLabel*>("lbImage");
    QLineEdit* leImage = modelDialog->findChild<QLineEdit*>("leImage");
    QLineEdit* leCompat = modelDialog->findChild<QLineEdit*>("leCompatibility");
    QComboBox* cbVendor = modelDialog->findChild<QComboBox*>("cbVendor");

    QString modelID = cbModel->itemData(current).toString();
    QSqlQuery q(database);

    if(q.exec(QString("SELECT * FROM model WHERE model_id=%1").arg(modelID)))
    {
        q.first();
        QString vendor = q.value(2).toString();
        QString compatibility = q.value(3).toString();
        QByteArray picData = qUncompress(QByteArray::fromHex(q.value(4).toByteArray()));
        QPixmap pic;
        if(!picData.isEmpty())
        {
            pic.loadFromData(picData);

        }
        else pic.load(":/null.png");
        pic = pic.scaled(512, 512, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        cbVendor->setCurrentIndex(cbVendor->findData(vendor));
        leCompat->setText(compatibility);
        leImage->setText("");
        lbImage->setPixmap(pic);
        toLog(LOG_OK, tr("Выборка информации о модели картриджа успешна"));
    }
    else toLog(LOG_ERR, tr("Не удалось запросить информацию о модели картриджа: ") + q.lastError().text());
}

void MainWindow::updateEditorSelected(int current)
{
    QSqlQuery q(database);
    QString modelID = cbCartridgeModel->currentData().toString();
    if(q.exec(QString("SELECT model_vendor from model WHERE model_id = %1").arg(modelID)))
    {
        q.first();
        QString vendorID = q.value(0).toString();
        cbCartridgeVendor->setCurrentIndex(cbCartridgeVendor->findData(vendorID));
        toLog(LOG_OK, tr("Выборка информации о производителе для модели картриджа успешна"));
    }
    else toLog(LOG_ERR, tr("Не удалось запросить информацию о производителе для модели картриджа: ") + q.lastError().text());
}

void MainWindow::updateEditor()
{
    QSqlQuery q(database);
    cbCartridgeModel->clear();
    cbCartridgeVendor->clear();
    cbCartridgeStatus->clear();
    if(q.exec(QString("SELECT * FROM model ORDER BY model_name")))
    {
        while(q.next()) cbCartridgeModel->addItem(q.value(1).toString(), q.value(0));
        toLog(LOG_OK, tr("Выборка списка моделей картриджей успешна"));
    }
    else toLog(LOG_ERR, tr("Не удалось запросить список моделей картриджей: ") + q.lastError().text());

    if(q.exec(QString("SELECT * FROM vendor ORDER BY vendor_name")))
    {
        while(q.next()) cbCartridgeVendor->addItem(q.value(1).toString(), q.value(0));
        toLog(LOG_OK, tr("Выборка списка производителей картриджей успешна"));
    }
    else toLog(LOG_ERR, tr("Не удалось запросить список производителей картриджей: ") + q.lastError().text());

    if(q.exec(QString("SELECT * FROM status ORDER BY status_name")))
    {
        while(q.next()) cbCartridgeStatus->addItem(q.value(1).toString(), q.value(0));
        cbCartridgeStatus->setCurrentIndex(cbCartridgeStatus->findText(tr("На складе")));
        toLog(LOG_OK, tr("Выборка списка состояний картриджей успешна"));
    }
    else toLog(LOG_ERR, tr("Не удалось запросить список состояний картриджей: ") + q.lastError().text());
//    cb
}

void MainWindow::setFilePath()
{
    QLineEdit* leImage = modelDialog->findChild<QLineEdit*>("leImage");
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::ExistingFile);
    dlg.setNameFilter(tr("Изображения (*.png *.jpg)"));
    dlg.setViewMode(QFileDialog::Detail);
    if(dlg.exec()) leImage->setText(dlg.selectedFiles().at(0));
    QPixmap img(dlg.selectedFiles().at(0));
    img = img.scaled(512, 512, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QLabel* lbImage = modelDialog->findChild<QLabel*>("lbImage");
    lbImage->setPixmap(img);
}
void MainWindow::loadDialogs()
{
    QUiLoader loader(this);
    QFile f(this);
    f.setFileName(":/prefDatabaseDialog.ui");
    f.open(QFile::ReadOnly);
    prefDatabaseDialog = (QDialog*)loader.load(&f);
    f.close();

    f.setFileName(":/tableEditor.ui");
    f.open(QFile::ReadOnly);
    prefEmployeDialog = (QDialog*)loader.load(&f);
    f.close();

    f.setFileName(":/tableEditor.ui");
    f.open(QFile::ReadOnly);
    prefDepartmentDialog = (QDialog*)loader.load(&f);
    f.close();

    prefDepartmentDialog->setWindowTitle(tr("Настройки - подразделения"));
    prefDepartmentDialog->setObjectName("prefDepartment");


    prefEmployeDialog->setWindowTitle(tr("Настройки - сотрудники"));
    prefEmployeDialog->setObjectName("prefEmploye");

    f.setFileName(":/tableEditor.ui");
    f.open(QFile::ReadOnly);
    vendorDialog = (QDialog*)loader.load(&f);
    f.close();

    f.setFileName(":/logDialog.ui");
    f.open(QFile::ReadOnly);
    logDialog = (QDialog*)loader.load(&f);
    f.close();

    lwLogList = logDialog->findChild<QListWidget*>("logList");

    vendorDialog->setWindowTitle(tr("Картриджи - производитель"));
    vendorDialog->setObjectName("cartridgeVendor");

    f.setFileName(":/modelDialog.ui");
    f.open(QFile::ReadOnly);
    modelDialog = (QDialog*)loader.load(&f);
    QLabel* lbImage = modelDialog->findChild<QLabel*>("lbImage");
    lbImage->setAlignment(Qt::AlignCenter);
    f.close();

}

void MainWindow::loadSets()
{
    QSettings s;
    server = s.value("server", "localhost").toString();
    port = s.value("port", "3306").toString();
    db = s.value("db", "cartridge_management").toString();
    user = s.value("user", "root").toString();
    password = s.value("password", "rhoer2Fa").toString();
    cmpRatio = s.value("compression", 0).toInt();

}

void MainWindow::saveSets()
{
    QLineEdit *param = prefDatabaseDialog->findChild<QLineEdit*>("leServer");
    if(param->text().length() > 0) server = param->text().trimmed();
    param  = prefDatabaseDialog->findChild<QLineEdit*>("lePort");
    if(param->text().length() > 0) port = param->text().trimmed();
    param  = prefDatabaseDialog->findChild<QLineEdit*>("leDatabase");
    if(param->text().length() > 0) db = param->text().trimmed();
    param  = prefDatabaseDialog->findChild<QLineEdit*>("leUser");
    if(param->text().length() > 0) user = param->text().trimmed();
    param  = prefDatabaseDialog->findChild<QLineEdit*>("lePassword");
    if(param->text().length() > 0) password = param->text().trimmed();
    QSpinBox *spBox  = prefDatabaseDialog->findChild<QSpinBox*>("sbCmpRatio");
    cmpRatio = spBox->value();

    QSettings s;
    s.setValue("server", server);
    s.setValue("port", port);
    s.setValue("db", db);
    s.setValue("user", user);
    s.setValue("password", password);
    s.setValue("compression", prefDatabaseDialog->findChild<QSpinBox*>("sbCmpRatio")->value() - 1);
}

void MainWindow::linkMenu()
{
    QAction* act = this->findChild<QAction*>("prefDatabase");
    connect(act, SIGNAL(triggered()), this, SLOT(showPrefDatabaseDialog()));
    act = this->findChild<QAction*>("prefEmploye");
    connect(act, SIGNAL(triggered()), this, SLOT(showPrefEmployeDialog()));

    act = this->findChild<QAction*>("prefDepartment");
    connect(act, SIGNAL(triggered()), this, SLOT(showPrefDepartmentDialog()));

    act = this->findChild<QAction*>("cartridgeVendorAction");
    connect(act, SIGNAL(triggered()), this, SLOT(showVendorDialog()));

    act = this->findChild<QAction*>("cartridgeModelAction");
    connect(act, SIGNAL(triggered()), this, SLOT(showModelDialog()));
}

void MainWindow::showPrefDatabaseDialog()
{
    QLineEdit* leServer = prefDatabaseDialog->findChild<QLineEdit*>("leServer");
    leServer->setText(server);
    QLineEdit* lePort = prefDatabaseDialog->findChild<QLineEdit*>("lePort");
    lePort->setText(port);
    QLineEdit* leDatabase = prefDatabaseDialog->findChild<QLineEdit*>("leDatabase");
    leDatabase->setText(db);
    QLineEdit* leUser = prefDatabaseDialog->findChild<QLineEdit*>("leUser");
    leUser->setText(user);
    QLineEdit* lePassword = prefDatabaseDialog->findChild<QLineEdit*>("lePassword");
    lePassword->setText(password);
    QSpinBox* sbCmpRatio = prefDatabaseDialog->findChild<QSpinBox*>("sbCmpRatio");
    sbCmpRatio->setValue(cmpRatio + 1);
    prefDatabaseDialog->show();

}

void MainWindow::showVendorDialog()
{
    updateVendorDialog();
    vendorDialog->show();
}

void MainWindow::showPrefEmployeDialog()
{
    updatePrefEmployeDialog();
    prefEmployeDialog->show();
}

void MainWindow::showPrefDepartmentDialog()
{
    updatePrefDepartmentDialog();
    prefDepartmentDialog->show();
}

void MainWindow::updateStatistics()
{
    QSqlQuery q(database);
    QString inUse, onWare, onFill, broken, total, requests;
    if(q.exec("SELECT COUNT(*) FROM cartridge WHERE cartridge_status=1"))
    {
        q.first();
        inUse = q.value(0).toString();
    }
    else toLog(LOG_ERR, tr("Не удалось запросить количество выданных картриджей: ") + q.lastError().text());

    if(q.exec("SELECT COUNT(*) FROM cartridge WHERE cartridge_status=2"))
    {
        q.first();
        onWare = q.value(0).toString();
    }
    else toLog(LOG_ERR, tr("Не удалось запросить количество картриджей на складе: ") + q.lastError().text());

    if(q.exec("SELECT COUNT(*) FROM cartridge WHERE cartridge_status=3"))
    {
        q.first();
        onFill = q.value(0).toString();
    }
    else toLog(LOG_ERR, tr("Не удалось запросить количество картриджей на заправке: ") + q.lastError().text());

    if(q.exec("SELECT COUNT(*) FROM cartridge WHERE cartridge_status=4"))
    {
        q.first();
        broken = q.value(0).toString();
    }
    else toLog(LOG_ERR, tr("Не удалось запросить количество неисправных картриджей: ") + q.lastError().text());

    if(q.exec("SELECT COUNT(*) FROM cartridge"))
    {
        q.first();
        total = q.value(0).toString();
    }
    else toLog(LOG_ERR, tr("Не удалось запросить общее количество картриджей: ") + q.lastError().text());

    if(q.exec("SELECT COUNT(*) FROM request"))
    {
        q.first();
        requests = q.value(0).toString();
    }
    else toLog(LOG_ERR, tr("Не удалось запросить общее количество заявок: ") + q.lastError().text());

    ui->lblUseValue->setText(inUse);
    ui->lblWireValue->setText(onWare);
    ui->lblServiceValue->setText(onFill);
    ui->lblBrokenValue->setText(broken);
    ui->lblTotalValue->setText(total);
    ui->lblMiddleValue->setText(requests);
    ui->dtRequestDate->setDateTime(QDateTime::currentDateTime());
}

void MainWindow::linkDialogs()
{
    QPushButton* prefDatabaseDialog_pbApply = prefDatabaseDialog->findChild<QPushButton*>("pbApply");
    connect(prefDatabaseDialog_pbApply, SIGNAL(clicked()), this, SLOT(saveSets()));

    QToolButton* prefEmployeDialog_tbNew = prefEmployeDialog->findChild<QToolButton*>("tbNew");
    connect(prefEmployeDialog_tbNew, SIGNAL(clicked()), this, SLOT(newEmploye()));

    QToolButton* prefEmployeDialog_tbEdit = prefEmployeDialog->findChild<QToolButton*>("tbEdit");
    connect(prefEmployeDialog_tbEdit, SIGNAL(clicked()), this, SLOT(editEmploye()));

    QToolButton* prefEmployeDialog_tbDelete = prefEmployeDialog->findChild<QToolButton*>("tbDelete");
    connect(prefEmployeDialog_tbDelete, SIGNAL(clicked()), this, SLOT(deleteEmploye()));

    QToolButton* vendorDialog_tbNew = vendorDialog->findChild<QToolButton*>("tbNew");
    connect(vendorDialog_tbNew, SIGNAL(clicked()), this, SLOT(newVendor()));

    QToolButton* vendorDialog_tbEdit = vendorDialog->findChild<QToolButton*>("tbEdit");
    connect(vendorDialog_tbEdit, SIGNAL(clicked()), this, SLOT(editVendor()));

    QToolButton* vendorDialog_tbDelete = vendorDialog->findChild<QToolButton*>("tbDelete");
    connect(vendorDialog_tbDelete, SIGNAL(clicked()), this, SLOT(deleteVendor()));

    QComboBox* modelDialog_cbModel = modelDialog->findChild<QComboBox*>("cbModel");
    connect(modelDialog_cbModel, SIGNAL(currentIndexChanged(int)), this, SLOT(updateModelDialogSelected(int)));

    QToolButton* modelDialog_tbNew = modelDialog->findChild<QToolButton*>("tbNew");
    connect(modelDialog_tbNew, SIGNAL(clicked()), this, SLOT(newModel()));

    QToolButton* modelDialog_tbEdit = modelDialog->findChild<QToolButton*>("tbEdit");
    connect(modelDialog_tbEdit, SIGNAL(clicked()), this, SLOT(editModel()));

    QToolButton* modelDialog_tbDelete = modelDialog->findChild<QToolButton*>("tbDelete");
    connect(modelDialog_tbDelete, SIGNAL(clicked()), this, SLOT(deleteModel()));

    QToolButton* modelDialog_tbImageLoad = modelDialog->findChild<QToolButton*>("tbImageLoad");
    connect(modelDialog_tbImageLoad, SIGNAL(clicked()), this, SLOT(setFilePath()));

    QToolButton* prefDepartmentDialog_tbNew = prefDepartmentDialog->findChild<QToolButton*>("tbNew");
    connect(prefDepartmentDialog_tbNew, SIGNAL(clicked()), this, SLOT(newDepartment()));

    QToolButton* prefDepartmentDialog_tbEdit = prefDepartmentDialog->findChild<QToolButton*>("tbEdit");
    connect(prefDepartmentDialog_tbEdit, SIGNAL(clicked()), this, SLOT(editDepartment()));

    QToolButton* prefDepartmentDialog_tbDelete = prefDepartmentDialog->findChild<QToolButton*>("tbDelete");
    connect(prefDepartmentDialog_tbDelete, SIGNAL(clicked()), this, SLOT(deleteDepartment()));

    QToolButton* logDialog_tbExport = logDialog->findChild<QToolButton*>("tbExport");
    connect(logDialog_tbExport, SIGNAL(clicked()), this, SLOT(exportLog()));

    QToolButton* logDialog_tbClear = logDialog->findChild<QToolButton*>("tbClear");
    connect(logDialog_tbClear, SIGNAL(clicked()), this, SLOT(clearLog()));
}

void MainWindow::newEmploye()
{
    QSqlQuery q(database);
    QComboBox *cbEmployes = prefEmployeDialog->findChild<QComboBox*>("cbField");
    QString newEmployeName = cbEmployes->currentText().trimmed();
    if(q.exec(QString("INSERT INTO employe (employe_name) VALUE('%1')").arg(newEmployeName)))
    {
        toLog(LOG_OK, tr("Добавление нового сотрудника успешно"));
        updatePrefEmployeDialog();

    }
    else toLog(LOG_ERR, tr("Не удалось добавить нового сотрудника: ") + q.lastError().text());
}

void MainWindow::editEmploye()
{
    QSqlQuery q(database);
    QComboBox *cbEmployes = prefEmployeDialog->findChild<QComboBox*>("cbField");
    QString employeID = cbEmployes->currentData().toString();
    QString newEmployeName = cbEmployes->currentText().trimmed();
    if(q.exec(QString("UPDATE employe SET employe_name = '%1' WHERE employe_id = %2").arg(newEmployeName).arg(employeID)))
    {
        toLog(LOG_OK, tr("Редактирование сотрудника успешно"));
        updatePrefEmployeDialog();

    }
    else toLog(LOG_ERR, tr("Не удалось отредактировать сотрудника: ") + q.lastError().text());
}

void MainWindow::deleteEmploye()
{
    QSqlQuery q(database);
    QComboBox *cbEmployes = prefEmployeDialog->findChild<QComboBox*>("cbField");
    QString employeID = cbEmployes->currentData().toString();
    if(q.exec(QString("DELETE FROM employe WHERE employe_id = %1").arg(employeID)))
    {
        toLog(LOG_OK, tr("Удаление сотрудника успешно"));
        updatePrefEmployeDialog();

    }
    else toLog(LOG_ERR, tr("Не удалось удалить сотрудника: ") + q.lastError().text());
}

void MainWindow::newDepartment()
{
    QSqlQuery q(database);
    QComboBox *cbDepartments = prefDepartmentDialog->findChild<QComboBox*>("cbField");
    QString newDepartment = cbDepartments->currentText().trimmed();
    if(q.exec(QString("INSERT INTO department (department_name) VALUE('%1')").arg(newDepartment)))
    {
        toLog(LOG_OK, tr("Добавление нового подразделения успешно"));
        updatePrefDepartmentDialog();

    }
    else toLog(LOG_ERR, tr("Не удалось добавить новое подразделение: ") + q.lastError().text());
}

void MainWindow::editDepartment()
{
    QSqlQuery q(database);
    QComboBox *cbDepartments = prefDepartmentDialog->findChild<QComboBox*>("cbField");
    QString departmentID = cbDepartments->currentData().toString();
    QString newDepartment = cbDepartments->currentText().trimmed();
    if(q.exec(QString("UPDATE department SET department_name = '%1' WHERE department_id = %2").arg(newDepartment).arg(departmentID)))
    {
        toLog(LOG_OK, tr("Редактирование подразделения успешно"));
        updatePrefDepartmentDialog();

    }
    else toLog(LOG_ERR, tr("Не удалось отредактировать подразделение: ") + q.lastError().text());
}

void MainWindow::deleteDepartment()
{
    QSqlQuery q(database);
    QComboBox *cbDepartments= prefDepartmentDialog->findChild<QComboBox*>("cbField");
    QString departmentID = cbDepartments->currentData().toString();
    if(q.exec(QString("DELETE FROM department WHERE department_id = %1").arg(departmentID)))
    {
        toLog(LOG_OK, tr("Удаление подразделения успешно"));
        updatePrefDepartmentDialog();

    }
    else toLog(LOG_ERR, tr("Не удалось удалить подразделение: ") + q.lastError().text());
}

void MainWindow::showAllRecords()
{
    ui->leSearch->setText("");
    ui->pbSearch->click();
    doSearch();
}

void MainWindow::newVendor()
{
    QSqlQuery q(database);
    QComboBox *cbVendors= vendorDialog->findChild<QComboBox*>("cbField");
    QString newVendorName = cbVendors->currentText().trimmed();
    if(q.exec(QString("INSERT INTO vendor (vendor_name) VALUE('%1')").arg(newVendorName)))
    {
        toLog(LOG_OK, tr("Новый производитель добавлен успешно"));
        updateVendorDialog();
        updateEditor();
    }
    else toLog(LOG_ERR, tr("Не удалось добавить нового производителя: ") + q.lastError().text());
}

void MainWindow::editVendor()
{
    QSqlQuery q(database);
    QComboBox *cbVendors = vendorDialog->findChild<QComboBox*>("cbField");
    QString vendorID = cbVendors->currentData().toString();
    QString newVendorName = cbVendors->currentText().trimmed();
    if(q.exec(QString("UPDATE vendor SET vendor_name = '%1' WHERE vendor_id = %2").arg(newVendorName).arg(vendorID)))
    {
        toLog(LOG_OK, tr("Редактирование производителя успешно"));
        updateVendorDialog();

    }
    else toLog(LOG_ERR, tr("Не удалось отредактировать производителя: ") + q.lastError().text());

}

void MainWindow::deleteVendor()
{
    QSqlQuery q(database);
    QComboBox *cbVendors = vendorDialog->findChild<QComboBox*>("cbField");
    QString vendorID = cbVendors->currentData().toString();
    QString newVendorName = cbVendors->currentText().trimmed();
    QStringList hLabels;
    if(q.exec(QString("DELETE FROM vendor WHERE vendor_id = %1").arg(vendorID)))
    {
        toLog(LOG_OK, tr("Удаление производителя успешно"));
        updateVendorDialog();

    }
    else toLog(LOG_ERR, tr("Не удалось удалить производителя: ") + q.lastError().text());
}

void MainWindow::doSearch()
{
    int targetField = cbSearchIn->currentIndex();
    QString query = "";
    QString filter = "";
    QString orderBy = "";
    QString searchText = leSearch->text().trimmed();
    QSqlQuery q(database);
    QStringList hLabels;

    if(rbCartridges->isChecked())
    {
        query = "SELECT cartridge_id, model_name, vendor_name, cartridge_cycle, status_name, model_compatibility FROM cartridge, model, vendor, status WHERE cartridge_model = model_id AND vendor_id = model_vendor AND cartridge_status = status_id";
        switch(targetField)
        {
            case 0:
                filter = " AND (cartridge_id LIKE '%" + searchText + "%')";
            break;

            case 1:
                filter = " AND (model_name LIKE '%" + searchText + "%')";
            break;

            case 2:
                filter = " AND (vendor_name LIKE '%" + searchText + "%')";
            break;

            case 3:
                filter = " AND (cartridge_cycle LIKE '%" + searchText + "%')";
            break;

            case 4:
                filter = " AND (status_name LIKE '%" + searchText + "%')";
            break;

            case 5:
                filter = " AND (model_compatibility LIKE '%" + searchText + "%')";
            break;
        }
        orderBy =  " ORDER BY cartridge_id";

        hLabels  << tr("Номер")  << tr("Модель")  << tr("Производитель")  << tr("Цикл")  << tr("Статус")  << tr("Совместимость");

    }
    else
    {
        query = "SELECT request_id, request_date, request_envoy, department_name, request_cartridge, "
                "employe_name FROM request, department, employe WHERE request_executor = employe_id AND request_department = department_id";
        switch(targetField)
        {
            case 0:
                filter = " AND (request_id LIKE '%" + searchText + "%')";
            break;

            case 1:
                filter = " AND (request_date LIKE '%" + searchText + "%')";
            break;

            case 2:
                filter = " AND (request_envoy LIKE '%" + searchText + "%')";
            break;

            case 3:
                filter = " AND (department_name LIKE '%" + searchText + "%')";
            break;

            case 4:
                filter = " AND (request_cartridge LIKE '%" + searchText + "%')";
            break;

            case 5:
                filter = " AND (employe_name LIKE '%" + searchText + "%')";
            break;
        }
        orderBy = " ORDER BY request_date";
        hLabels << tr("Номер")  << tr("Дата")  << tr("Автор")  << tr("Подразделение")  << tr("Картридж")  << tr("Исполнитель");
    }
    twTable->clear();
    if(q.exec(query + filter + orderBy))
    {
        int row = 0;
        int cols = q.record().count();
        twTable->setRowCount(q.size());
        twTable->setColumnCount(cols);

        while(q.next())
        {
            for(int i = 0; i < cols; ++i)
            {
                QString value = q.value(i).toString();
                if(i == 1 && rbRequests->isChecked()) value = QDateTime::fromString(value, "yyyy-MM-ddThh:mm:ss").toString("dd.MM.yy hh:mm");
                QTableWidgetItem* twi = new QTableWidgetItem(value);
                twi->setTextAlignment(Qt::AlignCenter);
                twTable->setItem(row, i, twi);
            }
            ++row;
        }
        twTable->setHorizontalHeaderLabels(hLabels);
        twTable->resizeColumnsToContents();
        toLog(LOG_OK, tr("Поиск выполнен успешно"));
    }
    else toLog(LOG_ERR, tr("Поиск не выполнен: ") + q.lastError().text());


}

void MainWindow::newModel()
{
    QComboBox* modelDialog_cbModel = modelDialog->findChild<QComboBox*>("cbModel");
    QComboBox* modelDialog_cbVendor = modelDialog->findChild<QComboBox*>("cbVendor");
    QLineEdit* modelDialog_leImage = modelDialog->findChild<QLineEdit*>("leImage");
    QLineEdit* modelDialog_leCompatibility = modelDialog->findChild<QLineEdit*>("leCompatibility");
    QString newModelName = modelDialog_cbModel->currentText().trimmed();
    QString vendorID = modelDialog_cbVendor->currentData().toString();
    QString vendorName = modelDialog_cbVendor->currentText().trimmed();
    QString modelCompatibility = modelDialog_leCompatibility->text().trimmed();
    QString path = modelDialog_leImage->text().trimmed();
    QSqlQuery q(database);
    QString query = "INSERT INTO model (model_name, model_vendor, model_compatibility, model_pic) VALUES ('%1', %2, '%3', '";

    QByteArray pic = uploadPictrue(path);

    query = query.arg(newModelName).arg(vendorID).arg(modelCompatibility) + pic + "')";
    if(q.exec(query))
    {
        toLog(LOG_OK, tr("Новая модель картриджа добавлена успешно"));
    }
    else toLog(LOG_ERR, tr("Не удалось добавить новую модель картриджа: ") + q.lastError().text());
    updateModelDialog();

}

void MainWindow::editModel()
{
    QComboBox* cbModel = modelDialog->findChild<QComboBox*>("cbModel");
    QComboBox* cbVendor = modelDialog->findChild<QComboBox*>("cbVendor");
    QLineEdit* leImage = modelDialog->findChild<QLineEdit*>("leImage");
    QLineEdit* leCompatibility = modelDialog->findChild<QLineEdit*>("leCompatibility");
    QString modelCompatibility = leCompatibility->text().trimmed();
    QString filePath = leImage->text().trimmed();
    QString modelID = cbModel->currentData().toString();
    QString modelName = cbModel->currentText().trimmed();
    QString vendorID = cbVendor->currentData().toString().trimmed();
    QString vendorName = cbVendor->currentText().trimmed();
    QSqlQuery q(database);
    QByteArray pic = uploadPictrue(filePath);
    QString query = "UPDATE model SET model_name = '%1', model_vendor=%2, model_compatibility = '%3', model_pic = '" + pic + "' WHERE model_id=%4";
    query = query.arg(modelName).arg(vendorID).arg(modelCompatibility).arg(modelID);
    if(q.exec(query))
    {
        toLog(LOG_OK, tr("Редактирование модели картриджа успешно"));
    }
    else toLog(LOG_ERR, tr("Не удалоь отредактировать модель картриджа"));
    updateModelDialog();
}

void MainWindow::deleteModel()
{
    QComboBox* cbModel = modelDialog->findChild<QComboBox*>("cbModel");
    QString modelID = cbModel->currentData().toString();
    QSqlQuery q(database);
    if(q.exec(QString("DELETE FROM model WHERE model_id=%1").arg(modelID)))
    toLog(LOG_OK, tr("Модель картриджа удалена успешно"));
    else toLog(LOG_ERR, tr("Не удалось удалить модель картриджа"));
    updateModelDialog();
}

void MainWindow::showModelDialog()
{
    updateModelDialog();
    modelDialog->show();

}

void MainWindow::clearLog()
{
    lwLogList->clear();
    ui->tbLog->setIcon(QIcon(QPixmap(":pic/log_s_ok.png")));
    ui->tbLog->setText("O");
}

void MainWindow::exportLog()
{
    QFileDialog dlg(this, tr("Экспорт журнала"));
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    dlg.setConfirmOverwrite(true);
    if(dlg.exec())
    {
        QString filePath = dlg.selectedFiles().at(0);
        QFile f(filePath);
            if(f.open(QFile::WriteOnly))
            {
                QTextStream out(&f);
                int count = lwLogList->count();
                for(int i = 0; i < count; ++i)
                        out << lwLogList->item(i)->icon().themeName()<< ": " << lwLogList->item(i)->text() << endl;
            f.close();
            toLog(LOG_OK, tr("Экспорт журнала выполнен успешно"));
            }
            else toLog(LOG_WARN, tr("Не удалось экспортировать журнал"));

    }
}

void MainWindow::updateEditorFields(int row, int col)
{
    int colCount = twTable->columnCount();
    if(rbCartridges->isChecked())
    {
        QString cartridgeID, cartridgeModel, cartridgeVendor, cartridgeCycle, cartridgeStatus, cartridgeCompatibility;
        cartridgeID = twTable->model()->index(row, 0).data().toString();
        cartridgeModel = twTable->model()->index(row, 1).data().toString();
        cartridgeVendor = twTable->model()->index(row, 2).data().toString();
        cartridgeCycle = twTable->model()->index(row, 3).data().toString();
        cartridgeStatus = twTable->model()->index(row, 4).data().toString();
        cartridgeCompatibility = twTable->model()->index(row, 5).data().toString();

        ui->leCartridgeID->setText(cartridgeID);
        ui->cbCartridgeModel->setCurrentIndex(ui->cbCartridgeModel->findText(cartridgeModel));
        ui->cbCartridgeVendor->setCurrentIndex(ui->cbCartridgeVendor->findText(cartridgeVendor));
        ui->leCartridgeCycle->setText(cartridgeCycle);
        ui->cbCartridgeStatus->setCurrentIndex(ui->cbCartridgeStatus->findText(cartridgeStatus));
    }
    else
    {
        QString requestID, requestDateTime, requestAuthor, requestDepartment, requestCartridge, requestExecutor;
        requestID = twTable->model()->index(row, 0).data().toString();
        requestDateTime = twTable->model()->index(row, 1).data().toString();
        requestAuthor = twTable->model()->index(row, 2).data().toString();
        requestDepartment = twTable->model()->index(row, 3).data().toString();
        requestCartridge = twTable->model()->index(row, 4).data().toString();
        requestExecutor = twTable->model()->index(row, 5).data().toString();

        ui->leRequestID->setText(requestID);
        ui->dtRequestDate->setDateTime(QDateTime::fromString(requestDateTime, "dd.MM.yy hh:mm"));
        ui->leRequestEnvoy->setText(requestAuthor);
        ui->cbRequestDepartment->setCurrentIndex(ui->cbRequestDepartment->findText(requestDepartment));
        ui->leRequestCartridge->setText(requestCartridge);
        ui->cbRequestExecutor->setCurrentIndex(ui->cbRequestExecutor->findText(requestExecutor));
    }
}

void MainWindow::newRecord()
{
    QSqlQuery q(database);
    QString lastID;
    QString query;
    if(rbCartridges->isChecked())
    {
        QString modelId = ui->cbCartridgeModel->currentData().toString();
        QString cartridgeCycle = ui->leCartridgeCycle->text().trimmed();
        if(cartridgeCycle.isEmpty()) cartridgeCycle = QString::number(0);
        QString statusID = ui->cbCartridgeStatus->currentData().toString();
        query = "INSERT INTO cartridge(cartridge_model, cartridge_cycle, cartridge_status) VALUES(%1, %2, %3)";
        query = query.arg(modelId).arg(cartridgeCycle).arg(statusID);
        if(q.exec(query))
        {
            if(q.exec("SELECT LAST_INSERT_ID()"))
            {
                q.first();
                lastID = q.value(0).toString();
                QString msg = tr("Картридж № %1 добавлен.\nНе забудьте наклеить наклейку");
                msg = msg.arg(lastID);
                QMessageBox::information(this, tr("Новый картридж"), msg, QMessageBox::Ok);
                toLog(LOG_OK, tr("Новый картридж добавлен успешно"));
            }
        }
        toLog(LOG_ERR, tr("Не удалось добавить новый картридж: ") + q.lastError().text());
    }
    else
    {
        QString requestDateTime = ui->dtRequestDate->dateTime().toString("yyyy-MM-dd hh:mm");
        QString requestAuthor = ui->leRequestEnvoy->text().trimmed();
        QString requestDepartmentID = ui->cbRequestDepartment->currentData().toString();
        QString requestCartridgeID = ui->leRequestCartridge->text().trimmed();
        QString requestExecutorID = ui->cbRequestExecutor->currentData().toString();
        QString query = "INSERT INTO request (request_date, request_envoy, request_department, request_cartridge, request_executor) "
                "VALUES ('%1', '%2', %3, %4, %5)";
        if(requestAuthor.length() > 0)
        {
        query = query.arg(requestDateTime).arg(requestAuthor).arg(requestDepartmentID).arg(requestCartridgeID).arg(requestExecutorID);
        if(q.exec(QString("SELECT COUNT(*) FROM cartridge WHERE cartridge_id = %1 AND cartridge_status = 2").arg(requestCartridgeID)))
        {
            q.first();
            if(q.value(0).toInt() != 0)
            {
                if(q.exec(query))
                {
                    if(q.exec(QString("UPDATE cartridge SET cartridge_status = 1 WHERE cartridge_id = %1").arg(requestCartridgeID)))
                    {

                        toLog(LOG_OK, tr("Новая заявка добавлена успешно"));
                        QMessageBox::information(this, tr("Новая заявка"), tr("Новая заявка оформлена.\nСпасибо за пользование сервисом!"), QMessageBox::Ok, QMessageBox::NoButton);


                    }
                    else toLog(LOG_WARN, tr("Новая заявка добавлена, но не удалось изменить статус картриджа: ") + q.lastError().text());
                }
                else
                {
                    toLog(LOG_ERR, tr("Не удалось добавить новую заявку: ") + q.lastError().text());
                    QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось добавить новую заявку.\nПодробности в журнале."), QMessageBox::Ok, QMessageBox::NoButton);
                }
            }
            else
            {
                toLog(LOG_WARN, tr("Не удалось добавить новую заявку: такого картриджа нет на складе"));
                QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось добавить новую заявку.\nПодробности в журнале."), QMessageBox::Ok, QMessageBox::NoButton);
            }

        }
        else toLog(LOG_ERR, tr("Не удалось добавить заявку, т.к. не удалось запросить наличие картриджа на складе: ") + q.lastError().text());
        }
        else toLog(LOG_ERR, tr("Не удалось добавить заявку: не указан автор заявки"));
        ui->leRequestEnvoy->setFocus();
    }
    showAllRecords();
}

void MainWindow::editRecord()
{
    QSqlQuery q(database);
    QString query = "";
    if(rbCartridges->isChecked())
    {
        QString cartridgeID = ui->leCartridgeID->text();
        QString modelId = ui->cbCartridgeModel->currentData().toString();
        QString cartridgeCycle = ui->leCartridgeCycle->text().trimmed();
        QString statusID = ui->cbCartridgeStatus->currentData().toString();
        if(!cartridgeID.isEmpty())
        {
        query = "UPDATE cartridge SET cartridge_model=%1, cartridge_cycle = %2, cartridge_status=%3 WHERE cartridge_id=%4";
        query = query.arg(modelId, cartridgeCycle, statusID, cartridgeID);
        if(q.exec(query))
        {
            toLog(LOG_OK, tr("Редактирование картриджа успешно"));
        }
        else toLog(LOG_ERR, tr("Не удалось отредактировать картридж: ") + q.lastError().text());
        }
        else toLog(LOG_ERR, tr("Не удалось несуществующий картридж"));
    }
    else
    {
        QString requestID = ui->leRequestID->text();
        QString requestDateTime = ui->dtRequestDate->text();
        QString requestAuthor = ui->leRequestEnvoy->text().trimmed();
        QString requestDepartmentID = ui->cbRequestDepartment->currentData().toString();
        QString requestCartridgeID = ui->leRequestCartridge->text().trimmed();
        QString requestExecutorID = ui->cbRequestExecutor->currentData().toString();
        if(!requestID.isEmpty())
        {
            query = "UPDATE request SET request_date = '%1', request_envoy = '%2', request_department = %3, request_cartridge = %4, request_executor = '%5' WHERE request_id = %6";
            if(q.exec(query.arg(requestDateTime).arg(requestAuthor).arg(requestDepartmentID).arg(requestCartridgeID).arg(requestExecutorID).arg(requestID)))
            {
                toLog(LOG_OK, tr("Редактирование заявки успешно"));

            }
            else toLog(LOG_ERR, tr("Не удалось отредактировать заявку: ") + q.lastError().text());
        }
        else toLog(LOG_ERR, tr("Не удалось несуществующую заявку"));

    }
    showAllRecords();
}

void MainWindow::deleteRecord()
{
    QSqlQuery q(database);
    if(rbCartridges->isChecked())
    {
        QString cartridgeID = ui->leCartridgeID->text();
        if(!cartridgeID.isEmpty())
        {
        if(q.exec("DELETE FROM cartridge WHERE cartridge_id=" + cartridgeID))
        {
            toLog(LOG_OK, tr("Удаление картриджа успешно"));
        }
        else toLog(LOG_ERR, tr("Не удалось удалить картридж: ") + q.lastError().text());
        }
        else toLog(LOG_ERR, tr("Не удалось удалить несуществующий картридж"));

    }
    else
    {
        QString requestID = ui->leRequestID->text();
        if(!requestID.isEmpty())
        {
        if(q.exec("DELETE FROM request WHERE request_id=" + requestID))
        {
            toLog(LOG_OK, tr("Удаление заявки успешно"));
        }
        else toLog(LOG_ERR, tr("Не удалось удалить заявку: ") + q.lastError().text());
        }
        else toLog(LOG_ERR, tr("Не удалось удалить несуществующую заявку"));

    }
    showAllRecords();
}

void MainWindow::clearForm()
{
    if(rbCartridges->isChecked())
    {
        ui->leCartridgeID->clear();
        ui->cbCartridgeModel->setCurrentIndex(0);
        updateEditorSelected(0);
        leCartridgeCycle->clear();
        cbCartridgeStatus->setCurrentIndex(0);
    }
    else
    {
        ui->leRequestID->clear();
        ui->leRequestEnvoy->clear();
        ui->leRequestCartridge->clear();
        ui->cbRequestDepartment->setCurrentIndex(0);
        ui->cbRequestExecutor->setCurrentIndex(0);
    }
}
