#include "configeditdialog.h"
#include "ui_configeditdialog.h"

#define TREE_COLUMN         0
#define VALUE_COLUMN        1
#define VISIBILITY_COLUMN   2

ConfigEditDialog::ConfigEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigEditDialog)
{
    ui->setupUi(this);
    ui->radiosTreeWidget->setColumnWidth(2,100);
}

ConfigEditDialog::~ConfigEditDialog()
{
    delete ui;
}

void ConfigEditDialog::updateRadio(Radio* radio)
{

}

void ConfigEditDialog::updateRadios(Radios* radios)
{
    ui->radiosTreeWidget->clear();
    foreach(auto radio, radios->m_radios){
        QTreeWidgetItem *radioItem = new QTreeWidgetItem(ui->radiosTreeWidget);

        // QTreeWidgetItem::setText(int column, const QString & text)
        radioItem->setText(TREE_COLUMN, radio->m_name);
        QVariant radio_var(QMetaType::QObjectStar, radio);
        radioItem->setData(TREE_COLUMN,Qt::UserRole, radio_var);
        radioItem->setFlags(radioItem->flags() | Qt::ItemIsUserCheckable |Qt::ItemIsSelectable);
        radioItem->setCheckState(VISIBILITY_COLUMN, Qt::Unchecked);

        QTreeWidgetItem *antennasItem = new QTreeWidgetItem(radioItem);
        antennasItem->setText(TREE_COLUMN, "Antennas");
        radioItem->addChild(antennasItem);
        antennasItem->setFlags(antennasItem->flags() | Qt::ItemIsUserCheckable |Qt::ItemIsSelectable);
        antennasItem->setCheckState(VISIBILITY_COLUMN, Qt::Unchecked);

        foreach(auto antenna, radio->m_antennas){
            addAntenna(antennasItem, antenna);
        }

        QTreeWidgetItem *objectsItem = new QTreeWidgetItem(radioItem);
        objectsItem->setText(TREE_COLUMN, "Objects");
        radioItem->addChild(objectsItem);
        objectsItem->setFlags(antennasItem->flags() | Qt::ItemIsUserCheckable |Qt::ItemIsSelectable);
        objectsItem->setFlags(objectsItem->flags() | Qt::ItemIsUserCheckable |Qt::ItemIsSelectable);

        foreach(auto objstr, radio->m_objects){
            QTreeWidgetItem *objItem = new QTreeWidgetItem(objectsItem);
            objItem->setText(TREE_COLUMN, objstr);
            objectsItem->addChild(objItem);
            objItem->setFlags(objItem->flags() | Qt::ItemIsUserCheckable |Qt::ItemIsSelectable);
            objItem->setCheckState(VISIBILITY_COLUMN, Qt::Unchecked);
        }
    }
}

void ConfigEditDialog::updateAntenna(Radio* radio, Antenna* antenna)
{

}


void ConfigEditDialog::addAntenna(QTreeWidgetItem *radioItem, Antenna* antenna)
{
    QTreeWidgetItem *antennaItem = new QTreeWidgetItem(radioItem);

    // QTreeWidgetItem::setText(int column, const QString & text)
    antennaItem->setText(TREE_COLUMN, antenna->m_name);

    antennaItem->setFlags(antennaItem->flags() | Qt::ItemIsUserCheckable |Qt::ItemIsSelectable);
    antennaItem->setCheckState(VISIBILITY_COLUMN, Qt::Unchecked);

    QVariant antenna_var(QMetaType::QObjectStar, antenna);
    radioItem->setData(TREE_COLUMN,Qt::UserRole, antenna_var);

    // QTreeWidgetItem::addChild(QTreeWidgetItem * child)
    radioItem->addChild(antennaItem);
}

