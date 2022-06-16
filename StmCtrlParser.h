#pragma once

#include <QObject>
#include <QByteArray>
#include <QString>

class StmCtrlParser : public QObject
{
    Q_OBJECT

public:
    enum class Mode {
        stable,
        rotate
    };

public:
    explicit StmCtrlParser(QObject *parent = nullptr);
    ~StmCtrlParser();
    void append(QByteArray& data);
    float angle();
    int seconds();
    bool reconfigEnable();
    Mode mode();

public slots:
    QString cmdReconfigEnable(bool en);
    QString cmdChipReconfig();
    QString cmdAngle(float val);
    QString cmdRotate(float val);

private:
    void parseCmd(QByteArray& cmd);

private:
    QByteArray buffer;
    float angleVal;
    int secondsVal;
    bool reconfigEnableVal;
    Mode modeVal;
};
