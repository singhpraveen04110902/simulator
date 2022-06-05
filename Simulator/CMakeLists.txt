set (simulator_NAME kaixjpSimulator)

add_executable(${simulator_NAME} kaixjp/Order.cpp kaixjp/KAIXJPExchangeInterface.cpp Main.cpp  ${applink_SOURCE})

target_include_directories(${simulator_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/quickfix ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/common ${CMAKE_CURRENT_SOURCE_DIR}/kaixjp)

target_link_libraries(${simulator_NAME} ${PROJECT_NAME})

if (NOT WIN32)
ADD_CUSTOM_TARGET(${simulator_NAME}_target ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE:kaixjpSimulator> ${CMAKE_SOURCE_DIR}/bin/${simulator_NAME})
else()
set_target_properties(${simulator_NAME} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug/simulator/
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release/simulator/
                      RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin/release/simulator/)
endif()

