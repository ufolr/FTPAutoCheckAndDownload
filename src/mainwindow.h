#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include "setting.h"
class TaskManager;
class DownLoadTaskInfo;

namespace Ui {
class Dialog;
}

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_StarWork_clicked();

    void iterateTask();
    void handleDownloadProgress(QString TaskName, quint64 now, quint64 total);
    void handleTaskConnect(QString taskName, bool isConnect);
    void onftpCommandStatus(QString status);

protected:
    //void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *e);

private:
    Ui::Dialog *ui;
    TaskManager *tm;
    Setting* setting;
    bool autoRun;
    quint16 listWidgetMaxCount;
    DownLoadTaskInfo *currentTaskinfo;

    void initTaskManagerConnection();

private slots:
    void onAutoRunStatusChange(int);
    void on_pushButton_taskManage_clicked();
    void on_pushButton_2_clicked();
    void messageReceived(const QString &str);
};

#endif // MAINWINDOW_H
