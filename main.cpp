#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QNetworkInterface>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));//为tr()这个做设置
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();

    MainWindow w;
    w.show();
    return a.exec();
}
QStringList get_mac(){
    QStringList mac_list;
    QString strMac;

}
