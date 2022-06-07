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
    QPushButton *levelCaliBtn;
    QPushButton *OpenCVParamsBtn;
    QPushButton *planeParamsBtn;
    QSpacerItem *verticalSpacer_2;
    QPushButton *startMeasureBtn;
    QPushButton *stopMeasureBtn;
    QPushButton *testBtn;
    QSpacerItem *verticalSpacer;
    QPushButton *exitBtn;
    QTextBrowser *textBrowser;
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_c;
    QLabel *label_yc1;
    QLabel *label_uv;
    QLabel *label_yc;
    QLabel *label_zc;
    QLabel *label_v;
    QLabel *label_11;
    QLabel *label_zc1;
    QLabel *label_w;
    QLabel *label_xc;
    QLabel *label_u;
    QLabel *label_xc1;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_u1;
    QLabel *label_v1;
    QLabel *label_u2;
    QLabel *label_v2;
    QLabel *label_3;
    QLabel *label_d;
    QWidget *layoutWidget1;
    QGridLayout *gridLayout_2;
    QLabel *label_ulvl;
    QLabel *label_ul;
    QLabel *label_xw1;
    QLabel *label_vr;
    QLabel *label_yw1;
    QLabel *label_xwywzw1;
    QLabel *label_urvr;
    QLabel *label_vl;
    QLabel *label_ur;
    QLabel *label_zw1;
    QLabel *label_12;
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

        levelCaliBtn = new QPushButton(verticalLayoutWidget);
        levelCaliBtn->setObjectName(QStringLiteral("levelCaliBtn"));

        verticalLayout->addWidget(levelCaliBtn);

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

        testBtn = new QPushButton(verticalLayoutWidget);
        testBtn->setObjectName(QStringLiteral("testBtn"));

        verticalLayout->addWidget(testBtn);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        exitBtn = new QPushButton(verticalLayoutWidget);
        exitBtn->setObjectName(QStringLiteral("exitBtn"));

        verticalLayout->addWidget(exitBtn);

        textBrowser = new QTextBrowser(centralWidget);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        textBrowser->setGeometry(QRect(0, 530, 341, 201));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(350, 530, 311, 201));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_c = new QLabel(layoutWidget);
        label_c->setObjectName(QStringLiteral("label_c"));
        label_c->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_c, 4, 0, 1, 1);

        label_yc1 = new QLabel(layoutWidget);
        label_yc1->setObjectName(QStringLiteral("label_yc1"));

        gridLayout->addWidget(label_yc1, 5, 2, 1, 1);

        label_uv = new QLabel(layoutWidget);
        label_uv->setObjectName(QStringLiteral("label_uv"));
        label_uv->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_uv, 1, 0, 1, 1);

        label_yc = new QLabel(layoutWidget);
        label_yc->setObjectName(QStringLiteral("label_yc"));

        gridLayout->addWidget(label_yc, 4, 2, 1, 1);

        label_zc = new QLabel(layoutWidget);
        label_zc->setObjectName(QStringLiteral("label_zc"));

        gridLayout->addWidget(label_zc, 4, 3, 1, 1);

        label_v = new QLabel(layoutWidget);
        label_v->setObjectName(QStringLiteral("label_v"));

        gridLayout->addWidget(label_v, 1, 2, 1, 1);

        label_11 = new QLabel(layoutWidget);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_11, 0, 0, 1, 4);

        label_zc1 = new QLabel(layoutWidget);
        label_zc1->setObjectName(QStringLiteral("label_zc1"));

        gridLayout->addWidget(label_zc1, 5, 3, 1, 1);

        label_w = new QLabel(layoutWidget);
        label_w->setObjectName(QStringLiteral("label_w"));
        label_w->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_w, 5, 0, 1, 1);

        label_xc = new QLabel(layoutWidget);
        label_xc->setObjectName(QStringLiteral("label_xc"));

        gridLayout->addWidget(label_xc, 4, 1, 1, 1);

        label_u = new QLabel(layoutWidget);
        label_u->setObjectName(QStringLiteral("label_u"));

        gridLayout->addWidget(label_u, 1, 1, 1, 1);

        label_xc1 = new QLabel(layoutWidget);
        label_xc1->setObjectName(QStringLiteral("label_xc1"));

        gridLayout->addWidget(label_xc1, 5, 1, 1, 1);

        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 2, 0, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_2, 3, 0, 1, 1);

        label_u1 = new QLabel(layoutWidget);
        label_u1->setObjectName(QStringLiteral("label_u1"));

        gridLayout->addWidget(label_u1, 2, 1, 1, 1);

        label_v1 = new QLabel(layoutWidget);
        label_v1->setObjectName(QStringLiteral("label_v1"));

        gridLayout->addWidget(label_v1, 2, 2, 1, 1);

        label_u2 = new QLabel(layoutWidget);
        label_u2->setObjectName(QStringLiteral("label_u2"));

        gridLayout->addWidget(label_u2, 3, 1, 1, 1);

        label_v2 = new QLabel(layoutWidget);
        label_v2->setObjectName(QStringLiteral("label_v2"));

        gridLayout->addWidget(label_v2, 3, 2, 1, 1);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_3, 2, 3, 1, 1);

        label_d = new QLabel(layoutWidget);
        label_d->setObjectName(QStringLiteral("label_d"));
        label_d->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_d, 3, 3, 1, 1);

        layoutWidget1 = new QWidget(centralWidget);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(670, 530, 351, 131));
        gridLayout_2 = new QGridLayout(layoutWidget1);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        label_ulvl = new QLabel(layoutWidget1);
        label_ulvl->setObjectName(QStringLiteral("label_ulvl"));
        label_ulvl->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_ulvl, 1, 0, 1, 1);

        label_ul = new QLabel(layoutWidget1);
        label_ul->setObjectName(QStringLiteral("label_ul"));

        gridLayout_2->addWidget(label_ul, 1, 1, 1, 1);

        label_xw1 = new QLabel(layoutWidget1);
        label_xw1->setObjectName(QStringLiteral("label_xw1"));

        gridLayout_2->addWidget(label_xw1, 3, 1, 1, 1);

        label_vr = new QLabel(layoutWidget1);
        label_vr->setObjectName(QStringLiteral("label_vr"));

        gridLayout_2->addWidget(label_vr, 2, 2, 1, 1);

        label_yw1 = new QLabel(layoutWidget1);
        label_yw1->setObjectName(QStringLiteral("label_yw1"));

        gridLayout_2->addWidget(label_yw1, 3, 2, 1, 1);

        label_xwywzw1 = new QLabel(layoutWidget1);
        label_xwywzw1->setObjectName(QStringLiteral("label_xwywzw1"));
        label_xwywzw1->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_xwywzw1, 3, 0, 1, 1);

        label_urvr = new QLabel(layoutWidget1);
        label_urvr->setObjectName(QStringLiteral("label_urvr"));
        label_urvr->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_urvr, 2, 0, 1, 1);

        label_vl = new QLabel(layoutWidget1);
        label_vl->setObjectName(QStringLiteral("label_vl"));

        gridLayout_2->addWidget(label_vl, 1, 2, 1, 1);

        label_ur = new QLabel(layoutWidget1);
        label_ur->setObjectName(QStringLiteral("label_ur"));

        gridLayout_2->addWidget(label_ur, 2, 1, 1, 1);

        label_zw1 = new QLabel(layoutWidget1);
        label_zw1->setObjectName(QStringLiteral("label_zw1"));

        gridLayout_2->addWidget(label_zw1, 3, 3, 1, 1);

        label_12 = new QLabel(layoutWidget1);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_12, 0, 0, 1, 4);

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
        levelCaliBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "\346\260\264\345\271\263\351\235\242\346\240\207\345\256\232", Q_NULLPTR));
        OpenCVParamsBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "\350\257\273\345\217\226OpenCV\347\233\270\346\234\272\346\240\207\345\256\232\346\225\260\346\215\256", Q_NULLPTR));
        planeParamsBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "\350\257\273\345\217\226\346\277\200\345\205\211\345\271\263\351\235\242\346\240\207\345\256\232\346\225\260\346\215\256", Q_NULLPTR));
        startMeasureBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "\345\274\200\345\247\213\346\265\213\351\207\217", Q_NULLPTR));
        stopMeasureBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "\347\273\223\346\235\237\346\265\213\351\207\217", Q_NULLPTR));
        testBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "\346\265\213\350\257\225\344\273\243\347\240\201", Q_NULLPTR));
        exitBtn->setText(QApplication::translate("PlumbLineInstrumentClass", "\351\200\200\345\207\272", Q_NULLPTR));
        label_c->setText(QApplication::translate("PlumbLineInstrumentClass", "\345\205\211\347\202\271\347\233\270\346\234\272\345\235\220\346\240\207", Q_NULLPTR));
        label_yc1->setText(QString());
        label_uv->setText(QApplication::translate("PlumbLineInstrumentClass", "\345\205\211\347\202\271\345\233\276\345\203\217\345\235\220\346\240\207uv", Q_NULLPTR));
        label_yc->setText(QString());
        label_zc->setText(QString());
        label_v->setText(QString());
        label_11->setText(QApplication::translate("PlumbLineInstrumentClass", "\346\277\200\345\205\211\344\270\211\350\247\222\346\263\225\350\256\241\347\256\227\346\225\260\346\215\256", Q_NULLPTR));
        label_zc1->setText(QString());
        label_w->setText(QApplication::translate("PlumbLineInstrumentClass", "\345\260\217\347\220\2031\347\233\270\346\234\272\345\235\220\346\240\207", Q_NULLPTR));
        label_xc->setText(QString());
        label_u->setText(QString());
        label_xc1->setText(QString());
        label->setText(QApplication::translate("PlumbLineInstrumentClass", "\345\260\217\347\220\2031\345\233\276\345\203\217\345\235\220\346\240\207", Q_NULLPTR));
        label_2->setText(QApplication::translate("PlumbLineInstrumentClass", "\345\260\217\347\220\2032\345\233\276\345\203\217\345\235\220\346\240\207", Q_NULLPTR));
        label_u1->setText(QString());
        label_v1->setText(QString());
        label_u2->setText(QString());
        label_v2->setText(QString());
        label_3->setText(QApplication::translate("PlumbLineInstrumentClass", "\350\267\235\347\246\273", Q_NULLPTR));
        label_d->setText(QString());
        label_ulvl->setText(QApplication::translate("PlumbLineInstrumentClass", "\345\267\246\345\233\276\345\203\217\345\235\220\346\240\207ulvl", Q_NULLPTR));
        label_ul->setText(QString());
        label_xw1->setText(QString());
        label_vr->setText(QString());
        label_yw1->setText(QString());
        label_xwywzw1->setText(QApplication::translate("PlumbLineInstrumentClass", "\344\270\226\347\225\214\345\235\220\346\240\207xwywzw", Q_NULLPTR));
        label_urvr->setText(QApplication::translate("PlumbLineInstrumentClass", "\345\217\263\345\233\276\345\203\217\345\235\220\346\240\207urvr", Q_NULLPTR));
        label_vl->setText(QString());
        label_ur->setText(QString());
        label_zw1->setText(QString());
        label_12->setText(QApplication::translate("PlumbLineInstrumentClass", "\345\260\217\347\220\203\345\217\214\347\233\256\350\256\241\347\256\227\346\225\260\346\215\256", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class PlumbLineInstrumentClass: public Ui_PlumbLineInstrumentClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLUMBLINEINSTRUMENT_H
