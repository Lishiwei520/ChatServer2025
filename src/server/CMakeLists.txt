
#定义了一个SRC_LIST变量，包含了目录下的所有源文件
aux_source_directory(. SRC_LIST)

#指定生成可执行文件
add_executable(ChatServer ${SRC_LIST})

#链接库 表示server这个目标程序需要链接muduo_net muduo_base pthread这三个库文件
target_link_libraries(ChatServer muduo_net muduo_base pthread)