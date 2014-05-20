lke2600读卡器的windows驱动
====

lke2600是接触式IC卡读卡器，本程序是windows上的ocx动态库，
通过串口驱动读卡器，实现读写ic卡.

1.0
实现基本功能
2.0
在1.0的基础上,
1 加入了try/catch异常处理, 捕捉SE异常. 
2 去掉CDebug类, 用CFileManage类代替
3 加入了一些配置文件
