#ifndef MW_H
#define MW_H

#include <QMainWindow>
#include <devicedeployment.h>

namespace Ui {
class MW;
}

class MW : public QMainWindow
{
    Q_OBJECT

public:
    explicit MW(QWidget *parent = 0);
    ~MW();

    void log(QString msg, int indent=0, bool error=false);

    int installFirmware( int address );
    int loadConfiguration( int address );


private slots:
    void on_start_clicked();

    void on_chooseSettingsFile_clicked();

    void on_chooseFWfile_clicked();

    void on_startAddr_valueChanged(int arg1);

    void on_endAddr_valueChanged(int arg1);

private:
    Ui::MW *ui;
    smbus bus;
};

#endif // MW_H
