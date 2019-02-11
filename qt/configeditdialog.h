#ifndef CONFIGEDITDIALOG_H
#define CONFIGEDITDIALOG_H

#include <QDialog>
#include "../src/radio.h"

namespace Ui {
class ConfigEditDialog;
}

class ConfigEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigEditDialog(QWidget *parent = 0);
    ~ConfigEditDialog();

    void updateRadio(Radio* radio);
    void updateRadios(Radios* radios);
    void updateAntenna(Radio* radio, Antenna* antenna);

    void addAntenna(QTreeWidgetItem *radioItem, Antenna* antenna);
    void addRadioObject(QTreeWidgetItem *radioItem, QString obj);

private:
    Ui::ConfigEditDialog *ui;
};

#endif // CONFIGEDITDIALOG_H
