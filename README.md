# Tetris
多人联机的俄罗斯方块游戏，游戏规则模仿任天堂Tetris99，基于qt，kcp，protobuf，qslog

## 游戏截图
![登录界面](https://img-blog.csdnimg.cn/e4e2b9da6b1f486ca634742d59d5972c.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)
![玩家菜单界面](https://img-blog.csdnimg.cn/44cf049fa6dd4ee6abb07473778c66cd.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)
![单人模式](https://img-blog.csdnimg.cn/497e996cc9114891b3aafc758d33fd30.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)

![九人模式](https://img-blog.csdnimg.cn/3841587f37d3430db574d645e4b0ada9.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)
![九人模式](https://img-blog.csdnimg.cn/822e28dfda3c47fc8be99424b8d3269a.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBALVotRy1ELQ==,size_20,color_FFFFFF,t_70,g_se,x_16)

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



