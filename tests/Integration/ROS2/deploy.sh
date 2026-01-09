#!/bin/bash

TEST_PKG_DIR="$1/hello_worlds_consumer"

mkdir -p ${TEST_PKG_DIR}
cp ./ros2_CMakeLists.txt ${TEST_PKG_DIR}/CMakeLists.txt
echo "✅ Deploy ${TEST_PKG_DIR}/CMakeLists.txt"
cp ./ros2_package.xml ${TEST_PKG_DIR}/package.xml
echo "✅ Deploy ${TEST_PKG_DIR}/package.xml"
cp ./consumer_test.cpp ${TEST_PKG_DIR}/consumer_test.cpp
echo "✅ Deploy ${TEST_PKG_DIR}/consumer_test.cpp"
