#include "serialInterface.h"

#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QDebug>
#include <QDataStream>

SerialInterface::SerialInterface(QObject *parent) : QThread(parent)
{
    this->serial = new QSerialPort();
    QObject::connect(this->serial,&QSerialPort::readyRead,this,&SerialInterface::on_serial_readyRead);
}
/**
 * @brief SerialInterface::setPath 设置路径
 * @param Path
 */
void SerialInterface::setPath(QString path)
{
    this->path = path;
}
/**
 * @brief SerialInterface::setId 设置ID
 * @param id
 */
void SerialInterface::setId(qint32 id )
{
    this->devID = id;
}

void SerialInterface::setFrequency(qint64 f)
{
    this->frequency = f;
}

SerialInterface::~SerialInterface()
{
    if(this->serial->isOpen())
    {
        this->serial->clear();
        this->serial->close();
    }
    this->serial->deleteLater();
}

void SerialInterface::stop()
{
    this->serial->clear();
    this->flag = false;
}

void SerialInterface::reset()
{
    this->flag = true;
}

void SerialInterface::serialClose()
{
    if(this->serial->isOpen())
    {
        this->serial->close();
    }
}

bool SerialInterface::serialOpen(QIODevice::OpenModeFlag flag)
{
    //如果串口已经打开了 先给他关闭了
    if(this->serial->isOpen())
    {
        this->serial->clear();
        this->serial->close();
    }

    this->serial->setPortName(serialName);
    //2获取当前波特率
    this->serial->setBaudRate(this->rate);
    //3设置数据位
    this->serial->setDataBits(this->bits);
    //qDebug()<<"2::"<<static_cast<QSerialPort::DataBits>(ui->cmbDataBits->currentText().toInt());
    //4设置奇偶校验位
    this->serial->setParity(this->parity);
    //qDebug()<<"3::"<<static_cast<QSerialPort::Parity>(ui->cmbCheckSum->currentText().toInt());
    //5设置停止位
    this->serial->setStopBits(this->stopBits);
    //qDebug()<<"4::"<<static_cast<QSerialPort::StopBits>(ui->cmbStopBit->currentText().toInt());
    //设置流控制 ？
    this->serial->setFlowControl(this->flowControl);

    //打开串口
    return this->serial->open(flag);
}

qint64 SerialInterface::write(QByteArray data)
{
    qint64 ilen = this->serial->write(data);
//    this->serial->flush();
    this->serial->waitForBytesWritten(1000);//等待发送完成

//    qDebug()<<"send::"<<ilen;
    return ilen;
}

void SerialInterface::setPortName(QString name)
{
    this->serialName = name;
}

void SerialInterface::setBaudRate(QSerialPort::BaudRate r)
{
    this->rate = r;
}

void SerialInterface::setDataBits(QSerialPort::DataBits b)
{
    this->bits = b;
}

void SerialInterface::setParity(QSerialPort::Parity p)
{
    this ->parity = p;
}

void SerialInterface::setStopBits(QSerialPort::StopBits stop)
{
    this->stopBits = stop;
}

void SerialInterface::setFlowControl(QSerialPort::FlowControl fc)
{
    this ->flowControl = fc;
}
/**
 * @brief SerialInterface::run 重写run方法 -- 线程执行函数
 */
void SerialInterface::run()
{
    bool done=true;
    /*打开文件*/
    QFile file(path);
    /*获取文件信息：长度*/
    QFileInfo info(path);

    if(file.open(QFile::ReadOnly))
    {

        long total = 0;
        //由于在前面文件就已经判断是否存在，

        while(!file.atEnd())
        {
            QByteArray block;
            QDataStream out(&block,QIODevice::WriteOnly);

            if(!this->flag)
            {
                qDebug()<<"停止";
                done = false;
                break;
            }

            qint32 id = 0xF400;
            id |= this->devID;

            QByteArray buffer = file.read(200);

            out<<static_cast<qint16>(0x5525) \
               <<static_cast<qint16>(0x00CA) \
               <<static_cast<qint16>(id) \
               <<static_cast<qint16>(buffer.size())\
               <<buffer;

//            if(this->frequency>0)
//            {
//                QThread::usleep(this->frequency);
//            }

//            this->serial->write(block);
//            this->serial->waitForBytesWritten(); //等待发送完成
            this->write(block);
            total +=buffer.size();
            float num = (static_cast<float>(total) / info.size())*100;
            emit curPercent( static_cast<int>(num) );

            //qDebug()<<"send:"<<block.size();
        }
        file.close();
    }
    else
    {
        done = false;
    }
    emit isDone(done);

}

void SerialInterface::on_serial_readyRead()
{
    //从缓冲区中读取数据
    QByteArray buffer = this->serial->readAll();

    //从界面中读取以前收到的数据
    emit readyRead(buffer);
}
