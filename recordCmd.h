#ifndef REMOTECMD_H
#define REMOTECMD_H

//客户端->服务器
#define     LOGIN            0x00D0    //登录数据
#define     REGISTER         0x00D1    //注册数据
#define     BACKUP           0x00D2    //备份数据

#define     BEND_FILE_SERVER 0x00D3    //客户端将折弯程序文件发送到服务器
#define     BEND_PIC_SERVER  0x00D5    //客户端将折弯图片文件发送到服务器
#define     CLIENT_NAME      0x00D4    //在客户端连入的时候发送本机名称

#define     M_STATE          0x00E0    //实时机床状态

//服务器->客户端
#define     LOGIN_ANSW       0x00F0    //登录数据服务器应答包
#define     REGISTER_ANSW    0x00F1    //注册数据服务器应答包

#define     BEND_FILE_CLIENT 0x00F2    //服务器端将折弯文件发送到指定客户端
#define     BEND_PIC_CLIENT  0x00F3    //服务器将图片发送到客户端

#define     SERVER_STATE     0x00EF    //从服务器发出实时状态数据



#endif // REMOTECMD_H
