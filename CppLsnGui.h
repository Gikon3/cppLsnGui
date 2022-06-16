#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QTranslator>
#include "ComPort.h"
#include "StmCtrlParser.h"
#include "DataParser.h"
#include "Settings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CppLsnGui; }
QT_END_NAMESPACE

class CppLsnGui : public QMainWindow
{
    Q_OBJECT

public:
    CppLsnGui(QWidget *parent = nullptr);
    ~CppLsnGui();

signals:
    void signalRefreshComList(QComboBox& box);
    void openDataCom(QString portName);
    void openStmCtrlCom(QString portName);
    void closeDataCom();
    void closeStmCtrlCom();
    void txCmd(QByteArray cmd);

private slots:
    void refreshDataComListClicked();
    void refreshStmCtrlComListClicked();
    void refreshComList(QComboBox& box);
    void connectDataClicked();
    void connectStmCtrlClicked();
    void resultClearClicked();
    void closeDataPort();
    void closeStmCtrlPort();
    void readyComData(QByteArray data);
    void readyComStmCtrl(QByteArray data);
    void angleClicked();
    void rotateClicked();
    void autoResetChipClicked();
    void resetChipClicked();
    void fileExplorerClicked();
    void fileWriteClicked();
    void actionAboutTriggered();
    void actionAboutQtTriggered();
    void openMenuSettings();

private:
    void drawResult();
    void swapLang();

private:
    QString convertFileSize(quint64 size);

private:
    Ui::CppLsnGui *ui;
    QThread* thrDataSerial;
    QThread* thrStmCtrlSerial;
    ComPort* portData;
    ComPort* portStmCtrl;
    StmCtrlParser* parserCmd;
    DataParser* parserData;
    QFile* file;
    QTextStream fileStream;
    Settings::Options options;
    QTranslator translator;
};
