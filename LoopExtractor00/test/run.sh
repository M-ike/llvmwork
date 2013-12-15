#!/bin/sh
src_path=./src

exec 2>/dev/null
echo "开始编译运行变换之前的sift_code代码"
./rm.sh
make -f Makefile_before
cd $src_path/bin
./sift 1>../../before.txt 
cd ../../

echo "开始编译运行变换之后的sift_code代码"
./rm.sh
cd $src_path
./run.sh
cd ../
make -f Makefile_after
cd $src_path/bin
./sift 1>../../after.txt 
cd ../../

echo "比较转换前后结果差异"
diff before.txt after.txt 1>result.txt
if [ -s result.txt ]; then
    echo "差异结果如下:"
    cat result.txt
else 
    echo "前后结果一样"
fi

echo "详细信息可以参考本目录下的before.txt,after.txt和result.txt"
echo "如果需要手工验证,请参考ReadMe文件"

