#include "recordClient.h"
#include "sysglobalvars.h"
#include <QTextCodec>
#include <QDir>

RecordClient::RecordClient(QObject *parent) :
    QObject(parent)
{
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    totalBytes = 0;
    recvdBytes = 0;
    serverCmd = 0;
}

void RecordClient::sendPicFile(QString path)
{
    QDir dir(path);
    QStringList filter;
    filter<<"*.bmp";
    dir.setNameFilters(filter);
    QFileInfoList list = dir.entryInfoList();
    for(int i = 0; i < list.length(); i++){
        QFile localFile(list.at(i).filePath());
        if(!localFile.open(QFile::ReadOnly))
            return;
        qDebug()<<"open data file success";
        qint64 totalBytes = 0;
        QByteArray outBlock;
        outBlock.resize(0);
        QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
        sendOut.setVersion(QDataStream::Qt_4_8);
        totalBytes = localFile.size();

        sendOut<<qint64(totalBytes)<<qint64(BEND_PIC_SERVER);
        totalBytes += outBlock.size();
        sendOut.device()->seek(0);
        sendOut<<totalBytes;
        tcpClient->write(outBlock);
        outBlock.resize(0);

        outBlock = localFile.readAll();
        tcpClient->write(outBlock);
        outBlock.resize(0);
        localFile.close();
    }
}

/********************************************
 *function:向服务器发送应答内容槽函数
 *adding:
 *author: xu
 *date: 2015/08/05
 *******************************************/
void RecordClient::sendMsg(QString strText,qint64 cmd)
{
    qDebug()<<"send msg";
    if(!tcpClient->isValid())
        return;
    //构造数据包
    qint64 totalBytes = 0;
    QByteArray outBlock;
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_8);
    totalBytes = strText.toUtf8().size();

    sendOut<<qint64(totalBytes)<<qint64(cmd);
    totalBytes += outBlock.size();
    sendOut.device()->seek(0);
    sendOut<<totalBytes;
    tcpClient->write(outBlock);
    outBlock.resize(0);
    for(int i = 0; i < 10000; i++);

    outBlock = strText.toUtf8();
    tcpClient->write(outBlock);
    outBlock.resize(0);
}

/********************************************
 *function:向服务器发送记录数据槽函数
 *adding:
 *author: xu
 *date: 2015/08/05
 *******************************************/
void RecordClient::sendRecord()
{
   QString fileName = filePath;
   QFile localFile(fileName);
   if(!localFile.open(QFile::ReadOnly))
       return;
   qDebug()<<"open data file success";
   qint64 totalBytes = 0;
   QByteArray outBlock;
   outBlock.resize(0);
   QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
   sendOut.setVersion(QDataStream::Qt_4_8);
   totalBytes = localFile.size();

   sendOut<<qint64(totalBytes)<<qint64(BACKUP);
   totalBytes += outBlock.size();
   sendOut.device()->seek(0);
   sendOut<<totalBytes;
   tcpClient->write(outBlock);
   outBlock.resize(0);

   outBlock = localFile.readAll();
   tcpClient->write(outBlock);
   outBlock.resize(0);
   localFile.close();
}

/********************************************
 *function:向服务器发送折弯文件槽函数
 *adding:
 *author: xu
 *date: 2016/03/07
 *******************************************/
void RecordClient::sendBendFile(QString path)
{
    QFile localFile(path);
    if(!localFile.open(QFile::ReadOnly))
        return;
    qDebug()<<"open data file success";
    qint64 totalBytes = 0;
    QByteArray outBlock;
    outBlock.resize(0);
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_8);
    totalBytes = localFile.size();

    sendOut<<qint64(totalBytes)<<qint64(BEND_FILE_SERVER);
    totalBytes += outBlock.size();
    sendOut.device()->seek(0);
    sendOut<<totalBytes;
    tcpClient->write(outBlock);
    outBlock.resize(0);

    outBlock = localFile.readAll();
    tcpClient->write(outBlock);
    outBlock.resize(0);
    localFile.close();
}

/********************************************
 *function:读取服务器发过来的数据
 *adding:
 *author: liu
 *date: 2015/07/26
 *******************************************/
void RecordClient::readMsg()
{
    //缓冲区没有数据，直接无视
    if(tcpClient->bytesAvailable() <= 0)
        return;
    //临时获得从缓存区取出来的数据,但是不确定每次取出来的是多少
    QByteArray buffer;
    buffer = tcpClient->readAll();
    //上次缓存加上这次缓存
    m_buffer.append(buffer);
    unsigned int totalLen = m_buffer.size();
    while (totalLen) {
        //与QDataStream绑定,方便操作
        QDataStream packet(m_buffer);
        packet.setVersion(QDataStream::Qt_4_8);
        //不够包头的数据直接不处理
        if(totalLen < MINSIZE)
            break;
        packet>>totalBytes>>serverCmd;
        //如果不够,长度等够了再来解析
        if(totalLen < totalBytes)
            break;
        //数据足够多，且满足我们定义的包头的几种类型
        switch (serverCmd) {
        case LOGIN_ANSW:
        {
            QByteArray datas = m_buffer.mid(MINSIZE,totalBytes-MINSIZE);
            QString strInfo;
            strInfo.prepend(datas);
            emit sendAnswerInfo(strInfo);
            qDebug()<<strInfo;
            break;
        }
        case REGISTER_ANSW:
        {
            QByteArray datas = m_buffer.mid(MINSIZE,totalBytes-MINSIZE);
            QString strInfo;
            strInfo.prepend(datas);
            emit sendAnswerInfo(strInfo);
            qDebug()<<strInfo;
            break;
        }
        case SERVER_STATE:
        {
            //qDebug()<<sizeof(stu_stateData);
            QByteArray state_data = m_buffer.mid(MINSIZE,totalBytes-MINSIZE);
            pthread_mutex_lock(&dataAreaMutex);
            memcpy(&stateData,state_data.data(),sizeof(stu_stateData));
            pthread_mutex_unlock(&dataAreaMutex);
            break;
        }
        default:
            break;
        }
        //缓存多余的数据
        buffer = m_buffer.right(totalLen-totalBytes);
        //更新长度
        totalLen = buffer.size();
        //更新多余的数据
        m_buffer = buffer;
    }
}

/********************************************
 *function:断开连接的槽函数
 *adding:
 *author: xu
 *date: 2015/07/29
 *******************************************/
void RecordClient::disConnectSlot()
{
    qDebug()<<"网络连接断开！";
    bLinked = false;
}

/********************************************
 *function:发送机床参数
 *adding:
 *author: liu
 *date: 2015/07/27
 *******************************************/
void RecordClient::connectSlot()
{
    if(tcpClient->isValid())
    {
        qDebug()<<"网络连接成功！";
        bLinked = true;
        sendMsg(tr("Client_PC"),CLIENT_NAME);
    }
}


