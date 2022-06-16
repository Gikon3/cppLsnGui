#include "CppLsnGui.h"
#include "ui_CppLsnGui.h"
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <map>

CppLsnGui::CppLsnGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CppLsnGui),
    parserCmd(new StmCtrlParser),
    parserData(new DataParser),
    file(new QFile(this)),
    fileStream(file)
{
    ui->setupUi(this);

    connect(ui->bDataRefreshListCom, &QPushButton::clicked, this, &CppLsnGui::refreshDataComListClicked);
    connect(ui->bStmCtrlRefreshListCom, &QPushButton::clicked, this, &CppLsnGui::refreshStmCtrlComListClicked);
    connect(this, &CppLsnGui::signalRefreshComList, this, &CppLsnGui::refreshComList);

    connect(ui->bDataConnectCom, &QPushButton::clicked, this, &CppLsnGui::connectDataClicked);
    connect(ui->bStmCtrlConnectCom, &QPushButton::clicked, this, &CppLsnGui::connectStmCtrlClicked);

    connect(ui->bDataClear, &QPushButton::clicked, ui->ptMain, &QPlainTextEdit::clear);

    connect(ui->bResultClear, &QPushButton::clicked, this, &CppLsnGui::resultClearClicked);

    connect(ui->bStmCtrlAngleTransmit, &QPushButton::clicked, this, &CppLsnGui::angleClicked);
    connect(ui->bStmCtrlRotateTransmit, &QPushButton::clicked, this, &CppLsnGui::rotateClicked);
    connect(ui->cbStmCtrlAutoResetChip, &QPushButton::clicked, this, &CppLsnGui::autoResetChipClicked);
    connect(ui->bStmCtrlResetChip, &QPushButton::clicked, this, &CppLsnGui::resetChipClicked);

    connect(ui->tbDataFileExplorer, &QPushButton::clicked, this, &CppLsnGui::fileExplorerClicked);
    connect(ui->bDataWriteToFile, &QPushButton::clicked, this , &CppLsnGui::fileWriteClicked);

    connect(ui->actionAboutProgram, &QAction::triggered, this, &CppLsnGui::actionAboutTriggered);
    connect(ui->actionAboutQt, &QAction::triggered, this, &CppLsnGui::actionAboutQtTriggered);
    connect(ui->actionSettings, &QAction::triggered, this, &CppLsnGui::openMenuSettings);

    thrDataSerial = new QThread(this);
    portData = new ComPort();
    portData->moveToThread(thrDataSerial);
    connect(thrDataSerial, &QThread::finished, portData, &ComPort::deleteLater);
    thrDataSerial->start();

    connect(this, &CppLsnGui::openDataCom, portData, &ComPort::openPort);
    connect(this, &CppLsnGui::closeDataCom, portData, &ComPort::closePort);
    connect(portData, &ComPort::timeoutConnection, this, &CppLsnGui::closeDataPort);
    connect(portData, &ComPort::readyRead, this, &CppLsnGui::readyComData);

    thrStmCtrlSerial = new QThread(this);
    portStmCtrl = new ComPort();
    portStmCtrl->moveToThread(thrStmCtrlSerial);
    connect(thrStmCtrlSerial, &QThread::finished, portStmCtrl, &ComPort::deleteLater);
    thrStmCtrlSerial->start();

    connect(this, &CppLsnGui::openStmCtrlCom, portStmCtrl, &ComPort::openPort);
    connect(this, &CppLsnGui::closeStmCtrlCom, portStmCtrl, &ComPort::closePort);
    connect(portStmCtrl, &ComPort::timeoutConnection, this, &CppLsnGui::closeStmCtrlPort);
    connect(portStmCtrl, &ComPort::readyRead, this, &CppLsnGui::readyComStmCtrl);
    connect(this, &CppLsnGui::txCmd, portStmCtrl, &ComPort::transmit);

    refreshComList(*ui->cbDataListCom);
    refreshComList(*ui->cbStmCtrlListCom);

    options.bufferStr = 200;
    options.language = Settings::Options::Lang::russian;
    ui->ptMain->setMaximumBlockCount(options.bufferStr);
    swapLang();
}

CppLsnGui::~CppLsnGui()
{
    delete file;
    delete parserData;
    delete parserCmd;
    thrDataSerial->quit();
    thrDataSerial->wait(1000);
    thrStmCtrlSerial->quit();
    thrStmCtrlSerial->wait(1000);
    delete ui;
}

void CppLsnGui::refreshDataComListClicked()
{
    emit signalRefreshComList(*ui->cbDataListCom);
}
void CppLsnGui::refreshStmCtrlComListClicked()
{
    emit signalRefreshComList(*ui->cbStmCtrlListCom);
}
void CppLsnGui::refreshComList(QComboBox& box)
{
    QStringList portList;
    QList<QSerialPortInfo> avaliablePorts = QSerialPortInfo::availablePorts();
    for (QSerialPortInfo& info : avaliablePorts) {
        if (!info.isBusy()) portList.append(info.portName());
    }
    QString current = box.currentText();
    box.clear();
    box.addItems(portList);
    box.setCurrentText(current);
}

