#include "setting.h"
#include <QDebug>
#include <QStringList>
#include <QPoint>
#include <QDateTime>
#include <QMutex>
#include <QDir>
#include <QCoreApplication>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QUrl>



Setting *Setting::instance = NULL;

Setting::Setting(QObject *parent) 
{
    m_ftpTaskPos = 0;
    m_httpTaskPos =0;
    m_TaskTriedTime = 0;
    m_taskPos =0;

    m_AutoRunStatus = false;
    readXMLSetting("tasks.xml");
}


Setting *Setting::getInstance()
{
    if (instance == NULL)
        instance = new Setting();

    return instance;
}

Setting::~Setting()
{
}


bool Setting::getNextTask(Task *t)
{
    qDebug()<<"POS"<<m_taskPos<< m_DownLoadTaskList.length();
    if(m_taskPos < m_DownLoadTaskList.length())
    {
        *t = m_DownLoadTaskList[m_taskPos];
        m_currentPos = m_taskPos;
        m_taskPos++;
        setCurrentTask(*t);
        return true;
    }
    else
    {
        return false;
    }
}

void Setting::setTaskLastDownFileTime(QDateTime dt)
{
    m_DownLoadTaskList[m_currentPos].lastDownFileTime = dt;
}

void Setting::setCurrentTaskLastStartDate(QDateTime dateTime)
{
    m_DownLoadTaskList[m_currentPos].lastStart = dateTime;

    quint8 tem = m_taskPos -1;
    qDebug()<<"lastdate"<<m_DownLoadTaskList[tem].lastStart;
}

void Setting::setLastDBCleanDate(QDateTime dateTime)
{
    m_DownLoadTaskList[m_currentPos].lastStart = dateTime;

    quint8 tem = m_taskPos -1;
    qDebug()<<"LastClean"<<m_DownLoadTaskList[tem].lastStart;
}

void Setting::reSetTaskListPos()
{
    m_taskPos = 0;
}

void Setting::setAutoRunStatus(bool autoRun)
{
    m_AutoRunStatus = autoRun;
}

void Setting::readXMLSetting(QString xmlFile)
{
    QFile *f = new QFile(xmlFile);
    if(f->open(QFile::ReadOnly))
    {
        QXmlStreamReader *reader = new QXmlStreamReader(f);

        while (reader->readNextStartElement())
        {
            if(reader->name() == "AutoRun")
            {
                if(reader->readElementText() == "True")
                    m_AutoRunStatus = true;
                else
                    m_AutoRunStatus = false;
            }
            if(reader->name() == "TaskConfig")
            {
                while (reader->readNextStartElement())
                {
                    if(reader->name() == "Task" ) //读到一个任务
                    {
                        Task t;
                        t.creatDate = QDate::currentDate();
                        while (reader->readNext())
                        {
                            if(reader->isStartElement())
                            {
                                //qDebug()<<"2"<<reader->name();
                                if(reader->name() == "Name")
                                    t.taskName = reader->readElementText().trimmed();
                                if(reader->name() == "Launch")
                                {
                                    QString elementText(reader->readElementText());
                                    if(elementText == "Season")
                                        t.startby = Task::Season;
                                    if(elementText == "Month")
                                        t.startby = Task::Month;
                                    if(elementText == "TenDays")
                                        t.startby = Task::TenDays;
                                    if(elementText == "Week")
                                        t.startby = Task::Week;
                                    if(elementText == "Day")
                                        t.startby = Task::Day;
                                    if(elementText == "Hour")
                                        t.startby = Task::Hour;
                                    if(elementText == "Minute")
                                        t.startby = Task::Minute;
                                }
                                if(reader->name() == "month")
                                    t.start_month = reader->readElementText().toShort();
                                if(reader->name() == "day")
                                    t.start_day = reader->readElementText().toShort();
                                if(reader->name() == "hour")
                                    t.start_hour = reader->readElementText().toShort();
                                if(reader->name() == "minute")
                                    t.start_minute = reader->readElementText().toShort();
                                if(reader->name() == "time")
                                {
                                    t.start_time = QTime::fromString(reader->readElementText(),"hh:mm");
                                }
                                if(reader->name() == "Try")
                                    t.tryTime = reader->readElementText().toInt();
                                if(reader->name() == "Host")
                                    t.Host = reader->readElementText().trimmed();
                                if(reader->name() == "Port")
                                    t.port = reader->readElementText().trimmed();
                                if(reader->name() == "User")
                                    t.user = reader->readElementText().trimmed();
                                if(reader->name() == "PWD")
                                    t.pwd = reader->readElementText().trimmed();
                                if(reader->name() == "DBCleanDays")
                                    t.DBCleanDays = reader->readElementText().trimmed().toInt();
                                if(reader->name() == "Dir")
                                    t.Dir = reader->readElementText().trimmed();
                                if(reader->name() == "SaveAs")
                                    t.saveDir = reader->readElementText().trimmed();
                                if(reader->name() == "Wait")
                                    t.waitSecond = reader->readElementText().trimmed().toInt();
                                if(reader->name() == "LastStar")
                                    t.lastStart = QDateTime::fromString(reader->readElementText().trimmed(),"yyyy-M-d,H:m:s");
                                if(reader->name() == "LastFinish")
                                    t.lastDownFileTime = QDateTime::fromString(reader->readElementText().trimmed(),"yyyy-M-d,H:m:s");
                            }
                            else if(reader->isEndElement() && reader->name() == "Task")
                                break;
                        }
                        m_DownLoadTaskList.append(t);
                        qDebug()<<"nnn"<<reader->name();
                        qDebug()<<"time"<<t.start_time.toString();
                    }
                }
            }

        }
    }
    f->close();
}

