////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE \n
/// Copyright (c) 2013, 中国科学技术大学(合肥) \n
/// All rights reserved. \n
///
/// @file LoopExtracAction.cpp
/// @brief 实现了LoopExtracAction.h中规定的接口
///
/// @version 0.1
/// @author xtxwy,zql,zk
///
/// 版本修订: \n
///     无
//////////////////////////////////////////////////////////////
#include "LoopExtractAction.h"
#include "llvm/Support/raw_ostream.h"

#define LINE_SIZE 500

int DebugOutput_LoopExtractAction = 0;
int ErrorOutput_LoopExtractAction = 0;

llvm::DenseSet<ValueDecl*> TempSet;
std::vector<VarDecl*> VarDefInLoop;
std::vector<DeclRefExpr*> VarUseInLoop;
std::vector<ReturnStmt*> ReturnInLoop;
llvm::DenseMap<ForStmt*, FunctionDecl*> ReturnInFile;

//保存结构体的相关信息
std::map<std::string,Decl*> allDecl;
std::map<std::string, VarDecl*> VarInFunction;
std::vector<Decl*> DeclToBeDealed;
std::vector<Decl*> DeclToBeDealedAfterDel;

std::vector<VarDecl*> globalDecl;
std::set<std::string> globalDeclName;
ASTConsumer* LoopExtractAction::CreateASTConsumer(CompilerInstance &CI, StringRef InFile) {
    return new LoopExtractorConsumer(InFile,CI);
}

char *itoa(int num);
void LoopExtractorConsumer::HandleTranslationUnit(ASTContext &Context) {
    TranslationUnitDecl* D = Context.getTranslationUnitDecl();
    D->dump(llvm::outs());
    std::string InputFileOld = "";
    InputFileOld += InputFile;
    InputFileOld.erase(InputFileOld.find_last_of('.'), InputFileOld.size());
    InputFileOld += "_old.c";

    FILE *fp = fopen(InputFile.c_str(), "r"); if(NULL == fp) return;
    FILE *fpt = fopen(InputFileOld.c_str(), "w");
    if(fpt == NULL) return;

    char aLine[LINE_SIZE] = { 0 };
    std::string temp_file = "";
    while(NULL != fgets(aLine, sizeof(aLine), fp)) {
        temp_file += aLine;
        memset(aLine, 0, sizeof(aLine));
    }
    fwrite(temp_file.c_str(), temp_file.size(), 1, fpt);
    fflush(fpt);
    fclose(fp);
    fclose(fpt);
    fpt = fopen(InputFileOld.c_str(), "r");
    if(NULL == fpt) return;
    
    //收集宏
    if(collectMacroPosition(fpt,Context) == false) llvm::errs() << "收集宏失败!";
    ReturnInFile.clear();
    globalDecl.clear();
    globalDeclName.clear();
    DeclToBeDealed.clear();
    DeclContext* DC = Context.getTranslationUnitDecl();
    LEVisitor.setTraverseActionTo2();
    LEVisitor.setForStmtActionTo2();
    LEVisitor.TraverseDecl(Context.getTranslationUnitDecl());
    /*std::vector<Decl*> tempDecl;
    for(std::vector<Decl*>::iterator sD = DeclToBeDealed.begin(),eD = DeclToBeDealed.end();sD != eD;++sD){
        Decl *D = *sD;
        if(true == isInternalDecl(D))
            tempDecl.push_back(D);
    }
    DeclToBeDealed.clear();
    for(std::vector<Decl*>::iterator sD = tempDecl.begin(),eD = tempDecl.end();sD != eD;++sD){
        Decl *D = *sD;
        //D->dumpColor();
        DeclToBeDealed.push_back(D);
    }*/
    SourceManager &SM = Context.getSourceManager();
    bool hasLoopOrNot = false; //记录这个文件是否包含循环
    bool loopChanged = false; //记录是否有满足要求的循环
    for(DeclContext::decl_iterator sD = DC->decls_begin(), eD = DC->decls_end(); sD != eD; ++sD) {
        DeclContext *DDC = dyn_cast<DeclContext>(*sD);
        //Decl::Kind DK =DDC->getDeclKind();
        if(NULL == DDC) {
            if(ErrorOutput_LoopExtractAction) llvm::errs() << "获取声明失败.\n";
            continue;
        }
        //llvm::outs() << DDC->getDeclKind() << "\n";
        if(DDC->getDeclKind() == Decl::Function) {
            FunctionDecl *FD = dyn_cast<FunctionDecl>(DDC);
            if(DebugOutput_LoopExtractAction) llvm::outs() << *FD << "\n";
            if(false == FD->isThisDeclarationADefinition()) continue;
            
            if(!(SM.isFromMainFile(FD->getLocation()))) continue;
            FunctionInSource.push_back(FD);
            //FD->dumpColor();
            VarInFunction.clear();
            Stmt *S = FD->getBody();
            if(NULL == S) {
                if(ErrorOutput_LoopExtractAction) llvm::errs() << "获取函数体失败.\n";
                continue;
            }

            if(false == collectTopLevelLoop(S)) return;
            if(TopLevelLoop.empty()) {
                if(DebugOutput_LoopExtractAction) {
                    std::string FuncName = FD->getNameInfo().getName().getAsString();
                    llvm::outs() << "函数" << FuncName << "没有循环.\n";
                    continue;
                }
            }

            hasLoopOrNot = true;
            //打印所有的Top Level循环
            if(DebugOutput_LoopExtractAction) {
                for(std::vector<ForStmt*>::iterator sFS = TopLevelLoop.begin(), eFS = TopLevelLoop.end(); sFS != eFS; ++sFS) {
                    ForStmt* FS = *sFS;
                    FS->dumpColor();
                }
            }
            //将函数内收集到的顶层循环，做分割。
            for(std::vector<ForStmt*>::iterator sF = LEVisitor.TopLoop.begin(), eF = LEVisitor.TopLoop.end(); sF != eF; ++sF) {
                Stmt *SS = *sF;
                if(ForStmt *FS = dyn_cast<ForStmt>(SS)) {
                    if(false == canLoopExtract(FS)) continue;
                    if(false == doLoopExtract(fpt, Context, FD, FS)) {
                        if(ErrorOutput_LoopExtractAction) llvm::errs() << "循环分割失败.\n";
                        continue;
                    }

                    loopChanged = true;
                }
            }
        }//end of if(DDC...  
     LEVisitor.TopLoop.clear();
    }//end of for
    //处理record类型
    //if(false == collectDeclToBeDealed()) llvm::errs() << "获取要处理的声明失败!";
    //llvm::outs() << DeclToBeDealed.size() <<"\n";
    //if(false == collectFuncContentAndPosition(fpt,Context)) llvm::errs() << "获取函数内容和位置失败!\n";
    //if(false == collectFuncPionterRecordAndposition(fpt,Context)) llvm::errs() << "获取带有函数指针的结构体失败\n";
    
    if(false == constructRecordDeclContent(fpt,Context)) llvm::errs() << "获取结构体声明失败!\n";

    if(false == hasLoopOrNot || false == loopChanged) return;
  
    //文件命名
    //规则:以前的文件名+"_loop_number" + ".h" => 其中number表示展开了多少个循环
    std::string HeaderFileName = "";
    std::string NewSourceFileName = "";
    std::string SourceFileName = "";

    HeaderFileName.append(InputFile);
    HeaderFileName.erase(HeaderFileName.find_last_of('.'), HeaderFileName.size());
    HeaderFileName += "_loop_";
    char *number = itoa(LoopCount - 1);
    HeaderFileName += std::string(number);
    free(number);
    NewSourceFileName += HeaderFileName;
    HeaderFileName += ".h";
    NewSourceFileName += ".c";
    SourceFileName += InputFile;

    std::string ErrorInfo;
    llvm::raw_fd_ostream HeaderFile(HeaderFileName.c_str(), ErrorInfo);
    llvm::raw_fd_ostream NewSourceFile(NewSourceFileName.c_str(), ErrorInfo);
    llvm::raw_fd_ostream SourceFile(SourceFileName.c_str(), ErrorInfo);

    if(DebugOutput_LoopExtractAction) llvm::outs() << "原文件名:" << SourceFileName << "\n";
    if(DebugOutput_LoopExtractAction) llvm::outs() << "新的头文件名:" << HeaderFileName << "\n";
    if(DebugOutput_LoopExtractAction) llvm::outs() << "新的源文件名:" << NewSourceFileName << "\n";

    //开始打印输出
    //1.打印头文件
    if(false == writeHeaderFile(fpt, HeaderFile, HeaderFileName)) {
        if(ErrorOutput_LoopExtractAction) llvm::errs() << "写头文件失败.\n";
        return;
    }

    HeaderFile.flush();
    //2.打印新的源文件
    if(false == writeNewSourceFile(fpt, NewSourceFile, HeaderFileName)) {
            if(ErrorOutput_LoopExtractAction) llvm::errs() << "写新的源文件失败.\n";
        return;
    }

    NewSourceFile.flush();
    //3.打印原始文件
    if(false ==  writeSourceFile(fpt, SourceFile, HeaderFileName, Context)) {
        if(ErrorOutput_LoopExtractAction) llvm::errs() << "写源文件失败.\n";
        return;
    }

    SourceFile.flush();
    fclose(fpt);
    return;
}

