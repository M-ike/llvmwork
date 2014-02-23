////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE \n
/// Copyright (c) 2013, 中国科学技术大学(合肥) \n
/// All rights reserved. \n
///
/// @file LoopExtracAction.h
/// @brief 循环分割的主要入口，分为两个部分： \n
///        1.循环分析 \n
///        2.文件拆分
///
/// @version 0.1
/// @author xtxwy,zql,zk
///
/// 版本修订: \n
///     无
//////////////////////////////////////////////////////////////
#ifndef _LOOP_EXTRACT_ACTION_
#define _LOOP_EXTRACT_ACTION_

 #include "clang/Lex/MacroInfo.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/DenseMap.h"
#include "clang/AST/Decl.h"
#include <stdio.h>

using namespace clang;
class LoopExtractorStmtPrinter;

class LoopExtractorVistor : public RecursiveASTVisitor<LoopExtractorVistor> {
private:
        std::vector<ForStmt*> TopLoop;
        int ForStmtAction;
        int TraverseAction;
public:
    LoopExtractorVistor() {}
    ~LoopExtractorVistor() {} 
    bool shouldWalkTypesOfTypeLocs() const { return false; }
    bool VisitVarDecl(VarDecl *S);
    bool VisitDeclRefExpr(DeclRefExpr *DR);
    bool VisitReturnStmt(ReturnStmt *RS);
    bool VisitRecordDecl(RecordDecl *RD);
    bool VisitEnumDecl(EnumDecl *ED);
    bool VisitTypedefDecl(TypedefDecl *TD);
    bool hasFuncPionter(RecordDecl *RD);
    friend class LoopExtractorConsumer;
    void setForStmtActionTo1() {
        ForStmtAction = 1;
    }
    void setForStmtActionTo2() {
        ForStmtAction = 2;
    }
    void setTraverseActionTo1(){
        TraverseAction=1;
    }
    void setTraverseActionTo2(){
        TraverseAction=2;
    }
    bool TraverseForStmt(ForStmt *s)
    {
        if (!WalkUpFromForStmt(s))
            return false;
        if (ForStmtAction == 2){
              for(Stmt::child_range range = s->children(); range; ++range)
              TraverseStmt(*range);
        }
        if(ForStmtAction == 1){
                TopLoop.push_back(s);
                //s->dumpColor();
                return true;
        }
        return true;
    }
    bool TraverseFunctionDecl(FunctionDecl *Func){
       if(TraverseAction == 2) {
            return true;
       }
       if(TraverseAction == 1){
        for(FunctionDecl::param_iterator sF=Func->param_begin(),eF=Func->param_end();sF!=eF;sF++){
               TraverseDecl(*sF);
            }
            TraverseStmt(Func->getBody());
           return true;
        }
       return true;
    }

};

class LoopExtractorConsumer : public ASTConsumer {
public:
    LoopExtractorConsumer(StringRef InFile,CompilerInstance &CI) : PP(CI.getPreprocessor()),InputFile(InFile.str()) { 
        LoopCount = 1;
        TopLevelLoop.clear(); 
        InputArgs.clear();
        OutputArgs.clear();
        FunctionProto.clear();
        CallProto.clear();
        LoopToFunction.clear();
        LoopPosition.clear();
        ForPosition.clear();
        DeclToBeDealedPosition.clear();
        MacroInFile.clear();
        MacroContentInFile.clear();
        MacroPosition.clear();
        FunctionInSourcePosition.clear();
    }
    ~LoopExtractorConsumer() {}
    virtual void HandleTranslationUnit(ASTContext &Context);
private:
    Preprocessor &PP;
    //用于记录一个文件中转换了的循环个数
    int LoopCount;
    //保存TopLevel的循环
    std::vector<ForStmt*> TopLevelLoop;
    //保存MacroInfo
    std::vector<const MacroInfo*> MacroInFile;
    
