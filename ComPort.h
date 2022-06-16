#pragma once

#include <QObject>
#include <QSerialPort>
#include <QTimer>

class ComPort : public QObject
{
    Q_OBJECT
public:
    explicit ComPort(QObject *parent = nullptr);
    ~ComPort();
    bool connectStatus();

signals:
    void readyRead(QByteArray rxData);
    void timeoutConnection();

public slots:
    void openPort(QString portName);
    void closePort();
    void transmit(QByteArray msg);

private slots:
    void handleReadyRead();
    void handleTimeout();

private:
    QSerialPort* port;
    QByteArray rxBuffer;
    QTimer* tmrCheckConnect;
    bool connectFl;
};
