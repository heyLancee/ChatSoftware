aux_source_directory(. SERVER_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./model MODEL_LIST)
aux_source_directory(. MAIN)

# 指定生成可执行文件
add_executable(chatServer ${MAIN} ${DB_LIST} ${SERVER_LIST} ${MODEL_LIST})
target_link_libraries(chatServer muduo_net muduo_base mysqlclient pthread)