bool LoopExtractorConsumer::collectMacroPosition(FILE *fp,ASTContext &Context){
    //struct MacroInfoChain *MIC = PP.MIChainHead;
    SourceManager &SM = Context.getSourceManager();
    
    /*typedef std::pair<IdentifierInfo*, MacroInfo*> id_macro_pair;
    SmallVector<id_macro_pair, 128> MacrosByID;
    for (Preprocessor::macro_iterator I = PP.macro_begin(), E = PP.macro_end();I != E; ++I) {
            if (I->first->hasMacroDefinition()){
                 id_macro_pair IM;
                 IM.first = I->first;
                 IM.second = I->second;
                MacrosByID.push_back(IM);
            }
    }

    for (unsigned i = 0, e = MacrosByID.size(); i != e; ++i) {
        MacroInfo &MI = *MacrosByID[i].second;
        // Ignore computed macros like __LINE__ and friends.
        if (MI.isBuiltinMacro()) continue;
    }*/


    //PP.PrintStats();
    for (Preprocessor::macro_iterator I = PP.macro_begin(), E = PP.macro_end();I != E; ++I){

     /*if(const IdentifierInfo *II = dyn_cast<const IdentifierInfo>(I->first))
        if(const  MacroDirective *MD = dyn_cast<const MacroDirective>(I->second)){
            int lineCount = 0;
            const MacroInfo *MI = MD->getMacroInfo();
            SourceLocation sSL=MI->getDefinitionLoc();
            SourceLocation eSL = MI->getDefinitionEndLoc();
            int lineStart = SM.getPresumedLoc(sSL).getLine();
            int lineEnd = SM.getPresumedLoc(eSL).getLine();
            if(SM.isFromMainFile(sSL)){
                MacroInFile.push_back(MI);
                std::string MacroName = II->getName();
                std::string MContent = "#ifndef ";
                MContent += MacroName;
                MContent += "\n";
                MacroContentInFile[MI] = MContent; 
                    
                std::string str = getStrBetweenTwoLine(lineStart,lineEnd,lineCount,fp);
                llvm::outs() << II->getName()<< "  "<< lineStart << "  " << lineEnd << "\n";
                MContent += str;
                MContent += "#endif\n";
            }
            
            const MacroDirective *preMD = MD->getPrevious();
            while(preMD != NULL){
                SourceLocation sSL1 = preMD->getLocation();
                if(SM.isFromMainFile(sSL1)){
                        
                    llvm::outs() << SM.getPresumedLoc(sSL1).getLine() << "qianyige  ";
                    llvm::outs() << SM.getPresumedLoc(sSL1).getColumn() << "\n";
                }
                preMD = preMD->getPrevious();
            }
       }*/

       if(const IdentifierInfo *II = dyn_cast<const IdentifierInfo>(I->first)){

            rewind(fp);
            int lineCount = 0;
            if(II->hasMacroDefinition()){
                MacroInfo *MI = PP.getMacroInfo(const_cast<IdentifierInfo*>(II));
                SourceLocation sSL=MI->getDefinitionLoc();
                SourceLocation eSL = MI->getDefinitionEndLoc();
                int lineStart = SM.getPresumedLoc(sSL).getLine();
                //int colStart = SM.getPresumedLoc(sSL).getColumn();
                int lineEnd = SM.getPresumedLoc(eSL).getLine();
                //int colEnd = SM.getPresumedLoc(eSL).getColumn();
                if(SM.isFromMainFile(sSL)){
                    MacroInFile.push_back(MI);
                    std::string MacroName = II->getName();
                    std::string MContent = "#ifndef ";
                    MContent += MacroName;
                    MContent += "\n";
                    
                    std::string str = getStrBetweenTwoLine(lineStart,lineEnd,lineCount,fp);
                    MContent += str;
                    MContent += "\n#endif\n";
                    MacroContentInFile[MI] = MContent; 
                    //llvm::outs() << MContent << "\n";
                    //llvm::outs() << II->getName()<< "  "<< lineStart << "  " << lineEnd << "\n";
                    //llvm::outs() << lineEnd << "  " << colEnd << "\n";
                    //llvm::outs() << II->getName() << "\n";
                    //llvm::outs() << MI->getDefinitionLength(SM) << "\n";
                }
            }

        }
    }
    return true;
}

bool LoopExtractorConsumer::collectFuncContentAndPosition(FILE *fp,ASTContext &Context){
    if(fp == NULL) {
        if(ErrorOutput_LoopExtractAction) llvm::errs() << "传入的文件指针无效.\n";
        return false;
    }    
    SourceManager &SM = Context.getSourceManager();
   
    for(std::vector<FunctionDecl*>::iterator sF = FunctionInSource.begin(),eF = FunctionInSource.end();sF != eF; ++sF){
        FunctionDecl *FD = *sF;
        //FD->dumpColor();
        if(FD == NULL ) {
            if(ErrorOutput_LoopExtractAction) llvm::errs() << "函数指针无效.\n";
            return false;
        }
        SourceRange SR = FD->getSourceRange();
        int lineStart = SM.getPresumedLoc(SR.getBegin()).getLine();
        int lineEnd = SM.getPresumedLoc(SR.getEnd()).getLine();
        FunctionInSourcePosition[lineStart] = lineEnd;
        //llvm::outs() << lineStart << "  "<<lineEnd << "\n";
        rewind(fp);
        std::string FunctionContent = "";
        int lineCount = 0;

        FunctionContent = getStrBetweenTwoLine(lineStart,lineEnd,lineCount,fp);
        /*char aLine[LINE_SIZE] = { 0 };
        while(NULL != fgets(aLine, sizeof(aLine), fp)) {
            if(++lineCount < lineStart) {
                memset(aLine, 0, sizeof(aLine));
                continue;
            }
            if(lineCount > lineEnd) break;
            FunctionContent += aLine;
            memset(aLine, 0, sizeof(aLine));
        }*/
        FunctionInSourceContent[FD] = FunctionContent;
        //llvm::outs() << FunctionContent << "\n";
    }
    return true;
}
bool LoopExtractorConsumer::collectFuncPionterRecordAndposition(FILE *fp,ASTContext &Context){
    if(fp == NULL) {
        if(ErrorOutput_LoopExtractAction) llvm::errs() << "传入的文件指针无效.\n";
            return false;
    }
    for(std::vector<VarDecl*>::iterator sVD = globalDecl.begin(),eVD = globalDecl.end();sVD != eVD;++sVD){
        VarDecl *VD = *sVD;
        std::string TypeR = "";
        TypeR += VD->getType().getAsString();
        //llvm::outs() << TypeR << "\n";
        
    }
    return true;
}
bool LoopExtractorConsumer::collectTopLevelLoop(Stmt *S) {
    if(NULL == S) return false;
    allDecl.clear(); //遍历每一个函数前清空allDecl
    for(Stmt::child_iterator sF = S->child_begin(), eF = S->child_end(); sF != eF; ++sF){
        Stmt *SS = *sF;
        if(DeclStmt *DS = dyn_cast<DeclStmt>(SS)){
            //DS->dumpColor();
            for(DeclStmt::decl_iterator sDS = DS->decl_begin(),eDS = DS->decl_end();sDS != eDS;++sDS){
                Decl *D = *sDS;
                if(VarDecl *VD = dyn_cast<VarDecl>(D)){
                    //VD->dumpColor();
                    std::string VarName = VD->getName();
                    if(VarInFunction[VarName] == NULL)
                        VarInFunction[VarName] = VD;
                }
                if(RecordDecl *RD = dyn_cast<RecordDecl>(D)){
                    std::string TypeR = "";
                    TypeR += RD->getNameAsString();
                    allDecl[TypeR] = RD;
                }
                if(EnumDecl *ED = dyn_cast<EnumDecl>(D)){
                    std::string TypeR = "";
                    TypeR += ED->getNameAsString();
                    allDecl[TypeR] = ED;
                }
            }
        }
    }
    LEVisitor.setForStmtActionTo1();
    LEVisitor.setTraverseActionTo1();
    for(Stmt::child_iterator sF = S->child_begin(), eF = S->child_end(); sF != eF; ++sF){
            Stmt *SS = *sF;
            LEVisitor.TraverseStmt(SS);
    }
    for(std::vector<ForStmt*>::iterator sFS=LEVisitor.TopLoop.begin(),eFS = LEVisitor.TopLoop.end();sFS != eFS; ++sFS ){
            ForStmt* FS = *sFS;
            TopLevelLoop.push_back(FS);
    }
    return true;    
}

