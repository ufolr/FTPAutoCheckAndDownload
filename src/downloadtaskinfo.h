#ifndef DOWNLOADTASKINFO_H
#define DOWNLOADTASKINFO_H

#include <QWidget>
#include <QString>
#include <QDateTime>
#include "setting.h"

namespace Ui {
class DownLoadTaskInfo;
}

class DownLoadTaskInfo : public QWidget
{
    Q_OBJECT

public:
    explicit DownLoadTaskInfo(QWidget *parent = 0);
    DownLoadTaskInfo(Task t, QWidget *parent = 0);
    ~DownLoadTaskInfo();

    void setTask(Task t);
    Task getTask();
    void setStatus(QString status);
    void setGetStatus(bool isGot);
    bool getGetStatus();
public slots:
    void updateProgeressBar(qint64 down, qint64 total);

private:
    Ui::DownLoadTaskInfo *ui;
    QString m_taskName;
    QDateTime m_downTime;
    QString formatFileSize(qint64 fs);
    Task m_currentTask;
    bool m_isGot;
};

#endif // DOWNLOADTASKINFO_H
