#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtTracker.h"
#include "TrackerTreeModel.h"

class QtTracker : public QMainWindow
{
    Q_OBJECT

public:
    QtTracker(QWidget *parent = Q_NULLPTR);

public slots:
	void do_insert_child();
	void expandItem(const QModelIndex &index);
	void hideItem();

private:
    Ui::QtTrackerClass ui;

	TrackerNodeIF *nodes;
	TrackerTreeModel *tracker_model;
};
