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
#include <QThread>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
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
#include <mylineedit.h>


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

    QToolButton*            tbNewRecord;
    QToolButton*            tbEditRecord;
    QToolButton*            tbDeleteRecord;

    QTableWidget*           twTable;

    QDialog*                prefDatabaseDialog;
    QDialog*                prefEmployeDialog;
    QDialog*                vendorDialog;
    QDialog*                modelDialog;
    QDialog*                prefDepartmentDialog;
    QDialog*                logDialog;

    QByteArray              uploadPictrue(QString path);

    void                    toLog(int type, QString msg);


private slots:
    void buildGUI();
    void makeConnections();
    void connectDB();
    void updateFieldsCombo();
    void updatePrefEmployeDialog();
    void updateVendorDialog();
    void updateModelDialog();
    void updatePrefDepartmentDialog();
    void updateModelDialogSelected(int);
    void updateEditorSelected(int);
    void updateEditor();
    void setFilePath();
    void loadDialogs();
    void loadSets();
    void saveSets();
    void linkMenu();
    void showPrefDatabaseDialog();
    void showVendorDialog();
    void showPrefEmployeDialog();
    void showPrefDepartmentDialog();
    void updateStatistics();

    void linkDialogs();
    void newEmploye();
    void editEmploye();
    void deleteEmploye();

    void newDepartment();
    void editDepartment();
    void deleteDepartment();
    void showAllRecords();
    void newVendor();
    void editVendor();
    void deleteVendor();
    void doSearch();

    void newModel();
    void editModel();
    void deleteModel();

    void showModelDialog();

    void clearLog();
    void exportLog();

    void updateEditorFields(int, int);

    void newRecord();
    void editRecord();
    void deleteRecord();
};

#endif // MAINWINDOW_H
