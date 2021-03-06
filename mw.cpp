#include "mw.h"
#include "ui_mw.h"
#include <QFileDialog>
#include <QDebug>

MW::MW(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MW)
{
    ui->setupUi(this);
    log("About: this app may be used for batch uploading firmware and settings to multiple drives in the SimpleMotion bus. Limitations: uploading firmware with this tool not yet supported on ARGON drives.");
    log("");
    log("Usage: fill in communication port name, select firmware and/or settings file, choose motor drive SimpleMotion bus address range where the files should be loaded, and click Start job.");
    ui->statusBar->showMessage("Version 1.2.0");
}

MW::~MW()
{
    delete ui;
}

void MW::log(QString msg, int indent, bool error )
{
    QString space;

    for(int i=0;i<indent;i++)
        space+="    ";

    if(error)
        msg="<b>"+msg+"</b>";

    ui->log->append(space+msg);
}

void MW::on_start_clicked()
{
    ui->start->setEnabled(false);

    smSetDebugOutput( SMDebugMid, stdout );//print some info from sm-functions
    smSetTimeout(100);//make it faster by using shorter timeout


    //OPEN BUS:
    bus=smOpenBus(ui->portName->text().toLatin1());
    if(bus<0)
    {
        log("Can't open bus",0,true);
        ui->start->setEnabled(true);
        return;
    }

    int i;

    //update all devices that we find
    for(i=ui->startAddr->value();i<=ui->endAddr->value();i++)
    {
        qApp->processEvents();//update UI

        if(i==74)
        {
            log("Skipping address range 74-244 because no SM device currently can have address in that range");
            i=245;
        }

        //test if drive is there
        smint32 dtype, busmode;
        SM_STATUS stat=smRead2Parameters(bus,i,SMP_DEVICE_TYPE,&dtype,SMP_BUS_MODE,&busmode);
        if(stat==SM_OK)//update
        {
            log(QString("Device type nr %1 found at address %2").arg(dtype).arg(i));

            if(!ui->FWFileName->text().isEmpty())
                installFirmware(i);
            if(!ui->DRCFileName->text().isEmpty())
                loadConfiguration(i);
        }
        else
        {
            log(QString("Skipping address %1 (no response)").arg(i));
        }
    }

    smCloseBus(bus);
    log("Finished");
    ui->start->setEnabled(true);
}

int MW::installFirmware( int address )
{
    log(QString("Installing FW on address %1").arg(address),1);
    FirmwareUploadStatus stat;

    //choose whether to erase settings upon install
    uint32_t option_bits=FW_UPLOAD_OPTION_NOP;
    if(ui->resetDriveSettings->isChecked())
        option_bits=FW_UPLOAD_OPTION_ERASE_SETTINGS;

    do
    {
        stat=smFirmwareUploadWithOptions(bus,address,ui->FWFileName->text().toLatin1(),option_bits);

        if(stat<0)
        {
            char err[100];
            smFirmwareUploadStatusToString(stat,err);
            if(stat!=FWAlreadyInstalled)
                log(QString("FW update failed: %1 (%2)").arg(err).arg((int)stat),2,true);
            else
                log(QString("FW update skipped: %1").arg(err).arg((int)stat),2);
            return 5;
        }

        ui->progressBar->setValue((int)stat);
        qApp->processEvents();//update UI
    } while((int)stat>=0 && stat!=FWComplete);

    if(stat==FWComplete)
        log(QString("Install success on address %1").arg(address),2);

    return 0;
}

int MW::loadConfiguration( int address )
{
    int skipped, errors;
    log(QString("Loading parameters on address %1").arg(address),1);
    LoadConfigurationStatus lcstat=smLoadConfiguration( bus, address, ui->DRCFileName->text().toLatin1(), 0, &skipped, &errors );
    log(QString("Load parameters: status %1, skipped %2, set errors %3").arg(getLoadConfigurationStatusString(lcstat)).arg(skipped).arg(errors),2 );

    if(lcstat==CFGComplete && skipped==0 && errors ==0 )
    {
        log("Setting parameters succeed",2);
        return 0;
    }
    else
    {
        log("Some errors occurred during parameter setting",2,true);
        return 6;
    }
}

void MW::on_chooseSettingsFile_clicked()
{
    QString fname=QFileDialog::getOpenFileName(this,tr("Open drive settings file"),"",tr("Firmware file (*.drc)"));

    if(fname.isNull()) return;//cancelled

    ui->DRCFileName->setText(fname);
}

void MW::on_chooseFWfile_clicked()
{
    QString fname=QFileDialog::getOpenFileName(this,tr("Open firmware file"),"",tr("Firmware file (*.gdf)"));

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
    if(ui->startAddr->value()>=arg1)//dont let start to be larger than end
        ui->startAddr->setValue(arg1);
}
