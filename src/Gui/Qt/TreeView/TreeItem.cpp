/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "TreeItem.h"

#include <K/Core/StringTools.h>

using std::shared_ptr;
using K::Core::Interface;

namespace K {
namespace Gui {
namespace Qt {

TreeView::TreeItem::TreeItem(const QString &label, const shared_ptr<Interface> &parameter, TreeItem *parent)
        : parent_(parent),
          label_(label),
          parameter_(parameter),
          checked_(false) {
    // Nop.
}

TreeView::TreeItem::~TreeItem() {
    qDeleteAll(children_);
}

void TreeView::TreeItem::AppendChild(TreeItem *item) {
    children_.append(item);
}

void TreeView::TreeItem::SetChecked(bool checked) {
    checked_ = checked;
}

bool TreeView::TreeItem::Checked() {
    return checked_;
}

shared_ptr<Interface> TreeView::TreeItem::parameter() {
    return parameter_;
}

TreeView::TreeItem *TreeView::TreeItem::Child(int row) {
    if (row < 0 || row >= children_.size()) {
        return nullptr;
    }
    return children_.at(row);
}

int TreeView::TreeItem::ChildCount() const {
    return children_.count();
}

int TreeView::TreeItem::ColumnCount() const {
    return 1;
}

QVariant TreeView::TreeItem::Data(int column) const {
    if (column == 0) {
        return label_;
    }

    return QVariant();
}

TreeView::TreeItem *TreeView::TreeItem::ParentItem() {
    return parent_;
}

int TreeView::TreeItem::Row() const {
    if (parent_) {
        return parent_->children_.indexOf(const_cast<TreeItem*>(this));
    }
    return 0;
}

}    // Namespace Qt.
}    // Namespace Gui.
}    // Namespace K.
