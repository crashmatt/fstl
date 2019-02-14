#include "configeditdialog.h"
#include "ui_configeditdialog.h"

#define TREE_COLUMN         0
#define VALUE_COLUMN        1
#define VISIBILITY_COLUMN   2

//QTreeWidgetUpdatorItem::QTreeWidgetUpdatorItem() : QTreeWidgetItem()
//{

//}

QTreeWidgetUpdatorItem::QTreeWidgetUpdatorItem(QTreeWidgetItem *parent, int type) : QTreeWidgetItem(parent, type)
{

}

QTreeWidgetUpdatorItem::QTreeWidgetUpdatorItem(const QTreeWidgetUpdatorItem &other) : QTreeWidgetItem(other.parent(), other.type())
{

}

QTreeWidgetUpdatorItem::~QTreeWidgetUpdatorItem()
{

}

void QTreeWidgetUpdatorItem::setData(int column, int role, const QVariant &value)
{
     if(role == Qt::CheckStateRole){
         QSignalBlocker(dynamic_cast<QObject*>(this));
     }
     QTreeWidgetItem::setData(column, Qt::CheckStateRole, value);
}

/******************************************************************************************************************/

ConfigEditDialog::ConfigEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigEditDialog)
{
    ui->setupUi(this);
    ui->radiosTreeWidget->setColumnWidth(2,100);

    //QObject::
    connect(ui->radiosTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(itemChanged(QTreeWidgetItem*,int)));
}

ConfigEditDialog::~ConfigEditDialog()
{
    delete ui;
}

void ConfigEditDialog::updateRadio(Radio* radio)
{
    if(!radioItemMap.contains(radio)){
        QTreeWidgetItem *radioItem = new QTreeWidgetItem(ui->radiosTreeWidget);
        radioItem->setExpanded(true);
        radioItemMap[radio] = radioItem;

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
        antennasItem->setExpanded(true);

        antennasItemMap[&radio->m_antennas] = antennasItem;

        foreach(auto antenna, radio->m_antennas){
            addAntenna(antennasItem, antenna);
        }

        QTreeWidgetItem *objectsItem = new QTreeWidgetItem(radioItem);
        objectsItem->setText(TREE_COLUMN, "Objects");
        radioItem->addChild(objectsItem);
        objectsItem->setExpanded(true);
        objectsItem->setFlags(antennasItem->flags() | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);

        foreach(auto objstr, radio->m_objects){
            QTreeWidgetItem *objItem = new QTreeWidgetItem(objectsItem);
            objItem->setText(TREE_COLUMN, objstr);
            objectsItem->addChild(objItem);
            objItem->setFlags(objItem->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
            objItem->setCheckState(VISIBILITY_COLUMN, Qt::Unchecked);
        }
    }
}

void ConfigEditDialog::updateRadios(Radios* radios)
{
    ui->radiosTreeWidget->clear();
    radioItemMap.clear();

    foreach(auto radio, radios->m_radios){
        updateRadio(radio);
    }

////    Gettings objects back from items
//    // From QVariant to QObject *
//    QObject * obj = qvariant_cast<QObject *>(item->data(Qt::UserRole));
//    // from QObject* to myClass*
//    myClass * lmyClass = qobject_cast<myClass *>(obj);
}

void ConfigEditDialog::updateAntenna(Radio* radio, Antenna* antenna)
{

}


void ConfigEditDialog::addAntenna(QTreeWidgetItem *radioItem, Antenna* antenna)
{
    QTreeWidgetItem *antennaItem = new QTreeWidgetItem(radioItem);
    antennaItem->setExpanded(true);
    antennaItemMap[antenna] = antennaItem;

    // QTreeWidgetItem::setText(int column, const QString & text)
    antennaItem->setText(TREE_COLUMN, antenna->m_name);

    antennaItem->setFlags(antennaItem->flags() | Qt::ItemIsUserCheckable |Qt::ItemIsSelectable);
    antennaItem->setCheckState(VISIBILITY_COLUMN, Qt::Unchecked);
    antennaItem->setFlags( antennaItem->flags() | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);

    QVariant antenna_var(QMetaType::QObjectStar, antenna);
    antennaItem->setData(TREE_COLUMN, Qt::UserRole, antenna_var);
    antennaItemMap[antenna] = antennaItem;

    // QTreeWidgetItem::addChild(QTreeWidgetItem * child)
    radioItem->addChild(antennaItem);
}


void ConfigEditDialog::itemChanged(QTreeWidgetItem *item, int column)
{
    auto flags = item->flags();
    if(column == VISIBILITY_COLUMN){
        if(item->checkState(column)){
            qDebug("Item checked by user");
        } else {
            qDebug("Item cleared by user");
        }
//            item->setDisabled(false);
//        item->setCheckState(VISIBILITY_COLUMN, Qt::Unchecked);
    }
}

