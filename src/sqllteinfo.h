#ifndef SQLLTEINFO_H
#define SQLLTEINFO_H

#include <QString>
#include <QDateTime>
#include <QtSql/QSqlDatabase>
#include <QUrlInfo>


class SqlLteInfo
{
public:
    SqlLteInfo();
    ~SqlLteInfo();
    bool checkFileName(QString fn, QString tableName);
    bool checkFileSize(quint64 fs, QString tableName);
    bool checkLastModified(QDateTime fdt, QString tableName);
    void insertFileInfo(const QUrlInfo &ui, QString tableName);
    bool checkDownLoaded(const QUrlInfo &ui, QString tableName);
    void setDBFileName(QString dbn);
    void creatTable(QString tableName);
    void cleanOutTimeData(QDateTime dt, QString tableName);

private:
    QSqlDatabase database;

};

#endif // SQLLTEINFO_H
