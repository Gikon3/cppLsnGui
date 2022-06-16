#include "StmCtrlParser.h"
#include <string>

StmCtrlParser::StmCtrlParser(QObject *parent) :
    QObject(parent),
    angleVal(0.0),
    secondsVal(0)
{
}

StmCtrlParser::~StmCtrlParser()
{
}

void StmCtrlParser::append(QByteArray& data)
{
    for (quint8 const it : data) {
        buffer += it;
        if (buffer.data()[buffer.size()-2] == '\r' && buffer.data()[buffer.size()-1] == '\n') {
            parseCmd(buffer);
            buffer.clear();
        }
    }
}

void StmCtrlParser::parseCmd(QByteArray& cmd)
{
    cmd = cmd.simplified();
    cmd = cmd.toLower();
    quint32 pos = -1;
    if ((pos = cmd.indexOf("#angle=") >= 0)) {
        QByteArray number(cmd.data() + pos + sizeof("#angle=") - 2);
        angleVal = number.toFloat();
    }
    else if ((pos = cmd.indexOf("#time=") >= 0)) {
        QByteArray number(cmd.data() + pos + sizeof("#time=") - 2);
        secondsVal = number.toUInt();
    }
    else if ((pos = cmd.indexOf("#autoreset=") >= 0)) {
        QByteArray number(cmd.data() + pos + sizeof("#autoreset=") - 2);
        reconfigEnableVal = number.toUInt();
    }
    else if ((pos = cmd.indexOf("#mode=") >= 0)) {
        QByteArray number(cmd.data() + pos + sizeof("#mode=") - 2);
        modeVal = Mode(number.toUInt());
    }
}

float StmCtrlParser::angle()
{
    return angleVal;
}

int StmCtrlParser::seconds()
{
    return secondsVal;
}

bool StmCtrlParser::reconfigEnable()
{
    return reconfigEnableVal;
}

StmCtrlParser::Mode StmCtrlParser::mode()
{
    return modeVal;
}

QString StmCtrlParser::cmdReconfigEnable(bool en)
{
    QString state = en ? "yes" : "no";
    return QString("#autoreset=" + state + "\r\n");
}

QString StmCtrlParser::cmdChipReconfig()
{
    return QString("#reset\r\n");
}

QString StmCtrlParser::cmdAngle(float val)
{
    QString number = QString("%1").arg(QString::number(val, 'f', 2));
    return QString("#angle=" + number + "\r\n");
}

QString StmCtrlParser::cmdRotate(float val)
{
    QString number = QString("%1").arg(QString::number(val, 'f', 2));
    return QString("#rotate=" + number + "\r\n");
}
