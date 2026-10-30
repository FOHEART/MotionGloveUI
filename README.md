# Windows Compile

编译Debug版本：

cmake -S . -B out/build -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Debug -DXSTYPES_FETCH_VIA_SSH=OFF -A x64

cmake --build out/build --config Debug --parallel

编译Release版本：

cmake -S . -B out/build -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release -DXSTYPES_FETCH_VIA_SSH=OFF -A x64

cmake --build out/build --config Release --parallel

# Ubuntu Compile

编译Debug版本：

cmake -S . -B out/build -DXSTYPES_FETCH_VIA_SSH=ON -DCMAKE_BUILD_TYPE=Debug

cmake --build out/build --config Debug --parallel

编译Release版本：

cmake -S . -B out/build -DXSTYPES_FETCH_VIA_SSH=ON -DCMAKE_BUILD_TYPE=Release

cmake --build out/build --config Release --parallel



# 编译为动/静态库

编译为动态库

添加 -DMOTIONGLOVE_UI_BUILD_SHARED ON

编译为静态库

添加 -DMOTIONGLOVE_UI_BUILD_SHARED OFF



使用CPM引入，编译为动态库

OPTIONS

"MOTIONGLOVE_UI_BUILD_SHARED ON"



使用CPM引入，编译为静态库

OPTIONS

"MOTIONGLOVE_UI_BUILD_SHARED OFF"

