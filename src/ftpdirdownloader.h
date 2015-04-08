#ifndef FTPDIRDOWNLOADER_H
#define FTPDIRDOWNLOADER_H

#include <QObject>
#include <QtNetwork/QUrlInfo>
#include <QString>
#include <QDateTime>
#include "sqllteinfo.h"
#include "setting.h"

class FtpClient;

class FtpDirDownLoader : public QObject
{
    Q_OBJECT
public:
    explicit FtpDirDownLoader(QObject *parent = 0);
    ~FtpDirDownLoader();

    void setHost(QString host);
    void setPort(QString port);
    void setUser(QString user);
    void setPwd(QString pwd);
    void setDownLoadDir(QString dir);
    void setSaveDir(QString dir);
    void setDBName(QString dbn);
    void setLastDownTime(QDateTime dt);
    void getDirList();
    void downLoad(const QList<QUrlInfo> &list);
    void star();
    void ftpClose();
    quint32 getTotal(){return fileCount;}
    QDateTime getLastDownFileTime();

signals:
    void listDownLoadFinish();
    void downloadProgress(quint32 now, quint32 total);
    void ftpConnected(bool);
    void ftpCommandStatus(QString);

public slots:
    void checkAndDownLoad(quint64 num);
    void fileGot(bool isGot);
private:
    FtpClient * fc;
    QString hostName;
    QString downLoadDirName;
    QString saveDirName;
    QList<QUrlInfo> fileList;
    SqlLteInfo *sql;
    quint32 fileCount;
    quint32 downloadedCount;
    QDateTime lastDownFileTime;
    QDateTime m_TasklastDownTime;
    QString taskTableName;
    void downLoadFile();
};

#endif // FTPDIRDOWNLOADER_H
