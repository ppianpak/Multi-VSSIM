/*
 * File: monitorform.cpp
 * Date: 2016. 8. 8.
 * Author: Yongseok Jin (mnm102211@gmail.com), Joohyun Kim
 * Copyright(c)2016
 * Hanyang University, Seoul, Korea
 * Embedded Software Systems Laboratory, All right reserved
 */

#include "monitorform.h"
#include "ui_monitorform.h"
#include <QFile>
#include <QFileDialog>

MonitorForm::MonitorForm(quint16 port, int vssim_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MonitorForm)
{
    /* initialize variables. */
	this->setWindowTitle("SSD Monitor [VSSIM" + QString::number(vssim_id) + "]");
	this->vssim_id = vssim_id;
    init_variables();

    ui->setupUi(this);

    /* initialize timer. */
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(1);

    /* initialize socket connected with VSSIM */
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReceive()));
    socket->connectToHost("127.0.0.1", port);
}

MonitorForm::~MonitorForm()
{
    delete ui;
}

/*
 * initialize variables.
 */
void MonitorForm::init_variables()
{
    /* initialize count variables. */
    time = 0;

    gcCount = 0;
    gcVSSIM[0] = gcVSSIM[1] = gcVSSIM[2] = 0;
    randMergeCount = seqMergeCount = 0;
    //overwriteCount = 0;

    writeCount = readCount = 0;
    writeSectorCount = readSectorCount = 0;
    trimEffect = trimCount = 0;
    writeAmpCount = writtenPageCount = 0;

    readTime = writeTime = 0;

    fflush(stdout);
}

/*
 * callback method for timer.
 */
void MonitorForm::onTimer()
{
    char sz_timer[20];
    time++;
    sprintf(sz_timer, "%lld", time);
    ui->txtTime->setText(sz_timer);
}

/*
 * callback method for socket.
 */
void MonitorForm::onReceive()
{
    QStringList szCmdList;
    QString szCmd;

    while(socket->canReadLine())
    {
        szCmd = socket->readLine();
        szCmdList = szCmd.split(" ");

        /* WRITE : write count, write sector count, write speed. */
        if(szCmdList[0] == "WRITE")
        {
            QTextStream stream(&szCmdList[2]);

            /* WRITE - PAGE : write count, write sector count. */
            if(szCmdList[1] == "PAGE")
            {
                unsigned int length;
                stream >> length;

                // Write Sector Number Count
                writeSectorCount += length;
                ui->txtWriteSectorCount->setText(QString::number(writeSectorCount));

                // Write SATA Command Count
                writeCount++;
                ui->txtWriteCount->setText(QString::number(writeCount));
            }
            /* WRITE - BW : write speed. */
            else if(szCmdList[1] == "BW")
            {
                double t;
                stream >> t;

                if(t != 0){
                    ui->txtWriteSpeed->setText(QString::number(t));
                }
            }
            else if(szCmdList[1] == "PPN")
            {
            	int32_t ppn;
            	stream >> ppn;
            	hash.insert(ppn, vssim_id);
            }
        }

        /* READ : read count, read sector count, read speed. */
        else if(szCmdList[0] == "READ")
        {
            QTextStream stream(&szCmdList[2]);

            /* READ - PAGE : read count, read sector count. */
            if(szCmdList[1] == "PAGE"){
                unsigned int length;

                /* Read Sector Number Count */
                stream >> length;
                readSectorCount += length;
                ui->txtReadSectorCount->setText(QString::number(readSectorCount));

                /* Read SATA Command Count */
                readCount++;
                ui->txtReadCount->setText(QString::number(readCount));
            }
            /* READ - BW : read speed. */
            else if(szCmdList[1] == "BW"){
                double t;
                stream >> t;

                if(t != 0)
                {
                    ui->txtReadSpeed->setText(QString::number(t));
                }
            }
        }

        /* GC : gc has been occured. */
        else if(szCmdList[0] == "GC")
        {
        	if(szCmdList[1] == "COUNT")
        	{
				gcCount++;
				ui->txtGCCount->setText(QString::number(gcCount));
        	}
        	else if(szCmdList[1] == "PPN")
			{
        		int32_t old_ppn;
				int32_t new_ppn;

        		QTextStream stream1(&szCmdList[2]);
        		QTextStream stream2(&szCmdList[3]);
        		stream1 >> old_ppn;
        		stream2 >> new_ppn;

        		if (hash.contains(old_ppn))
        		{
        			int owner_id = hash.value(old_ppn);
        			gcVSSIM[owner_id - 1]++;
        			ui->txtGCVSSIM1->setText(QString::number(gcVSSIM[0]));
        			ui->txtGCVSSIM2->setText(QString::number(gcVSSIM[1]));
        			ui->txtGCVSSIM3->setText(QString::number(gcVSSIM[2]));

        			hash.remove(old_ppn);
        			hash.insert(new_ppn, owner_id);
        		}
			}
        }

        /* WB : written block count, write amplification. */
        else if(szCmdList[0] == "WB")
        {
            long long int wb = 0;
            QTextStream stream(&szCmdList[2]);
            stream >> wb;

            /* WB - CORRECT : written block count. */
            if(szCmdList[1] == "CORRECT")
            {
				writtenPageCount += wb;
				ui->txtWrittenPage->setText(QString::number(writtenPageCount));
            }
            /* WB - others : write amplificaton. */
            else
            {
				writeAmpCount += wb;
				ui->txtWriteAmp->setText(QString::number(writeAmpCount));
            }
        }

        /* TRIM : trim count, trim effect. */
        else if(szCmdList[0] == "TRIM")
        {
            /* TRIM - INSERT : trim count. */
            if(szCmdList[1] == "INSERT")
            {
                trimCount++;
                ui->txtTrimCount->setText(QString::number(trimCount));
            }
            /* TRIM - others : trim effect. */
            else
            {
                int effect = 0;
                QTextStream stream(&szCmdList[2]);
                stream >> effect;
                trimEffect+= effect;
                ui->txtTrimEffect->setText(QString::number(trimEffect));
            }
        }

        /* UTIL : SSD Util. */
        else if(szCmdList[0] == "UTIL")
        {
            double util = 0;
            QTextStream stream(&szCmdList[1]);
            stream >> util;
            ui->txtSSDUtil->setText(QString::number(util));
        }

        /* put socket string to debug status. */
        ui->txtDebug->setText(szCmd);
    }
}


