#ifndef SETTING_H
#define SETTING_H

#include <QSettings>
#include <QString>
#include <QStringList>
#include <QDateTime>

struct Task
{
    enum StartBy
    {
        Season = 1,
        Month = 2,
        TenDays = 4,
        Week = 8,
        Day = 16,
        Hour = 32,
        Minute = 64
    };
    QString taskName;
    StartBy startby;//启动时间类型

    QTime start_time;
    quint8 start_month;//当启动类型是按季启动时需设置
    quint8 start_day;
    quint8 start_hour;
    quint8 start_minute;
    QDate creatDate;
    QDateTime lastStart;
    QDateTime lastDownFileTime;
    QDateTime lastDBClean;
    quint8 DBCleanDays;

    //如果是ftp下载
    QString Host;
    QString port;
    QString user;
    QString pwd;
    QString Dir;//下载文件在服务器上的路径

    QString saveDir;//保存文件路径
    quint32 waitSecond;
    quint8 tryTime;
};
class Setting : public QSettings
{
    Q_OBJECT
public:
    static Setting *getInstance();
    ~Setting();


    Task getCurrentTask(){return m_currentTask;}
    bool getNextTask(Task *t);
    quint8* getTaskTriedTime(){return &m_TaskTriedTime;}
    void setTaskLastDownFileTime(QDateTime dt);
    void setCurrentTaskLastStartDate(QDateTime dateTime);
    void setLastDBCleanDate(QDateTime dateTime);
    void reSetTaskListPos();
    QList<Task> *getTasks(){return &m_DownLoadTaskList;}
    void writeXMLSetting(QString xmlFile);
    void readXMLSetting(QString xmlFile);
    bool getAutoRunStatus(){return m_AutoRunStatus;}
    void setAutoRunStatus(bool autoRun);
   
public slots:


signals:


private:
    explicit Setting(QObject *parent = 0);
    static Setting *instance;
    void setCurrentTask(Task t){m_currentTask = t;}

    Task m_currentTask;
    QList<Task> m_DownLoadTaskList;
    QStringList m_ftpDownFileList;
    QStringList m_httpDownFileList;
    quint16 m_ftpTaskPos;//标示m_currentFtpDownFileName中下载了哪些任务
    quint16 m_httpTaskPos;
    quint8 m_taskPos;
    quint8 m_currentPos;
    QString m_destinationDir;//文件存放的文件夹

    quint8 m_TaskTriedTime;
    bool m_AutoRunStatus;

};

#endif // SETTING_H
