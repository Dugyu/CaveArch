#include "wholewindow.h"
#include "mypainterwidget.h"
#include "userinput.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

WholeWindow::WholeWindow(QWidget * parent): QWidget(parent) {
    painterWidget = new MyPainterWidget(0);
    userinput = new UserInput(0);
    button1 = new QPushButton("&Reset");
    button2 = new QPushButton("&Output");
    button3 = new QPushButton("&Reset");
    QVBoxLayout * layout = new QVBoxLayout();
    QHBoxLayout * top_layout = new QHBoxLayout();
    top_layout->addStretch();
    top_layout->addWidget(button1);
    top_layout->addWidget(button2);
    top_layout->addWidget(button3);
    top_layout->addStretch();
    layout->addWidget(painterWidget);
    layout->addWidget(userinput);
    this->setLayout(layout);
}
