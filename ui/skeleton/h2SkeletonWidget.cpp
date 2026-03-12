#include "h2SkeletonWidget.h"
#include "common_define_glove_hw.h"
#include <QStandardItemModel>
#include <QAbstractItemView>
#include <QDebug>
#include "CarbonUIClass.h"

h2SkeletonWidget::h2SkeletonWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    applyStylesheet();
    populateSkeletonTree();
}

h2SkeletonWidget::~h2SkeletonWidget()
{}

void h2SkeletonWidget::applyStylesheet()
{
    // Set background and text color
    this->setStyleSheet("background-color: #3b3b3b; color: white;");
	CarbonUIClass::applyTreeViewStyle(ui.treeView);
}

void h2SkeletonWidget::populateSkeletonTree()
{
    constexpr int kSkeletonIndexRole = Qt::UserRole + 1;

    // Create tree model and populate using KHS32_SkeletonIndex entries
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels(QStringList() << tr("Skeleton"));

    QStandardItem* rightHandItem = new QStandardItem(QString::fromStdString(kinemHumanSkeleton32[KHS32_RightHand]));
    rightHandItem->setEditable(false);
    rightHandItem->setData(static_cast<int>(KHS32_RightHand), kSkeletonIndexRole);

    // add children between RightHand and LeftHand
    for (int i = KHS32_RightHand + 1; i < KHS32_LeftHand; ++i)
    {
        QStandardItem* child = new QStandardItem(QString::fromStdString(kinemHumanSkeleton32[i]));
        child->setEditable(false);
        child->setData(i, kSkeletonIndexRole);
        rightHandItem->appendRow(child);
    }

    QStandardItem* leftHandItem = new QStandardItem(QString::fromStdString(kinemHumanSkeleton32[KHS32_LeftHand]));
    leftHandItem->setEditable(false);
    leftHandItem->setData(static_cast<int>(KHS32_LeftHand), kSkeletonIndexRole);

    // add children between LeftHand and end
    for (int i = KHS32_LeftHand + 1; i < KHS32_Count; ++i)
    {
        QStandardItem* child = new QStandardItem(QString::fromStdString(kinemHumanSkeleton32[i]));
        child->setEditable(false);
        child->setData(i, kSkeletonIndexRole);
        leftHandItem->appendRow(child);
    }

    model->appendRow(rightHandItem);
    model->appendRow(leftHandItem);

    // Set model to treeView and adjust appearance
    if (ui.treeView)
    {
        ui.treeView->setModel(model);
        ui.treeView->setHeaderHidden(true);
        ui.treeView->expandAll();

        QObject::connect(ui.treeView, &QTreeView::clicked, this,
            [kSkeletonIndexRole](const QModelIndex& idx)
            {
                bool ok = false;
                int skIndex = idx.data(kSkeletonIndexRole).toInt(&ok);
                if (ok && skIndex >= 0 && skIndex < KHS32_Count)
                {
                    qDebug() << "Skeleton:" << kinemHumanSkeleton32[skIndex].c_str();
                }
                else
                {
                    qDebug() << "Skeleton:" << idx.data(Qt::DisplayRole).toString();
                }
            });
    }
}

