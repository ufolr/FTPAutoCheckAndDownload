#include "tasksetdialog.h"
#include "ui_tasksetdialog.h"
#include <QDebug>
#include <QUrl>
#include <QFileDialog>

TaskSetDialog::TaskSetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskSetDialog)
{
    setting = Setting::getInstance();
    ui->setupUi(this);
    m_lastRow = -1;
    m_tasks = *(setting->getTasks());//程序内任务列表原指针,这里取其值不取地址
    Task t;
    if(!m_tasks.length())//为0
    {
        //如果没有任务那么就新建一个
        on_pushButton_newTask_clicked();
    }
    else
    {
        foreach(t,m_tasks)
        {
            ui->listWidget->addItem(t.taskName);
        }
    }
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(onCurrentRowChanged(int)));
    connect(ui->comboBox_launch,SIGNAL(currentIndexChanged(int)),this,SLOT(launchComboBoxChanged(int)));

    ui->spinBox_time->setEnabled(false);
    ui->comboBox_time->setEnabled(false);
//    ui->lineEdit_taskName->setText(tr("请创建任务"));
//    ui->lineEdit_taskName->setEnabled(false);
}

TaskSetDialog::~TaskSetDialog()
{
    delete ui;
}

void TaskSetDialog::initTaskInfo(Task t)
{
    //任务名
    ui->lineEdit_taskName->setEnabled(true);
    ui->lineEdit_taskName->setText(t.taskName);
    //任务类型
//    if(t.kind == "Http")
//        ui->comboBox_kind->setCurrentIndex(0);//0->Http,1->Ftp
//    else
//        ui->comboBox_kind->setCurrentIndex(1);
    //启动类型
    ui->spinBox_startMonth->setEnabled(false);
    ui->spinBox_time->setEnabled(false);
    ui->comboBox_time->setEnabled(false);
    switch (t.startby)
    {
    case Task::Season:
        ui->comboBox_launch->setCurrentIndex(0);
        ui->spinBox_startMonth->setEnabled(true);
        break;
    case Task::Month:
        ui->comboBox_launch->setCurrentIndex(1);
        break;
    case Task::TenDays:
        ui->comboBox_launch->setCurrentIndex(2);
        break;
    case Task::Week:
        ui->comboBox_launch->setCurrentIndex(3);
        break;
    case Task::Day:
        ui->comboBox_launch->setCurrentIndex(4);
        break;
    case Task::Hour:
        ui->comboBox_launch->setCurrentIndex(5);//间隔时间
        ui->comboBox_time->setCurrentIndex(0);//小时
        ui->spinBox_time->setEnabled(true);
        ui->comboBox_time->setEnabled(true);
        ui->spinBox_startDay->setEnabled(false);
        ui->timeEdit->setEnabled(false);
        break;
    case Task::Minute:
        ui->comboBox_launch->setCurrentIndex(5);
        ui->comboBox_time->setCurrentIndex(1);//小时
        ui->spinBox_time->setEnabled(true);
        ui->comboBox_time->setEnabled(true);
        ui->spinBox_startDay->setEnabled(false);
        ui->timeEdit->setEnabled(false);
        break;
    }
    //启动时间
    if(t.startby == Task::Season)
        ui->spinBox_startMonth->setValue(t.start_month);
    ui->spinBox_startDay->setValue(t.start_day);
    if(ui->comboBox_time->currentIndex() == 0)
        ui->spinBox_time->setValue(t.start_hour);
    if(ui->comboBox_time->currentIndex() == 1)
        ui->spinBox_time->setValue(t.start_minute);
    m_tHour = t.start_hour;
    m_tMinute = t.start_minute;
    ui->timeEdit->setTime(t.start_time);
    //登录
    ui->lineEdit_host->setText(t.Host);
    ui->lineEdit_port->setText(t.port);
    ui->lineEdit_UserName->setText(t.user);
    ui->lineEdit_Pwd->setText(t.pwd);
    //下载地址
    ui->lineEdit_url->setText(t.Dir);
    //储存目录
    ui->lineEdit_saveAs->setText(t.saveDir);
}

Task TaskSetDialog::creatNewTask()
{
    Task t;
    t.taskName = ui->lineEdit_taskName->text();
    switch (ui->comboBox_launch->currentIndex())
    {
    case 0:
        t.startby = Task::Season;
        t.start_month = ui->spinBox_startMonth->value();
        break;
    case 1:
        t.startby = Task::Month;
        break;
    case 2:
        t.startby = Task::TenDays;
        break;
    case 3:
        t.startby = Task::Week;
        break;
    case 4:
        t.startby = Task::Day;
        break;
    case 5:
        if(ui->comboBox_time->currentIndex() == 0)
            t.startby = Task::Hour;
        if(ui->comboBox_time->currentIndex() ==1)
            t.startby = Task::Minute;
        break;
    }
    t.start_day = ui->spinBox_startDay->value();
    t.start_hour = m_tHour;
    t.start_minute = m_tMinute;
    t.start_time = ui->timeEdit->time();
    t.Host = ui->lineEdit_host->text();
    t.port = ui->lineEdit_port->text();
    t.user = ui->lineEdit_UserName->text();
    t.pwd = ui->lineEdit_Pwd->text();
    t.Dir = ui->lineEdit_url->text();
    t.saveDir = ui->lineEdit_saveAs->text();


    return t;
}

