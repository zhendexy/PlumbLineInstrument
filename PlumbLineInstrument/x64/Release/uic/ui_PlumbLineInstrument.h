/********************************************************************************
** Form generated from reading UI file 'PlumbLineInstrument.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLUMBLINEINSTRUMENT_H
#define UI_PLUMBLINEINSTRUMENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlumbLineInstrumentClass
{
public:
    QWidget *centralWidget;
    QLabel *labelLeft;
    QLabel *labelRight;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *startCameraBtn;
    QPushButton *saveFrameBtn;
    QPushButton *stopCameraBtn;
    QSpacerItem *verticalSpacer_3;
    QPushButton *OpenCVCaliBtn;
    QPushButton *planeCaliBtn;
    QPushButton *OpenCVParamsBtn;
    QPushButton *planeParamsBtn;
    QSpacerItem *verticalSpacer_2;
    QPushButton *startMeasureBtn;
    QPushButton *stopMeasureBtn;
    QSpacerItem *verticalSpacer;
    QPushButton *exitBtn;
    QTextBrowser *textBrowser;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_uv;
    QLabel *label_u;
    QLabel *label_v;
    QLabel *label_c;
    QLabel *label_xc;
    QLabel *label_yc;
    QLabel *label_zc;
    QWidget *layoutWidget1;
    QGridLayout *gridLayout_2;
    QLabel *label_laserL;
    QLabel *label_worldz;
    QLabel *label_L;
    QLabel *label_zzz;
    QLabel *label_h;
    QLabel *label_laserh;
    QLabel *label_cameray;
    QLabel *label_yyy;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *PlumbLineInstrumentClass)
    {
        if (PlumbLineInstrumentClass->objectName().isEmpty())
            PlumbLineInstrumentClass->setObjectName(QStringLiteral("PlumbLineInstrumentClass"));
        PlumbLineInstrumentClass->resize(1196, 801);
        centralWidget = new QWidget(PlumbLineInstrumentClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        labelLeft = new QLabel(centralWidget);
        labelLeft->setObjectName(QStringLiteral("labelLeft"));
        labelLeft->setGeometry(QRect(0, 0, 500, 500));
        labelRight = new QLabel(centralWidget);
        labelRight->setObjectName(QStringLiteral("labelRight"));
        labelRight->setGeometry(QRect(510, 0, 500, 500));
        verticalLayoutWidget = new QWidget(centralWidget);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(1030, 0, 160, 751));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        startCameraBtn = new QPushButton(verticalLayoutWidget);
        startCameraBtn->setObjectName(QStringLiteral("startCameraBtn"));

        verticalLayout->addWidget(startCameraBtn);

        saveFrameBtn = new QPushButton(verticalLayoutWidget);
        saveFrameBtn->setObjectName(QStringLiteral("saveFrameBtn"));

        verticalLayout->addWidget(saveFrameBtn);

        stopCameraBtn = new QPushButton(verticalLayoutWidget);
        stopCameraBtn->setObjectName(QStringLiteral("stopCameraBtn"));

        verticalLayout->addWidget(stopCameraBtn);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);

        OpenCVCaliBtn = new QPushButton(verticalLayoutWidget);
        OpenCVCaliBtn->setObjectName(QStringLiteral("OpenCVCaliBtn"));

        verticalLayout->addWidget(OpenCVCaliBtn);

        planeCaliBtn = new QPushButton(verticalLayoutWidget);
        planeCaliBtn->setObjectName(QStringLiteral("planeCaliBtn"));

        verticalLayout->addWidget(planeCaliBtn);

        OpenCVParamsBtn = new QPushButton(verticalLayoutWidget);
        OpenCVParamsBtn->setObjectName(QStringLiteral("OpenCVParamsBtn"));

        verticalLayout->addWidget(OpenCVParamsBtn);

        planeParamsBtn = new QPushButton(verticalLayoutWidget);
        planeParamsBtn->setObjectName(QStringLiteral("planeParamsBtn"));

        verticalLayout->addWidget(planeParamsBtn);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        startMeasureBtn = new QPushButton(verticalLayoutWidget);
        startMeasureBtn->setObjectName(QStringLiteral("startMeasureBtn"));

        verticalLayout->addWidget(startMeasureBtn);

        stopMeasureBtn = new QPushButton(verticalLayoutWidget);
        stopMeasureBtn->setObjectName(QStringLiteral("stopMeasureBtn"));

        verticalLayout->addWidget(stopMeasureBtn);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        exitBtn = new QPushButton(verticalLayoutWidget);
        exitBtn->setObjectName(QStringLiteral("exitBtn"));

        verticalLayout->addWidget(exitBtn);

        textBrowser = new QTextBrowser(centralWidget);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        textBrowser->setGeometry(QRect(0, 530, 421, 201));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(500, 540, 321, 91));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_uv = new QLabel(layoutWidget);
        label_uv->setObjectName(QStringLiteral("label_uv"));

        gridLayout->addWidget(label_uv, 0, 0, 1, 1);

        label_u = new QLabel(layoutWidget);
        label_u->setObjectName(QStringLiteral("label_u"));

        gridLayout->addWidget(label_u, 0, 1, 1, 1);

        label_v = new QLabel(layoutWidget);
        label_v->setObjectName(QStringLiteral("label_v"));

        gridLayout->addWidget(label_v, 0, 2, 1, 1);

        label_c = new QLabel(layoutWidget);
        label_c->setObjectName(QStringLiteral("label_c"));

        gridLayout->addWidget(label_c, 1, 0, 1, 1);

        label_xc = new QLabel(layoutWidget);
        label_xc->setObjectName(QStringLiteral("label_xc"));

        gridLayout->addWidget(label_xc, 1, 1, 1, 1);

        label_yc = new QLabel(layoutWidget);
        label_yc->setObjectName(QStringLiteral("label_yc"));

        gridLayout->addWidget(label_yc, 1, 2, 1, 1);

        label_zc = new QLabel(layoutWidget);
        label_zc->setObjectName(QStringLiteral("label_zc"));

        gridLayout->addWidget(label_zc, 1, 3, 1, 1);

        layoutWidget1 = new QWidget(centralWidget);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(510, 660, 351, 71));
        gridLayout_2 = new QGridLayout(layoutWidget1);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        label_laserL = new QLabel(layoutWidget1);
        label_laserL->setObjectName(QStringLiteral("label_laserL"));

        gridLayout_2->addWidget(label_laserL, 0, 0, 1, 1);

        label_worldz = new QLabel(layoutWidget1);
        label_worldz->setObjectName(QStringLiteral("label_worldz"));

        gridLayout_2->addWidget(label_worldz, 0, 3, 1, 1);

        label_L = new QLabel(layoutWidget1);
        label_L->setObjectName(QStringLiteral("label_L"));

        gridLayout_2->addWidget(label_L, 1, 0, 1, 1);

        label_zzz = new QLabel(layoutWidget1);
        label_zzz->setObjectName(QStringLiteral("label_zzz"));

        gridLayout_2->addWidget(label_zzz, 1, 3, 1, 1);

        label_h = new QLabel(layoutWidget1);
        label_h->setObjectName(QStringLiteral("label_h"));

        gridLayout_2->addWidget(label_h, 1, 1, 1, 1);

        label_laserh = new QLabel(layoutWidget1);
        label_laserh->setObjectName(QStringLiteral("label_laserh"));

        gridLayout_2->addWidget(label_laserh, 0, 1, 1, 1);

        label_cameray = new QLabel(layoutWidget1);
        label_cameray->setObjectName(QStringLiteral("label_cameray"));

        gridLayout_2->addWidget(label_cameray, 0, 2, 1, 1);

        label_yyy = new QLabel(layoutWidget1);
        label_yyy->setObjectName(QStringLiteral("label_yyy"));

        gridLayout_2->addWidget(label_yyy, 1, 2, 1, 1);

        PlumbLineInstrumentClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(PlumbLineInstrumentClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1196, 23));
        PlumbLineInstrumentClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(PlumbLineInstrumentClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        PlumbLineInstrumentClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(PlumbLineInstrumentClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        PlumbLineInstrumentClass->setStatusBar(statusBar);

        retranslateUi(PlumbLineInstrumentClass);

        QMetaObject::connectSlotsByName(PlumbLineInstrumentClass);
    } // setupUi

    void retranslateUi(QMainWindow *PlumbLineInstrumentClass)
    {
        PlumbLineInstrumentClass->setWindowTitle(QApplication::translate("PlumbLineInstrumentClass", "PlumbLineInstrument", Q_NULLPTR));
        labelLeft->setText(QApplication::translate("PlumbLineInstrumentClass", "\345\267\246\347\233\270\346\234\272", Q_NULLPTR));
        labelRight->setText(QApplication::translate("PlumbLineInstrumentClass", "\345\217\263\347\233\270\346\234\272", Q_NULLPTR));
        startCameraBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "\345\274\200\345\247\213\346\213\215\346\221\204", Q_NULLPTR));
        saveFrameBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "\344\277\235\345\255\230\345\275\223\345\211\215\345\270\247", Q_NULLPTR));
        stopCameraBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "\347\273\223\346\235\237\346\213\215\346\221\204", Q_NULLPTR));
        OpenCVCaliBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "OpenCV\347\233\270\346\234\272\346\240\207\345\256\232", Q_NULLPTR));
        planeCaliBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "\346\277\200\345\205\211\345\271\263\351\235\242\346\240\207\345\256\232", Q_NULLPTR));
        OpenCVParamsBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "\350\257\273\345\217\226OpenCV\347\233\270\346\234\272\346\240\207\345\256\232\346\225\260\346\215\256", Q_NULLPTR));
        planeParamsBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "\350\257\273\345\217\226\346\277\200\345\205\211\345\271\263\351\235\242\346\240\207\345\256\232\346\225\260\346\215\256", Q_NULLPTR));
        startMeasureBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "\345\274\200\345\247\213\346\265\213\351\207\217", Q_NULLPTR));
        stopMeasureBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "\347\273\223\346\235\237\346\265\213\351\207\217", Q_NULLPTR));
        exitBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "\351\200\200\345\207\272", Q_NULLPTR));
        label_uv->setText(QApplication::translate("PlumbLineInstrumentClass", "\345\233\276\345\203\217\345\235\220\346\240\207uv", Q_NULLPTR));
        label_u->setText(QString());
        label_v->setText(QString());
        label_c->setText(QApplication::translate("PlumbLineInstrumentClass", "\347\233\270\346\234\272\345\235\220\346\240\207xyz", Q_NULLPTR));
        label_xc->setText(QString());
        label_yc->setText(QString());
        label_zc->setText(QString());
        label_laserL->setText(QApplication::translate("PlumbLineInstrumentClass", "\346\277\200\345\205\211\347\272\277\351\225\277L", Q_NULLPTR));
        label_worldz->setText(QApplication::translate("PlumbLineInstrumentClass", "\344\270\226\347\225\214\345\235\220\346\240\207z", Q_NULLPTR));
        label_L->setText(QString());
        label_zzz->setText(QString());
        label_h->setText(QString());
        label_laserh->setText(QApplication::translate("PlumbLineInstrumentClass", "\350\267\235\351\241\266\347\202\271\351\253\230\345\272\246h", Q_NULLPTR));
        label_cameray->setText(QApplication::translate("PlumbLineInstrumentClass", "\347\233\270\346\234\272\345\235\220\346\240\207y", Q_NULLPTR));
        label_yyy->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class PlumbLineInstrumentClass: public Ui_PlumbLineInstrumentClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLUMBLINEINSTRUMENT_H
