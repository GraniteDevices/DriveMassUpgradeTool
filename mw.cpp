#include "mw.h"
#include "ui_mw.h"
#include <QFileDialog>

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

void MW::log(QString msg, int indent)
{
    QString space;
    for(int i=0;i<indent;i++)
        space+="    ";
    ui->log->append(space+msg);
}

void MW::on_start_clicked()
{
    smSetDebugOutput( Low, stdout );//print some info from sm-functions
    smSetTimeout(100);//make it faster by using shorter timeout

    //OPEN BUS:
    bus=smOpenBus(ui->portName->text().toLatin1());
    if(bus<0)
    {
        log("Can't open bus");
        return;
    }

    int i;

    //update all devices that we find
    for(i=ui->startAddr->value();i<=ui->endAddr->value();i++)
    {
        qApp->processEvents();//update UI

        if(i==33)//skip 33-244
            i=245;

        //test if drive is there
        smint32 dtype, busmode;
        SM_STATUS stat=smRead2Parameters(bus,i,SMP_DEVICE_TYPE,&dtype,SMP_BUS_MODE,&busmode);
        if(stat==SM_OK)//update
        {
            log(QString("Device type nr %1 found at address %2").arg(dtype).arg(i));

            if(!ui->FWFileName->text().isEmpty())
                upgradeFW(i);
            if(!ui->DRCFileName->text().isEmpty())
                configure(i);
        }
        else
        {
            log(QString("Skipping address %1 (no response)").arg(i));
        }
    }

    smCloseBus(bus);
    log("Finished");
}

int MW::upgradeFW( int address )
{
    log(QString("Installing FW on address %1").arg(address),1);
    FirmwareUploadStatus stat;
    do
    {
        stat=smFirmwareUpload(bus,address,ui->FWFileName->text().toLatin1());

        if(stat<0)
        {
            log(QString("FW update failed, FirmwareUploadStatus %1").arg((int)stat),2);
            return 5;
        }

        ui->progressBar->setValue((int)stat);
        qApp->processEvents();//update UI
    } while((int)stat>=0 && stat!=FWComplete);

    if(stat==FWComplete)
        log(QString("Install success on address %1").arg(address),2);

    return 0;
}

int MW::configure( int address )
{
    //LOAD PARAMETERS:
    int skipped, errors;
    log(QString("Loading parameters on address %1").arg(address),1);
    LoadConfigurationStatus lcstat=smLoadConfiguration( bus, address, ui->DRCFileName->text().toLatin1(), 0, &skipped, &errors );
    log(QString("Load parameters: status %1, skipped %2, set errors %3").arg((int)lcstat).arg(skipped).arg(errors),2 );

    if(lcstat==CFGComplete && skipped==0 && errors ==0 )
    {
        log("Setting parameters succeed",2);
        return 0;
    }
    else
    {
        log("Some errors occurred during parameter setting",2);
        return 6;
    }
}

void MW::on_chooseSettingsFile_clicked()
{
    QString fname=QFileDialog::getOpenFileName(this,tr("Open drive settings file"),
                                               "",tr("Firmware file (*.drc)"));

    if(fname.isNull()) return;//cancelled

    ui->DRCFileName->setText(fname);
}

void MW::on_chooseFWfile_clicked()
{
    QString fname=QFileDialog::getOpenFileName(this,tr("Open firmware file"),
                                               "",tr("Firmware file (*.gdf)"));

    if(fname.isNull()) return;//cancelled

    ui->FWFileName->setText(fname);
}

void MW::on_startAddr_valueChanged(int arg1)
{
    if(ui->endAddr->value()<arg1)//dont let end be smaller than start
        ui->endAddr->setValue(arg1);
}

void MW::on_endAddr_valueChanged(int arg1)
{
    if(ui->startAddr->value()>=arg1)//dont let end be smaller than start
        ui->startAddr->setValue(arg1);
}
