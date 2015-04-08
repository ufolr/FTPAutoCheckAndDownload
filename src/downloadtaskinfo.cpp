#include "downloadtaskinfo.h"
#include "ui_downloadtaskinfo.h"

DownLoadTaskInfo::DownLoadTaskInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownLoadTaskInfo)
{
    ui->setupUi(this);

    ui->label_Time->setText(QDateTime::currentDateTime().toString("yy/M/d hh:mm"));
    ui->frame_base->setStyleSheet(".QFrame{background-color: rgba(227,227,227,150);border-radius:5px;alternate-background-color: rgba(245, 245, 245,150);}");
}

DownLoadTaskInfo::DownLoadTaskInfo(Task t, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownLoadTaskInfo)
{
    ui->setupUi(this);
    m_taskName = t.taskName;

    ui->label_taskName->setText(m_taskName);
    ui->label_Time->setText(QDateTime::currentDateTime().toString("yy/M/d hh:mm"));
    ui->frame_base->setStyleSheet(".QFrame{background-color: rgba(227,227,227,150);border-radius:5px;alternate-background-color: rgba(245, 245, 245,150);}");
}

DownLoadTaskInfo::~DownLoadTaskInfo()
{
    delete ui;
}

void DownLoadTaskInfo::setTask(Task t)
{
    m_currentTask = t;
}

Task DownLoadTaskInfo::getTask()
{
    return m_currentTask;
}

void DownLoadTaskInfo::setStatus(QString status)
{
    ui->label_down->setText(status);
}

void DownLoadTaskInfo::setGetStatus(bool isGot)
{
    m_isGot = isGot;
}

bool DownLoadTaskInfo::getGetStatus()
{
    return m_isGot;
}

void DownLoadTaskInfo::updateProgeressBar(qint64 down, qint64 total)
{
    QString down_s = QString::number(down);
    QString total_s = QString::number(total);

    ui->label_down->setText(down_s + "/" + total_s);
    if(down == total && down != 0)
        ui->label_down->setText(tr("完成"));
    if(down == total && down == 0)
    {
        ui->label_down->setText(tr("无更新的文件"));
        ui->progressBar->setValue(10000);
    }
    ui->progressBar->setValue(double(down)/double(total)*10000);
}

QString DownLoadTaskInfo::formatFileSize(qint64 fs)
{
    QString formatedSize;
    if(fs<=1024)//<1k
    {
        formatedSize = QString::number(fs) + "B";
    }
    else if(fs <= 1024*1024)//<1M
    {
        formatedSize = QString::number(float(fs)/1024.0,'f',2) + "KB";
    }
    else if(fs <= 1024*1024*1024)//<1G
    {
        formatedSize = QString::number(float(fs)/1024.0/1024.0,'f',2) + "MB";
    }
    else//>1G
    {
        formatedSize = QString::number(float(fs)/1024.0/1024.0/1024.0,'f',2) + "GB";
    }
    return formatedSize;
}
