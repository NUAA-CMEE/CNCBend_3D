#include "CNCGlobalFuncs.h"
#include "sysglobalvars.h"
#include "stdio.h"
#include <math.h>
#include <QDebug>


/********************************************
 *function:读取数据库，单个单元格读取
 *adding:
 *author: xu
 *date: 2014/11/9
 *******************************************/
QString ReadSqlData(QString filename,QString tablename ,int index,int mycolumn)
{
    sqlite3 *db = NULL;
    int rc = 0;
    char *errmsg = NULL;    //错误信息
    char **dbResult;        //是char** 类型，两个*号
    int nRow, nColumn;
    QString str=NULL;
    QString strSQL = QString("%1%2").arg( "select * from ").arg(tablename);
    rc = sqlite3_open(filename.toUtf8(),&db);
    if(rc != 0)
    {
        //cout<<__FILE__<<__LINE__<<"DB open error"<<endl;
        sqlite3_close(db);
        return str;
    }
    rc=sqlite3_get_table( db, strSQL.toUtf8(), &dbResult, &nRow, &nColumn, &errmsg );
    if (SQLITE_OK != rc)
    {
        sqlite3_free_table( dbResult );
        //关闭数据库
        sqlite3_close( db );
        return NULL;
    }
    if(index>=nRow)     //index从0开始，故范围在0到nRow之间，取不到nRow
    {
        sqlite3_free_table( dbResult );
        //关闭数据库
        sqlite3_close( db );
        return NULL;
    }
    if( 0 == rc )
    {
        //查询成功
        str=dbResult [nColumn*index+nColumn+mycolumn];
    }

    sqlite3_free_table( dbResult );
    //关闭数据库
    sqlite3_close( db );
    return str;
}

/********************************************
 *function:重载函数，根据命令读取数据库
 *adding: 返回读取的数据，行数和列数
 *author: xu
 *date: 2014/11/9
 *******************************************/
QStringList ReadSqlData(QString filename, QString strSQL, int *row, int *column)
{
    sqlite3 *db = NULL;
    int rc = 0;
    char *errmsg = NULL;    //错误信息
    char **dbResult;        //是char** 类型，两个*号
    int nRow, nColumn;
    QStringList strList;
    rc = sqlite3_open(filename.toUtf8(),&db);
    if(rc != 0)
    {
        sqlite3_close(db);
        return strList;
    }
    rc=sqlite3_get_table( db, strSQL.toUtf8(), &dbResult, &nRow, &nColumn, &errmsg );
    if (SQLITE_OK != rc)
    {
        sqlite3_free_table( dbResult );
        //关闭数据库
        sqlite3_close( db );
        return strList;
    }
    if( 0 == rc )
    {
        //查询成功
        for(int i = nColumn; i < (nRow+1)*nColumn; i++)
        {
            strList.append(dbResult[i]);
        }
        *row = nRow;
        *column = nColumn;
    }
    sqlite3_free_table( dbResult );
    //关闭数据库
    sqlite3_close( db );
    return strList;
}

/********************************************
 *function:重载函数，根据命令读取数据库
 *adding: 返回读取的数据
 *author: xu
 *date: 2014/11/9
 *******************************************/
QStringList ReadSqlData(QString filename, QString strSQL)
{
    sqlite3 *db = NULL;
    int rc = 0;
    char *errmsg = NULL;    //错误信息
    char **dbResult;        //是char** 类型，两个*号
    int nRow, nColumn;
    QStringList strList;
    rc = sqlite3_open(filename.toUtf8(),&db);
    if(rc != 0)
    {
        sqlite3_close(db);
        return strList;
    }
    rc=sqlite3_get_table( db, strSQL.toUtf8(), &dbResult, &nRow, &nColumn, &errmsg );
    if (SQLITE_OK != rc)
    {
        sqlite3_free_table( dbResult );
        //关闭数据库
        sqlite3_close( db );
        return strList;
    }
    if( 0 == rc )
    {
        //查询成功
        for(int i = nColumn; i < (nRow+1)*nColumn; i++)
        {
            strList.append(dbResult[i]);
        }
    }
    sqlite3_free_table( dbResult );
    //关闭数据库
    sqlite3_close( db );
    return strList;
}

int UpdateSqlData(QString filename,QString tablename ,QString columname, int index,QString strData)
{
    sqlite3 *db = NULL;
    int rc = 0;
    char *errmsg = NULL;        //错误信息

    QString strSQL=QString("%1%2%3%4%5%6%7%8%9")
                                    .arg( "update ")
                                    .arg(tablename)
                                    .arg(" set ")
                                    .arg(columname)
                                    .arg("=")
                                    .arg("'")
                                    .arg(strData)
                                    .arg( "' where id =")   //需要修改
                                    .arg(index+1);
    rc = sqlite3_open(filename.toUtf8(),&db);
    if(rc != 0)
    {
        sqlite3_close(db);
        return 1;
    }

    rc = sqlite3_exec(db, strSQL.toUtf8(), NULL,NULL,&errmsg);

    if(rc != 0 )
    {
        qDebug()<<strSQL;
        sqlite3_close(db);
        return 2;
    }
    sqlite3_close(db);
    return 0;
}

/********************************************
 *function:向数据库中更新数据
 *adding:
 *author: xu
 *date: 2014/11/19
 *******************************************/
