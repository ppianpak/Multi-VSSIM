/*
 * File: monitorform.h
 * Date: 2016. 8. 8.
 * Author: Yongseok Jin (mnm102211@gmail.com)
 * Copyright(c)2016
 * Hanyang University, Seoul, Korea
 * Embedded Software Systems Laboratory, All right reserved
 */

#ifndef MONITORFORM_H
#define MONITORFORM_H

#include <QDialog>
#include <QTimer>
#include <QTcpSocket>
#include <QHash>

// hash from phy_page_nb to vssim_id
static QHash<int, int> hash;

namespace Ui {
	class MonitorForm;
}

class MonitorForm : public QDialog
{
    Q_OBJECT

public slots:
    void onTimer();
    void onReceive();

public:
    explicit MonitorForm(quint16 port, int vssim_id, QWidget *parent = 0);
    ~MonitorForm();
    void init_variables();

private slots:
    void on_btnReset_clicked();
    void on_btnSave_clicked();

private:
    Ui::MonitorForm *ui;
    QTcpSocket *socket;
    QTimer *timer;
    int vssim_id;

    /* variables */
    long long int time;
    int CELL_PROGRAM_DELAY;

    long int gcCount;
    long int gcVSSIM[3];
    int randMergeCount, seqMergeCount;
    long int overwriteCount;

    long int writeCount, readCount;
    long int writeSectorCount, readSectorCount;
    int trimEffect, trimCount;
    long int writeAmpCount, writtenPageCount;

    double readTime, writeTime;
};

#endif // MONITORFORM_H
