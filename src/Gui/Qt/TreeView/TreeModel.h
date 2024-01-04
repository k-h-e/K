/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_GUI_QT_TREEVIEW_TREEMODEL_H
#define K_GUI_QT_TREEVIEW_TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <K/Gui/Qt/TreeView.h>

namespace K {
namespace Core {
    class Interface;
}
}

namespace K {
namespace Gui {
namespace Qt {

//! Adapted from Qt tree view example.
class TreeView::TreeModel : public QAbstractItemModel {
    Q_OBJECT

  public:
    explicit TreeModel(const QString &title, QObject *parent);
    TreeModel(const TreeModel &other)            = delete;
    TreeModel &operator=(const TreeModel &other) = delete;
    TreeModel(TreeModel &&other)                 = delete;
    TreeModel &operator=(TreeModel &&other)      = delete;
    ~TreeModel();

    TreeItem *RootItem();
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant data(const QModelIndex &index, int role) const override;
    ::Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, ::Qt::Orientation orientation, int role = ::Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  signals:
    void itemStateChanged(K::Core::Interface *item, bool checked, bool opened);

  private:
    TreeItem *rootItem_;
};

}    // Namespace Qt.
}    // Namespace Gui.
}    // Namespace K.

#endif    // K_GUI_QT_TREEVIEW_TREEMODEL_H
