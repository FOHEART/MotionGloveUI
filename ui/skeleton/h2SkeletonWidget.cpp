/**
 * @file  h2SkeletonWidget.cpp
 * @brief 手部骨骼显示控件的实现。
 * @ingroup h2ui
 * @version 1.0.0
 *
 * @details
 * 控件样式统一由 CarbonUIClass 提供；界面布局由同名 .ui 文件生成。
 * 函数级说明见对应头文件，此处不再重复 @param / @return。
 */

#include "h2SkeletonWidget.h"
#include "common_define_glove_hw.h"
#include <QStandardItemModel>
#include <QAbstractItemView>
#include <QDebug>
#include "CarbonUIQTreeView.h"

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
	CarbonUIQTreeView::applyStyle(ui.treeView);
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

