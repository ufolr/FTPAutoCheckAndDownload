#include "sqllteinfo.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

SqlLteInfo::SqlLteInfo()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    setDBFileName("FileRecord.db");
    database.setUserName("root");
    database.setPassword("123456");
    if (!database.open())
    {
        qDebug()<< "DB Open Filed";
        qDebug()<<database.lastError();
        return;
    }
    //QSqlQuery sql_query;
}

SqlLteInfo::~SqlLteInfo()
{
    database.close();
}

bool SqlLteInfo::checkFileName(QString fn, QString tableName)
{
    QSqlQuery sql_query;
    QString checkSql = "select * from "+tableName+" WHERE FileName='"+fn+"'";
    qDebug()<<checkSql;
    sql_query.prepare(checkSql);
    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
        return false;
    }
    else
    {
        if(sql_query.next())
        {
            if(sql_query.value(0).toString()==fn)
                return true;
            else
                return false;
        }
        else
        {
            return false;
        }
    }
}

bool SqlLteInfo::checkFileSize(quint64 fs, QString tableName)
{
    QSqlQuery sql_query;
    QString checkSql = "select * from "+tableName+" WHERE FileSize="+QString::number(fs);
    qDebug()<<checkSql;
    sql_query.prepare(checkSql);
    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
        return false;
    }
    else
    {
        if(sql_query.next())
        {
            if(sql_query.value(1).toLongLong()==fs)
                return true;
            else
                return false;
        }
        else
        {
            return false;
        }
    }
}

bool SqlLteInfo::checkLastModified(QDateTime fdt, QString tableName)
{
    //查询
    qint64 fdt_int = fdt.toMSecsSinceEpoch();
   QSqlQuery sql_query;
    QString checkSql = "select * from "+tableName+" WHERE LastModified=(:dt)";
    sql_query.prepare(checkSql);
    sql_query.bindValue(":dt",fdt_int);
    qDebug()<<checkSql;
    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
        return false;
    }
    else
    {
        if(sql_query.next())
        {
            if(sql_query.value(2).toLongLong()==fdt_int)
                return true;
            else
                return false;
        }
        else
        {
            return false;
        }
    }
}

void SqlLteInfo::insertFileInfo(const QUrlInfo &ui, QString tableName)
{
    QString fn = ui.name();
    quint64 fs = ui.size();
    QDateTime fdt = ui.lastModified();

    QSqlQuery sql_query(database);
    QString checkSql = "insert into "+tableName+"(FileName,FileSize,LastModified)"
                       "VALUES(:FileName,:FileSize,:LastModified)";
    sql_query.prepare(checkSql);
    sql_query.bindValue(":FileName",fn);
    sql_query.bindValue(":FileSize",fs);
    sql_query.bindValue(":LastModified",fdt.toMSecsSinceEpoch());
    qDebug()<<checkSql;

    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
    }
}

bool SqlLteInfo::checkDownLoaded(const QUrlInfo &ui, QString tableName)
{
    if(checkFileName(ui.name(), tableName))
    {
        if(checkFileSize(ui.size(), tableName)&&checkLastModified(ui.lastModified(), tableName))
            return true;
        else
            return false;
    }
    else
    {
        return false;
    }
}

void SqlLteInfo::setDBFileName(QString dbn)
{
    database.setDatabaseName(dbn);
}


void SqlLteInfo::creatTable(QString tableName)
{
    QSqlQuery sql_query(database);
    QString creatSql = QString()+"create table if not exists  "+tableName+" ("
                                                    +"FileName"+"  TEXT(128) NOT NULL DEFAULT filename,"
                                                    +"FileSize"+"  INTEGER,"
                                                    +"LastModified"+"  INTEGER"
                                                    ")";
    sql_query.prepare(creatSql);
    //sql_query.bindValue(":tableName",tableName);
    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
    }
}

void SqlLteInfo::cleanOutTimeData(QDateTime dt, QString tableName)
{
    //删除记录
    QSqlQuery sql_query(database);
    QString cleanSql = "delete FROM "+tableName+" where LastModified!=:dateTime";
    sql_query.prepare(cleanSql);
    sql_query.bindValue(":dateTime",dt.toMSecsSinceEpoch());
    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
    }
//    //清理数据文件
//    cleanSql = "VACUUM";
//    sql_query.prepare(cleanSql);
//    if(!sql_query.exec())
//    {
//        qDebug()<<sql_query.lastError();
//    }
}
