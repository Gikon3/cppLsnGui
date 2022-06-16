#pragma once

#include <QAbstractButton>
#include <QDialog>
#include <QTranslator>
#include <QMap>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

signals:
    void applyOptions();
    void closeWindow();

private slots:
    void btnClicked(QAbstractButton* btn);

public:
    struct Options
    {
        enum class Lang {
            russian,
            english
        };
        Lang language;
        int bufferStr;
    };

public:
    Settings(QWidget *parent, Options& op);
    Options options();
    void swapLang();

private:
    void apply();
    void ok();
    void cancel();

private:
    Ui::Settings *ui;
    Options saveOptions;
    const QMap<Options::Lang, QString> langNames;
};

