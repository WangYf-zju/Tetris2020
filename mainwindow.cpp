#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    InitTableList();
    connect(&timer, &QTimer::timeout, this, [&]() { UpdateTableList(); });
    connect(&app, &App::updateBoard, this, [&]() { ui->WBoard->UpdateBoard(); });
    connect(ui->PBRefresh, &QPushButton::clicked, this, [&]() { SearchDevice(); });
    connect(ui->PBSerial, &QPushButton::clicked, this, [&]() { ConnectSerial(); });
    connect(ui->PBCamera, &QPushButton::clicked, this, [&]() { ConnectCamera(); });
    connect(ui->PBSet, &QPushButton::clicked, this, [&]() { SetMachineState(); });
    connect(ui->PBStart, &QPushButton::clicked, this, [&]() { START(); });
    connect(ui->PBModify, &QPushButton::clicked, this, [&]() { ModifyBoard(); });
    connect(ui->PBClear, &QPushButton::clicked, this, [&]() { ClearBoard(); });
    SearchDevice(); 
}

MainWindow::~MainWindow()
{
    app.v.CloseCamera();
    delete ui;
}


void MainWindow::SearchDevice()
{
    QList<QSerialPortInfo> slist = QSerialPortInfo::availablePorts();
    QListIterator<QSerialPortInfo> si(slist);
    QList<QCameraInfo> clist = QCameraInfo::availableCameras();
    QListIterator<QCameraInfo> ci(clist);
    ui->CBSerial->clear();
    while (si.hasNext())
    {
        ui->CBSerial->addItem(si.next().portName());
    }
    ui->CBCamera->clear();
    while (ci.hasNext())
    {
        ui->CBCamera->addItem(ci.next().description());
    }
}


void MainWindow::ConnectCamera()
{
    if (!app.v.IsCameraOpen())
    {
        if (ui->CBCamera->currentIndex() < 0)
        {
            QMessageBox::warning(this, "Error", "No camera is selected!", "OK");
            return;
        }
        char index[] = { '0' + ui->CBCamera->currentIndex(), 0 };
        if (app.v.OpenCamera(index, ui->LBVideo))
        {
            ui->PBCamera->setText("Disconnect");
            timer.start(1000);
        }
        else
        {
            QMessageBox::warning(this, "Error", "Fail to open camera", "OK");
        }
    }
    else
    {
        timer.stop();
        app.v.CloseCamera();
        ui->PBCamera->setText("Connect");
    }
}


void MainWindow::ConnectSerial()
{
    if (!app.mc.IsSerialOpen())
    {
        if (ui->CBSerial->currentIndex() < 0)
        {
            QMessageBox::warning(this, "Error", "No serial port is selected!", "OK");
            return;
        }
        QString name = ui->CBSerial->currentText();
        if (app.mc.OpenSerial(name))
        {
            ui->PBSerial->setText("Disconnect");
        }
        else
        {
            QMessageBox::warning(this, "Error", "Fail to open serial prot", "OK");
        }
    }
    else
    {
        app.mc.CloseSerial();
        ui->PBSerial->setText("Connect");
    }
}

void MainWindow::SetMachineState()
{
    int16_t x = ui->SBX->value();
    int16_t y = ui->SBY->value();
    int16_t z = ui->SBZ->value();
    int16_t angle = ui->SBAngle->value();
    uint8_t speed = ui->SBSpeed->value();
    uint8_t valve = 0;
    if (ui->CBValve->isChecked()) valve = 1;
    app.mc.SetState(x, y, z, angle, speed, valve);
}

void MainWindow::ModifyBoard()
{
    ui->WBoard->ReverseModify();
    if (ui->WBoard->IsModifyBoard())
    {
        ui->PBModify->setText("OK");
    }
    else
    {
        ui->PBModify->setText("Modify");
    }
}

void MainWindow::ClearBoard()
{
    if (QMessageBox::warning(this, "Clear", "Clear board?",
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        TetrisAI::ClearBoard();
        ui->WBoard->update();
    }
}

void MainWindow::START()
{
    if (!app.IsAppStart())
    {
        if (!app.v.IsCameraOpen() || !app.mc.IsSerialOpen())
        {
            QMessageBox::warning(this, "Error", 
                "Camera or Serial Port has not been connected!", "OK");
            return;
        }
        app.StartApp();
        ui->PBStart->setText("STOP");
    }
    else
    {
        app.StopApp();
        ui->PBStart->setText("START");
        ui->PBSerial->setText("Connect");
    }

}

void MainWindow::InitTableList()
{
    itemModel = new QStandardItemModel;
    ui->TVList->setModel(itemModel);
    itemModel->setHorizontalHeaderItem(0, new QStandardItem("Type"));
    itemModel->setHorizontalHeaderItem(1, new QStandardItem("X"));
    itemModel->setHorizontalHeaderItem(2, new QStandardItem("Y"));
    itemModel->setHorizontalHeaderItem(3, new QStandardItem("Angle"));
    itemModel->setHorizontalHeaderItem(4, new QStandardItem("Time"));
}

void MainWindow::UpdateTableList()
{
    app.v.lock.lock();
    app.v.lock.unlock();
    for (int i = itemModel->rowCount() - 1; i >= 0; i--)
        itemModel->removeRow(i);
    for (int i = 0; i < app.v.viewList.length(); i++)
    {
        int type = app.v.viewList[i].type;
        int x = app.v.viewList[i].x;
        int y = app.v.viewList[i].y;
        int angle = app.v.viewList[i].angle;
        QString time = app.v.viewList[i].t.toString("hh:mm:ss.zzz");
        itemModel->setItem(i, 0, new QStandardItem(QString::number(type)));
        itemModel->setItem(i, 1, new QStandardItem(QString::number(x)));
        itemModel->setItem(i, 2, new QStandardItem(QString::number(y)));
        itemModel->setItem(i, 3, new QStandardItem(QString::number(angle)));
        itemModel->setItem(i, 4, new QStandardItem(time));
    }
}
