#ifndef CNCGLOBALFUNCS_H
#define CNCGLOBALFUNCS_H
#include <QString>
#include "sqlite3.h"


//数据库操作全局函数
extern QString ReadSqlData(QString filename,QString tablename ,\
                    int index,int mycolumn);
extern QStringList ReadSqlData(QString filename, QString strSQL, int *row, int *column);
extern QStringList ReadSqlData(QString filename, QString strSQL);
extern void InsertSqlData(QString filename,QString tablename ,\
                   QString indexname,QString sqldata);
extern int InsertSqlData(QString filename,QString strSQL);
extern int UpdateSqlData(QString filename,QString tablename ,\
                  QString columname, int index,QString strData);//0成功，1db错误，2table错误
extern int UpdateSqlData(QString filename,QString strSQL);
extern int getTableRow(QString filename,QString tablename);
extern void deleteSqlData(QString filename,QString tablename,int index);
extern void deleteAllSqlData(QString filename,QString tablename);


#endif // CNCGLOBALFUNCS_H
