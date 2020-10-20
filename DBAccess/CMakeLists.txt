include_directories(${includes_l})

add_library(DBAccess_lib SHARED ${PROJECT_SOURCE_DIR}/DBAccess/DBAccess.cpp)

find_path(HIREDIS_HEADER hiredis)
target_include_directories(DBAccess_lib PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis)
target_link_libraries(DBAccess_lib  ${HIREDIS_LIB})

find_path(REDIS_PLUS_PLUS_HEADER sw)
target_include_directories(DBAccess_lib PUBLIC ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
target_link_libraries(DBAccess_lib ${REDIS_PLUS_PLUS_LIB})
