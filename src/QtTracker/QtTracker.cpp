
#include "treemodel.h"

#include <QApplication>
#include <QFile>
#include <QTreeView>

#include "QtTracker.h"

void QtTracker::do_insert_child()
{
	tracker_model->insertRows(2, 1);
}

void QtTracker::expandItem(const QModelIndex &index)
{
	ui.treeView_2->isExpanded(index) ? ui.treeView_2->collapse(index) : ui.treeView_2->expand(index);
}

void QtTracker::hideItem()
{
	QModelIndex cur = ui.treeView_2->currentIndex();
	ui.treeView_2->setRowHidden(cur.row(), cur.parent(), true);

}

QtTracker::QtTracker(QWidget *parent)
	: QMainWindow(parent)
{
	
	ui.setupUi(this);
	
#if 0
	QObject::connect(
		ui.treeView_2, SIGNAL(clicked(const QModelIndex &)),
		this, SLOT(expandItem(const QModelIndex &))
		
	);
	QObject::connect(
		ui.treeView_2, SIGNAL(expandItem(const QModelIndex &)),
		this, SLOT(expandItem(const QModelIndex &))
	);
#endif
	


	vr_state_tracker_t tracker = load_vrstate_from_file("C:\\vr_streams\\unit_test1.bin");
	TrackerNodeIF * nodes = get_node_if(tracker);

	tracker_model = new TrackerTreeModel(nodes);
	ui.treeView_2->setModel(tracker_model);
	
	ui.treeView_2->expandAll();

	ui.treeView_2->resizeColumnToContents(0);


	
}
