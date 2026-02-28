#include "h2SkeletonWidget.h"
#include "common_define_glove_hw.h"
#include <QStandardItemModel>
#include <QAbstractItemView>

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
    // Ensure tree items also inherit white text and style selected state
    if (ui.treeView)
    {
        // Use resource images for branch indicators
        ui.treeView->setStyleSheet(R"(
            QTreeView { background-color: transparent; color: white; border: none; }
            QTreeView::item { color: white; border: none; }
            QTreeView::item:selected { background-color: #2e2e2e; color: white; border: none; }
            QTreeView::item:selected:active { background-color: #2e2e2e; color: white; }
            QTreeView::item:focus { outline: none; }
            QTreeView::branch { background: transparent; }
            /* Closed (right-pointing) arrow - white image resource */
            QTreeView::branch:closed:has-children {
                image: url(":/images/Resources/img/arrow_right.png");
                width: 24px; height: 24px; }
            /* Open (down-pointing) arrow - white image resource */
            QTreeView::branch:open:has-children {
                image: url(":/images/Resources/img/arrow_down.png");
                width: 24px; height: 24px; }
        )");

        // Ensure the view shows branch indicators and can expand
        ui.treeView->setRootIsDecorated(true);
        ui.treeView->setItemsExpandable(true);
        ui.treeView->setExpandsOnDoubleClick(true);
        // Ensure there is space for the branch icon
        ui.treeView->setIndentation(14);

        // Disable editing via the view
        ui.treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

void h2SkeletonWidget::populateSkeletonTree()
{
    // Create tree model and populate using KHS32_SkeletonIndex entries
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels(QStringList() << tr("Skeleton"));

    QStandardItem* rightHandItem = new QStandardItem(QString::fromStdString(kinemHumanSkeleton32[KHS32_RightHand]));
    rightHandItem->setEditable(false);
    // add children between RightHand and LeftHand
    for (int i = KHS32_RightHand + 1; i < KHS32_LeftHand; ++i)
    {
        QStandardItem* child = new QStandardItem(QString::fromStdString(kinemHumanSkeleton32[i]));
        child->setEditable(false);
        rightHandItem->appendRow(child);
    }

    QStandardItem* leftHandItem = new QStandardItem(QString::fromStdString(kinemHumanSkeleton32[KHS32_LeftHand]));
    leftHandItem->setEditable(false);
    // add children between LeftHand and end
    for (int i = KHS32_LeftHand + 1; i < KHS32_Count; ++i)
    {
        QStandardItem* child = new QStandardItem(QString::fromStdString(kinemHumanSkeleton32[i]));
        child->setEditable(false);
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
    }
}

