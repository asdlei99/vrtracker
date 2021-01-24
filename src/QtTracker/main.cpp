#include "QtTracker.h"
#include "treemodel.h"
#include <QApplication>
#include <QFile>
#include <QTreeView>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	QtTracker tracker;
	tracker.show();
	
	return app.exec();


}
