# Tetris
多人联机的俄罗斯方块游戏，游戏规则模仿任天堂Tetris99，基于qt，kcp，protobuf，qslog。

## 游戏截图
### 游戏菜单
windows端游戏菜单
![在这里插入图片描述](https://img-blog.csdnimg.cn/417d1825228a4227b6e1c9c3309497d1.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)
linux端游戏菜单
![在这里插入图片描述](https://img-blog.csdnimg.cn/7c3fbf60f52a47b78966b69226263996.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)
### 1v1对战
Windows游戏界面（左边为用户，右边为对手）
![在这里插入图片描述](https://img-blog.csdnimg.cn/95550e391a3a47808bb9cd764776bd29.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)
Windows方失败
![在这里插入图片描述](https://img-blog.csdnimg.cn/1e47b19ed1394e9dad00fc80c5aa5f7f.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)
Linux方胜利
![在这里插入图片描述](https://img-blog.csdnimg.cn/1835ad4480414f56a9072562efe33909.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)
### 9人对局
9人乱斗一方在累计伤害大于等于其他玩家人数的情况下向所有其他玩家发送一格黑块
![玩家向其他所有人发送一个黑块进行攻击](https://img-blog.csdnimg.cn/38fd2223d4f34a599a7dd0c8c6854145.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)
游戏进行到中期玩家逐渐减少
![在这里插入图片描述](https://img-blog.csdnimg.cn/1bbb13af1edd4a3d8478261affd1a6a1.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)
9人模式中的败者
![在这里插入图片描述](https://img-blog.csdnimg.cn/275489ebb5914172a2104737e4203b30.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)
9人模式中的胜者
![在这里插入图片描述](https://img-blog.csdnimg.cn/73fd4b941b4f4041836c5ad77dc8ba46.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)

## 游戏特性细节介绍
### 攻击机制
附加了玩家之间可以相互攻击的新玩法特性。（模仿任天堂Tetris99）
具体规则如下


- 玩家一次消除1行至4行方块时将造成1，3，5，7点伤害。伤害（白色方块）加入受击槽（下图左边方框）中储存，槽中黑色方块为受到的伤害的缓存，白色方块造成伤害的缓存。存储的伤害（白色方块）大于等于存活的其他玩家数量时会消耗白色方块，往其他玩家的受击槽中添加一个黑色方块，这算作对其他玩家造成伤害，计入score得分。
- 受击槽中存在黑色方块时便开始计时，10秒黑色方块减半(向上取整)，减去的数量会使玩家的对局中的所有方块往上移动一格，并且在最下一行添加一行随机缺一个口的黑方块（左边玩家累计收到了九点伤害）
  ![在这里插入图片描述](https://img-blog.csdnimg.cn/a86e363148d84cb1b0d7d27eb649e2cf.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)
### Tetris标准特性
拥有[Tetris Guideline](https://harddrop.com/wiki/Tetris_Guideline)俄罗斯方块开发指南中现代俄罗斯方块游戏中的所有特性。
保持官方的这些特性有助于俄罗斯方块老玩家们有良好的操作手感的同时也给游戏带来了操作和策略性。

#### Super Rotation System（超级旋转系统）
当旋转后方块有部分卡在边界或者现有方块中时，游戏系统将尝试将方块“踢”入附近的其他位置，称为wall kick。
IOTSZJL表示方块类型，0,R,2,L 方块的四种方向
下面第一张图为官网给除的wallkick数据，第二张图的解释为当方块L从方向0逆时针转到方向L时，系统会依次对方块旋转后偏移（0,0),(+1,0),(+1,+1),(0,+2),(+1,-2)当有一个匹配成功则旋转成功，否则旋转失败。![在这里插入图片描述](https://img-blog.csdnimg.cn/979111a5e2864cd3bb3887c8f4f50e74.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)![在这里插入图片描述](https://img-blog.csdnimg.cn/e09d863b763c4d319547d72b2efa880c.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)

#### 7 bag system （7袋系统）
方块的生成并不是完全随机的，而是每拿到7个方块每种方块（一共7个）必定会拿到一次。

#### hold piece （hold槽系统）
游戏需要有一个能暂时存放方块的hold box（盒子）来将目前的方块和hold box中的方块进行替换。（按s键）
![在这里插入图片描述](https://img-blog.csdnimg.cn/8484332bd20749679024a5336214db10.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)

####  soft drop（软降）
使方块加速下降。
####  hard drop（硬降）
使方块直线下降到底部并锁定。

#### ghost piece（幽灵方块）
用虚影显示当前方块硬降后的位置。
![在这里插入图片描述](https://img-blog.csdnimg.cn/0026fd2b0b3745f0879bb4080a4bbb0f.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)

####  lock delay（延迟锁定）
当方块接触底部不会立即锁定，触底超过0.5秒锁定方块。

## 实现原理解析

### 多人游戏的一致性问题
**State synchronization（状态同步）**

游戏程序逻辑运行在一个终端内（服务器），这个终端接受所有玩家的指令并将其输入程序内再返回游戏的状态。这样的话客户端只充当一个显示器的作用。

***状态同步的优点：***
由于每一帧的状态都会进行保存，所以可以更方便的切换到指定的帧状态，进行回滚操作
由于逻辑计算都在服务端进行，所以安全性非常高。除非是设计不够完善或是进攻服务器，否则难以被外挂支配。
***状态同步的缺点：***
开发难度相对较高，客户端只负责游戏画面显示和状态维护，服务器负责游戏逻辑的计算，需要联合调试的多。
由于每个对象状态都有大量的自有参数，所以传输数据量也非常庞大，相应的也会影响网络流畅度。
游戏的核心逻辑计算都在服务器运行，所以服务端压力比较大
使用状态同步的知名游戏：Dota2和LOL（英雄联盟）
**Lockstep（帧同步）**

游戏程序逻辑运行在每一个终端中，由服务器来同步所有玩家的输入使得每个玩家的游戏一致。

***开发注意事项***
- 游戏的一致性在帧同步的前提是游戏程序不能出现随机过程。
- 使用确定性的随机数机制，随机数种子由服务器统一设置。
- 使用线程分派代替随机抢占。
- 不使用浮点数而使用定点数，或限定各客户端所运行的硬件及操作系统从而保证浮点数运算的一致性。


***帧同步的优点：***
开发难度相对较小，分工明确游戏主体全在客户端中，服务器交流简单只负责玩家指令传播。
数据传输量较小，只需要传输操作指令。
逻辑计算在客户端执行，服务器压力小。
***帧同步的缺点：***
由于逻辑在客户端处理，游戏逻辑不能有随机过程。
由于逻辑端在客户端处理，客户拥有游戏的全部数据，外挂容易知道对局中其他玩家的信息，反外挂困难 。
使用帧同步的知名游戏：王者荣耀、魔兽争霸3

**因此，综合开发难度，和服务器负载能力（花少量金钱购买的云服务器）的限制下我选择使用帧同步方法来开发本游戏。**

### 网络协议选择
UDP+KCP
KCP是基于UDP的一个应用层协议
滑动窗口：防止发送和接受流量不匹配。
数据分段：实现发送比单个MTU更大的数据包。
接受序号和未接受序号：确保数据的可靠和有序。
下图是KCP的包头
![在这里插入图片描述](https://img-blog.csdnimg.cn/87a4d87dfd0a43e18b64dcd63138d929.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)
### 网络数据序列化需求
采用protobuf解决
protocol buffers 是一个google开发的，一种跨语言、跨平台、可扩展的序列化结构数据的工具，用于对结构化数据进行序列化（例如XML和JSON），但比它们更小，更快，更简单。采用.proto文件编写。
下图是我的协议定义，使用proto3语法
![在这里插入图片描述](https://img-blog.csdnimg.cn/b3b3b15b812c41d58fc0c8875ae746c9.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)
Account消息用来登录注册
Message消息用来公屏聊天
Information消息用来建立客户端服务器连接
Instruction消息用来处理游戏操作逻辑和开始结束指令


### 玩家数据保存
![在这里插入图片描述](https://img-blog.csdnimg.cn/bd6037b81784455eb9810f38d09403a9.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)

```sql
CREATE TABLE IF NOT EXISTS account(id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT NOT NULL UNIQUE, password TEXT NOT NULL, alias TEXT NOT NULL UNIQUE,rankpoint INTEGER DEFAULT (0), avatar TEXT)
```

玩家头像（avatar）采用base64编码保存



## 服务端客户端交互流程图
客户端服务端会话流程图
![在这里插入图片描述](https://img-blog.csdnimg.cn/325d30a77a9449bc9f51edc6cd6d9ce5.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)
1v1对战流程图（9人模式同理）
![在这里插入图片描述](https://img-blog.csdnimg.cn/5671aa1b156041618c5597291e05b151.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)
首先有两个客户端，一个服务端，客户端发起game1v1请求到服务器，服务器依次将请求入队，当队列人数大于2人时，新建一个服务端游戏对象，并且携带游戏对象端口号返回，每个客户端收到服务器回复的game1v1携带的端口号后新建一个客户端游戏对象，之后发送ReadyToStart消息给服务器，说明自己已经建好了游戏对象，之后服务端发送游戏人数数量的随机数种子Random seed给客户端游戏对象，最后再发送ReadyToStart（playernumber）携带有一个playernumber用来区分当前客户端游戏编号。客户端收到ReadyToStart后游戏正式开始，可以发送游戏指令，最后游戏结束，单人游戏游戏存活玩家少于1人，对人模式游戏存活玩家少于两人时客户端向服务器发送gameover消息，当服务器游戏对象收到本局游戏人数数量的gameover消息时会释放游戏对象。之后客户端会释放游戏对象，同时发送points消息结算本局得分。



## 编译帮助
### msvc cmake build log

```bash
D:\Qt\Tools\CMake_64\bin\cmake.exe -DCMAKE_BUILD_TYPE=Release -G "CodeBlocks - NMake Makefiles" D:\ClionProject\Tetris
-- The C compiler identification is MSVC 19.26.28806.0
-- The CXX compiler identification is MSVC 19.26.28806.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.26.28801/bin/Hostx64/x64/cl.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.26.28801/bin/Hostx64/x64/cl.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- using Visual Studio C++
show_files: KCP_SRCS files: 1
-- D:/ClionProject/Tetris/3rd/kcp/ikcp.c
-- D:/Qt/5.15.2/msvc2019_64/bin/Qt5Core.dll
show_files: QSLOG_SRC files: 5
-- D:/ClionProject/Tetris/3rd/qslog/QsLog.cpp
-- D:/ClionProject/Tetris/3rd/qslog/QsLogDest.cpp
-- D:/ClionProject/Tetris/3rd/qslog/QsLogDestConsole.cpp
-- D:/ClionProject/Tetris/3rd/qslog/QsLogDestFile.cpp
-- D:/ClionProject/Tetris/3rd/qslog/QsLogDestFunctor.cpp
-- Found Protobuf: optimized;D:/protobuf-3.15.6/msvc_build/Release/libprotobuf.lib;debug;D:/protobuf-3.15.6/msvc_build/Debug/libprotobufd.lib (found version "3.15.6")
show_files: PROTO_SRCS files: 1
-- D:/ClionProject/Tetris/3rd/protobuf/tetris.pb.cc
show_files: SIMPLECRYPT_SRC files: 1
-- D:/ClionProject/Tetris/3rd/simplecrypt/simplecrypt.cpp
show_files: PROJECT_SRC files: 7
-- D:/ClionProject/Tetris/server/game.cpp
-- D:/ClionProject/Tetris/server/game.h
-- D:/ClionProject/Tetris/server/global.cpp
-- D:/ClionProject/Tetris/server/global.h
-- D:/ClionProject/Tetris/server/main.cpp
-- D:/ClionProject/Tetris/server/server.cpp
-- D:/ClionProject/Tetris/server/server.h
show_files: PROJECT_FILE files: 13
-- D:/ClionProject/Tetris/tetris/game.cpp
-- D:/ClionProject/Tetris/tetris/game.h
-- D:/ClionProject/Tetris/tetris/gamewindow.cpp
-- D:/ClionProject/Tetris/tetris/gamewindow.h
-- D:/ClionProject/Tetris/tetris/global.cpp
-- D:/ClionProject/Tetris/tetris/global.h
-- D:/ClionProject/Tetris/tetris/helpwindow.cpp
-- D:/ClionProject/Tetris/tetris/helpwindow.h
-- D:/ClionProject/Tetris/tetris/helpwindow.ui
-- D:/ClionProject/Tetris/tetris/main.cpp
-- D:/ClionProject/Tetris/tetris/mainwindow.cpp
-- D:/ClionProject/Tetris/tetris/mainwindow.h
-- D:/ClionProject/Tetris/tetris/mainwindow.ui
-- Configuring done
-- Generating done
-- Build files have been written to: D:/ClionProject/Tetris/cmake-build-release

[Finished]
```




### mingw cmake build log

```bash
"C:\Program Files\JetBrains\CLion 2021.2.3\bin\cmake\win\bin\cmake.exe" -DCMAKE_BUILD_TYPE=Release -DCMAKE_DEPENDS_USE_COMPILER=FALSE -G "CodeBlocks - MinGW Makefiles" D:\ClionProject\Tetris
-- The C compiler identification is GNU 8.1.0
-- The CXX compiler identification is GNU 8.1.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: D:/Qt/Tools/mingw810_64/bin/gcc.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: D:/Qt/Tools/mingw810_64/bin/g++.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- using GCC
show_files: KCP_SRCS files: 1
-- D:/ClionProject/Tetris/3rd/kcp/ikcp.c
-- D:/Qt/5.15.2/mingw81_64/bin/Qt5Core.dll
show_files: QSLOG_SRC files: 5
-- D:/ClionProject/Tetris/3rd/qslog/QsLog.cpp
-- D:/ClionProject/Tetris/3rd/qslog/QsLogDest.cpp
-- D:/ClionProject/Tetris/3rd/qslog/QsLogDestConsole.cpp
-- D:/ClionProject/Tetris/3rd/qslog/QsLogDestFile.cpp
-- D:/ClionProject/Tetris/3rd/qslog/QsLogDestFunctor.cpp
-- Found Protobuf: D:/protobuf-3.15.6/mingw_build/lib/libprotobuf.a (found version "3.15.6")
show_files: PROTO_SRCS files: 1
-- D:/ClionProject/Tetris/3rd/protobuf/tetris.pb.cc
show_files: SIMPLECRYPT_SRC files: 1
-- D:/ClionProject/Tetris/3rd/simplecrypt/simplecrypt.cpp
show_files: PROJECT_SRC files: 7
-- D:/ClionProject/Tetris/server/game.cpp
-- D:/ClionProject/Tetris/server/game.h
-- D:/ClionProject/Tetris/server/global.cpp
-- D:/ClionProject/Tetris/server/global.h
-- D:/ClionProject/Tetris/server/main.cpp
-- D:/ClionProject/Tetris/server/server.cpp
-- D:/ClionProject/Tetris/server/server.h
show_files: PROJECT_FILE files: 13
-- D:/ClionProject/Tetris/tetris/game.cpp
-- D:/ClionProject/Tetris/tetris/game.h
-- D:/ClionProject/Tetris/tetris/gamewindow.cpp
-- D:/ClionProject/Tetris/tetris/gamewindow.h
-- D:/ClionProject/Tetris/tetris/global.cpp
-- D:/ClionProject/Tetris/tetris/global.h
-- D:/ClionProject/Tetris/tetris/helpwindow.cpp
-- D:/ClionProject/Tetris/tetris/helpwindow.h
-- D:/ClionProject/Tetris/tetris/helpwindow.ui
-- D:/ClionProject/Tetris/tetris/main.cpp
-- D:/ClionProject/Tetris/tetris/mainwindow.cpp
-- D:/ClionProject/Tetris/tetris/mainwindow.h
-- D:/ClionProject/Tetris/tetris/mainwindow.ui
-- Configuring done
-- Generating done
-- Build files have been written to: D:/ClionProject/Tetris/cmake-build-release-mingw

[Finished]
```

## 游戏源码和发布程序
[github链接](https://github.com/Z-G-D/Tetris/releases)

## 声明
本项目只用于交流学习，其中的游戏音乐图片均来源于个人收藏，如有侵权请联系删除。