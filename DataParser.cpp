#include "DataParser.h"
#include <QDebug>
#include <string>
#include <QDate>
#include <QTime>

DataParser::DataParser(QObject *parent) :
    QObject(parent)
{
}

QString DataParser::parse(QByteArray data)
{
    QString outStr("");
    for (const quint8 byte : data) {
        buffer += byte;
        if (countUnknown > (maxErrors*2)) {
            buffer += byte;
            countUnknown = 0;
        }
        if (buffer.size() == 4) {
            if (outStr.size() != 0) outStr += "\r\n";
            QDate date = QDate::currentDate();
            QTime time = QTime::currentTime();
            std::string str = buffer.toHex().toUpper().toStdString();
            quint32 word = std::stoul(str, nullptr, 16);
            buffer.clear();
            QString code("");
            outStr += date.toString(Qt::DefaultLocaleShortDate) + " " + time.toString() + " "
                      + code.fromStdString(str) + " " + codeParse(word);
        }
    }
    return outStr;
}

QMap<DataParser::Code, int> DataParser::getNote()
{
    return note;
}

QString DataParser::codeParse(quint32 code)
{
    static bool errNumberFl = false;
    static quint32 errNumber = 0;
    static quint32 errCount = 0;
    static bool hashValFl = false;
    static bool angleValFl = false;
    static Code lastCode = Code::msgError;

    switch (Code(code)) {
        case Code::start:
            errNumberFl = false;
            hashValFl = false;
            angleValFl = false;
            ++note[Code::start];
            return QString(tr("Start cpp lsn"));
        case Code::begin:
            errNumberFl = false;
            ++note[Code::begin];
            countUnknown = 0;
            return QString(tr("Begin message"));
        case Code::mem05:
            errNumberFl = true;
            lastCode = Code::mem05;
            return QString(tr("Memory0 pat 5"));
        case Code::mem0a:
            errNumberFl = true;
            lastCode = Code::mem0a;
            return QString(tr("Memory0 pat A"));
        case Code::mem15:
            errNumberFl = true;
            lastCode = Code::mem15;
            return QString(tr("Memory1 pat 5"));
        case Code::mem1a:
            errNumberFl = true;
            lastCode = Code::mem1a;
            return QString(tr("Memory1 pat A"));
        case Code::spiqf:
            errNumberFl = true;
            lastCode = Code::spiqf;
            return QString(tr("SPIQF"));
        case Code::uart0:
            errNumberFl = true;
            lastCode = Code::uart0;
            return QString(tr("UART0"));
        case Code::uart1:
            errNumberFl = true;
            lastCode = Code::uart1;
            return QString(tr("UART1"));
        case Code::i2cm:
            errNumberFl = true;
            lastCode = Code::i2cm;
            return QString(tr("I2CM"));
        case Code::spod:
            errNumberFl = true;
            lastCode = Code::spod;
            return QString(tr("SPOD"));
        case Code::end:
            errNumberFl = false;
            angleValFl = true;
            ++note[Code::end];
            return QString(tr("End message"));
        case Code::hashError:
            errNumberFl = false;
            ++note[Code::hashError];
            return QString(tr("Hash error"));
        case Code::msgError:
            errNumberFl = false;
            ++note[Code::msgError];
            return QString(tr("Message error"));
        case Code::timeout:
            errNumberFl = false;
            ++note[Code::timeout];
            return QString(tr("Silence of the chip"));
        default:
            if (errNumberFl) {
                errNumberFl = false;
                errNumber = code < maxErrors ? code * 2 : maxErrors * 2;
                errCount = 0;
                note[lastCode] += code;
                if (errNumber == 0) hashValFl = true;
                return QString(tr("Number errors %1")).arg(code);
            }
            if (errCount < errNumber && errCount % 2 == 0) {
                ++errCount;
                return QString(tr("Address %1")).arg((errCount + 1) / 2);
            }
            if (errCount < errNumber) {
                ++errCount;
                if (errCount == errNumber) hashValFl = true;
                return QString(tr("Error   %1")).arg(errCount / 2);
            }
            if (hashValFl) {
                hashValFl = false;
                return QString(tr("Hash"));
            }
            if (angleValFl) {
                angleValFl = false;
                return QString(tr("Angle %1")).arg(code);
            }
            ++countUnknown;
            return QString(tr("Unknown"));
    }
}

void DataParser::clear()
{
    const QList<Code> keys = note.keys();
    for (const Code& key : keys) {
        note[key] = 0;
    }
}
