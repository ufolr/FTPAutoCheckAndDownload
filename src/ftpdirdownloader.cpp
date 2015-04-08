#include "ftpdirdownloader.h"
#include "ftpclient.h"
#include <QDebug>
#include <QFile>
#include "sqllteinfo.h"

FtpDirDownLoader::FtpDirDownLoader(QObject *parent) : QObject(parent)
{
    fc = new FtpClient(this);
    sql = new SqlLteInfo();
//    setHost("10.128.86.4");
//    setPort("21");
//    setUser("anonymous");
//    setPwd("anonymous");
//    fc->ftpConnect();

//    fc->setFileName("F:/Temp/ftp/xz.php.fdl");
//    fc->getFile("www/xz.php");

//    setDownLoadDir("micaps/surface/N/");
//    setSaveDir("F:/Temp/ftp/");
//    getDirList();

    QDateTime fdt = QDateTime::fromString("2015-3-18-15:20:17","yyyy-M-d-H:m:s");

//    qDebug()<<"SQL1"<<sql.checkFileName("1.txt");
//    qDebug()<<"SQL2"<<sql.checkFileName("1");
//    qDebug()<<"SQL3"<<sql.checkFileSize(1);
//    qDebug()<<"SQL4"<<sql.checkFileSize(2);
//    qDebug()<<"SQL5"<<sql.checkLastModified(fdt)；
    connect(fc,SIGNAL(ftpConnected(bool)),this,SIGNAL(ftpConnected(bool)));
    connect(fc,SIGNAL(ftpCommandStatus(QString)),this,SIGNAL(ftpCommandStatus(QString)));

}

FtpDirDownLoader::~FtpDirDownLoader()
{
    fc->ftpClose();
    delete sql;
}

void FtpDirDownLoader::setHost(QString host)
{
    fc->setAddr(host);
    hostName = host;
}

void FtpDirDownLoader::setPort(QString port)
{
    fc->setPort(port);
}

void FtpDirDownLoader::setUser(QString user)
{
    fc->setUserName(user);
}

void FtpDirDownLoader::setPwd(QString pwd)
{
    fc->setPassword(pwd);
}

void FtpDirDownLoader::setDownLoadDir(QString dir)
{
    downLoadDirName = dir;
}

void FtpDirDownLoader::setSaveDir(QString dir)
{
    saveDirName = dir;
}

void FtpDirDownLoader::setDBName(QString dbn)
{
    sql->setDBFileName(dbn);
}

void FtpDirDownLoader::setLastDownTime(QDateTime dt)
{
    if(dt.isValid())
    {
        m_TasklastDownTime = dt;
        lastDownFileTime = m_TasklastDownTime;
    }
}

void FtpDirDownLoader::getDirList()
{
    fc->requestListInfo(downLoadDirName);
}

void FtpDirDownLoader::star()
{
    fc->ftpConnect();
    getDirList();
    connect(fc,SIGNAL(listGot(quint64)),this,SLOT(checkAndDownLoad(quint64)));
    connect(fc,SIGNAL(ftpGot(bool)),this,SLOT(fileGot(bool)));
    taskTableName = hostName+downLoadDirName;
    taskTableName = "\""+taskTableName+"\"";
    //如果没有任务对应的数据库变，则建立
    sql->creatTable(taskTableName);
}

void FtpDirDownLoader::ftpClose()
{
    fc->ftpClose();
}

QDateTime FtpDirDownLoader::getLastDownFileTime()
{
    return lastDownFileTime;
}

void FtpDirDownLoader::checkAndDownLoad(quint64 num)
{
    QList<QUrlInfo> ls = fc->getListInfo();//目录中所有的文件
    if(ls.length() != num || ls.length() == 0)
    {
        return;
    }
    foreach(QUrlInfo ui, ls)//判断每个文件是否需要下载
    {
        bool isAppend = false;
        if(ui.lastModified() > m_TasklastDownTime)//服务器文件时间晚于上次完成时间
        {
            qDebug()<<ui.name();
            qDebug()<<ui.lastModified();
            fileList.append(ui);
            qDebug()<<ui.name();
            isAppend = true;
        }
        if(ui.lastModified() == m_TasklastDownTime && !sql->checkDownLoaded(ui, taskTableName))//检查,未下载过,加入下载列表
        {
            fileList.append(ui);
            qDebug()<<ui.name();
            isAppend = true;
        }
        //获取下载列表中的最新时间
        if(isAppend)
        {
            if(lastDownFileTime <= ui.lastModified())
            {
                qDebug()<<"lastModified"<<fileList.first().lastModified().toString("yyyy-M-d,H:m:s");
                lastDownFileTime = ui.lastModified();
            }
        }
    }
    fileCount = fileList.length();
    downloadedCount =0;
    if(!fileList.isEmpty())
    {
        downLoadFile();
    }
    else//没有要下载的项目
    {
        //列表下载完成
        emit listDownLoadFinish();
        emit downloadProgress(0,0);
        this->deleteLater();
    }
}

void FtpDirDownLoader::fileGot(bool isGot)
{
    if(isGot)//接收到下载成功信号
    {
        emit downloadProgress(++downloadedCount,fileCount);
        QString firstFileName = fileList.first().name();
        //如果有重名文件存在，直接删除然后更新
        if(QFile::exists(saveDirName+firstFileName))
        {
            QFile::remove(saveDirName+firstFileName);
        }
        QFile::rename(saveDirName+firstFileName+".fdl",saveDirName+firstFileName);
        if(fileList.first().lastModified() == lastDownFileTime)//如果是在下载列表文件时间的最后那一分钟
        {
            //文件记录存入数据库
            sql->insertFileInfo(fileList.first(),taskTableName);
        }

        fileList.pop_front();
        if(!fileList.isEmpty())
        {
            downLoadFile();
        }
        else
        {
            //列表下载完成
            emit listDownLoadFinish();
            //清理上次记录的文件信息
            sql->cleanOutTimeData(lastDownFileTime,taskTableName);
            this->deleteLater();
        }
    }
    else//接收到下载失败信号
    {
        if(!fileList.isEmpty())
            downLoadFile();
    }
}

void FtpDirDownLoader::downLoadFile()
{
    QString fileName = fileList.first().name();
    fc->setFileName(saveDirName+fileName+".fdl");
    //qDebug()<<"getfile"<<fileName;
    fc->getFile(downLoadDirName+fileName);
}