/*
 * callback method for reset button click.
 */
void MonitorForm::on_btnReset_clicked()
{
    init_variables();

    /* set line edit text "0". */
    ui->txtWriteCount->setText("0");
    ui->txtWriteSpeed->setText("0");
    ui->txtWriteSectorCount->setText("0");

    ui->txtReadCount->setText("0");
    ui->txtReadSpeed->setText("0");
    ui->txtReadSectorCount->setText("0");

    ui->txtGCCount->setText("0");
    ui->txtGCVSSIM1->setText("0");
    ui->txtGCVSSIM2->setText("0");
    ui->txtGCVSSIM3->setText("0");

    ui->txtTrimCount->setText("0");
    ui->txtTrimEffect->setText("0");

    ui->txtWrittenPage->setText("0");
    ui->txtSSDUtil->setText("0");
    ui->txtTime->setText("0");
    ui->txtWriteAmp->setText("0");

    ui->txtDebug->setText("");
}


/*
 * callback method for save button click.
 */
void MonitorForm::on_btnSave_clicked()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, tr("Save file"), tr("./data"), tr("Data files (*.dat"));
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QTextStream out(&file);

    char s_timer[20];
    sprintf(s_timer, "%lld", time);

    out << "==========================================================================\n";
    out << "|                                                                        |\n";
    out << "|           VSSIM : Virtual SSD Simulator                                |\n";
    out << "|                                                                        |\n";
    out << "|                                   Embedded Software Systems Lab.       |\n";
    out << "|                        Dept. of Electronics and Computer Engineering   |\n";
    out << "|                                         Hanynag Univ, Seoul, Korea     |\n";
    out << "|                                                                        |\n";
    out << "==========================================================================\n\n";

    out << "Write Request\t" << writeCount << "\n";
    out << "Write Sector\t" << writeSectorCount << "\n\n";
    out << "Read Request\t" << readCount << "\n";
    out << "Read Sector\t" << readSectorCount << "\n\n";

    out << "Garbage Collection\n";
    out << "  Count (blocks)\t" << gcCount <<"\n";
    out << "  VSSIM1 (pages)\t" << gcVSSIM[0] << "\n";
    out << "  VSSIM2 (pages)\t" << gcVSSIM[1] << "\n";
    out << "  VSSIM3 (pages)\t" << gcVSSIM[2] << "\n\n";

    out << "TRIM Count\t" << trimCount << "\n";
    out << "TRIM effect\t" << trimEffect << "\n\n";

    out << "Write Amplification\t" << writeAmpCount << "\n";
    out << "Written Page\t" << writtenPageCount << "\n";
    out << "Run-time[ms]\t" << s_timer << "\n";

    file.close();
}
