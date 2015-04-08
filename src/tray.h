#ifndef TRAY_H
#define TRAY_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>

class Tray : public QObject
{
    Q_OBJECT
public:
    explicit Tray(QObject *parent = 0);
    ~Tray();

signals:
    void signal_hide();
    void signal_showNormal();
    void signal_quit();
    void signal_raise();

public slots:
    void trayiconActivated(QSystemTrayIcon::ActivationReason reason);
public:
    QSystemTrayIcon *trayIcon;
    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QMenu   *trayIconMenu;

};

#endif // TRAY_H