void CppLsnGui::connectDataClicked()
{
    if (!portData->connectStatus()) {
        QString portName = ui->cbDataListCom->currentText();
        ui->bDataConnectCom->setText(tr("Disconnect"));
        ui->cbDataListCom->setEnabled(false);
        ui->bDataRefreshListCom->setEnabled(false);
        emit openDataCom(portName);
    }
    else {
        closeDataPort();
    }
}
void CppLsnGui::connectStmCtrlClicked()
{
    if (!portStmCtrl->connectStatus()) {
        QString portName = ui->cbStmCtrlListCom->currentText();
        ui->bStmCtrlConnectCom->setText(tr("Disconnect"));
        ui->cbStmCtrlListCom->setEnabled(false);
        ui->bStmCtrlRefreshListCom->setEnabled(false);
        emit openStmCtrlCom(portName);
    }
    else {
        closeStmCtrlPort();
    }
}

void CppLsnGui::resultClearClicked()
{
    parserData->clear();
    drawResult();
}

void CppLsnGui::closeDataPort()
{
    emit closeDataCom();
    ui->bDataConnectCom->setText(tr("Connect"));
    ui->bDataConnectCom->setChecked(false);
    ui->cbDataListCom->setEnabled(true);
    ui->bDataRefreshListCom->setEnabled(true);
}
void CppLsnGui::closeStmCtrlPort()
{
    emit closeStmCtrlCom();
    ui->bStmCtrlConnectCom->setText(tr("Connect"));
    ui->bStmCtrlConnectCom->setChecked(false);
    ui->cbStmCtrlListCom->setEnabled(true);
    ui->bStmCtrlRefreshListCom->setEnabled(true);
}

void CppLsnGui::drawResult()
{
    std::map<DataParser::Code, int> note = parserData->getNote().toStdMap();
    for (auto elem : note) {
        switch (elem.first) {
            case DataParser::Code::begin: break;
            case DataParser::Code::start: ui->labelResultStartVal->setNum(elem.second); break;
            case DataParser::Code::mem05: ui->labelResultMem05Val->setNum(elem.second); break;
            case DataParser::Code::mem0a: ui->labelResultMem0AVal->setNum(elem.second); break;
            case DataParser::Code::mem15: ui->labelResultMem15Val->setNum(elem.second); break;
            case DataParser::Code::mem1a: ui->labelResultMem1AVal->setNum(elem.second); break;
            case DataParser::Code::spiqf: ui->labelResultSpiqfVal->setNum(elem.second); break;
            case DataParser::Code::uart0: ui->labelResultUart0Val->setNum(elem.second); break;
            case DataParser::Code::uart1: ui->labelResultUart1Val->setNum(elem.second); break;
            case DataParser::Code::i2cm: ui->labelResultI2cmVal->setNum(elem.second); break;
            case DataParser::Code::spod: ui->labelResultSpodVal->setNum(elem.second); break;
            case DataParser::Code::end: break;
            case DataParser::Code::hashError: ui->labelResultHashErrorVal->setNum(elem.second); break;
            case DataParser::Code::msgError: ui->labelResultMsgErrorVal->setNum(elem.second); break;
            case DataParser::Code::timeout: ui->labelResultTimeoutVal->setNum(elem.second); break;
        }
    }
}

void CppLsnGui::swapLang()
{
    switch (options.language) {
        case Settings::Options::Lang::russian:
            translator.load(":/langs/CppLsnGui_ru_RU");
            break;
        case Settings::Options::Lang::english:
        default:
            translator.load("");
    }
    qApp->installTranslator(&translator);
    ui->retranslateUi(this);
}

void CppLsnGui::readyComData(QByteArray data)
{
    QString parseData = parserData->parse(data);

    if (file->isOpen()) {
        ui->labelDataSizeFileVal->setText(convertFileSize(file->size()));
        fileStream << parseData << "\r\n";
    }

    drawResult();
    ui->ptMain->appendPlainText(parseData);
}
void CppLsnGui::readyComStmCtrl(QByteArray data)
{
    parserCmd->append(data);
    ui->labelStmCtrlCurrentAngleVal->setNum(parserCmd->angle());
    ui->labelStmCtrlSecondsVal->setNum(parserCmd->seconds());
    ui->cbStmCtrlAutoResetChip->setChecked(parserCmd->reconfigEnable());
    QFont fontAngle = ui->labelStmCtrlAngle->font();
    QFont fontRotate = ui->labelStmCtrlRotate->font();
    if (parserCmd->mode() == StmCtrlParser::Mode::stable) {
        fontAngle.setUnderline(true);
        fontRotate.setUnderline(false);
    }
    else if (parserCmd->mode() == StmCtrlParser::Mode::rotate) {
        fontAngle.setUnderline(false);
        fontRotate.setUnderline(true);
    }
    ui->labelStmCtrlAngle->setFont(fontAngle);
    ui->labelStmCtrlRotate->setFont(fontRotate);
}