bool LoopExtractorConsumer::constructRecordDeclContent(FILE *fp, ASTContext &Context){
    if(fp == NULL) {
        if(ErrorOutput_LoopExtractAction) llvm::errs() << "传入的文件指针无效.\n";
            return false;
        }
    for(std::vector<Decl*>::iterator sD = DeclToBeDealed.begin(),eD = DeclToBeDealed.end();sD != eD; sD++){

        Decl *D = *sD;
        rewind(fp);
        std::string DContent = "";
        SourceManager &SM = Context.getSourceManager();
        SourceRange SR = D->getSourceRange();
        int lineStart = SM.getPresumedLoc(SR.getBegin()).getLine();
        int lineEnd = SM.getPresumedLoc(SR.getEnd()).getLine();
        std::map<int, int>::iterator sM1 = DeclToBeDealedPosition.find(lineStart);
        if(sM1 != DeclToBeDealedPosition.end()){
            continue;
        }
        
        DeclToBeDealedAfterDel.push_back(D);
        DeclToBeDealedPosition[lineStart] = lineEnd;
        
        int lineCount = 0;
        DContent = getStrBetweenTwoLine(lineStart,lineEnd,lineCount,fp);
        DeclContent[D] = DContent;
    }
    return true;
}
//====================================循环分割的条件==============================
 struct ForData{
    int start;
    int end;
    int inc;
};
bool dealwithInit(ForStmt *FS, std::vector<ValueDecl*> &LoopVal, llvm::DenseMap<ValueDecl*, struct ForData> &ValForPair){
    Stmt *Init = FS->getInit();
    if(Init == NULL) return true;

    if(BinaryOperator *InitExpr = dyn_cast<BinaryOperator> (Init)){
        if(InitExpr->getOpcode() == BO_Assign){
            if(DeclRefExpr *DR = dyn_cast<DeclRefExpr>(InitExpr->getLHS())){
                if(IntegerLiteral *IL = dyn_cast<IntegerLiteral>(InitExpr->getRHS())){
                    ValForPair[DR->getDecl()].start = IL->getValue().getSExtValue();
                    ValForPair[DR->getDecl()].inc =0;
                    LoopVal.push_back(DR->getDecl());
                    return false;
                }
            }
        }else if(InitExpr->getOpcode() == BO_Comma){
                if(BinaryOperator *LInitExpr = dyn_cast<BinaryOperator> (InitExpr->getLHS())){
                    if(BinaryOperator *RInitExpr = dyn_cast<BinaryOperator> (InitExpr->getRHS())){
                        if(RInitExpr->getOpcode() == BO_Assign && LInitExpr->getOpcode() == BO_Assign){
                            if(DeclRefExpr *DR1 = dyn_cast<DeclRefExpr>(LInitExpr->getLHS())){
                                if(IntegerLiteral *LIL = dyn_cast<IntegerLiteral>(LInitExpr->getRHS())){
                                    ValForPair[DR1->getDecl()].start = LIL->getValue().getSExtValue();
                                    ValForPair[DR1->getDecl()].inc =0;
                                    LoopVal.push_back(DR1->getDecl());

                                }
                            }
                            if(DeclRefExpr *DR2 = dyn_cast<DeclRefExpr>(RInitExpr->getLHS())){
                                if(IntegerLiteral *RIL = dyn_cast<IntegerLiteral>(RInitExpr->getRHS())){
                                    ValForPair[DR2->getDecl()].start = RIL->getValue().getSExtValue();
                                    ValForPair[DR2->getDecl()].inc =0;
                                    LoopVal.push_back(DR2->getDecl());
                                    return false;
                                }
                            }
                        }
                    }
                }
        }
    }
    return true;
}
bool dealwithCond(ForStmt *FS,llvm::DenseMap<ValueDecl*, struct ForData> &ValForPair){
    Expr *Cond = FS->getCond();
    if(Cond == NULL) return true;

    if(BinaryOperator *CondExpr = dyn_cast<BinaryOperator> (Cond)){
        if(CondExpr->getOpcode() == BO_LT || CondExpr->getOpcode() == BO_LE){
            if(ImplicitCastExpr *LIC = dyn_cast<ImplicitCastExpr>(CondExpr->getLHS()))
                if(DeclRefExpr *DR = dyn_cast<DeclRefExpr>(LIC->getSubExpr())){
                    if(IntegerLiteral *IL = dyn_cast<IntegerLiteral>(CondExpr->getRHS())){
                        ValForPair[DR->getDecl()].end = IL->getValue().getSExtValue();
                        return false;
                    }
                }
        }else if (CondExpr->getOpcode() == BO_LAnd || CondExpr->getOpcode() == BO_LOr){
             if(BinaryOperator *LCondExpr = dyn_cast<BinaryOperator> (CondExpr->getLHS())){
                if(BinaryOperator *RCondExpr = dyn_cast<BinaryOperator> (CondExpr->getRHS())){
                    if(RCondExpr->isComparisonOp() && LCondExpr->isComparisonOp()){
                        if(ImplicitCastExpr *LIC = dyn_cast<ImplicitCastExpr>(LCondExpr->getLHS()))
                            if(DeclRefExpr *DR1 = dyn_cast<DeclRefExpr>(LIC->getSubExpr())){
                                if(IntegerLiteral *LIL = dyn_cast<IntegerLiteral>(LCondExpr->getRHS())){
                                    ValForPair[DR1->getDecl()].end = LIL->getValue().getSExtValue();
                                }
                            }    
                        if(ImplicitCastExpr *RIC = dyn_cast<ImplicitCastExpr>(RCondExpr->getLHS())){
                            if(DeclRefExpr *DR2 = dyn_cast<DeclRefExpr>(RIC->getSubExpr())){
                                if(IntegerLiteral *RIL = dyn_cast<IntegerLiteral>(RCondExpr->getRHS())){
                                    ValForPair[DR2->getDecl()].end = RIL->getValue().getSExtValue();
                                    return false;
                                }
                            }
                        }
                    }
                }
             }
        }
    }
    return true;
}

bool dealwithInc(ForStmt *FS,llvm::DenseMap<ValueDecl*, struct ForData> &ValForPair){
    Expr *Inc = FS->getInc();
    if(Inc== NULL) return true;

    if(UnaryOperator *IncExpr = dyn_cast<UnaryOperator>(Inc)){
        if(DeclRefExpr *DR = dyn_cast<DeclRefExpr>(IncExpr->getSubExpr())){
            ValForPair[DR->getDecl()].inc = 1;
            return false;
        }
    }
    if(BinaryOperator *IncExpr = dyn_cast<BinaryOperator>(Inc)){
        if(IncExpr->getOpcode() == BO_AddAssign || IncExpr->getOpcode() == BO_SubAssign){
            if(DeclRefExpr *DR = dyn_cast<DeclRefExpr>(IncExpr->getLHS())){
                if(IntegerLiteral *IL = dyn_cast<IntegerLiteral>(IncExpr->getRHS())){
                    ValForPair[DR->getDecl()].inc = IL->getValue().getSExtValue();
                    return false;
                }
            }
        }else if(IncExpr->getOpcode() == BO_Comma){
            if(UnaryOperator *LIncExpr = dyn_cast<UnaryOperator>(IncExpr->getLHS())){
                if(UnaryOperator *RIncExpr = dyn_cast<UnaryOperator>(IncExpr->getRHS())){
                    if(DeclRefExpr *DR1 = dyn_cast<DeclRefExpr>(LIncExpr->getSubExpr())){
                        ValForPair[DR1->getDecl()].inc = 1;
                    }
                    if(DeclRefExpr *DR2 = dyn_cast<DeclRefExpr>(RIncExpr->getSubExpr())){
                        ValForPair[DR2->getDecl()].inc = 1;
                        return false;
                    }
                }
            }
        }
    }
        return true;
}

int getLoopNumber(ForStmt *FS,llvm::DenseMap<ValueDecl*,struct ForData> &ValForPair,std::vector<ValueDecl*> &LoopVal){
    int minLoopnum = loop_min_trip_count;
    int maxLoopnum = 0;
    int temp;
    bool isLAnd;
    Expr *Cond = FS->getCond();
    if(BinaryOperator *CondExpr = dyn_cast<BinaryOperator>(Cond)){
        if(CondExpr->getOpcode() == BO_LAnd)
          isLAnd = true;
        else
          isLAnd = false;
    }
    for(std::vector<ValueDecl*>::iterator sVD = LoopVal.begin(),eVD = LoopVal.end();sVD != eVD; sVD++){
        ValueDecl *VD = *sVD;
        if(ValForPair[VD].inc == 0) return -1;
            if(ValForPair[VD].start > ValForPair[VD].end){
                temp = (ValForPair[VD].start - ValForPair[VD].end)/ValForPair[VD].inc;
                if(minLoopnum > temp)
                    minLoopnum = temp;
                if(maxLoopnum < temp)
                    maxLoopnum = temp;
            }else{
                 temp = (ValForPair[VD].end - ValForPair[VD].start)/ValForPair[VD].inc;
                if(minLoopnum > temp)
                    minLoopnum = temp;
                if(maxLoopnum < temp)
                   maxLoopnum = temp;
            }
        }
    if(isLAnd)
        return minLoopnum;
    else
        return maxLoopnum;
}
bool LoopExtractorConsumer::canLoopExtract(ForStmt *FS){
    int LoopNumber;
    std::vector<ValueDecl*> LoopVal;
    llvm::DenseMap<ValueDecl*, struct ForData> ValForPair;
    if(dealwithInit(FS,LoopVal,ValForPair)||dealwithCond(FS,ValForPair)||dealwithInc(FS,ValForPair))
        return true;

    LoopNumber= getLoopNumber(FS,ValForPair,LoopVal);
    if(LoopNumber == -1) return true;
    if (LoopNumber < loop_min_trip_count) 
        return false;
    else
        return true;


}

//TODO:只处理了简单情况:for(i = 0; i < 100; i++)
/*bool LoopExtractorConsumer::canLoopExtract(ForStmt *FS) {
    Stmt *Init = FS->getInit();
    Expr *Cond = FS->getCond();
    Expr *Inc = FS->getInc();
    if(Init == NULL || Cond == NULL) return true;

    int start = 0;
    int end = 0;
    int inc = 0;
    //for(i = 0; ...)情况
    if(BinaryOperator *InitExpr = dyn_cast<BinaryOperator>(Init)) {
        if(InitExpr->getOpcode() == BO_Assign) {
            Expr *RV = InitExpr->getRHS();
            if(IntegerLiteral *IL = dyn_cast<IntegerLiteral>(RV)) {
               start =  IL->getValue().getSExtValue();
            } else return true;
        } else return true;
    } else return true;

    if(BinaryOperator *ConExpr = dyn_cast<BinaryOperator>(Cond)) {
        Expr *RV = ConExpr->getRHS();
        if(IntegerLiteral *IL = dyn_cast<IntegerLiteral>(RV)) {
             end =  IL->getValue().getSExtValue();
        } else return true;
    } else return true;
    if (UnaryOperator *IncExpr = dyn_cast<UnaryOperator>(Inc)){
       if(IncExpr->getOpcode() == UO_PostInc){
          inc = 1;
       }else return true;
    } else if(BinaryOperator *IncExpr = dyn_cast<BinaryOperator>(Inc)){
             if(IncExpr->getOpcode() == BO_AddAssign)
             {
                   Expr *RV = IncExpr->getRHS();
                   if(IntegerLiteral *IL = dyn_cast<IntegerLiteral>(RV))
                   {
                        inc = IL->getValue().getSExtValue();
                   }else return true;
              }else return true;
          }else return true;
    if(start == end) return true;
    else if(start < end) {
        if((end - start + 1)/inc >= loop_min_trip_count) return true;
        else return false;
    } else {
        if((start - end + 1)/inc >= loop_min_trip_count) return true;
        else return false;
    }

    return true;
}*/
//===============================================================================


