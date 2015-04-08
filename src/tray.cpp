#include "tray.h"

Tray::Tray(QObject *parent) :
    QObject(parent)
{
    //创建图标
    QIcon icon = QIcon("TrayIcon.png");
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(icon);
    trayIcon->setToolTip(tr("气象数据下载器"));
    trayIcon->show();
    //气泡提示
    QString titlec=tr("气象数据下载器");
    QString textc=tr("程序已转到后台服务");
    trayIcon->showMessage(titlec,textc,QSystemTrayIcon::Information,5000);
    //添加单/双击鼠标相应
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,SLOT(trayiconActivated(QSystemTrayIcon::ActivationReason)));
    //创建动作
    minimizeAction = new QAction(tr("最小化"), this);
    connect(minimizeAction, SIGNAL(triggered()), SIGNAL(signal_hide()));
    restoreAction = new QAction(tr("还原"), this);
    connect(restoreAction, SIGNAL(triggered()),SIGNAL(signal_showNormal()));
    quitAction = new QAction(tr("退出"), this);
    connect(quitAction, SIGNAL(triggered()),SIGNAL(signal_quit()));

    //创建右键弹出菜单
    trayIconMenu = new QMenu;
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();//分隔
    trayIconMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayIconMenu);
}

Tray::~Tray()
{
    trayIconMenu->deleteLater();
    trayIcon->deleteLater();
}

void Tray::trayiconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    //单击托盘图标
    case QSystemTrayIcon::Trigger:
        break;
    case QSystemTrayIcon::DoubleClick:
        //双击托盘图标
        emit signal_showNormal();
        emit signal_raise();
        break;
    default:
        break;
    }
}
