#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPortInfo>
#include <QCameraInfo>
#include <QMessageBox>
#include <QTimer>
#include <QStandardItemModel>
#include "App.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    App app;
    QStandardItemModel *itemModel;
    QTimer timer;

public:
    void SearchDevice();
    void ConnectCamera();
    void ConnectSerial();
    void SetMachineState();
    void ModifyBoard();
    void ClearBoard();
    void START();
    void InitTableList();
    void UpdateTableList();
    void LockUI();
    void UnlockUI();
};
#endif // MAINWINDOW_H
