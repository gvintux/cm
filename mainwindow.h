#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QMenuBar>
#include <QRadioButton>
#include <QComboBox>
#include <QSettings>
#include <QFile>
#include <QUiLoader>
#include <QAction>
#include <QPushButton>
#include <QStatusBar>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QStackedWidget>
#include <QPixmap>

#include <QDebug>

#include <QFileDialog>
#include <QToolButton>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QtSql/QSqlRecord>
#include <QTimer>
#include <QSpinBox>
#include <QVariant>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>

const int               LOG_OK      = 0;
const int               LOG_WARN    = 1;
const int               LOG_ERR     = 2;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

    int cmpRatio;
    QString                 db;
    QString                 server;
    QString                 port;
    QString                 user;
    QString                 password;
    Ui::MainWindow*         ui;
    QRadioButton*           rbRequests;
    QRadioButton*           rbCartridges;
    QComboBox*              cbSearchIn;
    QStackedWidget*         swEditor;
    QSqlDatabase            database;

    QTimer                  timer;

    QListWidget*            lwLogList;

    QLineEdit*              leCartridgeID;
    QComboBox*              cbCartridgeModel;
    QComboBox*              cbCartridgeVendor;
    QLineEdit*              leCartridgeCycle;
    QComboBox*              cbCartridgeStatus;
    QLineEdit*              leCartridgeCompatibility;
    QLineEdit*              leSearch;

    QPushButton*            pbSearch;

    QPushButton*            tbNewRecord;
    QPushButton*            tbEditRecord;
    QPushButton*            tbDeleteRecord;

    QTableWidget*           twTable;

    QDialog*                prefDatabaseDialog;
    QDialog*                prefEmployeDialog;
    QDialog*                vendorDialog;
    QDialog*                modelDialog;
    QDialog*                prefDepartmentDialog;
    QDialog*                logDialog;
    QDialog*                changeCartridgeDialog;

    QByteArray              uploadPictrue(QString path);

    void                    toLog(int type, QString msg);


private slots:
    void buildGUI();
    void clearForm();
    void clearLog();
    void connectDB();
    void deleteDepartment();
    void deleteEmploye();
    void deleteModel();
    void deleteRecord();
    void deleteVendor();
    void doSearch();
    void editDepartment();
    void editEmploye();
    void editModel();
    void editRecord();
    void editVendor();
    void exportLog();
    void linkDialogs();
    void linkMenu();
    void loadDialogs();
    void loadSets();
    void makeConnections();
    void newDepartment();
    void newEmploye();
    void newModel();
    void newRecord();
    void recycle();
    void newVendor();
    void saveSets();
    void setFilePath();
    void showAllRecords();
    void showModelDialog();
    void showPrefDatabaseDialog();
    void showPrefDepartmentDialog();
    void showPrefEmployeDialog();
    void showVendorDialog();
    void updateEditor();
    void updateEditorFields(int, int);
    void updateEditorSelected(int);
    void updateFieldsCombo();
    void updateModelDialog();
    void updateChangeDialog();
    void updateModelDialogSelected(int);
    void updatePrefDepartmentDialog();
    void updatePrefEmployeDialog();
    void updateStatistics();
    void updateVendorDialog();
    void changeCartridge();

};

#endif // MAINWINDOW_H
