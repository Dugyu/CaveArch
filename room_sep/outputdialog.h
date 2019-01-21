#ifndef OUTPUTDIALOG_H
#define OUTPUTDIALOG_H
#include <QDialog>
#include <QFileDialog>
#include <QPushButton>
#include <QDialogButtonBox>


class OutputDialog: public QFileDialog {
    Q_OBJECT
public:
    OutputDialog(QWidget * parent);
private slots:

public:
    QDialogButtonBox * dialog_button;

};

#endif // OUTPUTDIALOG_H
