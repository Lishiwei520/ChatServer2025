#g++ -o server -g muduo_server.cpp -l/usr/include -L/user/lib -lmuduo_net -lmuduo_base -lpthread

cmake_minimum_required(VERSION 3.0) #要求cmake最低的版本号
project(main) #工程名字
# 配置编译选项  -g
set(CMAKE_CXX_FLAGS ${CMALE_CXX_FLAGS} -g)

# 配置头文件搜索路径  -l/usr/include
# include_directories()
# 配置库文件搜索路径  -L/user/lib
# link_directories()
 
# 设置需要编译的源文件列表
set(SRC_LIST muduo_server.cpp)
# 把.指定路径下的所有源文件名字放入变量名SRC_LIST里面
#aux_source_directory(. SRC_LIST)

#设置可执行文件最终储存的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) #PROJECT_SOURCE_DIR为工程根目录
#设置静态库最终储存的路径

# 生成可执行文件，由SRC＿LIST变量所定义的源文件编译而来
add_executable(server muduo_server.cpp) 

#链接库 表示server这个目标程序需要链接muduo_net muduo_base pthread这三个库文件
target_link_libraries(server muduo_net muduo_base pthread)
