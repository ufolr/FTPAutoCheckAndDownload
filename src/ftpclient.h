#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QObject>
#include <QHash>
#include <QDateTime>

class QFtp;
class QFile;
class QTimer;
class QUrlInfo;

class FtpClient : public QObject
{
    Q_OBJECT
public:
    explicit FtpClient(QObject *parent = 0);
    FtpClient(const FtpClient &fc);
    ~FtpClient();
    int ftpConnect();
    void ftpClose();
    void makeFTPConnection(QFtp *pQFtp);
    void setAddr(const QString& address){m_addr = address;}
    void setPort(const QString& port){m_port = port;}
    void setUserName(const QString& username){m_userName = username;}
    void setPassword(const QString& pwd){m_password = pwd;}
    void setFileName(const QString& fileName){m_fileName = fileName;}
    void setReConnect(bool isRe){m_isReConnect = isRe;}

    QString	getStatus(){return m_ftpStatus; }
    void getFile(const QString& ftpFileName);

    QString getPort()const {return m_port;}
    QString getAddr()const {return m_addr;}
    QString getUser()const {return m_userName;}
    QString getPwd()const {return m_password;}
    QString getFileName()const {return m_fileName;}


signals:
    void ftpConnected(bool isConnect);
    void ftpLogined(bool isLogined);
    void ftpDownloading();
    void ftpGot(bool error);
    void dataTransferProgress(qint64,qint64);
    void ftpCommandStatus(QString);
    void cmdList(bool cmdlist);
    void listGot(quint64 num);
    void ftpReConnected(bool isConnect);
    void ftpReLogined(bool isLogined);



public slots:
    void slot_ftpCommandStarted(int);
    void slot_ftpCommandFinished(int commandId,bool error);
    void statusChanged(int i);
    void handleTimeOut();
    void timeOutWatcher(qint64 down, qint64 total);
    void addToFileList(const QUrlInfo &ui);
    void requestListInfo(QString dir);
    QList<QUrlInfo>& getListInfo(){return m_fileList;}
    void listGotclot(quint64 num);

private:
    QList<QUrlInfo> m_fileList;
    QFtp*	m_ftp;
    QFile*	m_file;
    QString m_fileName;
    QString m_addr;
    QString m_port;
    QString m_userName;
    QString m_password;
    QString m_ftpStatus;
    QTimer *m_timeOut;
    qint64 m_lastDown;
    quint32 m_fileListLength;
    bool m_isReConnect;
};

#endif // FTPCLIENT_H
