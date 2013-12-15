#目录设置
ROOT := .
OBJ_DIR := $(ROOT)
#CPP文件目录
LIB_DIR := $(ROOT)
TOOL_DIR := $(ROOT)
#头文件目录
INC_DIR := $(ROOT)
#要编译的文件
LIB_FILES := LoopExtractAction.cpp main.cpp 
#LIB_FILES := main.cpp 
TOOL_FILES := 

#给LIB_FILES加上LIB_DIR,给TOOL_FILES加上TOOL_DIR
FILES = $(patsubst %,$(LIB_DIR)/%,$(LIB_FILES))  $(patsubst %,$(TOOL_DIR)/%,$(TOOL_FILES))

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(FILES))

#编译器
CXX = clang++ -I$(INC_DIR)
CXXFLAGS = -O2 -fno-rtti
LLVM_LIBS = all
LLVM_CONFIG_CXX = llvm-config --cxxflags 
LLVM_CONFIG_LD = llvm-config --ldflags --libs $(LLVM_LIBS)
#要加载的clang的库
CLANGLIBS = \
	-lclangTooling \
	-lclangFrontendTool -lclangFrontend -lclangDriver \
	-lclangSerialization -lclangCodeGen -lclangParse -lclangSema \
	-lclangStaticAnalyzerFrontend -lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangARCMigrate \
	-lclangRewriteFrontend -lclangRewriteCore \
	-lclangAnalysis -lclangEdit -lclangAST -lclangBasic -lclangLex \

all: $(OBJ_DIR)/driver
#链接
$(OBJ_DIR)/driver: $(OBJS)
	$(CXX) $(OBJS) -o $@ $(CLANGLIBS) `$(LLVM_CONFIG_LD)`
#编译
$(OBJ_DIR)/%.o:	%.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CXXFLAGS) `$(LLVM_CONFIG_CXX)` -o $@ $<

#清除
clean:
	rm *.o
	rm driver
