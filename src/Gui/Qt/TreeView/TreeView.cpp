/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Gui/Qt/TreeView.h>

#include <K/Core/Interface.h>
#include "TreeItem.h"
#include "TreeModel.h"

using std::shared_ptr;
using K::Core::Interface;

namespace K {
namespace Gui {
namespace Qt {

TreeView::TreeView(const QString &title, QWidget *parent)
        : QTreeView(parent),
          title_(title) {
    Clear();
}

void TreeView::Clear() {
    auto model = new TreeModel(title_, this);
    setModel(model);
    connect(model, &TreeModel::itemStateChanged, this, &TreeView::itemStateChanged);
}

TreeView::TreeItem *TreeView::RootItem() {
    return static_cast<TreeModel *>(model())->RootItem();
}

TreeView::TreeItem *TreeView::AddItem(const QString &label, bool checked, const shared_ptr<Interface> &parameter,
                                      TreeItem *parentItem) {
    auto item = new TreeItem(label, parameter, parentItem);
    parentItem->AppendChild(item);
    item->SetChecked(checked);
    return item;
}

}    // Namespace Qt.
}    // Namespace Gui.
}    // Namespace K.
