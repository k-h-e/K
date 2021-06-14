#include "TreeModel.h"
#include "TreeItem.h"

#include <QStringList>
using std::shared_ptr;
using K::Core::Interface;

namespace K {
namespace UI {
namespace Qt {

TreeView::TreeModel::TreeModel(const QString &title, QObject *parent)
        : QAbstractItemModel(parent) {
    rootItem_ = new TreeItem(title, nullptr, nullptr);
}

TreeView::TreeModel::~TreeModel() {
    delete rootItem_;
}

TreeView::TreeItem *TreeView::TreeModel::RootItem() {
    return rootItem_;
}

int TreeView::TreeModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return static_cast<TreeItem*>(parent.internalPointer())->ColumnCount();
    }
    return rootItem_->ColumnCount();
}

bool TreeView::TreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (role == ::Qt::CheckStateRole) {
            bool checked = (value == ::Qt::Checked);
            item->SetChecked(checked);
            shared_ptr<Interface> parameter = item->parameter();    // Hold on to parameter!
            emit dataChanged(index, index);
            emit itemStateChanged(parameter.get(), checked, false);
            return true;
        }
    }

    return false;
}

QVariant TreeView::TreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    if ((role == ::Qt::CheckStateRole) && (index.column() == 0)) {
        return static_cast<int>(item->Checked() ? ::Qt::Checked : ::Qt::Unchecked);
    }

    if (role != ::Qt::DisplayRole) {
        return QVariant();
    }

    return item->Data(index.column());
}

::Qt::ItemFlags TreeView::TreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return ::Qt::NoItemFlags;
    }

    ::Qt::ItemFlags flags = ::Qt::ItemIsEnabled | ::Qt::ItemIsSelectable;
    if (index.column() == 0) {
        flags |= ::Qt::ItemIsUserCheckable;
    }
    return flags;
}

QVariant TreeView::TreeModel::headerData(int section, ::Qt::Orientation orientation, int role) const {
    if ((orientation == ::Qt::Horizontal) && (role == ::Qt::DisplayRole)) {
        return rootItem_->Data(section);
    }
    return QVariant();
}

QModelIndex TreeView::TreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    TreeItem *parentItem;
    if (!parent.isValid()) {
        parentItem = rootItem_;
    }
    else {
        parentItem = static_cast<TreeItem*>(parent.internalPointer());
    }

    TreeItem *childItem = parentItem->Child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex TreeView::TreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) {
        return QModelIndex();
    }

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->ParentItem();
    if (parentItem == rootItem_) {
        return QModelIndex();
    }
    return createIndex(parentItem->Row(), 0, parentItem);
}

int TreeView::TreeModel::rowCount(const QModelIndex &parent) const {
    TreeItem *parentItem;
    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) {
        parentItem = rootItem_;
    }
    else {
        parentItem = static_cast<TreeItem*>(parent.internalPointer());
    }
    return parentItem->ChildCount();
}

}    // Namespace Qt.
}    // Namespace UI.
}    // Namespace K.