    std::vector<FunctionDecl*> FunctionInSource;
    //std::map<int,int> FunctionInSourcePosition;
    std::vector< std::pair<int,std::pair<int,std::pair<int, int> > > > FunctionInSourcePosition; 
    //保存MacroContent
    std::map<int,std::string> MacroContentInFile;
    std::vector< std::pair<int,std::pair<int,std::pair<int, int> > > > MacroPosition;
    llvm::DenseMap<FunctionDecl*,std::string> FunctionInSourceContent;
    //保存输入参数
    llvm::DenseMap<ForStmt*, std::vector<ValueDecl*> > InputArgs;
    //保存输出参数
    llvm::DenseMap<ForStmt*, std::vector<ValueDecl*> > OutputArgs;
    //函数原型
    llvm::DenseMap<ForStmt*, std::string> FunctionProto;
    llvm::DenseMap<ForStmt*, std::string> CallProto;
    //保存循环转换后的函数
    llvm::DenseMap<ForStmt*, std::string> LoopToFunction;
    //llvm::DenseMap<int,std::string> LoopToFunction;
    //记录循环位置:开始和结束
    //std::map<int, int> LoopPosition;
    std::vector< std::pair<int,std::pair<int,std::pair<int, int> > > > LoopPosition;
    //记录结构体的位置:开始和结束
    //std::map<int, int> DeclToBeDealedPosition;
    std::vector< std::pair<int,std::pair<int,std::pair<int, int> > > > DeclToBeDealedPosition;
    //记录for循环位置和对应的循环
    std::map<int, ForStmt*> ForPosition;
    //记录宏的信息
    

    std::string getStrBetweenTwoLine(int lineStart,int lineEnd,int &lineCount,FILE *fp);
    std::string InputFile;
    
    //记录文件结构体
    //std::vector<Decl*> DeclToBeDealed;
    //保存结构体的内容
    llvm::DenseMap<Decl*,std::string> DeclContent;

    LoopExtractorVistor LEVisitor;
    //bool collectDeclToBeDealed(); //获取要处理的声明
    bool collectFuncContentAndPosition(FILE *fp,ASTContext &Context);
    bool collectFuncPionterRecordAndposition(FILE *fp,ASTContext &Context);
    bool collectMacroPosition(FILE *fp,ASTContext &Context);
    bool collectTopLevelLoop(Stmt *S);
    bool canLoopExtract(ForStmt *FS);
    bool doLoopExtract(FILE *fp, ASTContext &Context, FunctionDecl *FD, ForStmt *FS);
    bool collectInputArgs(FunctionDecl *FD, ForStmt *FS,ASTContext &Context);
    bool collectOutputArgs(FunctionDecl *FD, ForStmt *FS);
    bool constructFunctionAndCallProto(ASTContext &ASTC, FunctionDecl *FD, ForStmt* FStmt);
    bool constructFunction(FILE *fp, ASTContext &context, FunctionDecl *FD, ForStmt* FS);
    //写文件相关接口
    bool writeHeaderFile(FILE *fp, raw_ostream &O, std::string HeaderFileName);
    bool writeNewSourceFile(FILE *fp, raw_ostream &O, std::string HeaderFileName);
    bool writeSourceFile(FILE *fp, raw_ostream &O, std::string HeaderFileName, ASTContext &Context);
    std::string getHeader(FILE *fp);
    std::string getForStmt(FILE *fp, ForStmt *FS, ASTContext &ASTC);
    std::string getMacroInCodeBlock(FILE *fp);
    bool isInternalVarDecl(DeclRefExpr *DR);
    bool isInternalDecl(Decl *D);
    bool isInTwoLocation(int lineStart,int colStart,int lineEnd,int colEnd ,int lineCount,int colCount);
    bool isInLocationVector(std::vector< std::pair<int,std::pair<int,std::pair<int, int> > > > Position,
                                                       int lineCount,int colCount,int &IN);
   


    bool constructRecordDeclContent(FILE *fp, ASTContext &Context);
};

class LoopExtractAction : public ASTFrontendAction {
public:
    virtual ASTConsumer *CreateASTConsumer(CompilerInstance &CI, StringRef InFile);
};

extern int loop_min_trip_count;
#endif
