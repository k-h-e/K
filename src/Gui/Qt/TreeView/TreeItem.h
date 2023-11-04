/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_GUI_QT_TREEVIEW_TREEITEM_H
#define K_GUI_QT_TREEVIEW_TREEITEM_H

#include <memory>
#include <QVector>
#include <K/Gui/Qt/TreeView.h>

namespace K {
namespace Gui {
namespace Qt {

//! Adapted from Qt tree view example.
class TreeView::TreeItem {
  public:
    explicit TreeItem(const QString &label, const std::shared_ptr<K::Core::Interface> &parameter, TreeItem *parentItem);
    TreeItem(const TreeItem &other)            = delete;
    TreeItem &operator=(const TreeItem &other) = delete;
    TreeItem(TreeItem &&other)                 = delete;
    TreeItem &operator=(TreeItem &&other)      = delete;
    ~TreeItem();

    void AppendChild(TreeItem *child);
    void SetChecked(bool checked);
    bool Checked();
    std::shared_ptr<K::Core::Interface> parameter();

    TreeItem *Child(int row);
    int ChildCount() const;
    int ColumnCount() const;
    QVariant Data(int column) const;
    int Row() const;
    TreeItem *ParentItem();

  private:
    TreeItem                                   *parent_;
    QVector<TreeItem*>                         children_;
    QString                                    label_;
    std::shared_ptr<K::Core::Interface>        parameter_;
    bool                                       checked_;
};

}    // Namespace Qt.
}    // Namespace Gui.
}    // Namespace K.

#endif    // K_GUI_QT_TREEVIEW_TREEITEM_H