bool LoopExtractorConsumer::doLoopExtract(FILE *fp, ASTContext &Context, FunctionDecl *FD, ForStmt *FS) {
    VarDefInLoop.clear();
    VarUseInLoop.clear();
    TempSet.clear();
    ReturnInLoop.clear();
    //1.确定输入参数
    if(false == collectInputArgs(FD, FS,Context)) {
        if(ErrorOutput_LoopExtractAction) llvm::errs() << "输入参数收集失败.\n";
        return false;
    }
    //2.确定输出参数
    if(false == collectOutputArgs(FD, FS)) {
        if(ErrorOutput_LoopExtractAction) llvm::errs() << "输出参数收集失败.\n";
        return false;
    }

    //3.构造函数和调用原型
    if(false == constructFunctionAndCallProto(Context, FD, FS)) {
        if(ErrorOutput_LoopExtractAction) llvm::errs() << "函数原型构造失败.\n";
        return false;
    }
    
    //4.构造函数体
    if(false == constructFunction(fp, Context, FD, FS)) {
        if(ErrorOutput_LoopExtractAction) llvm::errs() << "函数构造失败.\n";
        return false;
    }

    return true;
}


bool LoopExtractorConsumer::collectInputArgs(FunctionDecl *FD, ForStmt *FS,ASTContext &ASTC) {
    LEVisitor.setForStmtActionTo2();
    LEVisitor.setTraverseActionTo1();
    LEVisitor.TraverseStmt(FS);
    if(DebugOutput_LoopExtractAction) llvm::outs() << "Def:" << VarDefInLoop.size() << "\n";
    if(DebugOutput_LoopExtractAction) llvm::outs() << "Use:" << VarUseInLoop.size() << "\n";
    if(ReturnInLoop.size() != 0) {
        for(std::vector<ReturnStmt*>::iterator sRS = ReturnInLoop.begin(), eRS = ReturnInLoop.end(); sRS != eRS; ++sRS) {
            ReturnInFile[FS] = FD;
        }
    }

    if(VarUseInLoop.size() == VarDefInLoop.size()) {
        InputArgs[FS].push_back(NULL);
        return true;
    }



    //TODO:暂且做保守性处理
    for(std::vector<DeclRefExpr*>::iterator sDR = VarUseInLoop.begin(), eDR = VarUseInLoop.end(); sDR != eDR; ++sDR) {
        DeclRefExpr *DR = *sDR;
        ValueDecl *VD = DR->getDecl();
        if(DebugOutput_LoopExtractAction) llvm::outs() << VD->getName() << "类型为:" << VD->getType().getAsString() << "\n";
        Decl::Kind DK = VD->getKind();
        if(DK == Decl::Function) {
            if(true == isInternalVarDecl(DR)) return false;
            else {
                VarUseInLoop.erase(sDR);
                sDR = VarUseInLoop.begin();
                eDR = VarUseInLoop.end();
            }

            continue;
        }

        //TODO：为什么Decl::Record判断不了结构体
        //if(DK == Decl::Record || DK == Decl::CXXRecord)  
        if(VD->getType()->isStructureType() || VD->getType()->isUnionType()) {
            //if(true == isInternalDecl(DR)) return false;
            std::string TypeName = "";
            TypeName += VD->getType().getAsString();
            size_t SpacePos = TypeName.find_last_of(' ');
            TypeName.erase(0,SpacePos+1);
            //llvm::outs() << TypeName << "\n";
            if(allDecl[TypeName] != NULL) {
                //allDecl[TypeName]->dumpColor();
                return false;
            }
            //DeclNameUsedInfor.insert(TypeName);
            continue;
        }
        if(DK == Decl::EnumConstant || DK == Decl::Enum) {
            if(true == isInternalVarDecl(DR)) return false;
            else if(DK == Decl::EnumConstant) { //处理isdigit(), isspace等
                VarUseInLoop.erase(sDR);
                sDR = VarUseInLoop.begin();
                eDR = VarUseInLoop.end();
            }

            continue;
        }
        if(DK == Decl::Var) {
            //VD->dumpColor();
            if(VarDecl *VVD = dyn_cast<VarDecl>(VD))
            if(VVD->hasGlobalStorage() && !(VVD->isStaticLocal())) {
                if(isInternalDecl(VVD)){
                    //VVD->dumpColor();
                    //llvm::outs() << VVD->getName() << "\n";
                    std::set<std::string>::iterator SI = globalDeclName.find(VVD->getName());
                    if(SI == globalDeclName.end()){
                        globalDeclName.insert(VVD->getName());
                    //VarDecl *VDDef = VVD->getDefinition();
                    //VDDef->dumpColor();
                    /*if(true == isInternalDecl(VDDef))
                        VDDef->dumpColor();*/
                        DeclToBeDealed.push_back(VVD);
                    }
                }
                VarUseInLoop.erase(sDR);
                sDR = VarUseInLoop.begin();
                eDR = VarUseInLoop.end();
            }

            continue;
        }
            
        if(DK == Decl::Typedef || DK == Decl::TypeAlias) {
            TypedefNameDecl &TD = cast<TypedefNameDecl>(*VD);
            QualType Ty = TD.getUnderlyingType();
            if(Ty->isStructureType() || Ty->isUnionType()) {
                if(true == isInternalVarDecl(DR)) return false;
            }

            continue;
        }//end of if
      
    }
    //end of for

    for(std::vector<DeclRefExpr*>::iterator sDR = VarUseInLoop.begin(), eDR = VarUseInLoop.end(); sDR != eDR; ++sDR) {
        DeclRefExpr *DR = *sDR;
        ValueDecl *VD = DR->getDecl();
	    QualType T = VD->getType();
       
        //为数组变量时,例如int a[n],在for中用到sizeof(a),将ｎ作为参数传进去,sizeof(a)改为sizeof(int)*n.
	if(const ArrayType *AT = ASTC.getAsArrayType(T)) {
            std::string Type = "";
            Type += AT->getElementType().getAsString();
            std::string TypeName = VD->getType().getAsString();
            int startpos = TypeName.find('[');
            int endpos = TypeName.find(']');
            std::string VarName;
            VarName.assign(TypeName.begin()+startpos+1,TypeName.begin()+endpos);
            bool hasExsit = false;
            if(VarInFunction[VarName] != NULL){
                for(std::vector<ValueDecl*>::iterator sVD = InputArgs[FS].begin(),eVD = InputArgs[FS].end();sVD != eVD; ++sVD){
                    ValueDecl *VVD = *sVD;
                    if(VarInFunction[VarName] == VVD){
                        hasExsit = true;
                        break;
                    }
                }
                if(hasExsit == false){
                    InputArgs[FS].push_back(VarInFunction[VarName]);
                }
            }
        }

        size_t count = 0;
        for(std::vector<VarDecl*>::iterator sVD = VarDefInLoop.begin(), eVD = VarDefInLoop.end(); sVD != eVD; ++sVD) {
            VarDecl *VD1 = *sVD;
            if(VD != VD1) count++; 
        }

        if(count == VarDefInLoop.size()) InputArgs[FS].push_back(VD);
    }

    return true;
}

bool LoopExtractorConsumer::collectOutputArgs(FunctionDecl *FD, ForStmt *FS) {
    //TODO：现在输入参数就是输出参数
    return true;
}

