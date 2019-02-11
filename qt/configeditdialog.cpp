#include "configeditdialog.h"
#include "ui_configeditdialog.h"

ConfigEditDialog::ConfigEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigEditDialog)
{
    ui->setupUi(this);
}

ConfigEditDialog::~ConfigEditDialog()
{
    delete ui;
}
