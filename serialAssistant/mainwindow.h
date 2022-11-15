#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serialInterface.h"

#include <QMainWindow>
#include <QButtonGroup>
#include <QSerialPort>        //提供访问串口的功能
#include <QSerialPortInfo>    //提供系统中存在的串口的信息
namespace Ui {
class MainWindow;
}

enum RBTN
{
    isDSP = 0,  //DSP单选按钮
    isFPGA = 1, //FPGA单选按钮
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    //初始化UI组件
    void initUI();
    //初始化串口，
    void initSerial();
    //显示文本输入框
    void ShowEdit(QString buffer);
    //窗口禁用
    void EnableUi(bool);
    ~MainWindow();

protected slots:
    //单选按钮组槽方法
    void slots_DSP_FPGA();
    //选择bin文件路径
    void on_btnSelectFile_clicked();
    //打开串口
    void on_btnOpen_clicked();
    //接收串口数据
    void slot_serial_readyRead(QByteArray);
    //发送文件数据
    void on_btnSend_clicked();
    //显示当前进度条状态
    void slot_progressBar_curPercent(int cur);
    //启动更新
    void on_btnStart_clicked();
    //完成更新
    void on_btnEnd_clicked();
    //点击设备ID编辑完成时
    void slot_devID_editFin();
    //发送文件完成
    void slots_sendFileDone(bool done);
private:
    Ui::MainWindow *ui;
    SerialInterface *serial;
    //单选框按钮
    QButtonGroup* btnGroup;
//    //串口对象
//    QSerialPort* serial;
    RBTN dfSelected;
};

#endif // MAINWINDOW_H
