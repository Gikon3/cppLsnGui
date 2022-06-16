#pragma once

#include <QObject>
#include <QMap>

class DataParser : public QObject
{
    Q_OBJECT
public:
    explicit DataParser(QObject *parent = nullptr);

public:
    enum class Code : quint32
    {
        start = 0xF0DAA000,
        begin = 0xF0DA0000,
        mem05 = 0xF0DA0010,
        mem0a = 0xF0DA0011,
        mem15 = 0xF0DA0012,
        mem1a = 0xF0DA0013,
        spiqf = 0xF0DA0020,
        uart0 = 0xF0DA0030,
        uart1 = 0xF0DA0040,
        i2cm = 0xF0DA0050,
        spod = 0xF0DA0060,
        end = 0xF0DA0EFF,
        msgError = 0xF0DA0F00,
        hashError = 0xF0DA0F01,
        timeout = 0xF0DA0F02
    };

signals:

public slots:
    QString parse(QByteArray data);
    QMap<DataParser::Code, int> getNote();
    void clear();

private:
    QString codeParse(quint32 code);

private:
    const quint32 maxErrors = 64;
    QByteArray buffer;
    int countUnknown = 0;
    QMap<Code, int> note;
};