bool LoopExtractorVistor::VisitVarDecl(VarDecl *S) {
    if(TraverseAction == 1 && ForStmtAction == 2){
        if(DebugOutput_LoopExtractAction) S->dumpColor();
        VarDefInLoop.push_back(S);
    }
    /*if(TraverseAction == 1 && ForStmtAction == 1){
        std::string VarName = S->getName();
        if(VarInFunction[VarName] == NULL)
            VarInFunction[VarName] = S;
    }*/
    if(TraverseAction == 2 ){
        SourceManager &SM = S->getASTContext().getSourceManager();
        if(SM.isFromMainFile(S->getLocation())){
            //DeclToBeDealed.push_back(S);
            globalDecl.push_back(S);

            //S->dumpColor();
       }
    }

    return true;
}
bool LoopExtractorVistor::hasFuncPionter(RecordDecl *RD){
     if(RD == NULL) llvm::errs() << "指针为空\n";
     for(RecordDecl::field_iterator sRF = RD->field_begin(),eRF = RD->field_end();sRF != eRF;++sRF){
        FieldDecl *FD = *sRF;
        //llvm::outs() << FD->getType().getAsString() << "\n";
        //llvm::outs() << FD->getKind() << "\n";
        if(FD->getType()->isPointerType()){
            if(FD->getType()->getPointeeType()->isFunctionType()){
                //FD->dumpColor();
                return true;
            }
        }
    }
    return false;
}
bool LoopExtractorVistor::VisitRecordDecl(RecordDecl *RD){
    /*if(TraverseAction == 1 && ForStmtAction == 1){
        std::string TypeR = "";
        TypeR += RD->getNameAsString();
        allDecl[TypeR] = RD;
        //RD->dumpColor();
    }*/
    //llvm::outs() << RD->getKind() << "\n";
    if(TraverseAction == 2){
        SourceManager &SM = RD->getASTContext().getSourceManager();
        if(SM.isFromMainFile(RD->getLocation())){
            //RD->dumpColor();
            DeclToBeDealed.push_back(RD);
        }
    }
    if(true == hasFuncPionter(RD)){
        //FunctionRecord.push_back(RD);
        std::string RDName = "";
        RDName += RD->getNameAsString();
        //llvm::outs() << RDName << "\n";
    }
    /*for(RecordDecl::field_iterator sRF = RD->field_begin(),eRF = RD->field_end();sRF != eRF;++sRF){
        FieldDecl *FD = *sRF;
        //llvm::outs() << FD->getType().getAsString() << "\n";
        //llvm::outs() << FD->getKind() << "\n";
        if(FD->getType()->isPointerType()){                  
            if(FD->getType()->getPointeeType()->isFunctionType()){
                FD->dumpColor();
            }
        }
    }*/
    return true;
}
bool LoopExtractorVistor::VisitTypedefDecl(TypedefDecl *TD){
    if(TraverseAction==2){
        SourceManager &SM = TD->getASTContext().getSourceManager();
        if(SM.isFromMainFile(TD->getLocation())){
            //TD->dumpColor();
            DeclToBeDealed.push_back(TD);
        }
        //TD->dumpColor();
        //llvm::outs() << TD->getUnderlyingType().getAsString() << "\n";
    }
    return true;
}
bool LoopExtractorVistor::VisitEnumDecl(EnumDecl *ED){
    /*if(TraverseAction == 1 && ForStmtAction == 1){
        std::string TypeR = "";
        TypeR += ED->getNameAsString();
        allDecl[TypeR] = ED;
    }*/
    //ED->dumpColor();
    if(TraverseAction == 2){
        SourceManager &SM = ED->getASTContext().getSourceManager();
        if(SM.isFromMainFile(ED->getLocation())){
            DeclToBeDealed.push_back(ED);
        }
    }
    return true;
}
bool LoopExtractorVistor::VisitDeclRefExpr(DeclRefExpr *DR) {
    if(TraverseAction == 1 && ForStmtAction == 2){
        if(DebugOutput_LoopExtractAction) DR->dumpColor();
        ValueDecl *VD = DR->getDecl();
        if(!TempSet.count(VD)) {
            if(DebugOutput_LoopExtractAction) VD->dumpColor();
            TempSet.insert(VD);
            VarUseInLoop.push_back(DR);
        }
    }

    return true;
}

bool LoopExtractorVistor::VisitReturnStmt(ReturnStmt *RS) {
    if(TraverseAction == 1 && ForStmtAction == 2){
        ReturnInLoop.push_back(RS);
    }
    return true;
}

char *itoa(int num) {
    int sign = num, i = 0, j = 0;
    char temp[100] = {0};
    char *str = (char*)malloc(sizeof(char) * 100);
    memset(str, 0, sizeof(char) * 100);

    if(sign < 0) num = -num;
    do {
        temp[i] = num %10 + '0';
        num /= 10;
        i++;
    } while (num > 0);

    if(sign < 0) temp[i++] = '-';
    temp[i] = '\0';

    i--;
    while(i >= 0) {
        str[j] = temp[i];
        j++; i--;
    }

    str[j] = '\0';
    return str;
}

