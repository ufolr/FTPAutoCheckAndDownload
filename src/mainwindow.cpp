#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "taskmanager.h"
#include "downloadtaskinfo.h"
#include "tasksetdialog.h"
#include "tray.h"
#include <QCloseEvent>

#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    currentTaskinfo = NULL;

    ui->statusBar->setText(tr("Copyright(c) 2014 王增武＆李睿 成都信息工程学院."));
    setting = Setting::getInstance();
    tm = new TaskManager;
    listWidgetMaxCount = 50;

    QTimer *timer = new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(iterateTask()));
    timer->start(2000);//遍历任务列表时间
    ui->textBrowser->document()->setMaximumBlockCount(200);//最大行数

    //自动运行
    if(setting->getAutoRunStatus())
    {
        ui->checkBox_autoRun->setChecked(true);
        autoRun = true;
    }
    else
    {
        ui->checkBox_autoRun->setChecked(false);
        autoRun = false;//设置为false则启动后不会自动下载
    }
    connect(ui->checkBox_autoRun,SIGNAL(stateChanged(int)),this,SLOT(onAutoRunStatusChange(int)));

    initTaskManagerConnection();

    //托盘
    Tray *tray = new Tray(this);
    connect(tray,SIGNAL(signal_hide()),this,SLOT(hide()));
    connect(tray,SIGNAL(signal_showNormal()),this,SLOT(showNormal()));
    connect(tray,SIGNAL(signal_quit()),this,SLOT(on_pushButton_2_clicked()));
    connect(tray,SIGNAL(signal_raise()),this,SLOT(raise()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_StarWork_clicked()
{
    if(ui->StarWork->isChecked())
    {
        autoRun = true;
    }
}

void MainWindow::iterateTask()
{
    if(autoRun && !tm->getWorkingState())//没有在工作状态
        tm->starTasks();//启动遍历
}

void MainWindow::handleTaskConnect(QString taskName, bool isConnect)
{
    Task t = setting->getCurrentTask();
    if(taskName == t.taskName && isConnect)
    {

        currentTaskinfo = new DownLoadTaskInfo(t);

        QListWidgetItem *item = new QListWidgetItem;
        if(ui->listWidget->count() == listWidgetMaxCount)
            ui->listWidget->takeItem(listWidgetMaxCount-1);
        ui->listWidget->insertItem(0,item);
        item->setSizeHint(QSize(311,24));
        ui->listWidget->setItemWidget(item,currentTaskinfo);
        currentTaskinfo->setTask(t);
    }
}

void MainWindow::onftpCommandStatus(QString status)
{
    status.replace(".fdl","");
    ui->textBrowser->append(QDateTime::currentDateTime().toString("[yy-MM-dd hh:mm:ss]") + status);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    e->ignore();
    this->hide();
}

void MainWindow::handleDownloadProgress(QString TaskName, quint64 now, quint64 total)
{
    //ui->listWidget->findChild()
    currentTaskinfo->updateProgeressBar(now, total);
    QString progress;
    double value;
    if(total)
    {
        value = double(now)/double(total)*100;
        progress = QString("%1").arg(QString::number(double(now)/double(total)*100.0,'f',2));
    }
    else
    {
        value = 100;
        progress ="100";
    }
    ui->progressBar->setFormat(progress);
    ui->progressBar->setValue(value);
}

void MainWindow::initTaskManagerConnection()
{
    connect(tm,SIGNAL(downloadProgress(QString,quint64,quint64)),this, SLOT(handleDownloadProgress(QString,quint64,quint64)));
    connect(tm,SIGNAL(taskConnect(QString,bool)),this,SLOT(handleTaskConnect(QString,bool)));
    connect(tm,SIGNAL(ftpCommandStatus(QString)),this,SLOT(onftpCommandStatus(QString)));
}

void MainWindow::onAutoRunStatusChange(int)
{
    //autorun状态改变后写入tasks.xml
    setting->setAutoRunStatus(ui->checkBox_autoRun->isChecked());
    setting->writeXMLSetting("tasks.xml");
}

void MainWindow::on_pushButton_taskManage_clicked()
{
    TaskSetDialog *dlg = new TaskSetDialog(this);
    dlg->show();
}

void MainWindow::on_pushButton_2_clicked()
{
    exit(0);//关闭程序
}

void MainWindow::messageReceived(const QString &str)
{
    if(str == "weak_up")
        this->showNormal();
}