void Setting::writeXMLSetting(QString xmlFile)
{
    QList<Task> *tasks = getTasks();
    QFile f(xmlFile);
    if(!f.open(QFile::WriteOnly))
    {
        return;
    }
    QXmlStreamWriter writer(&f);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("Setting");
    if(m_AutoRunStatus)
        writer.writeTextElement("AutoRun","True");
    else
        writer.writeTextElement("AutoRun","False");

    writer.writeStartElement("TaskConfig");
    foreach (Task t, *tasks)
    {
        writer.writeStartElement("Task");
        writer.writeTextElement("Name",t.taskName);
        switch (t.startby)
        {
        case Task::Season:
            writer.writeTextElement("Launch","Season");
            break;
        case Task::Month:
            writer.writeTextElement("Launch","Month");
            break;
        case Task::TenDays:
            writer.writeTextElement("Launch","TenDays");
            break;
        case Task::Week:
            writer.writeTextElement("Launch","Week");
            break;
        case Task::Day:
            writer.writeTextElement("Launch","Day");
            break;
        case Task::Hour:
            writer.writeTextElement("Launch","Hour");
            break;
        case Task::Minute:
            writer.writeTextElement("Launch","Minute");
            break;
        }
        if(t.startby == Task::Season)
            writer.writeTextElement("month",QString::number(t.start_month));

        writer.writeTextElement("day",QString::number(t.start_day));
        writer.writeTextElement("hour",QString::number(t.start_hour));
        writer.writeTextElement("minute",QString::number(t.start_minute));
        writer.writeTextElement("time",t.start_time.toString("hh:mm"));
        writer.writeTextElement("Try",QString::number(t.tryTime));
        writer.writeTextElement("Host",t.Host);
        writer.writeTextElement("Port",t.port);
        writer.writeTextElement("User",t.user);
        writer.writeTextElement("PWD",t.pwd);
        writer.writeTextElement("Dir",t.Dir);
        writer.writeTextElement("SaveAs",t.saveDir);
        writer.writeTextElement("Wait",QString::number(t.waitSecond));
        writer.writeTextElement("LastStar",t.lastStart.toString("yyyy-M-d,H:m:s"));
        writer.writeTextElement("LastFinish",t.lastDownFileTime.toString("yyyy-M-d,H:m:s"));
        writer.writeEndElement();
    }
    writer.writeEndElement();
    writer.writeEndElement();
    writer.writeEndDocument();
    f.close();
}