bool LoopExtractorConsumer::constructFunctionAndCallProto(ASTContext &ASTC, FunctionDecl *FD, ForStmt* FS) {
    if(FD == NULL || FS == NULL) return false;

    //1.构造函数原型
    std::string FuncPrototype = "";
    std::string CallPrototype = "";
    FuncPrototype += "void ";
    FuncPrototype += FD->getNameInfo().getName().getAsString();
    FuncPrototype += "_loop_";
    char *number = itoa(LoopCount);
    FuncPrototype += std::string(number);
    FuncPrototype += "(";
    llvm::DenseMap<ForStmt*, std::vector<ValueDecl*> >::iterator sFV = InputArgs.find(FS);
    if(sFV == InputArgs.end()) return false;
    for(std::vector<ValueDecl*>::iterator sVD = sFV->second.begin(), eVD = sFV->second.end(); sVD != eVD; ++sVD) {
        ValueDecl *VD = *sVD;
        if(VD == NULL) break;
        QualType T = VD->getType();
        std::string Type = "";
        if (ParmVarDecl *Parm = dyn_cast<ParmVarDecl>(VD)) T = Parm->getOriginalType();
        bool isArray = false;
        if(const ArrayType *AT = ASTC.getAsArrayType(T)) {
            Type += AT->getElementType().getAsString();
            isArray = true;
        } else Type += T.getAsString();
        std::string Name = VD->getName();
        FuncPrototype += Type;
        FuncPrototype += " ";
        if(false == isArray) FuncPrototype += "*";
        FuncPrototype += Name;
        if(true == isArray) FuncPrototype += "[]";
        FuncPrototype += ", ";
    }

    if(ReturnInLoop.size() != 0) {
        FuncPrototype += "int *re_arg_pa1_";
        FuncPrototype += std::string(number);
        if(!FD->getResultType()->isVoidType()) {
            QualType QT = FD->getResultType();
            std::string arg_return = "";
            arg_return += QT.getAsString();
            arg_return += " *re_arg_pa2_";
            arg_return += std::string(number);
            FuncPrototype += ", ";
            FuncPrototype += arg_return;
        }
    }

    if(FuncPrototype[FuncPrototype.size() - 1] == ' ') {
        FuncPrototype.erase(FuncPrototype.find_last_of(' '), 1);
        FuncPrototype.erase(FuncPrototype.find_last_of(','), 1);
    }

    FuncPrototype += ");";
    FunctionProto[FS] = FuncPrototype;

    //2.构造调用原型
    CallPrototype += FD->getNameInfo().getName().getAsString();
    CallPrototype += "_loop_";
    CallPrototype += std::string(number);
    CallPrototype += "(";
    for(std::vector<ValueDecl*>::iterator sVD = sFV->second.begin(), eVD = sFV->second.end(); sVD != eVD; ++sVD) {
        ValueDecl *VD = *sVD;
        if(NULL == VD) break;
        std::string Name = VD->getName();
        QualType T = VD->getType();
        if(!ASTC.getAsArrayType(T)) CallPrototype += "&";
        CallPrototype += Name;
        CallPrototype += ", ";
    }

    if(ReturnInLoop.size() != 0)  {
        CallPrototype += "&re_arg_pa1_";
        CallPrototype += std::string(number);
        if(DebugOutput_LoopExtractAction) llvm::outs() << "类型检查:" << FD->getResultType().getAsString() << "\n";
        if(!FD->getResultType()->isVoidType()) {
            std::string arg_return = "";
            arg_return += ", &re_arg_pa2_";
            arg_return += std::string(number);
            CallPrototype += arg_return;
        }
    }

    if(CallPrototype[CallPrototype.size() - 1] == ' ') {
        CallPrototype.erase(CallPrototype.find_last_of(' '), 1);
        CallPrototype.erase(CallPrototype.find_last_of(','), 1);
    }

    CallPrototype += ");";
    CallProto[FS] = CallPrototype;

    LoopCount++;
    free(number);
    if(DebugOutput_LoopExtractAction) llvm::outs() << "函数原型:" << FuncPrototype << "\n";
    if(DebugOutput_LoopExtractAction) llvm::outs() << "调用原型:" << CallPrototype << "\n";
    return true;
}
bool isInQoute(std::map<size_t, size_t> &doublequote,size_t pos){
    for(std::map<size_t,size_t>::iterator sM = doublequote.begin(),eM = doublequote.end();sM!=eM;++sM){
        if(pos > sM->first && pos < sM->second)
            return true;
    }
    return false;
}
bool LoopExtractorConsumer::constructFunction(FILE *fp, ASTContext &context, FunctionDecl *FD, ForStmt *FS) {
    std::string NewFunc = "";
    llvm::DenseMap<ForStmt*, std::string>::iterator sFS = FunctionProto.find(FS);
    if(sFS == FunctionProto.end()) return false;

    std::string FuncProto = sFS->second;
    FuncProto.erase(FuncProto.find_last_of(";"), 1);
    NewFunc += FuncProto;
    NewFunc += "\n";
    NewFunc += "{\n";

    std::string ForBody = getForStmt(fp, FS, context);
    if(ForBody.empty()) {
        if(ErrorOutput_LoopExtractAction) llvm::errs() << "循环读取失败.\n";
        return false;
    }
    std::map<size_t, size_t> doublequote;
    
    llvm::DenseMap<ForStmt*, std::vector<ValueDecl*> >::iterator sFV = InputArgs.find(FS);
    if(sFV == InputArgs.end()) return false;
    for(std::vector<ValueDecl*>::iterator sVD = sFV->second.begin(), eVD = sFV->second.end(); sVD != eVD; ++sVD) {
        ValueDecl *VD = *sVD;
        if(NULL == VD) break;
        QualType T = VD->getType();
        if(context.getAsArrayType(T)) continue;
        doublequote.clear();
        size_t pos1,pos2 = 0; 
        pos2 = ForBody.find("\"", 0);
        while(std::string::npos != pos2){
            pos1 = pos2; 
            pos2 = ForBody.find("\"", pos2+1);
            while(ForBody[pos2-1] == '\\')
                pos2 = ForBody.find("\"", pos2+1);
            doublequote[pos1] = pos2; 
            pos2 = ForBody.find("\"", pos2+1);
        }

        std::string name = VD->getName();
        size_t pos = ForBody.find(name, 0);
        while(std::string::npos != pos) {
            size_t size = name.size();
            while(isalpha(ForBody[pos - 1]) || isdigit(ForBody[pos - 1]) || ForBody[pos - 1] == '_' || \
                  isalpha(ForBody[pos + size]) || isdigit(ForBody[pos + size]) || ForBody[pos + size] == '_') {
                pos += size;
                pos = ForBody.find(name.c_str(), pos);
                if(std::string::npos == pos) break;
            }
            
            if(std::string::npos == pos) break;
            if((ForBody[pos - 1] == '>' && ForBody[pos - 2] == '-') || ForBody[pos - 1] == '.' ) {
                pos += size;
                pos = ForBody.find(name.c_str(), pos);
                if(std::string::npos == pos) break;
                continue;
            }
            if(std::string::npos == pos) break;
            if(true == isInQoute(doublequote,pos)){
               pos += name.size();
               pos = ForBody.find(name.c_str(), pos);
               continue;                  
            }
            std::string re = "(*";
            re += name;
            re += ")";
            ForBody.replace(pos, size, re.c_str());
            //更新 doublequote 
            for(std::map<size_t,size_t>::iterator sM = doublequote.begin(),eM = doublequote.end();sM!=eM;++sM){
                if(sM->first > pos) 
                    pos1 = sM->first +3;
                else
                    pos1 = sM->first ;
                if(sM->second > pos) 
                    pos2 = sM->second + 3;
                else
                    pos2 = sM->second;
                doublequote.erase(sM);
                doublequote[pos1] = pos2;
            }
            pos += re.size();
            //pos += re.size();
            pos = ForBody.find(name.c_str(), pos);
        }
    }

    if(ReturnInLoop.size() != 0) {
        size_t pos = ForBody.find("return", 0);
        while(isalpha(ForBody[pos - 1]) || isdigit(ForBody[pos - 1]) || ForBody[pos - 1] == '_' || \
            isalpha(ForBody[pos + 6]) || isdigit(ForBody[pos + 6]) || ForBody[pos + 6] == '_') {
            pos += 6;
            pos = ForBody.find("return", pos);
            if(std::string::npos == pos) break;
        }

        if(std::string::npos == pos) return false;

        llvm::DenseMap<ForStmt*, std::string>::iterator sDM = CallProto.find(FS);
        if(sDM == CallProto.end()) return false;
        std::string callName = sDM->second;
        int callNamePos = callName.find("_loop_");
        std::string cN = "";
        callNamePos += 6;
        int i = 0;
        char temp = callName.at(callNamePos);
        while(temp != '(') {
            cN += temp; 
            i++; callNamePos++;
            temp = callName.at(callNamePos); 
        }

        for(std::vector<ReturnStmt*>::iterator sRS = ReturnInLoop.begin(), eRS = ReturnInLoop.end(); sRS != eRS; ++sRS) {
            ReturnStmt *RS = *sRS;
            if(NULL == RS) return false;

            size_t pos_end = ForBody.find(";", pos);

            std::string rep = "";
            rep += "{ (*re_arg_pa1_";
            rep += cN;
            rep += ") = ";
            rep += "0";
            rep += "; ";
            if(!FD->getResultType()->isVoidType()) {
                rep += "(*re_arg_pa2_";
                rep += cN;
                rep += ") =";
                char expr[100] = { 0 };
                ForBody.copy(expr, pos_end - pos + 1, pos + 6); 
                int len = strlen(expr);
                while(expr[len] != ';') len--;
                expr[len] = '\0';

                rep += expr;
                rep += "; ";
            }
            rep += "return; }";

            ForBody.replace(pos, pos_end - pos + 1, rep.c_str());
            pos += rep.size();
            pos = ForBody.find("return", pos);
            while(isalpha(ForBody[pos - 1]) || isdigit(ForBody[pos - 1]) || ForBody[pos - 1] == '_' || \
                  isalpha(ForBody[pos + 6]) || isdigit(ForBody[pos + 6]) || ForBody[pos + 6] == '_') {
                pos += 6;
                pos = ForBody.find("return", pos);
                if(std::string::npos == pos) break;
            }

            if(std::string::npos == pos) break;
        }
    }

    NewFunc += ForBody;
    NewFunc += "}";
    
    size_t pos = 0;
    pos = NewFunc.find("sizeof", 0);
    size_t start=0,end=0,insteadEnd=0,insteadStart=0;
    while(std::string::npos != pos){
        if(isalpha(NewFunc[pos - 1]) || isdigit(NewFunc[pos - 1]) || NewFunc[pos - 1] == '_' || \
          isalpha(NewFunc[pos + 6]) || isdigit(NewFunc[pos + 6]) || NewFunc[pos + 6] == '_'){
            pos += 6;
            pos = NewFunc.find("sizeof", pos);
            if(std::string::npos == pos) break;
        }
        insteadStart = pos;
        pos += 6;
        while(!(isalpha(NewFunc[pos])) && !(isdigit(NewFunc[pos])) && NewFunc[pos] != '_')
        {
            pos +=1;
        }
        start = pos;
        while(isalpha(NewFunc[pos]) || isdigit(NewFunc[pos]) || NewFunc[pos] == '_'){
            pos += 1;
        }
        end = pos;
        std::string VarName;
        VarName.assign(NewFunc.begin()+start,NewFunc.begin()+end);
        while(NewFunc[pos] == ' ') pos += 1;
        if(NewFunc[pos] == ')')
            insteadEnd = pos+1;
        else
            insteadEnd = pos;
	    pos = NewFunc.find("sizeof",pos);
        VarDecl *VD= VarInFunction[VarName];
        if(VD == NULL) continue;
        QualType T = VD->getType();
        if(const ArrayType *AT = context.getAsArrayType(T)) {
            std::string Type = "";
            Type += AT->getElementType().getAsString();
            std::string TypeName = VD->getType().getAsString();
            start=TypeName.find("[",0);
            while(!(isalpha(TypeName.at(start)))&&!(isdigit(TypeName.at(start)))&&TypeName.at(start) != '_')
                start += 1;
            end = start;
            while(isalpha(TypeName.at(end)) || isdigit(TypeName.at(end)) || TypeName.at(end) == '_')
                end += 1;
            std::string arraySize = TypeName.assign(TypeName.begin()+start,TypeName.begin()+end);
            if(VarInFunction[arraySize] != NULL){
                std::string tempstring = "(*";
                tempstring += arraySize;
                tempstring += ")";
                arraySize = tempstring;
            }
            std::string insteadString = "";
            insteadString += "sizeof(";
            insteadString += Type;
            insteadString += ")";
            insteadString += "*";
            insteadString += arraySize;
            NewFunc.replace(NewFunc.begin()+insteadStart,NewFunc.begin()+insteadEnd,insteadString);
        }
    }

    LoopToFunction[FS] = NewFunc;
    if(DebugOutput_LoopExtractAction) llvm::outs() << "构造的函数体如下:\n" << NewFunc << "\n";
    return true;
}

bool LoopExtractorConsumer::writeHeaderFile(FILE *fp, raw_ostream &O, std::string FN) {
    std::string FileName = "";
    FileName += FN;
    FileName.replace(FileName.find_last_of('.'), 1, "_");
    if(std::string::npos != FileName.find_last_of('/')) FileName.erase(0, FileName.find_last_of('/') + 1);
    O << "#ifndef " << FileName << "_\n";
    O << "#define " << FileName << "_\n";
    O << "\n";
    
    //插入头文件和宏声明
    rewind(fp);
    std::string header = getHeader(fp);
    //llvm::outs() << header << "\n";
    if(header.empty() && DebugOutput_LoopExtractAction) llvm::outs() << "没有头文件?\n";
    else O << header << "\n";
    //写结构体
    rewind(fp);
    int lineCount = 0;
    char aLine[LINE_SIZE] = {0};
    while(NULL != fgets(aLine, sizeof(aLine), fp)){
        lineCount++;
        std::map<int,int>::iterator sM = DeclToBeDealedPosition.find(lineCount);
        if(sM != DeclToBeDealedPosition.end()){
            O << aLine ;
            while(lineCount < sM->second){
                fgets(aLine,sizeof(aLine),fp);
                lineCount++;
                 if(lineCount == sM->second){
                    std::string str = "";
                    str += aLine;
                    size_t pos = str.find("/*");
                    if(pos != std::string::npos)
                        str.erase(str.begin() + pos,str.end());
                    O << str;
                 }else{
                    O << aLine;
                 }
            }
        }
    }

    /*for(llvm::DenseMap<Decl*, std::string>::iterator sDR = DeclContent.begin(),eDR = DeclContent.end();sDR != eDR;++sDR ){
        std::string DContent = sDR->second;
        O << DContent <<"\n";
        //llvm::outs() << DContent << "\n";
    }*/
    /*for(std::vector<Decl*>::iterator sD = DeclToBeDealedAfterDel.begin(),eD=DeclToBeDealedAfterDel.end();sD!=eD;sD++){
        Decl *D = *sD;
        std::string DContent = DeclContent[D];
        O << DContent <<"\n";
    }*/

    for(llvm::DenseMap<ForStmt*, std::string>::iterator sDF = FunctionProto.begin(), eDF = FunctionProto.end(); sDF != eDF; ++sDF) {
        std::string FuncName = sDF->second;
        O << FuncName << "\n";
    }
    O << "\n";
    O << "#endif\n";
    return true;
}