void CppLsnGui::angleClicked()
{
    if (portStmCtrl->connectStatus()) {
        float angleVal = ui->sbStmCtrlAngle->value();
        emit txCmd(parserCmd->cmdAngle(angleVal).toUtf8());
    }
}
void CppLsnGui::rotateClicked()
{
    if (portStmCtrl->connectStatus()) {
        float rotateVal = ui->sbStmCtrlRotate->value();
        emit txCmd(parserCmd->cmdRotate(rotateVal).toUtf8());
    }
}
void CppLsnGui::autoResetChipClicked()
{
    if (portStmCtrl->connectStatus()) {
        if (ui->cbStmCtrlAutoResetChip->isChecked()) emit txCmd(parserCmd->cmdReconfigEnable(true).toUtf8());
        else emit txCmd(parserCmd->cmdReconfigEnable(false).toUtf8());
    }
}

void CppLsnGui::resetChipClicked()
{
    if (portStmCtrl->connectStatus()) {
        emit txCmd(parserCmd->cmdChipReconfig().toUtf8());
    }
}

void CppLsnGui::fileExplorerClicked()
{
    QString lastFilePath = QFileInfo(*file).filePath();
    QString filename = QFileDialog::getSaveFileName(this, tr("Open file"),
                                                    lastFilePath.size() ? lastFilePath: QDir::homePath() + "\\Desktop",
                                                    "All files (*.*);; Text document (*.txt *.log)", nullptr,
                                                    QFileDialog::DontConfirmOverwrite);
    if(filename.size()) ui->lineDataFilename->setText(filename);
}

void CppLsnGui::fileWriteClicked()
{
    QString filename = ui->lineDataFilename->text();

    if (!ui->bDataWriteToFile->isChecked()) {
        file->close();
        ui->labelDataSizeFileVal->setText(convertFileSize(file->size()));
        ui->bDataWriteToFile->setText(tr("Write"));
        ui->ptMain->appendPlainText(tr("------ End of file ------"));
        ui->bDataWriteToFile->setChecked(false);
    }
    else {
        file->setFileName(filename);
        bool status = file->open(QIODevice::WriteOnly | QIODevice::NewOnly);
        if (!status) {
            QMessageBox messageBox(this);
            messageBox.addButton(tr("Rewrite"), QMessageBox::AcceptRole);
            messageBox.addButton(tr("Append"), QMessageBox::RejectRole);
            messageBox.addButton(tr("Cancel"), QMessageBox::DestructiveRole);
            messageBox.setText(tr("File exists!"));
            messageBox.setIcon(QMessageBox::Warning);
            switch (messageBox.exec()) {
                case QMessageBox::AcceptRole: status = file->open(QIODevice::WriteOnly); break;
                case QMessageBox::RejectRole: status = file->open(QIODevice::WriteOnly | QIODevice::Append); break;
                case QMessageBox::DestructiveRole: status = false; break;
            }
        }
        if (status) {
            ui->bDataWriteToFile->setChecked(true);
            ui->bDataWriteToFile->setText(tr("Stop"));
            ui->labelDataFilenameVal->setText(filename);
            ui->labelDataSizeFileVal->setText(convertFileSize(file->size()));
            ui->ptMain->appendPlainText(tr("----- Start of file -----"));
        }
        else {
            ui->bDataWriteToFile->setChecked(false);
        }
    }
}

QString CppLsnGui::convertFileSize(quint64 size)
{
    double calcSize = size;
    enum Postfix
    {
        B = 0,
        KiB,
        MiB,
        GiB
    };
    Postfix postfix = B;

    while (calcSize >= 1024 && postfix != GiB) {
        calcSize /= 1024;
        postfix = Postfix(static_cast<int>(postfix) + 1);
    }

    QString number = QString("%1").arg(QString::number(calcSize, 'f', 2));
    switch (postfix) {
        case B: return QString(number + " " + tr("B"));
        case KiB: return QString(number + " " + tr("KiB"));
        case MiB: return QString(number + " " + tr("MiB"));
        case GiB: return QString(number + " " + tr("GiB"));
        default: return QString(tr("Error"));
    }
}

void CppLsnGui::actionAboutTriggered()
{
    QMessageBox::about(this, tr("About CppLsnGui"), tr("The application is designed to monitor\nthe tests of the CPP LSN microcircuit,\ncontrol the servo drive and STM32"));
}

void CppLsnGui::actionAboutQtTriggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

void CppLsnGui::openMenuSettings()
{
    Settings::Options::Lang oldLanguage = options.language;
    Settings window(this, options);
    window.setModal(true);
    auto copyOptions = [&window, &oldLanguage, this]()
    {
        options = window.options();
        if (options.language != oldLanguage) {
            swapLang();
            window.swapLang();
        }
        oldLanguage = options.language;
    };
    connect(&window, &Settings::applyOptions, copyOptions);
    connect(&window, &Settings::closeWindow, &window, &Settings::close);
    window.exec();
    ui->ptMain->setMaximumBlockCount(options.bufferStr);
}
