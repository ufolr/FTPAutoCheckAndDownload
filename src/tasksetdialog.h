#ifndef TASKSETDIALOG_H
#define TASKSETDIALOG_H

#include <QDialog>
#include <setting.h>

class QListWidgetItem;

namespace Ui {
class TaskSetDialog;
}

class TaskSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TaskSetDialog(QWidget *parent = 0);
    ~TaskSetDialog();
    
    void initTaskInfo(Task t);
    Task creatNewTask();
    void changeTmpTaskList(quint16 index);


private:
    Ui::TaskSetDialog *ui;
    Setting *setting;
    QList<Task> m_tasks;
    qint16 m_lastRow;
    quint16 m_tHour;
    quint16 m_tMinute;
private slots:
    void onCurrentRowChanged(int row);
    void on_pushButton_newTask_clicked();
    void on_pushButton_delete_clicked();
    void on_pushButton_apply_clicked();
    void launchComboBoxChanged(int index);

    void on_pushButton_ok_clicked();
    void on_pushButton_cancel_clicked();
    void on_pushButton_saveAs_clicked();
    void on_comboBox_time_currentIndexChanged(int index);
    void on_spinBox_time_valueChanged(int arg1);
};

#endif // TASKSETDIALOG_H
