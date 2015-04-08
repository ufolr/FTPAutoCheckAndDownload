#include "taskmanager.h"
#include "ftpdirdownloader.h"

#include <QFile>
#include <QDebug>
#include <QDateTime>
TaskManager::TaskManager(QObject *parent) : QObject(parent)
{
    isRun = false;
    isWorking = false;
    setting = Setting::getInstance();
    setting->readXMLSetting("Task.xml");
}

TaskManager::~TaskManager()
{

}

void TaskManager::starTasks()
{
    isWorking = true;
    Task t;
    if(setting->getNextTask(&t))//成功获取到下一个任务
    {
         if(checkTime(t))
        {
            //时间符合启动任务
            isRun = true;
            downloader = new FtpDirDownLoader(this);
            qDebug()<<downloader;
            downloader->setHost(t.Host);
            downloader->setPort(t.port);
            downloader->setUser(t.user);
            downloader->setPwd(t.pwd);
            downloader->setDownLoadDir(t.Dir);
            downloader->setSaveDir(t.saveDir);
            downloader->setDBName("./TaskDB/"+t.taskName);
            downloader->setLastDownTime(t.lastDownFileTime);//任务配置中最后下载成功的时间
            downloader->star();
            connect(downloader,SIGNAL(listDownLoadFinish()),this,SLOT(oneTaskFinish()));
            connect(downloader,SIGNAL(downloadProgress(quint32,quint32)),this,SLOT(handleDownloadProgress(quint32,quint32)));
            connect(downloader,SIGNAL(ftpConnected(bool)),this,SLOT(handleTaskDownloaderConnect(bool)));
            connect(downloader,SIGNAL(ftpCommandStatus(QString)),this,SIGNAL(ftpCommandStatus(QString)));
        }
        else
        {
            //时间不符合,遍历下一个
            starTasks();
        }
    }
    else
    {
        //任务遍历一遍
        isWorking = false;
        setting->reSetTaskListPos();//重置任务列表位置
    }
}

void TaskManager::oneTaskFinish()
{
    //下载完成更新配置文件中的最后下载文件时间
    qDebug()<<downloader->getLastDownFileTime();
    setting->setTaskLastDownFileTime(downloader->getLastDownFileTime());
    isRun = false;
    setting->setCurrentTaskLastStartDate(QDateTime::currentDateTime());
    setting->writeXMLSetting("tasks.xml");//把最后下载文件时间和最后完成任务的时间写入配置文件
    starTasks();
}

void TaskManager::handleDownloadProgress(quint32 now, quint32 total)
{
    QString taskName = setting->getCurrentTask().taskName;
    emit downloadProgress(taskName,now,total);
}

void TaskManager::handleTaskDownloaderConnect(bool isConnect)
{
    //发送当前任务是否连接成功
    Task t = setting->getCurrentTask();
    emit taskConnect(t.taskName, isConnect);
}

bool TaskManager::checkTime(Task t)
{
    if(isRun)
        return false;
    QDateTime now = QDateTime::currentDateTime();

    if(t.lastStart < now)//上次启动的时间早于当前时间
    {
        qDebug()<<t.start_time.isNull();

            //bool值默认为true;
            if(t.start_time > now.time() && (t.startby != Task::Hour || t.startby != Task::Minute))//设定任务开始时间大于当前时间
            {
                return false;
            }
            if(t.startby == Task::Season)
            {
                if(now.date().month() % 3 != t.start_month%3 || now.date().day() != t.start_day)//每季第几月第几天,任意一个不满足
                    return false;
            }
            else if(t.startby == Task::Month)
            {
                if(now.date().day() != t.start_day)//每月第几天
                    return false;
            }
            else if(t.startby == Task::TenDays)
            {
                if(now.date().day() % 10 != t.start_day % 10)//每旬第几天
                    return false;
            }
            else if(t.startby == Task::Week)
            {
                if(now.date().dayOfWeek() !=t.start_day)//每周第几天
                    return false;
            }
            else if(t.startby == Task::Day)
            {
                if(now.date().toJulianDay() % t.start_day != t.creatDate.toJulianDay() % t.start_day)//每隔几天//JulianDay
                    return false;
            }
            else if(t.startby == Task::Hour)
            {
                if(t.lastStart.secsTo(now) < t.start_hour*3600)
                    return false;
            }
            else if(t.startby == Task::Minute)
            {
                if(t.lastStart.secsTo(now) < t.start_minute*60)
                    return false;
            }
            else
            {
                return false;
            }
    }
    else
    {
        return false;
    }
}


