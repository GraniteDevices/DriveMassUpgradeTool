#ifndef MW_H
#define MW_H

#include <QMainWindow>

namespace Ui {
class MW;
}

class MW : public QMainWindow
{
    Q_OBJECT

public:
    explicit MW(QWidget *parent = 0);
    ~MW();

private:
    Ui::MW *ui;
};

#endif // MW_H
