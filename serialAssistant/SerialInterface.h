#ifndef SENDFILE_H
#define SENDFILE_H

#include <QThread>
#include <QSerialPort>        //提供访问串口的功能
#include <QSerialPortInfo>    //提供系统中存在的串口的信息
#include <QByteArray>

class SerialInterface : public QThread
{
    Q_OBJECT
public:
    explicit SerialInterface(QObject *parent = nullptr);
    void setPath(QString Path);
    void setId(qint32);
    void setFrequency(qint64 f);
    ~SerialInterface();
    void stop();
    void reset();
    void serialClose();
    bool serialOpen(QIODevice::OpenModeFlag);//打开串口
    qint64 write(QByteArray);
    void setPortName(QString);
    void setBaudRate(QSerialPort::BaudRate);
    void setDataBits(QSerialPort::DataBits);
    void setParity(QSerialPort::Parity);
    void setStopBits(QSerialPort::StopBits);
    void setFlowControl( QSerialPort::FlowControl);
signals:
    void isDone(bool);//处理完成信号
    void curPercent(int percent);//已经发送文件
    void readyRead(QByteArray);
protected:
    void run();

public slots:
    void on_serial_readyRead();
private:
    //串口对象
    QString serialName;//串口名
    QSerialPort::BaudRate rate;//波特率
    QSerialPort::DataBits bits;//数据位
    QSerialPort::Parity parity;//
    QSerialPort::StopBits stopBits;//停止位
    QSerialPort::FlowControl flowControl;//控制位

    QSerialPort* serial;
    QString path;
    qint32 devID;
    bool flag=true;
    qint64 frequency=0;
};

#endif // SENDFILE_H