int UpdateSqlData(QString filename, QString strSQL)
{
    sqlite3 *db = NULL;
    int rc = 0;
    char *errmsg = NULL;        //错误信息
    rc = sqlite3_open(filename.toUtf8(),&db);
    if(rc != 0)
    {
        sqlite3_close(db);
        return 1;
    }
    rc = sqlite3_exec(db, strSQL.toUtf8(), NULL,NULL,&errmsg);

    if(rc != 0 )
    {
        qDebug()<<strSQL;
        sqlite3_close(db);
        return 2;
    }
    sqlite3_close(db);
    return 0;
}

/********************************************
 *function:向数据库中插入数据
 *adding:
 *author: xu
 *date: 2014/11/9
 *******************************************/
void InsertSqlData(QString filename,QString tablename ,QString indexname,QString sqldata)
{

    sqlite3 *db = NULL;
    int rc = 0;
    char *errmsg = NULL;        //错误信息
    //char **dbResult;
    QString strSQL = QString("%1%2%3%4%5%6%7").arg( "insert into ")
                                            .arg(tablename)
                                            .arg("(")
                                            .arg(indexname)
                                            .arg(") values (")
                                            .arg(sqldata)
                                            .arg(")");

    rc = sqlite3_open(filename.toUtf8(),&db);
    if(rc != 0)
    {
        //cout<<__FILE__<<__LINE__<<"DB open error"<<endl;
        sqlite3_close(db);
        return;
    }
    if(0)   //如果数据库不存在则建立数据库
    {
        //建立数据库，插入一些数据
        rc = sqlite3_exec(db,"create table programlistpic(id INTEGER primary key,productID TEXT,drawnumber TEXT,thickness  TEXT,materialID TEXT,lenth TEXT,direction  TEXT,picfilename TEXT)",NULL,NULL,&errmsg );
        //ID号，内容
        if(rc != 0)	//未完成，忽略已存在错误
        {
            // do nothing
        }
        return; //仅仅完成建表工作
    }
    rc = sqlite3_exec( db,strSQL.toUtf8(), 0, 0, &errmsg );
    if(rc != 0 )
    {
        //cout<<__FILE__<<__LINE__<< "fail to insert"<<endl;
        sqlite3_close(db);
        return ;
    }
    sqlite3_close(db);
    return;
}

/********************************************
 *function:重载函数，根据命令插入数据库
 *adding:
 *author: xu
 *date: 2014/11/9
 *******************************************/
int InsertSqlData(QString filename, QString strSQL)
{
    sqlite3 *db = NULL;
    int rc = 0;
    char *errmsg = NULL;        //错误信息
    rc = sqlite3_open(filename.toUtf8(),&db);
    if(rc != 0)
    {
        sqlite3_close(db);
        return -1;
    }
    rc = sqlite3_exec(db,strSQL.toUtf8(), 0, 0, &errmsg);
    if(rc != 0 )
    {
        sqlite3_close(db);
        return -1;
    }
    sqlite3_close(db);
    return 0;
}

int getTableRow(QString filename, QString tablename)
{
    sqlite3 *db = NULL;
    int rc = 0;
    char *errmsg = NULL;    //错误信息
    char **dbResult;        //是char** 类型，两个*号
    int nRow, nColumn;
    QString strSQL = QString("%1%2").arg( "select * from ").arg(tablename);
    rc = sqlite3_open(filename.toUtf8(),&db);
    if(rc != 0)
    {
        //cout<<__FILE__<<__LINE__<<"DB open error"<<endl;
        sqlite3_close(db);
        return 0;
    }
    rc=sqlite3_get_table(db, strSQL.toUtf8(), &dbResult, &nRow, &nColumn, &errmsg);
    if (SQLITE_OK != rc)
    {
        sqlite3_free_table(dbResult);
        //关闭数据库
        sqlite3_close(db);
        return 0;
    }

    sqlite3_free_table(dbResult);
    //关闭数据库
    sqlite3_close(db);
    return nRow;
}


void deleteSqlData(QString filename, QString tablename, int index)
{
    sqlite3 *db = NULL;
    int rc = 0;
    char *errmsg = NULL;    //错误信息

    QString strSQL = QString("%1%2%3%4").arg( "delete from ").arg(tablename).arg(" where id = ").arg(index);
    rc = sqlite3_open(filename.toUtf8(),&db);
    if(rc != 0)
    {
        //cout<<__FILE__<<__LINE__<<"DB open error"<<endl;
        sqlite3_close(db);
        return;
    }
    rc = sqlite3_exec( db,strSQL.toUtf8(), 0, 0, &errmsg );
    if(rc != 0 )
    {
        //cout<<__FILE__<<__LINE__<< "fail to insert"<<endl;
        sqlite3_close(db);
        return ;
    }
    sqlite3_close(db);
}

void deleteAllSqlData(QString filename, QString tablename)
{
    sqlite3 *db = NULL;
    int rc = 0;
    char *errmsg = NULL;    //错误信息

    QString strSQL = QString("%1%2").arg( "delete from ").arg(tablename);
    rc = sqlite3_open(filename.toUtf8(),&db);
    if(rc != 0)
    {
        //cout<<__FILE__<<__LINE__<<"DB open error"<<endl;
        sqlite3_close(db);
        return;
    }
    rc = sqlite3_exec( db,strSQL.toUtf8(), 0, 0, &errmsg );
    if(rc != 0 )
    {
        //cout<<__FILE__<<__LINE__<< "fail to insert"<<endl;
        sqlite3_close(db);
        return ;
    }
    sqlite3_close(db);
}
