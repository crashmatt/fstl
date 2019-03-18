#ifndef CONFIGEDITDIALOG_H
#define CONFIGEDITDIALOG_H

#include <QDialog>
#include <QMap>
#include <QTreeWidgetItem>
#include "../src/radio.h"

namespace Ui {
class ConfigEditDialog;
}

class QTreeWidgetUpdatorItem : public QTreeWidgetItem
{
public:
//    QTreeWidgetUpdatorItem();
    explicit QTreeWidgetUpdatorItem(QTreeWidgetItem *parent, int type = Type);
    QTreeWidgetUpdatorItem(const QTreeWidgetUpdatorItem &other);
    virtual ~QTreeWidgetUpdatorItem();

    virtual void setData(int column, int role, const QVariant &value);

protected:
    bool isUpdating;
};

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

Q_SIGNALS:

public Q_SLOTS:
    void itemChanged(QTreeWidgetItem *item, int state);

protected:
    QMap<Radio*, QTreeWidgetItem*> radioItemMap;
    QMap<QVector<Antenna*>*, QTreeWidgetItem*> antennasItemMap;
    QMap<QStringList*, QTreeWidgetItem*> objectsItemMap;
    QMap<Antenna*, QTreeWidgetItem*> antennaItemMap;
    QMap<QTreeWidgetItem*, bool> itemChangeInProgress;

private:
    Ui::ConfigEditDialog *ui;
};

#endif // CONFIGEDITDIALOG_H