void TaskSetDialog::changeTmpTaskList(quint16 index)
{
    //应用后将任务写入临时list
    Task t = creatNewTask();
    m_tasks[index] = t;
    qDebug()<<m_tasks[index].taskName;

    //更新list显示
    ui->listWidget->item(index)->setText(t.taskName);
}

void TaskSetDialog::on_pushButton_newTask_clicked()
{
    Task t;
    t.taskName = "NewTask";
    t.creatDate = QDate::currentDate();
    t.startby = Task::Season;
    t.start_month = 0;
    t.start_day = 0;
    t.start_hour = 0;
    t.start_minute = 5;
    t.start_time = QTime::currentTime();

    m_tasks.append(t);
    ui->listWidget->addItem(t.taskName);
    ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
    initTaskInfo(t);
}

void TaskSetDialog::on_pushButton_delete_clicked()
{
    quint16 index = ui->listWidget->currentRow();
    quint16 count = ui->listWidget->count();
    ui->listWidget->takeItem(index);
    m_tasks.removeAt(index);
    //删除之后的界面
    qDebug()<<index<<count;
    if(index + 1 < count)//后面还有显示后面一条
    {
        ui->listWidget->setCurrentRow(index);
        Task t = m_tasks[index];
        initTaskInfo(t);
    }
    else if(index > 0 && index + 1 == count)//不是第一条,后面没有,显示前面一条
    {
        ui->listWidget->setCurrentRow(index - 1);
        Task t = m_tasks[index - 1];
        initTaskInfo(t);
    }
    else if(index == 0 && count == 1)//只有一条，删除第一条
    {
        ui->listWidget->setCurrentRow(-1);
        Task t;
        t.taskName = "";
        t.startby = Task::Minute;
        t.start_month = 0;
        t.start_day = 0;
        t.start_minute = 5;
        t.start_time = QTime::currentTime();
        initTaskInfo(t);
        on_pushButton_newTask_clicked();
    }
}

void TaskSetDialog::on_pushButton_apply_clicked()
{
    //应用后将任务写入临时list
    Task t = creatNewTask();
    quint16 currentRow = ui->listWidget->currentRow();
    m_tasks[currentRow] = t;
    qDebug()<<m_tasks[currentRow].taskName;
    //更新list显示
    ui->listWidget->currentItem()->setText(t.taskName);

    //将临时list写入系统list
    QList<Task> *tasks =  setting->getTasks();
    tasks->clear();
    foreach(Task tmp,m_tasks)
    {
        tasks->append(tmp);
    }
    //写入xml
    setting->writeXMLSetting("tasks.xml");
}

void TaskSetDialog::launchComboBoxChanged(int index)
{
    //每季度的index为0
    if(index)//不为0
        ui->spinBox_startMonth->setEnabled(false);
    else
        ui->spinBox_startMonth->setEnabled(true);
    if(index != 5)
    {
        ui->spinBox_time->setEnabled(false);
        ui->comboBox_time->setEnabled(false);
        ui->spinBox_startDay->setEnabled(true);
        ui->timeEdit->setEnabled(true);
    }
    else
    {
        ui->spinBox_time->setEnabled(true);
        ui->comboBox_time->setEnabled(true);
        ui->spinBox_startDay->setEnabled(false);
        ui->timeEdit->setEnabled(false);
    }
}

void TaskSetDialog::onCurrentRowChanged(int row)
{
    if(m_lastRow != -1)
        changeTmpTaskList(m_lastRow);//暂存之前的修改
    Task t;
    quint16 index = row;
    if(index < m_tasks.length())
        t = m_tasks[index];
    initTaskInfo(t);
    m_lastRow = row;
}



void TaskSetDialog::on_pushButton_ok_clicked()
{
    //应用
    on_pushButton_apply_clicked();
    //关闭
    close();
}

void TaskSetDialog::on_pushButton_cancel_clicked()
{
    close();
}

void TaskSetDialog::on_pushButton_saveAs_clicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this,
            tr("文件存放到"),
            "",QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
    fileName = fileName + "/";

    if (!fileName.isNull())
    {
        ui->lineEdit_saveAs->setText(fileName);
    }
    else
    {
        //点的是取消
    }
}

void TaskSetDialog::on_comboBox_time_currentIndexChanged(int index)
{
    if(index)//index =1,分钟
    {
        ui->spinBox_time->setValue(m_tMinute);
    }
    else
    {
        ui->spinBox_time->setValue(m_tHour);
    }
}

void TaskSetDialog::on_spinBox_time_valueChanged(int arg1)
{
    if(ui->comboBox_time->currentIndex())//index =1,分钟
    {
        m_tMinute = ui->spinBox_time->value();
    }
    else
    {
        m_tHour = ui->spinBox_time->value();
    }
}
