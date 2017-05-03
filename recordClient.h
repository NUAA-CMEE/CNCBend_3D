#ifndef RECORDCLIENT_H
#define RECORDCLIENT_H

#include <QThread>
#include <QtNetwork/QtNetwork>
#include <QCryptographicHash>
#include "recordCmd.h"

#define MINSIZE 2*sizeof(qint64)

class RecordClient : public QObject
{
    Q_OBJECT
public:
    explicit RecordClient(QObject *parent = 0);

    int bSending;           //正在发送数据

    qint64 totalBytes;      //接受数据的总字节数
    qint64 recvdBytes;      //已经收到的字节数
    qint64 serverCmd;       //接受数据包的类型
    QByteArray inBlock;     //接受缓冲
    QFile  *localFile;      //当前接受文件的指针
    QByteArray m_buffer;    //缓存上一次或多次的未处理的数据

    int work_mode;          //服务器所处工作模式
    int request_data;       //服务器请求的数据
public:
//    stu_dataPacket dataBuff;     //数据缓存
//    Diagnosis_Show diagState;

signals:
    void sendAnswerInfo(QString msg);

private slots:
    void readMsg(void);
    void connectSlot();
    void disConnectSlot();
    void sendMsg(QString strText, qint64 cmd);
    void sendRecord();
    void sendBendFile(QString path);
    void sendPicFile(QString path);
//    void sendPLCFile();
//    void sendMachineConstsFile();
//    void sendProgConstsFile();
//    void sendManualFile();
//    void sendCurStateData();
//    void sendDiagStateData();
//    void sendRecordFile(QString strName);
//    void sendPosCmd(qint64 cmd, MainMode mode);


};

#endif // RECORDCLIENT_H
