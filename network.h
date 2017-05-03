#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QCryptographicHash>

class NetWork : public QObject
{
    Q_OBJECT
public:
    explicit NetWork(QObject *parent = 0);
    ~NetWork();

    void netConnection();
    void sendLoginInfo(QString usr,QString pswd);
    void sendRegisterInfo(QString usr,QString pswd);
    bool getLinkState();
    void backUpData();
    void sendBendFile(QString path);
    void sendBendPic(QString path);
    QString getMsg();

signals:
    void sendMsgSign(QString strText,qint64 cmd);
    void sendRecordFileSign();
    void sendBendFileSign(QString path);
    void sendBendPicSign(QString path);

public slots:
    void getAnswerInfo(QString msg);

private:
    //md5加密，防止数据包在传输过程中被别人捕获。
    QCryptographicHash *md5;
    QString answerMsg;
};

#endif // NETWORK_H
