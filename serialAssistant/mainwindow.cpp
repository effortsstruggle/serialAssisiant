#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QString>
#include <QFileDialog>
#include <QByteArray>
#include <QMessageBox>
#include <QByteArray>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //初始化ui状态
    this->EnableUi(false);
    //初始化ui
    this->initUI();
    //初始化串口助手
    this->initSerial();
}
/**
 * @brief MainWindow::initUI 初始化组件
 */
void MainWindow::initUI()
{    
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    //默认设置ID为0
    ui->devID->setText("0");
    QValidator* validator =new QIntValidator(-100,10000,this);
    ui->devID->setValidator(validator);

    //进度条初始化为0
    ui->progressBar->setValue(0);

    ui->editFrequency->setText("0");
    QValidator* validator1 =new QIntValidator(0,1000000000,this);
    ui->editFrequency->setValidator(validator1);

    //分组单选框
    this->btnGroup = new QButtonGroup();
    this->btnGroup->addButton(ui->rbtnDSP,RBTN::isDSP);
    this->btnGroup->addButton(ui->rbtnFPGA,RBTN::isFPGA);
    //FPGA单选框默认
    ui->rbtnFPGA->setChecked(true);

    //初始化当前串口的信息，添加到comBox中
    foreach (const QSerialPortInfo &qspinfo, QSerialPortInfo::availablePorts())
    {
        ui->cmbSerial->addItem(qspinfo.portName());
    }

    //初始化波特率 BaudRate
    ui->cmbRate->addItem(QString("%1").arg(QSerialPort::Baud1200));
    ui->cmbRate->addItem(QString("%1").arg(QSerialPort::Baud2400));
    ui->cmbRate->addItem(QString("%1").arg(QSerialPort::Baud4800));
    ui->cmbRate->addItem(QString("%1").arg(QSerialPort::Baud9600));
    ui->cmbRate->addItem(QString("%1").arg(QSerialPort::Baud19200));
    ui->cmbRate->addItem(QString("%1").arg(QSerialPort::Baud38400));
    ui->cmbRate->addItem(QString("%1").arg(QSerialPort::Baud57600));
    ui->cmbRate->addItem(QString("%1").arg(QSerialPort::Baud115200));
    //波特率默认选中115200
    ui->cmbRate->setCurrentIndex(7);
    //初始化数据位
    ui->cmbDataBits->addItem(QString("%1").arg(QSerialPort::Data5));
    ui->cmbDataBits->addItem(QString("%1").arg(QSerialPort::Data6));
    ui->cmbDataBits->addItem(QString("%1").arg(QSerialPort::Data7));
    ui->cmbDataBits->addItem(QString("%1").arg(QSerialPort::Data8));
    //数据位默认选中8bit
    ui->cmbDataBits->setCurrentIndex(3);

    this->dfSelected = RBTN::isFPGA;

    connect(ui->rbtnDSP,SIGNAL(clicked(bool)),
                this,SLOT(slots_DSP_FPGA()));
    connect(ui->rbtnFPGA,SIGNAL(clicked(bool)),
            this,SLOT(slots_DSP_FPGA()));

    connect(ui->devID,&QLineEdit::editingFinished,this,&MainWindow::slot_devID_editFin);
}
/**
 * @brief MainWindow::initSerial 初始化串口
 */
void MainWindow::initSerial()
{
    //创建串口助手（子线程）
    this->serial =new SerialInterface();

    //信号槽
    connect(this->serial,&SerialInterface::curPercent,this,&MainWindow::slot_progressBar_curPercent);
    connect(this->serial,&SerialInterface::isDone,this,&MainWindow::slots_sendFileDone);
    connect(this->serial,&SerialInterface::readyRead,this,&MainWindow::slot_serial_readyRead);
}
/**
 * @brief MainWindow::ShowEdit 显示文本输入框(以行方式添加）
 * @param buffer
 */
