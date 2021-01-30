# STM32_POCSAG_Transmit
这个项目主要由STM32F103RCT6、ESP8266-01S、SR_FRS_1WV模块组成，需要配合上位机进行使用。主要用于生成POCSAG编码并进行NFM调制发送，使接收设备可以收到消息。可以向老式的BP机发送消息，前提是BP机必须工作在V段频率且支持POCSAG编码，关于这方面的知识可以从这里学习：https://www.bilibili.com/video/BV1YE411N7p7/

STM32F103RCT6可以更换成其他M3内核的单片机。

ESP8266-01S模块需要进行改装，需要将自带的1MB Flash更换成4MB，不能直接使用。如果不需要WIfi控制功能，也可以不装ESP8266。

SR_FRS_1WV模块也需要改装，不可直接使用，需要拆开屏蔽罩，将压控振荡器的输入端（26M晶振1脚）引出并与模块的第四脚相接才可以进行正常的发送。

进行无线发送时需要注意，不可空载测试，必须要接上V段天线或者假负载，否则会烧坏RF模块的功放！！

由于发送功率较大，已达到1W，空旷通信距离可以超过1km，因此务必遵守当地无线电相关法则，不可干扰其他设备正常运行，不可干扰其他人通信和卫星通信，务必通过业余无线电考试之后再进行操作！！

PCB地址：https://lceda.cn/verylk/xun-hu-fa-ye-jiv1-0_copy_copy

源码地址：https://github.com/359303267/STM32_POCSAG_Transmit

软件地址：https://github.com/359303267/STM32_POCSAG_Transmit/releases

使用教程：https://www.bilibili.com/video/BV1sZ4y1u76M
# 版权说明
该项目遵循GPL协议，个人DIY免费，未经授权不能用于商业用途！！！授权请联系359303267@qq.com  

