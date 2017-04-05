#include "mw.h"
#include "ui_mw.h"

MW::MW(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MW)
{
    ui->setupUi(this);
}

MW::~MW()
{
    delete ui;
}