bool LoopExtractorConsumer::writeNewSourceFile(FILE *fp, raw_ostream &O, std::string FileName) {
    std::string headerFile = "";
    size_t pos = FileName.find_last_of('/');
    if(std::string::npos != pos) {
        headerFile.assign(FileName,pos+1, FileName.size() - pos );
    } else headerFile += FileName;
    
    O << "#include \"" << headerFile << "\"\n";
    //写代码内部宏
    /*std::string macro = getMacroInCodeBlock(fp);
    if(!macro.empty()) O << macro;
    llvm::outs() << macro << "\n";*/
    for(std::vector<const MacroInfo*>::iterator sMI = MacroInFile.begin(),eMI = MacroInFile.end();sMI != eMI;++sMI){
        const MacroInfo *MI = *sMI;
        O << MacroContentInFile[MI];
    }

    O << "\n";
    for(llvm::DenseMap<ForStmt*, std::string>::iterator sDF = LoopToFunction.begin(), eDF = LoopToFunction.end(); sDF != eDF; ++sDF) {
        std::string NewFunc = sDF->second;
        O << NewFunc << "\n";
    }
    return true;
}

static void delString(char *str, int *lineCount, FILE* fp) {
    int i;
    char aLine[LINE_SIZE] = { 0 };
    while(1) {
        i=0;
        while(str[i] !='\0') i++;
        if(str[i - 2] == '\\') {   
            memset(aLine, 0, sizeof(aLine));
            if(NULL != fgets(aLine, sizeof(aLine), fp)) {
                (*lineCount)++;
                str = aLine;
            }
        } else break;
    }

    return;
}

bool LoopExtractorConsumer::writeSourceFile(FILE *fp, raw_ostream &O, std::string FileName, ASTContext &Context) {
    std::string headerFile = "";
    size_t pospos = FileName.find_last_of('/');
    if(std::string::npos != pospos) {
        headerFile.assign(FileName,pospos+1, FileName.size() - pospos );
    } else headerFile += FileName;
    O << "#include \"" << headerFile << "\"\n";
    if(fp == NULL) {
        if(ErrorOutput_LoopExtractAction) llvm::errs() << "传入的文件指针无效.\n";
        return false;
    }
    
    /*rewind(fp);
    char aLine[LINE_SIZE] = { 0 };
    int lineCount = 0;
    std::string temp = "";
    while(NULL != fgets(aLine, sizeof(aLine), fp)){
        lineCount++;
        std::map<int, int>::iterator sM1 = FunctionInSourcePosition.find(lineCount);

        if(sM1 != FunctionInSourcePosition.end()){
            //当是函数的最后一行时,出现"/ *",处理注释
            if(lineCount == sM1->second) {
                std::string str = "";
                str += aLine;
                size_t pos = str.find("/ *");
                if(pos != std::string::npos){
                    //llvm::outs() << aLine << "  " << str << "\n";
                    str.erase(str.begin()+pos,str.end());
                    O << str;
                }
            }
            O << aLine;
            while(lineCount < sM1->second) {
                fgets(aLine, sizeof(aLine), fp);
                lineCount++;
                std::map<int, int>::iterator sM = LoopPosition.find(lineCount);
                //std::map<int, int>::iterator sM1 = DeclToBeDealedPosition.find(lineCount);
   
                if(sM != LoopPosition.end()) {
                    std::map<int, ForStmt*>::iterator sF = ForPosition.find(lineCount);
                    if(sF == ForPosition.end()) return false;
                    ForStmt *FS = sF->second;
                    llvm::DenseMap<ForStmt*, FunctionDecl*>::iterator sDM = ReturnInFile.find(FS);
                    llvm::DenseMap<ForStmt*, std::string>::iterator sFS = CallProto.find(FS);
                    if(sFS != CallProto.end()) {
                        std::string callStmt = sFS->second;
                        int pos = callStmt.find("_loop_", 0);
                        std::string cN = "";
                        int i = 0, temp_pos = pos;
                        temp_pos += 6;
                        char temp = callStmt.at(temp_pos);
                        while(temp != '(') {
                            cN += temp;
                            i++; temp_pos++;
                            temp = callStmt.at(temp_pos);
                        }

                        if(sDM != ReturnInFile.end()) { 
                            O << "\t{ int re_arg_pa1_" << cN << " = -1; "; 
                            FunctionDecl *FD = sDM->second;
                            if(!FD->getResultType()->isVoidType()) {
                                O << FD->getResultType().getAsString() << " re_arg_pa2_" << cN << ";";
                            }
                        }

                        O<< "\n";
                        O << "    " << callStmt << "\n";
                        if(sDM != ReturnInFile.end()) {
                            O << "\tif(re_arg_pa1_" << cN << " != -1)";
                            FunctionDecl *FD = sDM->second;
                            if(NULL == FD) return false;
                            if(FD->getResultType()->isVoidType()) O << " return; }\n";
                            else O << " return re_arg_pa2_" << cN << "; }\n";
                        }
                    } else return false;
                    //删除for
                    while(lineCount < sM->second) {
                        fgets(aLine, sizeof(aLine), fp);
                        lineCount++;
                    }
                    //处理for最后一行注释
                    if(lineCount == sM->second) {
                        std::string str = "";
                        str += aLine;
                        size_t pos = str.find("/ *");
                        if(pos != std::string::npos){
                            str.erase(str.begin(),str.begin() + pos);
                            O << str;
                        }
                    }
                        

                    LoopPosition.erase(sM);
                }
                if(sM == LoopPosition.end()){               
                    O << aLine;
                }
                memset(aLine, 0, sizeof(aLine));
            }
        }
    }*/
                   
    rewind(fp);    
    char aLine[LINE_SIZE] = { 0 };
    int lineCount = 0;
    bool skip = false;
    while(NULL != fgets(aLine, sizeof(aLine), fp)) {
        lineCount++;
        int i = 0;
         int ifcount = 0;
        int j= 0;
        char *str = aLine;
        std::string temp = "";
        while(str[i] == ' ') { i++; }//消除空格
        if(str[i] == '#'){
            if(temp.assign(str+i+1,7) == "include") {
                std::string str2 = aLine;
                size_t pos = str2.find("/*");
                if(pos != std::string::npos){
                    str2.erase(str2.begin(),str2.begin()+pos);
                    O << str2;
                }
                memset(aLine, 0, sizeof(aLine));
                continue;
            }
        }
        /*if(false == skip) {
            //判断以#开头的
            if(str[i] == '#') {
                if(temp.assign(str+i+1,2)=="if") {
                    ifcount++;
                    delString(str, &lineCount, fp);
                    while(ifcount > 0) {
                        memset(aLine, 0, sizeof(aLine));
                        fgets(aLine, sizeof(aLine), fp);
                        lineCount++;
                        str = aLine; i = 0;
                        while(*str == ' ') { str++; i++; }
                        if(temp.assign(str+i,3)=="#if") ifcount++;
                        if(temp.assign(str+i,6)=="#endif") ifcount--;
                        delString(str, &lineCount, fp);
                    }
                //其他情况，如#include、#define、#undef.
                } else delString(str, &lineCount, fp);
                    continue;
                //if(temp.assign(str+i+1,7)=="include") {
                  //  delString(str, &lineCount, fp);
                   // continue;
                //}
            } else if(temp.assign(str+i,2) == "//") continue;
            else if(temp.assign(str+i,2) == "/ *") { //处理注释
                while(1) {
                    str = aLine;
                    j = 0;
                    while(str[j] != '\0') j++;
                    if(str[j-3] == '*' && str[j-2] == '/') break;
                    memset(aLine, 0, sizeof(aLine));
                    fgets(aLine, sizeof(aLine), fp);
                    lineCount++;
                }

                continue;
            } else if(*str == '\n' || *str == '\r') continue;
            
        }
            skip = true;
        */
       
        //处理循环
        std::map<int, int>::iterator sM = LoopPosition.find(lineCount);
        std::map<int, int>::iterator sM1 = DeclToBeDealedPosition.find(lineCount);
   
        if(sM != LoopPosition.end()) {
            std::map<int, ForStmt*>::iterator sF = ForPosition.find(lineCount);
            if(sF == ForPosition.end()) return false;
                ForStmt *FS = sF->second;
            llvm::DenseMap<ForStmt*, FunctionDecl*>::iterator sDM = ReturnInFile.find(FS);
            llvm::DenseMap<ForStmt*, std::string>::iterator sFS = CallProto.find(FS);
            if(sFS != CallProto.end()) {
                std::string callStmt = sFS->second;
                int pos = callStmt.find("_loop_", 0);
                std::string cN = "";
                int i = 0, temp_pos = pos;
                temp_pos += 6;
                char temp = callStmt.at(temp_pos);
                while(temp != '(') {
                    cN += temp;
                    i++; temp_pos++;
                    temp = callStmt.at(temp_pos);
                }

                if(sDM != ReturnInFile.end()) { 
                    O << "\t{ int re_arg_pa1_" << cN << " = -1; "; 
                    FunctionDecl *FD = sDM->second;
                    if(!FD->getResultType()->isVoidType()) {
                        O << FD->getResultType().getAsString() << " re_arg_pa2_" << cN << ";";
                    }
                }

                O<< "\n";
                O << "    " << callStmt << "\n";
                if(sDM != ReturnInFile.end()) {
                    O << "\tif(re_arg_pa1_" << cN << " != -1)";
                    FunctionDecl *FD = sDM->second;
                    if(NULL == FD) return false;
                    if(FD->getResultType()->isVoidType()) O << " return; }\n";
                    else O << " return re_arg_pa2_" << cN << "; }\n";
                }
            } else return false;
            //删除for
            while(lineCount < sM->second) {
                fgets(aLine, sizeof(aLine), fp);
                lineCount++;
            }
            if(lineCount == sM->second) {
                std::string str = "";
                str += aLine;
                size_t pos = str.find("/*");
                if(pos != std::string::npos){
                    str.erase(str.begin(),str.begin()+pos);
                    O << str;
                }
             }

            LoopPosition.erase(sM);
        }
        if(sM1 != DeclToBeDealedPosition.end()){
            while(lineCount < sM1->second){
                fgets(aLine,sizeof(aLine),fp);
                lineCount++;
            }
            if(lineCount == sM1->second) {
                std::string str = "";
                str += aLine;
                size_t pos = str.find("/*");
                if(pos != std::string::npos){
                    str.erase(str.begin(),str.begin()+pos);
                    O << str;
                }
            }
           
        }
             
        if(sM1 == DeclToBeDealedPosition.end() && sM == LoopPosition.end()){               
            O << aLine;
        }
        
        memset(aLine, 0, sizeof(aLine));
    }

    return true;
}

