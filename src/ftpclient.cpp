#include "ftpclient.h"
#include <QtNetwork/QFtp>
#include <QFile>
#include <QMessageBox>
#include <QTimer>
#include <QtNetwork/QUrlInfo>

#include <QDebug>

FtpClient::FtpClient(QObject *parent) :
    QObject(parent)
{
    m_file =NULL;
    m_ftp = new QFtp(this);
    setPort("21");//设置默认端口
    setUserName("anonymous");
    setPassword("anonymous");
    m_timeOut = new QTimer(this);
    connect(m_timeOut,SIGNAL(timeout()),this,SLOT(handleTimeOut()));
    //当每条命令开始执行时发出相应的信号
    connect(m_ftp,SIGNAL(commandFinished(int ,bool)),this,SLOT(slot_ftpCommandFinished(int,bool)));
    connect(m_ftp,SIGNAL(commandStarted(int)), this,SLOT(slot_ftpCommandStarted(int)));
    connect(m_ftp,SIGNAL(dataTransferProgress(qint64,qint64)),this,SIGNAL(dataTransferProgress(qint64,qint64)));
    connect(m_ftp,SIGNAL(dataTransferProgress(qint64,qint64)),this,SLOT(timeOutWatcher(qint64,qint64)));
    connect(m_ftp,SIGNAL(stateChanged(int)),this,SLOT(statusChanged(int)));
    connect(m_ftp,SIGNAL(listInfo(QUrlInfo)),this,SLOT(addToFileList(QUrlInfo)));
    connect(this,SIGNAL(listGot(quint64)),this,SLOT(listGotclot(quint64)));
}

FtpClient::~FtpClient()
{
    m_file->deleteLater();
}

int FtpClient::ftpConnect()
{
    if(m_addr.isEmpty()||m_port.isEmpty()||m_userName.isEmpty()||m_password.isEmpty())
    {
        QMessageBox::information(0,
                                 tr("错误提示："),
                                 tr("无法连接FTP服务器,请仔细检查地址、端口号、用户名、密码是否正确。"),
                                 QMessageBox::Ok);
        emit ftpConnected(false);
        return -1;
    }
    qDebug()<<m_ftp->connectToHost(m_addr,m_port.toUShort()); //连接到服务器
    qDebug()<<m_ftp->login(m_userName,m_password);  //登录
    emit ftpConnected(true);
    return 1;
}

void FtpClient::ftpClose()
{
    m_ftp->close();
}

void FtpClient::getFile(const QString &ftpFileName)
{
//    if(QFile::exists(m_fileName))//判断文件是否存在
//    {
//        QMessageBox::information(0,tr("FTP"),
//                                 tr("文件：%1 已存在.").arg(m_fileName));
//        m_ftpStatus = tr("文件%1已存在。").arg(m_fileName);
//        return;
//    }
    //文件下载默认存储路径：/home/briup/下载
    m_file = new QFile(m_fileName);
    if(!m_file->open(QFile::WriteOnly))
    {
        QMessageBox::information(0,tr("FTP"),
                                 tr("创建文件%1失败: %2。").arg(m_fileName).arg(m_file->errorString()));
        m_ftpStatus = tr("创建文件%1失败: %2。").arg(m_fileName).arg(m_file->errorString());
        delete m_file;
        return;
    }
    //m_ftpStatus = tr("文件:%1创建成功。").arg(m_fileName);
    m_ftp->get(ftpFileName,m_file);
    m_timeOut->start(15000);//超时时间
    emit ftpDownloading();
}

void FtpClient::slot_ftpCommandStarted(int)
{
    if(m_ftp->currentCommand() == QFtp::ConnectToHost)
    {
        m_ftpStatus = tr("正在连接到FTP服务器:%1").arg(m_addr);
    }
    if (m_ftp->currentCommand() == QFtp::Login){
        m_ftpStatus = tr("%1正在登录…").arg(m_userName);
    }
    if (m_ftp->currentCommand() == QFtp::Get){
        m_ftpStatus = tr("文件：%1开始下载..").arg(m_file->fileName());
    }
    if (m_ftp->currentCommand() ==QFtp::List)
    {
        m_ftpStatus = tr("正在获取列表…");
    }
    else if (m_ftp->currentCommand() == QFtp::Close){
        m_ftpStatus = tr("正在关闭连接…");
    }
    emit ftpCommandStatus(m_ftpStatus);
}

void FtpClient::slot_ftpCommandFinished(int commandId, bool error)
{
    //格式化状态信息
    if (m_ftp->currentCommand() == QFtp::ConnectToHost)
    {
        if (error) {
            m_ftpStatus = tr("无法连接FTP服务器:%1").arg(m_addr);

            emit ftpConnected(false);
            return;
        }
    }
    if(m_ftp->currentCommand() == QFtp::Login)
    {
        m_ftpStatus = tr("成功连接上FTP服务器：%1 ").arg(m_addr);
        emit ftpConnected(true);
    }
    qDebug()<<"command"<<m_ftp->currentCommand();
    if(m_ftp->currentCommand() == QFtp::Get)
    {
        if(error)
        {
            m_ftpStatus = tr("下载文件:%1出错。").arg(m_file->fileName());
            m_file->close();
            m_file->remove();
            //this->deleteLater();

            emit ftpGot(false);
        }else
        {
            m_ftpStatus = tr("文件：%1下载完成").arg(m_file->fileName());
            //m_ftp->close();//否则下载完一个文件后ftp就断开了，
            m_file->close();
            //this->deleteLater();

            emit ftpGot(true);
            if(m_timeOut->isActive())//重置超时
            {
                m_timeOut->stop();
            }
        }
        //progressDialog->hide();
    }
    if (m_ftp->currentCommand() == QFtp::Close)
    {
        m_ftpStatus = tr("FTP服务器连接已关闭");
    }
    if(m_ftp->currentCommand() == QFtp::List)
    {
        m_ftpStatus = tr("列表获取成功.");
        emit listGot(m_fileList.length());
     }
    emit ftpCommandStatus(m_ftpStatus);
    qDebug()<<m_ftpStatus;
}

void FtpClient::statusChanged(int i)
{
    qDebug()<<"statusChanged"<<i;
}

void FtpClient::handleTimeOut()
{
    m_ftpStatus = tr("下载超时");
    emit ftpGot(false);
    qDebug()<<"timeout...";
    m_timeOut->stop();
    m_ftp->abort();
    m_file->close();
    m_file->remove();

    //this->deleteLater();
}

void FtpClient::timeOutWatcher(qint64 down,qint64 total)
{
    if(m_lastDown != down)//未超时
    {
        if(m_timeOut->isActive())//重置超时
        {
            m_timeOut->stop();
        }
        m_timeOut->start(15000);
    }
    m_lastDown = down;
}

void FtpClient::addToFileList(const QUrlInfo& ui)
{
    //提取出文件
    if(ui.isFile())
        m_fileList.append(ui);
}

void FtpClient::requestListInfo(QString dir)
{
    m_ftp->list(dir);
}

void FtpClient::listGotclot(quint64 num)
{
    qDebug()<<"listget!"<<num;
}
