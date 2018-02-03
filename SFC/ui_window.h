/********************************************************************************
** Form generated from reading UI file 'window.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "canvas.h"

QT_BEGIN_NAMESPACE

class Ui_Window
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QPushButton *stop_start_b;
    QFrame *line_11;
    QCheckBox *with_axes;
    QLabel *label;
    QSlider *horizontalSlider;
    QFrame *line;
    QLabel *label_2;
    QSlider *horizontalSlider_2;
    QFrame *line_2;
    QLabel *label_3;
    QPushButton *background_color;
    QFrame *line_3;
    QCheckBox *checkBox;
    QLabel *label_4;
    QPushButton *equilibrium_color;
    QFrame *line_4;
    QLabel *label_5;
    QGridLayout *gridLayout;
    QPushButton *first_color;
    QPushButton *second_color;
    QFrame *line_5;
    QFrame *line_10;
    QGridLayout *gridLayout_3;
    QFrame *line_7;
    QFormLayout *formLayout;
    QLabel *label_6;
    QLineEdit *p_value;
    QLabel *label_8;
    QLineEdit *b_value;
    QLabel *label_7;
    QLineEdit *r_value;
    QLabel *label_15;
    QLineEdit *start_x;
    QLabel *label_16;
    QLineEdit *start_y;
    QLabel *label_17;
    QLineEdit *start_z;
    QLabel *label_18;
    QLineEdit *step_size;
    QLabel *label_9;
    QComboBox *method;
    QFrame *line_9;
    QPushButton *pushButton_2;
    QSpacerItem *verticalSpacer;
    QFrame *line_8;
    QPushButton *pushButton;
    QFrame *line_6;
    Canvas *canvas;

    void setupUi(QMainWindow *Window)
    {
        if (Window->objectName().isEmpty())
            Window->setObjectName(QStringLiteral("Window"));
        Window->setWindowModality(Qt::ApplicationModal);
        Window->resize(1289, 951);
        centralWidget = new QWidget(Window);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        stop_start_b = new QPushButton(centralWidget);
        stop_start_b->setObjectName(QStringLiteral("stop_start_b"));
        QIcon icon;
        icon.addFile(QStringLiteral("../../../usr/share/icons/Adwaita/16x16/actions/media-playback-start.png"), QSize(), QIcon::Normal, QIcon::On);
        stop_start_b->setIcon(icon);

        verticalLayout->addWidget(stop_start_b, 0, Qt::AlignHCenter|Qt::AlignVCenter);

        line_11 = new QFrame(centralWidget);
        line_11->setObjectName(QStringLiteral("line_11"));
        line_11->setFrameShape(QFrame::HLine);
        line_11->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_11);

        with_axes = new QCheckBox(centralWidget);
        with_axes->setObjectName(QStringLiteral("with_axes"));
        with_axes->setChecked(true);

        verticalLayout->addWidget(with_axes);

        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        horizontalSlider = new QSlider(centralWidget);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(horizontalSlider->sizePolicy().hasHeightForWidth());
        horizontalSlider->setSizePolicy(sizePolicy);
        horizontalSlider->setMinimum(1);
        horizontalSlider->setMaximum(100);
        horizontalSlider->setValue(20);
        horizontalSlider->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(horizontalSlider);

        line = new QFrame(centralWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout->addWidget(label_2);

        horizontalSlider_2 = new QSlider(centralWidget);
        horizontalSlider_2->setObjectName(QStringLiteral("horizontalSlider_2"));
        sizePolicy.setHeightForWidth(horizontalSlider_2->sizePolicy().hasHeightForWidth());
        horizontalSlider_2->setSizePolicy(sizePolicy);
        horizontalSlider_2->setMinimum(1);
        horizontalSlider_2->setMaximum(100);
        horizontalSlider_2->setValue(10);
        horizontalSlider_2->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(horizontalSlider_2);

        line_2 = new QFrame(centralWidget);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_2);

        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout->addWidget(label_3);

        background_color = new QPushButton(centralWidget);
        background_color->setObjectName(QStringLiteral("background_color"));
        background_color->setAutoFillBackground(false);

        verticalLayout->addWidget(background_color);

        line_3 = new QFrame(centralWidget);
        line_3->setObjectName(QStringLiteral("line_3"));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_3);

        checkBox = new QCheckBox(centralWidget);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        verticalLayout->addWidget(checkBox);

        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        verticalLayout->addWidget(label_4);

        equilibrium_color = new QPushButton(centralWidget);
        equilibrium_color->setObjectName(QStringLiteral("equilibrium_color"));

        verticalLayout->addWidget(equilibrium_color);

        line_4 = new QFrame(centralWidget);
        line_4->setObjectName(QStringLiteral("line_4"));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_4);

        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        verticalLayout->addWidget(label_5);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(-1, 0, -1, -1);
        first_color = new QPushButton(centralWidget);
        first_color->setObjectName(QStringLiteral("first_color"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(first_color->sizePolicy().hasHeightForWidth());
        first_color->setSizePolicy(sizePolicy1);
        first_color->setAcceptDrops(false);

        gridLayout->addWidget(first_color, 1, 0, 1, 1, Qt::AlignHCenter);

        second_color = new QPushButton(centralWidget);
        second_color->setObjectName(QStringLiteral("second_color"));

        gridLayout->addWidget(second_color, 1, 1, 1, 1, Qt::AlignHCenter);


        verticalLayout->addLayout(gridLayout);

        line_5 = new QFrame(centralWidget);
        line_5->setObjectName(QStringLiteral("line_5"));
        line_5->setLineWidth(3);
        line_5->setFrameShape(QFrame::HLine);
        line_5->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_5);

        line_10 = new QFrame(centralWidget);
        line_10->setObjectName(QStringLiteral("line_10"));
        line_10->setFrameShape(QFrame::HLine);
        line_10->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_10);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setSpacing(6);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setContentsMargins(0, 0, -1, -1);
        line_7 = new QFrame(centralWidget);
        line_7->setObjectName(QStringLiteral("line_7"));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(239, 41, 41, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(255, 147, 147, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush2);
        QBrush brush3(QColor(247, 94, 94, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush3);
        QBrush brush4(QColor(119, 20, 20, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
        QBrush brush5(QColor(159, 27, 27, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        QBrush brush6(QColor(255, 255, 255, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush6);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush);
        QBrush brush7(QColor(247, 148, 148, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush7);
        QBrush brush8(QColor(255, 255, 220, 255));
        brush8.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush8);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush7);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush8);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush8);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        line_7->setPalette(palette);
        line_7->setLineWidth(5);
        line_7->setFrameShape(QFrame::VLine);
        line_7->setFrameShadow(QFrame::Sunken);

        gridLayout_3->addWidget(line_7, 0, 0, 1, 1);

        formLayout = new QFormLayout();
        formLayout->setSpacing(6);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setContentsMargins(0, 0, -1, -1);
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_6);

        p_value = new QLineEdit(centralWidget);
        p_value->setObjectName(QStringLiteral("p_value"));
        sizePolicy.setHeightForWidth(p_value->sizePolicy().hasHeightForWidth());
        p_value->setSizePolicy(sizePolicy);

        formLayout->setWidget(0, QFormLayout::FieldRole, p_value);

        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QStringLiteral("label_8"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_8);

        b_value = new QLineEdit(centralWidget);
        b_value->setObjectName(QStringLiteral("b_value"));
        sizePolicy.setHeightForWidth(b_value->sizePolicy().hasHeightForWidth());
        b_value->setSizePolicy(sizePolicy);

        formLayout->setWidget(1, QFormLayout::FieldRole, b_value);

        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QStringLiteral("label_7"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_7);

        r_value = new QLineEdit(centralWidget);
        r_value->setObjectName(QStringLiteral("r_value"));
        sizePolicy.setHeightForWidth(r_value->sizePolicy().hasHeightForWidth());
        r_value->setSizePolicy(sizePolicy);

        formLayout->setWidget(2, QFormLayout::FieldRole, r_value);

        label_15 = new QLabel(centralWidget);
        label_15->setObjectName(QStringLiteral("label_15"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_15);

        start_x = new QLineEdit(centralWidget);
        start_x->setObjectName(QStringLiteral("start_x"));
        sizePolicy.setHeightForWidth(start_x->sizePolicy().hasHeightForWidth());
        start_x->setSizePolicy(sizePolicy);

        formLayout->setWidget(3, QFormLayout::FieldRole, start_x);

        label_16 = new QLabel(centralWidget);
        label_16->setObjectName(QStringLiteral("label_16"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_16);

        start_y = new QLineEdit(centralWidget);
        start_y->setObjectName(QStringLiteral("start_y"));
        sizePolicy.setHeightForWidth(start_y->sizePolicy().hasHeightForWidth());
        start_y->setSizePolicy(sizePolicy);

        formLayout->setWidget(4, QFormLayout::FieldRole, start_y);

        label_17 = new QLabel(centralWidget);
        label_17->setObjectName(QStringLiteral("label_17"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_17);

        start_z = new QLineEdit(centralWidget);
        start_z->setObjectName(QStringLiteral("start_z"));
        sizePolicy.setHeightForWidth(start_z->sizePolicy().hasHeightForWidth());
        start_z->setSizePolicy(sizePolicy);

        formLayout->setWidget(5, QFormLayout::FieldRole, start_z);

        label_18 = new QLabel(centralWidget);
        label_18->setObjectName(QStringLiteral("label_18"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_18);

        step_size = new QLineEdit(centralWidget);
        step_size->setObjectName(QStringLiteral("step_size"));
        sizePolicy.setHeightForWidth(step_size->sizePolicy().hasHeightForWidth());
        step_size->setSizePolicy(sizePolicy);

        formLayout->setWidget(6, QFormLayout::FieldRole, step_size);

        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QStringLiteral("label_9"));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_9);

        method = new QComboBox(centralWidget);
        method->setObjectName(QStringLiteral("method"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(method->sizePolicy().hasHeightForWidth());
        method->setSizePolicy(sizePolicy2);

        formLayout->setWidget(7, QFormLayout::FieldRole, method);


        gridLayout_3->addLayout(formLayout, 0, 1, 1, 1);


        verticalLayout->addLayout(gridLayout_3);

        line_9 = new QFrame(centralWidget);
        line_9->setObjectName(QStringLiteral("line_9"));
        line_9->setFrameShape(QFrame::HLine);
        line_9->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_9);

        pushButton_2 = new QPushButton(centralWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        verticalLayout->addWidget(pushButton_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        line_8 = new QFrame(centralWidget);
        line_8->setObjectName(QStringLiteral("line_8"));
        line_8->setFrameShape(QFrame::HLine);
        line_8->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line_8);

        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        verticalLayout->addWidget(pushButton);


        horizontalLayout->addLayout(verticalLayout);

        line_6 = new QFrame(centralWidget);
        line_6->setObjectName(QStringLiteral("line_6"));
        line_6->setFrameShape(QFrame::VLine);
        line_6->setFrameShadow(QFrame::Sunken);

        horizontalLayout->addWidget(line_6);

        canvas = new Canvas(centralWidget);
        canvas->setObjectName(QStringLiteral("canvas"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(canvas->sizePolicy().hasHeightForWidth());
        canvas->setSizePolicy(sizePolicy3);
        canvas->setFocusPolicy(Qt::StrongFocus);

        horizontalLayout->addWidget(canvas);

        Window->setCentralWidget(centralWidget);
        QWidget::setTabOrder(canvas, pushButton);

        retranslateUi(Window);
        QObject::connect(pushButton, SIGNAL(clicked()), Window, SLOT(close()));
        QObject::connect(with_axes, SIGNAL(stateChanged(int)), Window, SLOT(toggle_axis(int)));
        QObject::connect(horizontalSlider, SIGNAL(valueChanged(int)), Window, SLOT(resize_axis(int)));
        QObject::connect(horizontalSlider_2, SIGNAL(valueChanged(int)), Window, SLOT(drawing_speed(int)));
        QObject::connect(background_color, SIGNAL(clicked()), Window, SLOT(background_color()));
        QObject::connect(checkBox, SIGNAL(stateChanged(int)), Window, SLOT(toggle_equilibrium(int)));
        QObject::connect(equilibrium_color, SIGNAL(clicked()), Window, SLOT(equilibrium_color()));
        QObject::connect(first_color, SIGNAL(clicked()), Window, SLOT(first_color()));
        QObject::connect(second_color, SIGNAL(clicked()), Window, SLOT(second_color()));
        QObject::connect(pushButton_2, SIGNAL(clicked()), Window, SLOT(new_start()));
        QObject::connect(stop_start_b, SIGNAL(clicked()), Window, SLOT(stop_start()));

        QMetaObject::connectSlotsByName(Window);
    } // setupUi

    void retranslateUi(QMainWindow *Window)
    {
        Window->setWindowTitle(QApplication::translate("Window", "FPL", Q_NULLPTR));
        stop_start_b->setText(QString());
        with_axes->setText(QApplication::translate("Window", "Show Axis", Q_NULLPTR));
        label->setText(QApplication::translate("Window", "Axis size:", Q_NULLPTR));
        label_2->setText(QApplication::translate("Window", "Speed of drawing:", Q_NULLPTR));
        label_3->setText(QApplication::translate("Window", "Background color:", Q_NULLPTR));
        background_color->setText(QString());
        checkBox->setText(QApplication::translate("Window", "Show equilibrium", Q_NULLPTR));
        label_4->setText(QApplication::translate("Window", "Equilibrium color:", Q_NULLPTR));
        equilibrium_color->setText(QString());
        label_5->setText(QApplication::translate("Window", "Line color:", Q_NULLPTR));
        first_color->setText(QString());
        second_color->setText(QString());
        label_6->setText(QApplication::translate("Window", "Prandtl no.", Q_NULLPTR));
        p_value->setText(QApplication::translate("Window", "10.0", Q_NULLPTR));
        label_8->setText(QApplication::translate("Window", "b", Q_NULLPTR));
        b_value->setText(QApplication::translate("Window", "2.66", Q_NULLPTR));
        label_7->setText(QApplication::translate("Window", "Rayleign no.", Q_NULLPTR));
        r_value->setText(QApplication::translate("Window", "28.0", Q_NULLPTR));
        label_15->setText(QApplication::translate("Window", "Starting x", Q_NULLPTR));
        start_x->setText(QApplication::translate("Window", "0.01", Q_NULLPTR));
        label_16->setText(QApplication::translate("Window", "Starting y", Q_NULLPTR));
        start_y->setText(QApplication::translate("Window", "0.0", Q_NULLPTR));
        label_17->setText(QApplication::translate("Window", "Starting z", Q_NULLPTR));
        start_z->setText(QApplication::translate("Window", "0.0", Q_NULLPTR));
        label_18->setText(QApplication::translate("Window", "Smoothness", Q_NULLPTR));
        step_size->setText(QApplication::translate("Window", "0.001", Q_NULLPTR));
        label_9->setText(QApplication::translate("Window", "Method", Q_NULLPTR));
        method->clear();
        method->insertItems(0, QStringList()
         << QApplication::translate("Window", "Euler", Q_NULLPTR)
         << QApplication::translate("Window", "Runge-Kutta 2", Q_NULLPTR)
         << QApplication::translate("Window", "Runge-Kutta 4", Q_NULLPTR)
        );
        pushButton_2->setText(QApplication::translate("Window", "Restart with new values", Q_NULLPTR));
        pushButton->setText(QApplication::translate("Window", "Exit", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Window: public Ui_Window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WINDOW_H
