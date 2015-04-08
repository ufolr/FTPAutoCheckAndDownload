#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QObject>
#include <QList>
//#include <QString>
#include "setting.h"
class Setting;
class FtpDirDownLoader;

class TaskManager : public QObject
{
    Q_OBJECT
public:
    explicit TaskManager(QObject *parent = 0);
    ~TaskManager();
    void starTasks();
    bool getWorkingState(){return isWorking;}

signals:
    void downloadProgress(QString taskName,quint64 now, quint64 total);
    void taskConnect(QString taskName, bool isConnect);
    void ftpCommandStatus(QString);
public slots:
    void oneTaskFinish();
    void handleDownloadProgress(quint32 now, quint32 total);
    void handleTaskDownloaderConnect(bool isConnect);
private:
    Setting *setting;
    QList<Task> *list;
    FtpDirDownLoader *downloader;
    bool checkTime(Task t);
    bool isRun;
    bool isWorking;
};

#endif // TASKMANAGER_H
