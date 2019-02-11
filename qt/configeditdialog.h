#ifndef CONFIGEDITDIALOG_H
#define CONFIGEDITDIALOG_H

#include <QDialog>

namespace Ui {
class ConfigEditDialog;
}

class ConfigEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigEditDialog(QWidget *parent = 0);
    ~ConfigEditDialog();

private:
    Ui::ConfigEditDialog *ui;
};

#endif // CONFIGEDITDIALOG_H