static void addString(char *str, std::string *header, FILE* fp) {
    header->append(std::string(str));
    int i;
    char aLine[LINE_SIZE] = { 0 };
    while(1) {
        i=0;
        while(str[i] !='\0') i++;
        if(str[i - 2] == '\\') {   
            memset(aLine, 0, sizeof(aLine));
            if(NULL != fgets(aLine, sizeof(aLine), fp)) {
                header->append(aLine);
                str = aLine;
            }
        } else break;
    }

    return;
}

std::string LoopExtractorConsumer::getHeader(FILE *fp) {
    std::string header = "";
    std::string temp = "";
    if(fp == NULL) {
        if(ErrorOutput_LoopExtractAction) llvm::errs() << "传入的文件指针无效.\n";
        return "";
    }

    /*rewind(fp);
    std::string header1 = "";
    int lineCount = 0;
    char aLine1[LINE_SIZE] = { 0 };
    
    while(NULL != fgets(aLine1, sizeof(aLine1), fp)){
        lineCount++;
        std::map<int, int>::iterator sM = FunctionInSourcePosition.find(lineCount);
       
        if(sM != FunctionInSourcePosition.end()){
            while(lineCount < sM->second) {
                fgets(aLine1, sizeof(aLine1), fp);
                lineCount++;
            }
        }else{
            if(aLine1[0] != '\0')
                header1 += aLine1;
        }
        memset(aLine1, 0, sizeof(aLine1));
    }

    if(DebugOutput_LoopExtractAction) llvm::outs() << header1 << "\n";
    return header1;
    */
    /*rewind(fp);
    char aLine[LINE_SIZE] = { 0 };
    int ifcount = 0;
    int i = 0;
    int j;
    while(NULL != fgets(aLine, sizeof(aLine), fp)) {
        char *str = aLine;
        while(*str == ' ') { str++; i++; }//消除空格
        //判断以#开头的
        if(str[i] == '#') {
            if(temp.assign(str+i+1,2)=="if") {
                ifcount++;
                addString(str, &header, fp);
                while(ifcount > 0) {
                    memset(aLine, 0, sizeof(aLine));
                    fgets(aLine, sizeof(aLine), fp);
                    str = aLine; i = 0;
                    while(*str == ' ') { str++; i++; }
                    if(temp.assign(str+i,3)=="#if") ifcount++;
                    if(temp.assign(str+i,6)=="#endif") ifcount--;
                    addString(str, &header, fp);
                    }
            //其他情况，如#include、#define、#undef.
            } else addString(str, &header, fp);
        } else if(temp.assign(str+i,2) == "//") addString(str, &header, fp);
        else if(temp.assign(str+i,2) == "/ *") { //处理多行注释
                                       
            while(1) {
                header += aLine;
                str = aLine;
                j = 0;
                while(str[j] != '\0') j++;
                if(str[j-3] == '*' && str[j-2] == '/') break;
                memset(aLine, 0, sizeof(aLine));
                fgets(aLine, sizeof(aLine), fp);
            }
        } else if(*str == '\n' || *str == '\r') continue;
        else break;
    }//end of while
    */
    rewind(fp);
    char aLine[LINE_SIZE] = { 0 };
    int i ;
    int j=0;
    while(NULL != fgets(aLine, sizeof(aLine), fp)){
        i=0;
        char *str = aLine;
         j++;
        while(str[i] == ' ') { 
            i++; 
        }//消除空格
        if(str[i] == '#') {
            //llvm::outs() << str << "\n";
            if(temp.assign(str+i+1,7) == "include"){
                //addString(str, &header, fp);
                std::string str2 = str;
                size_t pos = str2.find("/ *");
                if(pos != std::string::npos){
                    str2.erase(str2.begin()+pos,str2.end());
                    str2 += "\n";
                }
                header += str2;
            }
        }
    }

    if(DebugOutput_LoopExtractAction && !header.empty()) llvm::outs() << "头文件:\n" << header << "\n";
    
    
    return header;
}

std::string LoopExtractorConsumer::getForStmt(FILE *fp, ForStmt *FS, ASTContext &context) {
    SourceRange SR = FS->getSourceRange();
    std::string forStmt = "";
    if(fp == NULL) {
        if(ErrorOutput_LoopExtractAction) llvm::errs() << "传入的文件指针无效.\n";
        return "";
    }
    
    rewind(fp);
    SourceManager &SM = context.getSourceManager();
    int lineStart = SM.getPresumedLoc(SR.getBegin()).getLine();
    int lineEnd = SM.getPresumedLoc(SR.getEnd()).getLine();

    LoopPosition[lineStart] = lineEnd;
    ForPosition[lineStart] = FS;

    int lineCount = 0;

    forStmt = getStrBetweenTwoLine(lineStart,lineEnd,lineCount,fp);
    /*char aLine[LINE_SIZE] = { 0 };
    while(NULL != fgets(aLine, sizeof(aLine), fp)) {
        if(++lineCount < lineStart) {
            memset(aLine, 0, sizeof(aLine));
            continue;
        }
        if(lineCount > lineEnd) break;
        forStmt += aLine;
        //
        memset(aLine, 0, sizeof(aLine));
    }*/

    if(DebugOutput_LoopExtractAction && !forStmt.empty()) llvm::outs() << "循环体为:\n" << forStmt << "\n";
    return forStmt;
}

std::string LoopExtractorConsumer::getMacroInCodeBlock(FILE *fp) {
    std::string macro = "";
    std::string temp = "";
    if(NULL == fp) return "";
    
    rewind(fp);
    char aLine[LINE_SIZE] = {0};
    int ifcount = 0;
    int i = 0; int j = 0;
    bool skip = false;
    while(NULL != fgets(aLine, sizeof(aLine), fp)) {
        char *str = aLine;
        i = 0;
        while(*str == ' ') { str++; i++; }
        if(false == skip) {
            if(str[i] == '#') {
                if(temp.assign(str+i+1,2)=="if") {
                    ifcount++;
                    delString(str, &j, fp);
                    while(ifcount > 0) {
                        memset(aLine, 0, sizeof(aLine));
                        fgets(aLine, sizeof(aLine), fp);
                        str = aLine; i = 0;
                        while(*str == ' ') { str++; i++; }
                        if(temp.assign(str+i,3)=="#if") ifcount++;
                        if(temp.assign(str+i,6)=="#endif") ifcount--;
                        delString(str, &j, fp);
                    }
                } else delString(str, &j, fp);
                continue;
            } else if(temp.assign(str+i,2) == "//") { delString(str, &j, fp); continue; }
            else if(temp.assign(str+i,2) == "/*") {
                while(1) {
                    str = aLine;
                    j = 0;
                    while(str[j] != '\0') j++;
                    if(str[j-3] == '*' && str[j-2] == '/') break;
                    memset(aLine, 0, sizeof(aLine));
                    fgets(aLine, sizeof(aLine), fp);
                }

                continue;
            } else if(!strncmp(str, "typedef", 7)) {
              //TODO:处理typedef结构 
              continue;
            } else if(*str == '\n' || *str == '\r') continue;
            else skip = true;
        }

        if(str[i] == '#') {
            if(temp.assign(str+i+1,2)=="if") {
                ifcount++;
                while(ifcount > 0) {
                    memset(aLine, 0, sizeof(aLine));
                    fgets(aLine, sizeof(aLine), fp);
                    str = aLine; i = 0;
                    while(*str == ' ') { str++; i++; }
                    if(temp.assign(str+i,3)=="#if") ifcount++;
                    if(temp.assign(str+i,6)=="#endif") ifcount--;
                }
            } else addString(str, &macro, fp);
        }
    }//end of while
   
    return macro;
}

bool LoopExtractorConsumer::isInternalVarDecl(DeclRefExpr *DRE) {
    ValueDecl *VD = DRE->getDecl();
    if(NULL == VD) return false;
    SourceManager &SM = VD->getASTContext().getSourceManager();
    if(SM.isFromMainFile(VD->getLocation())) return true;
    else return false;
}
bool LoopExtractorConsumer::isInternalDecl(Decl *D){
    if(NULL == D) return false;
    SourceManager &SM = D->getASTContext().getSourceManager();
    if(SM.isFromMainFile(D->getLocation())) return true;
    else return false;
}

std::string LoopExtractorConsumer::getStrBetweenTwoLine(int lineStart,int lineEnd,int &lineCount, FILE *fp){
    std::string StringBetweenTwoLine = "";
    
    char aLine[LINE_SIZE] = { 0 };
    while(NULL != fgets(aLine, sizeof(aLine), fp)) {
        if(++lineCount < lineStart) {
            memset(aLine, 0, sizeof(aLine));
            continue;
        }
        if(lineCount > lineEnd) break;
        if(lineCount == lineEnd){
            std::string str = "";
            str += aLine;
            size_t pos = str.find("/*");
            if(pos != std::string::npos)
                str.erase(str.begin() + pos,str.end());
            StringBetweenTwoLine += str;
        }else{
            StringBetweenTwoLine += aLine;
        }
        memset(aLine, 0, sizeof(aLine));
    }
    return StringBetweenTwoLine;
}
