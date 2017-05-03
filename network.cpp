#include "network.h"
#include "sysglobalvars.h"
#include <QDebug>
#include "recordCmd.h"

NetWork::NetWork(QObject *parent) :
    QObject(parent)
{
    md5 = new QCryptographicHash(QCryptographicHash::Md5);
    answerMsg = "";
}

NetWork::~NetWork()
{
}

/********************************************
 *function:用户网络服务器登录
 *adding:
 *author: xu
 *date: 2015/12/19
 *******************************************/
void NetWork::netConnection()
{
    //新建一个线程，处理网络通信
    tcpClient = new QTcpSocket();
    //连接之前判断是否已经连接服务器，已连接的要断开
    if(tcpClient->isValid())
        tcpClient->close();
    //一旦连接成功,QTcpSocket类将发出connected()信号
    tcpClient->connectToHost(strHost,16689);
    mClient = new RecordClient(0);
    connect(tcpClient,SIGNAL(connected()),mClient,SLOT(connectSlot()));
    connect(tcpClient,SIGNAL(disconnected()),mClient,SLOT(disConnectSlot()));
    connect(tcpClient,SIGNAL(readyRead()),mClient,SLOT(readMsg()));
    connect(this,SIGNAL(sendMsgSign(QString,qint64)),mClient,SLOT(sendMsg(QString,qint64)));
    connect(this,SIGNAL(sendBendFileSign(QString)),mClient,SLOT(sendBendFile(QString)));
    connect(this,SIGNAL(sendBendPicSign(QString)),mClient,SLOT(sendPicFile(QString)));
    connect(mClient,SIGNAL(sendAnswerInfo(QString)),this,SLOT(getAnswerInfo(QString)));
    connect(this,SIGNAL(sendRecordFileSign()),mClient,SLOT(sendRecord()));
    clientThread = new QThread();
    mClient->moveToThread(clientThread);
    tcpClient->moveToThread(clientThread);
    clientThread->start();
}

/********************************************
 *function:向服务器段发送数据包进行认证操作
 *adding:
 *author: xu
 *date: 2015/12/19
 *******************************************/
void NetWork::sendLoginInfo(QString usr, QString pswd)
{
//    connect(this,SIGNAL(sendMsg(QString,qint64)),mClient,SLOT(sendMsg(QString,qint64)));
//    connect(mClient,SIGNAL(sendAnswerInfo(QString)),this,SLOT(getAnswerInfo(QString)));
    qDebug()<<"login verify";
    QString msg = usr+"#"+pswd;
    //对登录账号和登录密码进行md5加密
    md5->reset();
    md5->addData(msg.toUtf8());
    msg = "";
    msg.append(md5->result().toHex());
    emit sendMsgSign(msg,LOGIN);
}

/********************************************
 *function:向服务器段发送注册信息
 *adding:
 *author: xu
 *date: 2015/12/20
 *******************************************/
void NetWork::sendRegisterInfo(QString usr, QString pswd)
{
//    connect(this,SIGNAL(sendMsg(QString,qint64)),mClient,SLOT(sendMsg(QString,qint64)));
//    connect(mClient,SIGNAL(sendAnswerInfo(QString)),this,SLOT(getAnswerInfo(QString)));
    QString msg = usr+"#"+pswd;
    emit sendMsgSign(msg,REGISTER);
}

/********************************************
 *function:获取网络连接状态
 *adding:
 *author: xu
 *date: 2015/12/20
 *******************************************/
bool NetWork::getLinkState()
{
    return bLinked;
}

/********************************************
 *function:备份记录数据
 *adding:
 *author: xu
 *date: 2015/12/20
 *******************************************/
void NetWork::backUpData()
{
    //connect(this,SIGNAL(sendRecordFile()),mClient,SLOT(sendRecord()));
    emit sendRecordFileSign();
}

/********************************************
 *function:发送折弯程序文件
 *adding:
 *author: xu
 *date: 2016/03/07
 *******************************************/
void NetWork::sendBendFile(QString path)
{
    emit sendBendFileSign(path);
}

void NetWork::sendBendPic(QString path)
{
    emit sendBendPicSign(path);
}

/********************************************
 *function:获取服务器应答信息
 *adding:
 *author: xu
 *date: 2015/12/20
 *******************************************/
QString NetWork::getMsg()
{
    return answerMsg;
}

void NetWork::getAnswerInfo(QString msg)
{
    answerMsg = msg;
}

