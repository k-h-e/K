/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_GUI_QT_TREEVIEW_H_
#define K_GUI_QT_TREEVIEW_H_

#include <memory>
#include <QTreeView>

namespace K {
namespace Core {
    class Interface;
}
}

namespace Vectoid {
namespace SceneGraph {
    class Node;
}
}

namespace K {
//! Graphical user interface elements.
namespace Gui {
//! <c>Qt</c>-based UI elements.
namespace Qt {

//! Simple tree view.
class TreeView : public QTreeView {
    Q_OBJECT

  public:
    class TreeItem;

    TreeView(const QString &title, QWidget *parent);

    void Clear();
    TreeItem *RootItem();
    TreeItem *AddItem(const QString &label, bool checked, const std::shared_ptr<K::Core::Interface> &parameter,
                      TreeItem *parentItem);

  signals:
    void itemStateChanged(K::Core::Interface *item, bool checked, bool opened);

  private:
    class TreeModel;
    class Visitor;

    QString title_;
};

}    // Namespace Qt.
}    // Namespace Gui.
}    // Namespace K.

#endif    // K_GUI_QT_TREEVIEW_H_
