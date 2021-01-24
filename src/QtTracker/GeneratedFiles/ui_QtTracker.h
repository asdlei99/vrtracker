/********************************************************************************
** Form generated from reading UI file 'QtTracker.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTTRACKER_H
#define UI_QTTRACKER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtTrackerClass
{
public:
    QWidget *centralWidget;
    QTreeView *treeView_2;
    QPushButton *pushButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtTrackerClass)
    {
        if (QtTrackerClass->objectName().isEmpty())
            QtTrackerClass->setObjectName(QStringLiteral("QtTrackerClass"));
        QtTrackerClass->resize(1023, 715);
        centralWidget = new QWidget(QtTrackerClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        treeView_2 = new QTreeView(centralWidget);
        treeView_2->setObjectName(QStringLiteral("treeView_2"));
        treeView_2->setGeometry(QRect(0, 10, 771, 561));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(820, 80, 75, 23));
        QtTrackerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QtTrackerClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1023, 21));
        QtTrackerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtTrackerClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QtTrackerClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QtTrackerClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtTrackerClass->setStatusBar(statusBar);

        retranslateUi(QtTrackerClass);
        QObject::connect(pushButton, SIGNAL(clicked()), QtTrackerClass, SLOT(hideItem()));

        QMetaObject::connectSlotsByName(QtTrackerClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtTrackerClass)
    {
        QtTrackerClass->setWindowTitle(QApplication::translate("QtTrackerClass", "QtTracker", Q_NULLPTR));
        pushButton->setText(QApplication::translate("QtTrackerClass", "PushButton", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class QtTrackerClass: public Ui_QtTrackerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTTRACKER_H
