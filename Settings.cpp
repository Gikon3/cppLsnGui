#include "Settings.h"
#include "ui_Settings.h"
#include <QDebug>

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings),
    langNames({{Options::Lang::english, tr("English")}, {Options::Lang::russian, tr("Russian")}})
{
    ui->setupUi(this);

    const QList<QString>& names = langNames.values();
    for (const QString& name : names) {
        ui->cbLang->addItem(name);
    }
    connect(ui->bBox, &QDialogButtonBox::clicked, this, &Settings::btnClicked);

    saveOptions.bufferStr = 200;
    saveOptions.language = Options::Lang::russian;
}

Settings::Settings(QWidget *parent, Options& opt) :
    Settings(parent)
{
    ui->sbBuffer->setValue(opt.bufferStr);
    const QList<Options::Lang>& idLangs = langNames.keys();
    for (const Options::Lang lang : idLangs) {
        if (lang == opt.language) {
            ui->cbLang->setCurrentText(langNames[lang]);
            break;
        }
    }
    swapLang();
}

Settings::~Settings()
{
    delete ui;
}

Settings::Options Settings::options()
{
    return saveOptions;
}

void Settings::swapLang()
{
    ui->retranslateUi(this);
}

void Settings::apply()
{
    const Options::Lang oldLang = saveOptions.language;
    saveOptions.bufferStr = ui->sbBuffer->text().toInt();
    const QList<Options::Lang>& idLangs = langNames.keys();
    for (const Options::Lang lang : idLangs) {
        if (ui->cbLang->currentText() == langNames[lang]) {
            saveOptions.language = lang;
            break;
        }
    }
    if (saveOptions.language != oldLang) swapLang();
    emit applyOptions();
}
void Settings::ok()
{
    apply();
    cancel();
}
void Settings::cancel()
{
    emit closeWindow();
}

void Settings::btnClicked(QAbstractButton* btn)
{
    switch (ui->bBox->buttonRole(btn)) {
        case QDialogButtonBox::ButtonRole::ApplyRole: apply(); break;
        case QDialogButtonBox::ButtonRole::AcceptRole: ok(); break;
        case QDialogButtonBox::ButtonRole::RejectRole:
        default: cancel();
    }
}