void MainWindow::ShowEdit(QString msg)
{
    QString buffer = ui->editRecv->toPlainText();
    if(buffer!="")
       buffer += "\n";
    buffer += msg;
    //清空以前显示
    ui->editRecv->clear();
    //重新显示
    ui->editRecv->append(buffer);
}
/**
 * @brief MainWindow::EnableUi 窗口禁用
 * @param stat
 */
void MainWindow::EnableUi(bool status)
{
    if(status == true) //已经打开
    {
        ui->cmbSerial->setEnabled(false);
        ui->cmbRate->setEnabled(false);
        ui->editFrequency->setEnabled(false);
        ui->cmbDataBits->setEnabled(false);
        ui->cmbCheckSum->setEnabled(false);
        ui->cmbStopBit->setEnabled(false);
        ui->devID->setEnabled(false);
        ui->rbtnDSP->setEnabled(false);
        ui->rbtnFPGA->setEnabled(false);
        ui->btnStart->setEnabled(true);
        ui->btnEnd->setEnabled(true);
        ui->btnSelectFile->setEnabled(true);
        ui->btnSend->setEnabled(true);
    }
    else
    {
        ui->cmbSerial->setEnabled(true);
        ui->cmbRate->setEnabled(true);
        ui->editFrequency->setEnabled(true);
        ui->cmbDataBits->setEnabled(true);
        ui->cmbCheckSum->setEnabled(true);
        ui->cmbStopBit->setEnabled(true);
        ui->devID->setEnabled(true);
        ui->rbtnDSP->setEnabled(true);
        ui->rbtnFPGA->setEnabled(true);
        ui->btnStart->setEnabled(false);
        ui->btnEnd->setEnabled(false);
        ui->btnSelectFile->setEnabled(false);
        ui->btnSend->setEnabled(false);
    }
}

MainWindow::~MainWindow()
{
    //关闭串口
    this->serial->serialClose();

    //停止线程
    if(this->serial->isRunning())
    {
        this->serial->stop();
    }

    //等待结束
    this->serial->wait();

    //释放内存
    if(this->serial)
        this->serial->deleteLater();
    delete ui;
}
/**
 * @brief MainWindow::slots_DSP_FPGA 单选框槽
 */
void MainWindow::slots_DSP_FPGA()
{
    switch(this->btnGroup->checkedId())
       {
       case RBTN::isDSP:
//           qDebug() <<"isDSP"<<endl;
           break;
       case RBTN::isFPGA:
//           qDebug() <<"isFPGA"<<endl;
           break;
       default:
       break;
        }

}
/**
 * @brief MainWindow::on_btnSelectFile_clicked 选择文件路径
 */
void MainWindow::on_btnSelectFile_clicked()
{ //选择单个文件
    QString curPath=QDir::currentPath();//获取系统当前目录
    //获取应用程序的路径
    QString dlgTitle="选择bin文件"; //对话框标题
    QString filter="文本文件(*.bin);;所有文件(*.*)"; //文件过滤器
    QString aFileName=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);
    if (!aFileName.isEmpty())
        ui->editPath->setText(aFileName);

}
/**
 * @brief MainWindow::on_btnOpen_clicked 打开助手
 */
void MainWindow::on_btnOpen_clicked()
{
    if(ui->btnOpen->text() == tr("打开串口"))
    {

        //1获取当前选择的串口
        QString serialName=ui->cmbSerial->currentText();
        if(serialName == "")
        {
            QMessageBox::question(this,"ERROR","Local Serial don`t selected!");
            return;
        }
        this->serial->setPortName(serialName);
        //2获取当前波特率
        this->serial->setBaudRate(static_cast<QSerialPort::BaudRate>(ui->cmbRate->currentText().toLong()));

        //3设置数据位
        this->serial->setDataBits(static_cast<QSerialPort::DataBits>(ui->cmbDataBits->currentText().toInt()));

        //4设置奇偶校验位
        this->serial->setParity(static_cast<QSerialPort::Parity>(ui->cmbCheckSum->currentText().toInt()));

        //5设置停止位
        this->serial->setStopBits(static_cast<QSerialPort::StopBits>(ui->cmbStopBit->currentText().toInt()));

        //设置流控制 ？
        this->serial->setFlowControl(QSerialPort::NoFlowControl);

        //打开串口
        bool openStatus =  this->serial->serialOpen(QIODevice::ReadWrite);
        if(false==openStatus)
        {
            QMessageBox::question(this,tr("ERROR"),tr("打开口串口助手失败"));
            return ;
        }
        else //打开成功
        {
            this->serial->reset();

            ui->btnOpen->setText(tr("关闭串口"));

            this->EnableUi(true);

            return ;
        }        
    }
    else
    {
        //关闭正在运行的线程
        this->serial->stop();
        ui->btnOpen->setText("打开串口");
        this->EnableUi(false);

        //关闭串口
        this->serial->serialClose();
    }
}
/**
 * @brief MainWindow::on_serial_readyRead 串口数据槽
 */
