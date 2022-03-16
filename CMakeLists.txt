cmake_minimum_required(VERSION 2.8)

project(*****)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

#set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/home/***/aarch64-project/3rdlib/usr/lib/pkgconfig")
# bfl (Bayesian Filtering Library) is a third party package that uses pkg-config
#find_package(PkgConfig)
#pkg_check_modules(BFL REQUIRED orocos-bfl)

#include_directories(${BFL_INCLUDE_DIRS})
#link_directories(${BFL_LIBRARY_DIRS})

message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

# # find opencv 
if(CROSS_COMPILING)
  set(OpenCV_DIR "/home/***/aarch64-project/3rdlib/usr/share/OpenCV")
endif()

find_package(OpenCV 3 REQUIRED)
if(NOT OpenCV_FOUND)
  message(FATAL_ERROR "OpenCV 3.0 not found.")
endif()
message(STATUS "OpenCV Versino: ${OpenCV_VERSION}")

# # find eigen3
if(CROSS_COMPILING)
  set(Eigen3_DIR "/home/***/aarch64-project/3rdlib/usr/share/eigen3/cmake")
endif()

find_package(Eigen3 3.1.0 REQUIRED)
if(NOT Eigen3_FOUND)
  message(FATAL_ERROR "Eigen3 not found.")
endif()

# # find boost
if(CROSS_COMPILING)
  set(BOOST_ROOT "/home/***/aarch64-project/3rdlib/usr")
endif()

find_package(Boost REQUIRED)
if(NOT Boost_FOUND)
  message(FATAL_ERROR "Boost not found")  
endif()

# # find openssl
if(CROSS_COMPILING)
  set(OPENSSL_INCLUDE_DIR /home/***/aarch64-project/3rdlib/usr/include/openssl)
  FILE(GLOB OPENSSL_SSL_LIBRARY /home/***/aarch64-project/3rdlib/usr/lib64/libssl.a)
  FILE(GLOB OPENSSL_CRYPTO_LIBRARY /home/***/aarch64-project/3rdlib/usr/lib64/libcrypto.a)
else()
  find_package(OpenSSL REQUIRED)
endif()

message(STATUS "OPENCV_INCLUDE_DIR   ${OpenCV_INCLUDE_DIRS}")
message(STATUS "EIGEN3_INCLUDE_DIR   ${EIGEN3_INCLUDE_DIR}")
message(STATUS "BOOST_INCLUDE_DIR    ${Boost_INCLUDE_DIRS}")
message(STATUS "OPENSSL_INCLUDE_DIR  ${OPENSSL_INCLUDE_DIR}")
message(" ")


include_directories(${PROJECT_SOURCE_DIR}/include/common)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(${PROJECT_NAME} STATIC

 #include/common/geometry_msgs_ekf/Header.h
 #include/common/geometry_msgs_ekf/Quaternion.h
 #include/common/geometry_msgs_ekf/Transform.h
 #include/common/geometry_msgs_ekf/TransformStamped.h
 #include/common/geometry_msgs_ekf/Vector3.h

 #include/common/tf2_ekf/buffer_core.h
 #include/common/tf2_ekf/convert.h
 #include/common/tf2_ekf/time_cache.h
 #include/common/tf2_ekf/transform_storage.h

 #include/common/tf2_ekf/include/tf2/exceptions.h
 #include/common/tf2_ekf/include/tf2/time_cache.h
 #include/common/tf2_ekf/include/tf2/transform_datatypes.h
 #include/common/tf2_ekf/include/tf2/utils.h

 #include/common/tf2_ekf/include/tf2/LinearMath/Matrix3x3.h
 #include/common/tf2_ekf/include/tf2/LinearMath/MinMax.h
 #include/common/tf2_ekf/include/tf2/LinearMath/QuadWord.h
 #include/common/tf2_ekf/include/tf2/LinearMath/Quaternion.h
 #include/common/tf2_ekf/include/tf2/LinearMath/Scalar.h
 #include/common/tf2_ekf/include/tf2/LinearMath/Transform.h
 #include/common/tf2_ekf/include/tf2/LinearMath/Vector3.h

 #include/common/tf2_ekf/impl/convert.h
 #include/common/tf2_ekf/impl/utils.h

 #include/common/tf2_ros_ekf/buffer.h
 #include/common/tf2_ros_ekf/buffer_interface.h

 #include/common/tf_ekf/tf.h
 #include/common/tf_ekf/Transform.h
 #include/common/tf_ekf/transform_datatypes.h

 #include/common/tf_struct/macros.h
 #include/common/tf_struct/Matrix3x3.h
 #include/common/tf_struct/MinMax.h
 #include/common/tf_struct/QuadWord.h
 #include/common/tf_struct/Quaternion.h
 #include/common/tf_struct/Scalar.h
 #include/common/tf_struct/time_cache.h
 #include/common/tf_struct/Vector3.h

 #include/common/time/duration.h
 #include/common/time/rate.h
 #include/common/time/rostime_decl.h
 #include/common/time/time.h
 #include/common/time/impl/duration.h
 #include/common/time/impl/exception.h
 #include/common/time/impl/platform.h
 #include/common/time/impl/time.h

 #include/robot_pose_ekf/fusion_datatype.h
 #include/robot_pose_ekf/nonlinearanalyticconditionalgaussianodo.h
 #include/robot_pose_ekf/odom_estimation.h
 #include/robot_pose_ekf/odom_estimation_node.h

 #src/common/tf2_ekf/buffer_core.cpp
 #src/common/tf2_ekf/cache.cpp
 #src/common/tf2_ekf/static_cache.cpp

 #src/common/tf2_ros_ekf/buffer.cpp

 #src/common/tf_ekf/cache.cpp
 #src/common/tf_ekf/tf.cpp

 #src/common/time/duration.cpp
 #src/common/time/time.cpp

 #src/robot_pose_ekf/nonlinearanalyticconditionalgaussianodo.cpp
 #src/robot_pose_ekf/odom_estimation.cpp
 #src/robot_pose_ekf/odom_estimation_node.cpp
)
