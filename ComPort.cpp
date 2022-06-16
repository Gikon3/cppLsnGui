#include "ComPort.h"

ComPort::ComPort(QObject *parent) :
    QObject(parent),
    port(new QSerialPort(this)),
    tmrCheckConnect(new QTimer(this)),
    connectFl(false)
{
    connect(port, &QSerialPort::readyRead, this, &ComPort::handleReadyRead);
    connect(tmrCheckConnect, &QTimer::timeout, this, &ComPort::handleTimeout);
}

ComPort::~ComPort()
{
    tmrCheckConnect->stop();
    closePort();
    delete port;
    port = nullptr;
    delete tmrCheckConnect;
    tmrCheckConnect = nullptr;
}

bool ComPort::connectStatus()
{
    return connectFl;
}

void ComPort::openPort(QString portName)
{
    port->setPortName(portName);
    port->setBaudRate(QSerialPort::Baud19200);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);

    if (port->open(QSerialPort::ReadWrite)) {
        port->flush();
        tmrCheckConnect->setInterval(1500);
        tmrCheckConnect->start();
        connectFl = true;
    }
    else {
        connectFl = false;
        emit timeoutConnection();
    }
}

void ComPort::closePort()
{
    connectFl = false;
    port->close();
    tmrCheckConnect->stop();
}

void ComPort::transmit(QByteArray msg)
{
    port->write(msg);
}

void ComPort::handleReadyRead()
{
    connectFl = true;
    rxBuffer.append(port->readAll());
    emit readyRead(rxBuffer);
    rxBuffer.clear();
}

void ComPort::handleTimeout()
{
//    if (!connectFl) {
//        closePort();
//        emit timeoutConnection();
//    }
//    connectFl = false;
}