void MainWindow::slot_serial_readyRead(QByteArray buffer)
{
    //从界面中读取以前收到的数据
    this->ShowEdit(QString(buffer));
}
/**
 * @brief MainWindow::on_btnSend_clicked 发送文件
 */
void MainWindow::on_btnSend_clicked()
{
    //获取发送文件的路径
    QString path = ui->editPath->text();
    QFileInfo  info(path);
    if(!info.exists() || !info.isFile())
    {
        QMessageBox::about(this,tr("错误"),tr("文件打开失败"));
        return;
    }
    //设置进度条状态
    ui->progressBar->setValue(0);

    this->serial->setPath(ui->editPath->text());
    this->serial->setId(ui->devID->text().toInt());
    this->serial->setFrequency(2);

    this->serial->start();

    //禁用发送按钮
     ui->btnSend->setEnabled(false);
}
/**
 * @brief MainWindow::slot_progressBar_curPercent 显示当前进度条状态
 * @param cur
 */
void MainWindow::slot_progressBar_curPercent(int cur)
{
    ui->progressBar->setValue(cur);
}
/**
 * @brief MainWindow::on_btnStart_clicked 启动更新---发送更新状态
 */
void MainWindow::on_btnStart_clicked()
{
    /*获取ID*/
    quint32 id =static_cast<quint32>(ui->devID->text().toUInt());


    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    qint32 devId = 0xF400;
    devId |=static_cast<quint8>(id);
    out << static_cast<short>(0x5525)<<static_cast<short>(0x0002)<<static_cast<short>(devId)<<static_cast<short>(0xF400);

    this->serial->write(block);

    /*更新UI*/
    //从界面中读取以前收到的数据
    this->ShowEdit(QString(tr("启动更新，设备ID：%1")).arg(static_cast<quint32>(id)));
}
/**
 * @brief MainWindow::on_btnEnd_clicked 完成更新---发送完成更新状态（等待回复信息）
 */
void MainWindow::on_btnEnd_clicked()
{
    /*获取ID*/
    quint32 id =static_cast<quint32>(ui->devID->text().toUInt());

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    qint32 devId = 0xF400;
    devId |=static_cast<quint8>(id);
    out << static_cast<short>(0x5525)<<static_cast<short>(0x0002)<<static_cast<short>(devId)<<static_cast<short>(0xF4F4);

    this->serial->write(block);

    /*更新UI*/
    //从界面中读取以前收到的数据
    this->ShowEdit(QString(tr("更新完成，设备ID：%1")).arg(static_cast<quint32>(id)));
}
/**
 * @brief MainWindow::slot_devID_editFin 修改设备ID完成时
 */
void MainWindow::slot_devID_editFin()
{
    qint32 devID = ui->devID->text().toInt();
    if(devID <0 || devID > 255)
    {
        QMessageBox::about(this,tr("错误"),tr("设备ID范围0~255"));
    }
}
/**
 * @brief MainWindow::slots_sendFileDone 发送文件完成
 */
void MainWindow::slots_sendFileDone(bool done)
{
    ui->btnSend->setEnabled(true);
    //文件发送完成，显示状态
    if(done)
        this->ShowEdit(tr("发送文件完成"));
    else
        this->ShowEdit(tr("发送文件失败"));
}














