#include "outputdialog.h"
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QFile>


OutputDialog::OutputDialog(QWidget * parent): QFileDialog(parent) {
    dialog_button = new QDialogButtonBox;
    setFileMode(DirectoryOnly);
    setDirectory(QString("/Users/huanghuangsunyang/"));
//    QVBoxLayout * main_layout;
//    main_layout->addWidget(dialog_button);
//    setLayout(main_layout);
}
