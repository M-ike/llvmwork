
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
int GotoCount;

llvm::DenseSet<ValueDecl*> TempSet;
std::vector<VarDecl*> VarDefInLoop;
std::vector<DeclRefExpr*> VarUseInLoop;
std::vector<ReturnStmt*> ReturnInLoop;
llvm::DenseMap<ForStmt*, FunctionDecl*> ReturnInFile;
std::map<std::string, VarDecl*> GlobalDeclToDeal;
//保存goto相关信息
std::vector<GotoStmt*> GotoInLoop;
std::map<std::string, LabelDecl*> LabelInLoop;
std::map<std::string, int> GotoLabel;
std::vector<Decl*> RecordDeclToDeal;
std::map<std::string, Decl*> TypedefDeclToDeal;

//保存结构体的相关信息
std::map<std::string,Decl*> allDecl;
std::map<std::string, VarDecl*> VarInFunction;
std::vector<Decl*> DeclToBeDealed;
std::vector<Decl*> DeclToBeDealedAfterDel;

std::vector<VarDecl*> globalDecl;
std::set<std::string> globalDeclName;
int headerLineEnd;
ASTConsumer* LoopExtractAction::CreateASTConsumer(CompilerInstance &CI, StringRef InFile) {
    return new LoopExtractorConsumer(InFile,CI);
}

char *itoa(int num);
void LoopExtractorConsumer::HandleTranslationUnit(ASTContext &Context) {
    ///TranslationUnitDecl* D = Context.getTranslationUnitDecl();
    //D->dump(llvm::outs());
    llvm::outs() << InputFile << "\n";
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
    GlobalDeclToDeal.clear();
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
    /*for(std::vector<Decl*>::iterator sD = RecordDeclToDeal.begin(), eD = RecordDeclToDeal.end(); sD != eD; ++sD){
        Decl *D = *sD;
        NamedDecl *ND=dyn_cast<NamedDecl>(D);
        std::string Dname = ND->getNameAsString();
        if(TypedefDeclToDeal[Dname] == NULL)  DeclToBeDealed.push_back(D);        
    }*/
    
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

     if(const IdentifierInfo *II = dyn_cast<const IdentifierInfo>(I->first))
        if(const  MacroDirective *MD = dyn_cast<const MacroDirective>(I->second)){
            SourceLocation SL=MD->getLocation();
            if(SM.isFromMainFile(SL)){
                if(MD->isDefined()){
                    const MacroInfo *MI = MD->getMacroInfo();
                    SourceLocation ISL = MI->getDefinitionLoc();
                    SourceLocation ESL = MI->getDefinitionEndLoc();
                    int lineStart = SM.getPresumedLoc(ISL).getLine();
                    int lineEnd = SM.getPresumedLoc(ESL).getLine();
                    //llvm::outs() << lineStart << "  "<< lineEnd <<"\n";
                    int lineCount = 0;
                    rewind(fp);
                    std::string str = getStrBetweenTwoLine(lineStart,lineEnd,lineCount,fp);
                    std::string MContent = "#ifndef ";
                    MContent += II->getName();
                    MContent += "\n";
                    MContent += str;
                    MContent += "#endif\n";
                    
                    std::pair<std::string,std::string> Pair;
                    Pair.first = MContent;
                    Pair.second = II->getName();
                    MacroContentInFile[lineStart] = Pair;
                    llvm::outs() << lineStart << "  " << Pair.second << "\n";
                }else{
                    std::string str = "#undef ";
                    str += II->getName();
                    str += "\n";
                    
                    std::string MContent = "#ifdef ";
                    MContent += II->getName();
                    MContent += "\n";
                    MContent += str;
                    MContent += "#endif\n";
                    int lineStart = SM.getPresumedLoc(SL).getLine();
                    std::pair<std::string,std::string> Pair;
                    Pair.first = MContent;
                    Pair.second = II->getName();
                    MacroContentInFile[lineStart] = Pair;
                    llvm::outs() << lineStart << "  " << Pair.second << "\n";
                }

            }else continue;
            
            const MacroDirective *preMD = MD->getPrevious();
            while(preMD != NULL){
                SourceLocation sSL1 = preMD->getLocation();
                if(SM.isFromMainFile(sSL1)){
                    
                    if(preMD->isDefined()){
                        
                        const MacroInfo *MI = preMD->getMacroInfo();
                        SourceLocation ISL = MI->getDefinitionLoc();
                        SourceLocation ESL = MI->getDefinitionEndLoc();
                        int lineStart = SM.getPresumedLoc(ISL).getLine();
                        int lineEnd = SM.getPresumedLoc(ESL).getLine();
                        //llvm::outs() << lineStart << "  "<< lineEnd <<"\n";
                        int lineCount = 0;
                        rewind(fp);
                        std::string str = getStrBetweenTwoLine(lineStart,lineEnd,lineCount,fp);
                        std::string MContent = "#ifndef ";
                        MContent += II->getName();
                        MContent += "\n";
                        MContent += str;
                        MContent += "#endif\n";
                        std::pair<std::string,std::string> Pair;
                        Pair.first = MContent;
                        Pair.second = II->getName();
                        MacroContentInFile[lineStart] = Pair;
                        llvm::outs() << lineStart << "  " << Pair.second << "\n";

                    }else{
                        std::string str = "#undef ";
                        str += II->getName();
                        str += "\n";
                        std::string MContent = "#ifdef ";
                        MContent += II->getName();
                        MContent += "\n";
                        MContent += str;
                        MContent += "#endif\n";
                        int lineStart = SM.getPresumedLoc(SL).getLine();
                        std::pair<std::string,std::string> Pair;
                        Pair.first = MContent;
                        Pair.second = II->getName();
                        MacroContentInFile[lineStart] = Pair;
                        llvm::outs() << lineStart << "  " << Pair.second << "\n";
                    }
                        
                }
                preMD = preMD->getPrevious();
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
    const LangOptions &Lang = Context.getLangOpts();

   
    for(std::vector<FunctionDecl*>::iterator sF = FunctionInSource.begin(),eF = FunctionInSource.end();sF != eF; ++sF){
        FunctionDecl *FD = *sF;
        //FD->dumpColor();
        if(FD == NULL ) {
            if(ErrorOutput_LoopExtractAction) llvm::errs() << "函数指针无效.\n";
            return false;
        }
        SourceRange SR = FD->getSourceRange();

        SourceLocation End = SR.getEnd();
        SourceLocation EndLoc(clang::Lexer::getLocForEndOfToken(End, 0, SM, Lang));
        bool Invalid = false;
        const char *Ptr = SM.getCharacterData(EndLoc, &Invalid);
        int k= 0;
        while(Ptr[k] != ';'){
            k++;
        }

            
        int lineStart = SM.getPresumedLoc(SR.getBegin()).getLine();
        int lineEnd = SM.getPresumedLoc(SR.getEnd()).getLine();

        int colStart = SM.getPresumedLoc(SR.getBegin()).getColumn();
        int colEnd = SM.getPresumedLoc(EndLoc).getColumn();
        colEnd += k;


        std::pair<int,std::pair<int,std::pair<int,int> > > Position;
        Position.first = lineStart;
        Position.second.first = colStart;
        Position.second.second.first = lineEnd;
        Position.second.second.second = colEnd;
        FunctionInSourcePosition.push_back(Position);

        //FunctionInSourcePosition[lineStart] = lineEnd;
        //llvm::outs() << lineStart << "  "<<lineEnd << "\n";
        rewind(fp);
        std::string FunctionContent = "";
        int lineCount = 1; 
        int colCount = 0;
        char ch = ' ';
        while( (ch = fgetc(fp)) != EOF){
            colCount++;
            if(true == isInTwoLocation(lineStart,colStart,lineEnd,colEnd,lineCount,colCount)){
                FunctionContent += ch;
                //llvm::outs() << ch;
            }
            if(ch == '\n') {
                lineCount++;
                colCount = 0;
            }
            ch = ' ';
        }
        FunctionContent += '\n';


        //FunctionContent = getStrBetweenTwoLine(lineStart,lineEnd,lineCount,fp);
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
    const LangOptions &Lang = Context.getLangOpts();
    for(std::vector<Decl*>::iterator sD = DeclToBeDealed.begin(),eD = DeclToBeDealed.end();sD != eD; sD++){

        Decl *D = *sD;
        rewind(fp);
        std::string DContent = "";
        SourceManager &SM = Context.getSourceManager();
        SourceRange SR = D->getSourceRange();

        SourceLocation End = SR.getEnd();
        SourceLocation EndLoc(clang::Lexer::getLocForEndOfToken(End, 0, SM, Lang));
        bool Invalid = false;
        bool isSpace = true;
        const char *Ptr = SM.getCharacterData(EndLoc, &Invalid);
        int k= 0;
        while(Ptr[k] != ';'){
            k++;
            if(Ptr[k] != ' ' && Ptr[k] != ';') {
                isSpace = false;
            }
        }

        int lineStart = SM.getPresumedLoc(SR.getBegin()).getLine();
        int lineEnd = SM.getPresumedLoc(EndLoc).getLine();
        int colStart = SM.getPresumedLoc(SR.getBegin()).getColumn();
        int colEnd = SM.getPresumedLoc(EndLoc).getColumn();
        if(isSpace) colEnd += k;

        std::pair<int,std::pair<int,std::pair<int,int> > > Position;
        Position.first = lineStart;
        Position.second.first = colStart;
        Position.second.second.first = lineEnd;
        Position.second.second.second = colEnd;
        //llvm::outs() << "Decl:"<< lineStart << " " << colStart << " " << lineEnd << " " << colEnd << "\n";
        /*std::map<int, int>::iterator sM1 = DeclToBeDealedPosition.find(Position);
        if(sM1 != DeclToBeDealedPosition.end()){
            continue;
        }*/
        
        DeclToBeDealedAfterDel.push_back(D);
        DeclToBeDealedPosition.push_back(Position);
        

        //DContent = getStrBetweenTwoLine(lineStart,lineEnd,lineCount,fp);
        int lineCount = 1;
        int colCount = 0;
        char ch = ' ';
        while( (ch = fgetc(fp)) != EOF){
            colCount++;
            if(true == isInTwoLocation(lineStart,colStart,lineEnd,colEnd,lineCount,colCount)){
                DContent += ch;
                //llvm::outs() << ch;
            }
            if(ch == '\n') {
                lineCount++;
                colCount = 0;
            }
            ch = ' ';
        }
        DContent += '\n';

        DeclContent[D] = DContent;
        //llvm::outs() << DContent << "\n";
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
    GotoInLoop.clear();
    GotoLabel.clear();
    LabelInLoop.clear();
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
    GotoCount = 1;
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

    if(LabelInLoop.size() != 0) return false;
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
            if(VarDecl *VVD = dyn_cast<VarDecl>(VD)){
            if(VVD->hasGlobalStorage() && !(VVD->isStaticLocal())) {
                std::string varName = VVD->getName();
                const char *scname = "";
                StorageClass SC = VVD->getStorageClass();
                if (SC != SC_None) scname = VarDecl::getStorageClassSpecifierString(SC);
                if(strcmp(scname,"static") != 0){
                     if(isInternalDecl(VVD)){
                      std::set<std::string>::iterator SI = globalDeclName.find(VVD->getName());
                      if(SI == globalDeclName.end()){
                        globalDeclName.insert(VVD->getName());
                        if(GlobalDeclToDeal[varName] == NULL)
                              GlobalDeclToDeal[varName] = VVD;
                      }
                    }
                     VarUseInLoop.erase(sDR);
                     sDR = VarUseInLoop.begin();
                     eDR = VarUseInLoop.end();
                }
            }
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
            RecordDecl *RDEF = RD->getDefinition();
            std::string RDName = "";
            RDName += RD->getNameAsString();
            if(RDName != "")
                 DeclToBeDealed.push_back(RDEF);
        }
    }
    if(true == hasFuncPionter(RD)){
        //FunctionRecord.push_back(RD);
        std::string RDName = "";
        RDName += RD->getNameAsString();
        //llvm::outs() << RDName << "\n";
    }
   
    return true;
}
bool LoopExtractorVistor::VisitTypedefDecl(TypedefDecl *TD){
    if(TraverseAction==2){
        SourceManager &SM = TD->getASTContext().getSourceManager();
        if(SM.isFromMainFile(TD->getLocation())){
           /* std::string type = TD->getUnderlyingType().getAsString();
            llvm::outs() << type << "111\n";
            if(type.find("struct ")+1) type.erase(0,7);
            if(type.find("enum ")+1) type.erase(0,5);
            if(type.find("union ")+1) type.erase(0,6);
            TypedefDeclToDeal[type] = TD;*/
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
            EnumDecl *EDEF = ED->getDefinition();
            std::string EDName = "";
            EDName += ED->getNameAsString();
            if(EDName != "") DeclToBeDealed.push_back(EDEF);
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


bool LoopExtractorVistor::VisitGotoStmt(GotoStmt *GS) {
    if(TraverseAction == 1 && ForStmtAction == 2){
        GotoInLoop.push_back(GS);
        LabelDecl *LD = GS->getLabel();
        std::string Name = LD->getNameAsString();
        if(GotoLabel.find(Name) == GotoLabel.end()){
             GotoLabel[Name] = GotoCount;
             GotoCount++;
        }

    }
    return true;
}


bool LoopExtractorVistor::VisitLabelStmt(LabelStmt *LS) {
    if(TraverseAction == 1 && ForStmtAction == 2){
        LabelDecl *LD = LS->getDecl();
        std::string Name = LD->getNameAsString();
        LD->dump();
        if(LabelInLoop[Name] == NULL){
            LabelInLoop[Name] = LD;
        }
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
    FuncPrototype += "int ";
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
        bool isRegister = false;
        if(VarDecl *vVD=dyn_cast<VarDecl>(VD)){
            if(vVD->getStorageClass() == SC_Register)
                isRegister = true;
        }

        if (ParmVarDecl *Parm = dyn_cast<ParmVarDecl>(VD)) T = Parm->getOriginalType();
        bool isArray = false;
        if(const ArrayType *AT = ASTC.getAsArrayType(T)) {
            Type += AT->getElementType().getAsString();
            isArray = true;
        } else Type += T.getAsString();
        std::string Name = VD->getName();
        if(isRegister){
            llvm::outs() << Name << "\n";
            FuncPrototype += Type;
            FuncPrototype += " *register_";
            FuncPrototype += std::string(number);
            FuncPrototype += Name;
        }else{
            FuncPrototype += Type;
            FuncPrototype += " ";
            if(false == isArray) 
                FuncPrototype += "*";
            FuncPrototype += Name;
            if(true == isArray) FuncPrototype += "[]";
        }
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
    //收集参数中寄存器变量并做处理
    std::string registerBefore ="";
    std::string registerend= "";
    for(std::vector<ValueDecl*>::iterator sVD = sFV->second.begin(),eVD=sFV->second.end();sVD != eVD; ++sVD ){
        ValueDecl *VD= *sVD;
        if(NULL == VD) break;
        if(VarDecl *vVD=dyn_cast<VarDecl>(VD)){
            if(vVD->getStorageClass() == SC_Register){
                std::string TypeName = vVD->getType().getAsString();
                std::string Name = vVD->getName();
                registerBefore += TypeName;
                registerBefore += " register_";
                registerBefore += std::string(number);
                registerBefore += Name;
                registerBefore += " = ";
                registerBefore += Name;
                registerBefore += ";\n";
                registerend += "\n";
                registerend += Name;
                registerend += " = register_";
                registerend += std::string(number);
                registerend += Name;
                registerend += ";\n";
                
            }
        }
        
    }
    //寄存器变量转化为普通变量.
    CallPrototype += "\n";
    CallPrototype += registerBefore;


    //2.构造调用原型
    CallPrototype += "		fv = ";
    CallPrototype += FD->getNameInfo().getName().getAsString();
    CallPrototype += "_loop_";
    CallPrototype += std::string(number);
    CallPrototype += "(";
    for(std::vector<ValueDecl*>::iterator sVD = sFV->second.begin(), eVD = sFV->second.end(); sVD != eVD; ++sVD) {
        ValueDecl *VD = *sVD;
        if(NULL == VD) break;
       bool isRegister = false;
        if(VarDecl *vVD=dyn_cast<VarDecl>(VD)){
            if(vVD->getStorageClass() == SC_Register)
                isRegister = true;
        }

        std::string Name = VD->getName();
        QualType T = VD->getType();
        if(!isRegister){
            if(!ASTC.getAsArrayType(T)) CallPrototype += "&";
        }else{
            if(!ASTC.getAsArrayType(T)) {
                CallPrototype += "&register_";
                CallPrototype += std::string(number);
            }
        }
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

    CallPrototype += ");\n";
    if(GotoInLoop.size() !=0){
      for(std::map<std::string, int>::iterator sGL = GotoLabel.begin(), eGL = GotoLabel.end(); sGL != eGL; ++sGL) {
                      std::string temp = sGL->first;
                      int t = sGL->second;
                      char *t1 = itoa(t);
                      std::string temp1 = "";
                      temp1 += std::string(t1);
                      if(LabelInLoop[temp] == NULL){
                         CallPrototype += "     if(fv == ";
                         CallPrototype += temp1;
                         CallPrototype +=") goto ";
                         CallPrototype += temp;
                         CallPrototype +=";\n";
                     }
                    }

    }

     //寄存器变量复制给原来的变量
    CallPrototype += registerend;
    CallPrototype += "\n";
    CallProto[FS] = CallPrototype;

    //LoopCount++;
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
bool isInComment(std::string &ForBody,size_t pos){
    
    std::map<size_t, size_t> doublequote;
    size_t pos1,pos2 = 0; 
    pos2 = ForBody.find("\"", 0);
    while( ForBody[pos2-1] == '\\' ||(ForBody[pos2-1] == '\'' && ForBody[pos2+1] == '\'')) 
        pos2 = ForBody.find("\"", pos2+1);
    while(std::string::npos != pos2){
        pos1 = pos2; 
        pos2 = ForBody.find("\"", pos2+1);
        while( ForBody[pos2-1] == '\\' ||(ForBody[pos2-1] == '\'' && ForBody[pos2+1] == '\'')) 
            pos2 = ForBody.find("\"", pos2+1);
        doublequote[pos1] = pos2; 
        pos2 = ForBody.find("\"", pos2+1);
        while( ForBody[pos2-1] == '\\' ||(ForBody[pos2-1] == '\'' && ForBody[pos2+1] == '\'')) 
            pos2 = ForBody.find("\"", pos2+1);

    }

    size_t comEndpos = ForBody.find("*/",pos);
    size_t comBeginpos = ForBody.find("/*",pos);

    if(comEndpos != std::string::npos && comBeginpos == std::string::npos){ 
        if(isInQoute(doublequote,comEndpos) == false)
            return true;
    }
    if(comEndpos != std::string::npos && comBeginpos != std::string::npos) {
        if(comEndpos < comBeginpos){
           if(isInQoute(doublequote,comEndpos) == false)
            return true;
        }
    }
    return false;
}
bool LoopExtractorConsumer::constructFunction(FILE *fp, ASTContext &context, FunctionDecl *FD, ForStmt *FS) {
    std::string NewFunc = "";
    char *number = itoa(LoopCount);
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
        //如果输入参数是register类型的变量,则跳过.
         bool isRegister = false;
         if(VarDecl *vVD = dyn_cast<VarDecl>(VD)) {
            if( vVD->getStorageClass() == SC_Register ){
                isRegister = true;
            }
        }
        QualType T = VD->getType();
        if(context.getAsArrayType(T)) continue;
        doublequote.clear();
        size_t pos1,pos2 = 0; 
        pos2 = ForBody.find("\"", 0);
        while( ForBody[pos2-1] == '\\'|| (ForBody[pos2-1] == '\'' && ForBody[pos2+1] == '\'')) 
            pos2 = ForBody.find("\"", pos2+1);
        while(std::string::npos != pos2){
            pos1 = pos2; 
            pos2 = ForBody.find("\"", pos2+1);
            while(ForBody[pos2-1] == '\\' || (ForBody[pos2-1] == '\'' && ForBody[pos2+1] == '\''))
                pos2 = ForBody.find("\"", pos2+1);
            doublequote[pos1] = pos2; 
            pos2 = ForBody.find("\"", pos2+1);
            while( ForBody[pos2-1] == '\\' ||(ForBody[pos2-1] == '\'' && ForBody[pos2+1] == '\'')) 
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
            //当替换的是寄存器变量时,加前缀register_.
            if(isRegister) {
                re += "register_";
                re += std::string(number);
            }


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

    size_t pos = ForBody.find("return", 0);
    while(isInComment(ForBody,pos) == true && pos != std::string::npos){
        pos = ForBody.find("return",pos+6);
    }
    if(ReturnInLoop.size() != 0 && pos !=std::string::npos) {

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

        for(std::vector<ReturnStmt*>::iterator sRS = ReturnInLoop.begin(), eRS = ReturnInLoop.end(); sRS != eRS && pos != std::string::npos; ++sRS) {
            ReturnStmt *RS = *sRS;
            if(NULL == RS) return false;

            size_t pos_end = ForBody.find(";", pos);

            std::string rep = "";
            rep += "{(*re_arg_pa1_";
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
            rep += "return 0;}";

            ForBody.replace(pos, pos_end - pos + 1, rep.c_str());
            pos += rep.size();
            pos = ForBody.find("return", pos);
            while(isInComment(ForBody,pos) == true ){
                pos = ForBody.find("return",pos+6);
            }

            while(isalpha(ForBody[pos - 1]) || isdigit(ForBody[pos - 1]) || ForBody[pos - 1] == '_' || \
                  isalpha(ForBody[pos + 6]) || isdigit(ForBody[pos + 6]) || ForBody[pos + 6] == '_') {
                pos += 6;
                pos = ForBody.find("return", pos);
                if(std::string::npos == pos) break;
            }

            if(std::string::npos == pos) break;
        }
    }

    size_t pos1 = ForBody.find("goto", 0);
    while(isInComment(ForBody,pos1) == true && pos1 != std::string::npos){
        pos1 = ForBody.find("goto",pos1+4);
    }
    if(GotoInLoop.size() != 0 && pos1 !=std::string::npos) {

        while(isalpha(ForBody[pos1 - 1]) || isdigit(ForBody[pos1 - 1]) || ForBody[pos1 - 1] == '_' || \
            isalpha(ForBody[pos1 + 4]) || isdigit(ForBody[pos1 + 4]) || ForBody[pos1 + 4] == '_') {
            pos1 += 4;
            pos1 = ForBody.find("goto", pos1);
            if(std::string::npos == pos1) break;
        }
        if(std::string::npos == pos1) return false;

        for(std::vector<GotoStmt*>::iterator sGS = GotoInLoop.begin(), eGS = GotoInLoop.end(); sGS != eGS && pos1 != std::string::npos; ++sGS) {
            GotoStmt *GS = *sGS;
            if(NULL == GS) return false;
            size_t pos1_end = ForBody.find(";", pos1);
            std::string gsbody= ForBody.substr(pos1+5,pos1_end - pos1-5);
            if(LabelInLoop[gsbody] == NULL){
               int LabelValue = GotoLabel[gsbody];
               char *lv = itoa(LabelValue);
               std::string rep = "";
               rep += "return ";
               rep += std::string(lv);
               rep += ";";

               ForBody.replace(pos1, pos1_end - pos1 + 1, rep.c_str());
               pos1 += rep.size();
               pos1 = ForBody.find("goto", pos1);
              } else{
               pos1 = ForBody.find("goto", pos1_end);
               }
               while(isInComment(ForBody,pos1) == true ){
                    pos1 = ForBody.find("goto",pos1+4);
               }

               while(isalpha(ForBody[pos1 - 1]) || isdigit(ForBody[pos1 - 1]) || ForBody[pos1 - 1] == '_' || \
                  isalpha(ForBody[pos1 + 4]) || isdigit(ForBody[pos1+ 4]) || ForBody[pos1 + 4] == '_') {
                   pos1 += 4;
                   pos1 = ForBody.find("goto", pos1);
                   if(std::string::npos == pos1) break;
               }

              if(std::string::npos == pos1) break;

       }
    }



    NewFunc += ForBody;
    NewFunc += "\nreturn 0;";
    NewFunc += "\n}";
   
    pos = 0;
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

    LoopCount++;
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
    //写条件编译的头文件.
    rewind(fp);
    int lineCount = 0;
    char aLine[LINE_SIZE] = {0};
    while(fgets(aLine,sizeof(aLine),fp) != NULL){
        lineCount++;
        std::map<int,std::pair<std::string,std::string> >::iterator sM = MacroContentInFile.find(lineCount);
        std::map<int,std::pair<std::string,std::string> >::iterator sM1 = ConditionCompileContent.find(lineCount);
        //std::map<int,std::vector<std::string> >::iterator sM2 = ConditionCompileToMacroName.find(lineCount);
        if(sM != MacroContentInFile.end()){
            bool isIn = false;
            llvm::outs() << "MacroName1:" << lineCount << "   "<<sM->second.second << "\n";
            for(std::vector<std::string>::iterator sV = MacroNameVec.begin(),eV = MacroNameVec.end();sV!=eV;sV++){
                llvm::outs() <<lineCount<<"MacroName:" <<*sV << "\n";
                if(sM->second.second == *sV){
                    isIn = true;
                    break;
                }
            }
            if(isIn){
                O << sM->second.first << "\n";
            }
        }
        if(sM1 != ConditionCompileContent.end()){
            O << sM1->second.first << "\n";
        }
        std::map<int,int>::iterator sMM = ConditionCompileLoc.find(lineCount);
        if(sMM != ConditionCompileLoc.end()){
            while(fgets(aLine,sizeof(aLine),fp) != NULL){
                lineCount++;
                if(lineCount >= sMM->second){
                    break;
                }
          }
        }
        //include        
        std::map<int,std::string>::iterator sM3= IncludeContent.find(lineCount);
        if(sM3 !=IncludeContent.end()){
            O << sM3->second <<"\n";
        }

    }
    //写结构体
    rewind(fp);
    lineCount = 1;
    int colCount = 0;
    char ch = ' ';
    //std::string temp = "";
    int IN = 0;
    int IN1 = 0;
    while((ch = fgetc(fp)) != EOF){
        colCount++;
        if(true == isInLocationVector(DeclToBeDealedPosition,lineCount,colCount,IN)){
            O << ch;
            if(IN == 0 ){
               if(ch != ';'&& !isInLocationVector(DeclToBeDealedPosition,lineCount,colCount+1,IN1)){
                  O << ";\n";
                }else{
                  O << "\n";
                }
            }
        }
        if(ch == '\n'){
            //llvm::outs() << lineCount << "\n";
            colCount = 0;
            lineCount++; 
        }

    }

    /*while(NULL != fgets(aLine, sizeof(aLine), fp)){
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
                    size_t pos = str.find("/ *");
                    if(pos != std::string::npos)
                        str.erase(str.begin() + pos,str.end());
                    O << str;
                 }else{
                    O << aLine;
                 }
            }
        }
    }*/

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
    for(std::map<std::string, VarDecl*>::iterator sGD = GlobalDeclToDeal.begin(), eGD = GlobalDeclToDeal.end(); sGD != eGD; ++sGD) {
         std::string temp = sGD->first;
         VarDecl* VD = sGD->second;
         std::string ty = VD->getType().getAsString();
         if(VD->getType()->isArrayType()){
             llvm::outs()<<ty<<"\n";
             size_t pos = ty.find("[");
             ty.erase(ty.begin() + pos,ty.end());
             O << "extern "<<ty<<" "<<temp<<"[];\n";
         }else{
         O << "extern "<<ty<<" "<<temp<<";\n";
         }
    }

    rewind(fp);
    char aLine[LINE_SIZE] = {0};
    int lineCount = 0;
    while(NULL != fgets(aLine,sizeof(aLine),fp)){
        lineCount++;
        std::map<int,std::pair<std::string,std::string> >::iterator sM = MacroContentInFile.find(lineCount);
        std::map<int,ForStmt*>::iterator sM1 = ForPosition.find(lineCount);
        if(sM != MacroContentInFile.end()){
            O << sM->second.first << "\n";
        }
        if(sM1 != ForPosition.end()){
            ForStmt *FS = sM1->second;
            llvm::DenseMap<ForStmt*,std::string>::iterator sMM = LoopToFunction.find(FS);
            if(sMM != LoopToFunction.end())
                O << sMM->second  << "\n";
        }
    }
    //写代码内部宏
    /*std::string macro = getMacroInCodeBlock(fp);
    if(!macro.empty()) O << macro;
    llvm::outs() << macro << "\n";*/

    /*for(std::vector<const MacroInfo*>::iterator sMI = MacroInFile.begin(),eMI = MacroInFile.end();sMI != eMI;++sMI){
        const MacroInfo *MI = *sMI;
        O << MacroContentInFile[MI];
    }

    O << "\n";
    for(llvm::DenseMap<ForStmt*, std::string>::iterator sDF = LoopToFunction.begin(), eDF = LoopToFunction.end(); sDF != eDF; ++sDF) {
        std::string NewFunc = sDF->second;
        O << NewFunc << "\n";
    }*/
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
                   

    //写入后的内容,只包含原函数部分,其他部分放到了头文件中.
    char aLine[LINE_SIZE] = {0};
    rewind(fp);
    int lineCount = 0;
    //首先跳过文件的头部.
    if(NULL !=fgets(aLine,sizeof(aLine),fp)){    
        while(++lineCount < headerLineEnd ){
            fgets(aLine, sizeof(aLine), fp);
        }
    }
     O<< "int fv;\n";
    lineCount = headerLineEnd + 1;
    int colCount = 0;
    char ch = ' ';
    int IN = 0;
    int IN2 = 0;
    int IN3 = 0;
    std::string temptype = "";
    while((ch = fgetc(fp)) != EOF){
        colCount++;
        //llvm::outs() << ch << lineCount << colCount;
        /*if(false == isInLocationVector(DeclToBeDealedPosition,lineCount,colCount,IN)){
            O << ch;
        }
        if(ch == '\n'){
            colCount = 0;
            lineCount++;
        }*/
        
        std::map<int,std::pair<std::string,std::string> >::iterator sM = ConditionCompileContent.find(lineCount);
        std::map<int,int>::iterator sM1 = ConditionCompileLoc.find(lineCount);
        if(sM != ConditionCompileContent.end() && sM1 != ConditionCompileLoc.end()){
            O <<sM->second.second << "\n";
            char aLine[LINE_SIZE] = {0};
            while(lineCount <= sM1->second){
                fgets(aLine,sizeof(aLine),fp);
                lineCount++;
            }
            colCount = 0;
            continue;
        }
        //include        
        std::map<int,std::string>::iterator sM3= IncludeContent.find(lineCount);
        if(sM3 !=IncludeContent.end()){
            char aLine[LINE_SIZE] = {0};
            fgets(aLine,sizeof(aLine),fp);
            lineCount++;
            colCount = 0;
            continue;
        }

        bool b1=isInLocationVector(DeclToBeDealedPosition,lineCount,colCount,IN);
        bool b3=isInLocationVector(DeclToBeDealedPosition,lineCount,colCount+1,IN3);
        bool b2=isInLocationVector(LoopPosition,lineCount,colCount,IN2); 
        if(!b1 && !b2){
            O << ch;
            if(ch == '\n'){
                //llvm::outs() << lineCount << "\n";
                colCount = 0;
                lineCount++; 
            }

            continue;
        }
        if( b1 && !b2 ){
            temptype += ch;
            if(IN == 0 ){
                if(ch != ';'&& !b3){
                   temptype.erase(temptype.find('{'), temptype.size());
                   //llvm::outs()<<temptype<<"结构体\n";
                   O << " ";
                   O << temptype;
                   O << " ";
                }else{
                   O << " ";
                }
                temptype = "";
            }
        }
        if( !b1 && b2){
            if(IN2 == 0){
                O << "\n";
            } 
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
                    O << "{";
                    if(sDM != ReturnInFile.end()) { 
                        O << "\t int re_arg_pa1_" << cN << " = -1; "; 
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
                        if(FD->getResultType()->isVoidType()) O << " return; \n";
                        else O << " return re_arg_pa2_" << cN << "; \n";
                    }
                   O<<"}";
                } else return false;
                //删除循环.
                while(true ==  isInLocationVector(LoopPosition,lineCount,colCount,IN)){
                    if(ch == '\n'){
                        lineCount++;
                        colCount = 1;
                    }
                    ch = fgetc(fp);
                    colCount++;
                    if(ch == EOF) break;
                }


        }
        if(ch == '\n'){
            //llvm::outs() << lineCount << "\n";
            colCount = 0;
            lineCount++; 
        }

            
    }
    /*rewind(fp);
    lineCount=0;
    while(NULL != fgets(aLine, sizeof(aLine), fp)) {
        while(++lineCount <= headerLineEnd ){
            fgets(aLine, sizeof(aLine), fp);
        }
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
                size_t pos = str.find("/ *");
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
                size_t pos = str.find("/ *");
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
    }*/
    llvm::outs() << "headerLineEnd "<<headerLineEnd << "\n";

    return true;
}
void getSymbol(char *(&cp),std::vector<std::string> &MacroNameVec){
   while((*cp) != '\0'){
        if((*cp== '/' && *(cp+1) =='*') || (*cp =='/' && *(cp+1)=='/')) return;
        std::string Name = "";
        char tempch[LINE_SIZE] = {0};
        int i=0; 
        if(isalpha(*cp) || (*cp) == '_'){
            tempch[i++] = *cp;
            cp++;
            while(isdigit(*cp) || isalpha(*cp) || (*cp) == '_'){
                tempch[i++]= *cp;
                cp++;
            }
            tempch[i++] = '\0';
            Name = tempch;
           // llvm::outs() << Name << "\n";
            if(Name != "defined"){
                bool isIn = false;
                for(std::vector<std::string>::iterator sN=MacroNameVec.begin(),eN = MacroNameVec.end();sN != eN; sN++){
                    if(Name == *sN ) {
                        isIn = true;
                        break;
                    }
                }
                if(isIn == false)
                    MacroNameVec.push_back(Name);
            }
        }else{
             cp++;
        }
   }
}
void skipcomment(char *(&cp),FILE *(&fp),int &line,std::string &CTempstr){
    char aLine[LINE_SIZE] = {0};
    char *cpp = cp;
    std::string str = "";

    bool EndComment = false; 
    //size_t pos ;
    int k = 0;
    while(*cp == ' ') {
            cp++;
    }
    //llvm::outs() << "cp1:" << cp ;
    //llvm::outs() << str.assign(cp,2) <<"       str1\n";
    if(str.assign(cp,2) != "/*" && str.assign(cp,2) != "//")  return;
    while(str.assign(cp,2)== "/*" || str.assign(cp,2) == "//"){
        if(str.assign(cp,2) == "//"){
            return;

        }
        k = 0;
        while(cpp[k+1] != '\0'){
            if(cpp[k] == '*' && cpp[k+1] == '/'){
                EndComment = true;
                break;
            }
            k++;
        }
        //llvm::outs() << k <<cpp[k] << "\n";
        while(!EndComment ){
            fgets(aLine,sizeof(aLine),fp);
            cpp = aLine;
            //llvm::outs() << "cp3:" << cpp;
            line++;
            k=0;
            while(cpp[k+1] != '\0'){
                if(cpp[k] == '*' && cpp[k+1] == '/'){
                    EndComment = true;
                    break;
                }else{
                    k++;
                }
            }
            //llvm::outs() << cpp << " &&" << k <<"\n";
            //cp = cpp + k;
            if(EndComment){
                str.assign(cpp,k+2);
                //k += 2;
                //llvm::outs() <<cpp[k-1] <<"str1:" << str;
                CTempstr += str;
                break;
            }else{
                CTempstr += aLine;
            }
            memset(aLine,0,sizeof(aLine));
        }
        cp = cpp+k+2;
        //llvm::outs() << "cp2:" << cp << "\n"; 
        while(cpp[k] == ' '){
            cp++;
            k++;
        }
    }
    //llvm::outs() << "cp4: " << cp;
    if(*cp != '\n' ){
        k = 0;
        //llvm::outs() << "3" << cp << "\n";
        while(*cp != '\n') {cp++; k++;}
        //llvm::outs() << "2" <<cp << "\n";
        fseek(fp,-(k+1),SEEK_CUR);
        fgets(aLine,sizeof(aLine),fp);
        //llvm::outs() << "aline:"<<aLine ;
        fseek(fp,-(k+1),SEEK_CUR);

        line--;
    }
    CTempstr += "\n";
    return;
}
/*void delNoContentIf(std::string &str){
    size_t posArray[256] = {0};
    posArray[0] = 0;
    size_t pos = posArray[0];
    pos = str.find('\n',pos);
    std::string tempstr = "";
    int k= 0;
    while(pos != std::string::npos){
        k++;
        pos++;
        while(str[pos] == ' ') pos++;
        posArray[k] == pos;
    }
    k--;
    bool done = false;
    while(!done){
        done = true;
        int k= 0;
        while(pos != std::string::npos){
            k++;
            pos++;
            while(str[pos] == ' ') pos++;
            posArray[k] == pos;
        }
        k--;
        for(int i=0;i<=k;i++){
            if(tempstr.assign(str+posArray[i],3) == "#if" && tempstr.assign(str+posArray[i+1],6) == "#endif"){
                str.erase(posArray[i],posArray[i+2]-1);
                done = false;
                continue;
            }
            if(tempstr.assign(str+posArray[i],3) == "#if" && tempstr.assign(str+posArray[i+1],5) == "#else" \
                tempstr.assign(str+posArray[i+2],6) == ){
        }
    }
   return;
}*/
std::string LoopExtractorConsumer::getHeader(FILE *fp) {
    std::string header = "";
    std::string temp = "";
    std::string ifContent = "";//临时存放#if..#end的内容.
    if(fp == NULL) {
        if(ErrorOutput_LoopExtractAction) llvm::errs() << "传入的文件指针无效.\n";
        return "";
    }

 
/*   rewind(fp);
    char aLine[LINE_SIZE] = { 0 };
    int ifcount = 0;
    int i;
    int j;
    bool haveComment;
    bool done = false;
    while(NULL != fgets(aLine, sizeof(aLine), fp)) {
        char *str = aLine;
        //判断以#开头的
        haveComment = true;
        while(haveComment){
            i= 0;
            while(str[i] == ' ') {i++; }//消除空格
            if(str[i] == '#') {
                if(temp.assign(str+i+1,2)=="if") {
                    int flags = 0;//flags标志着在if...后面是条件编译.
                    ifcount++;
                    addString(str, &ifContent, fp);
                    while(ifcount > 0) {
                        memset(aLine, 0, sizeof(aLine));
                        fgets(aLine, sizeof(aLine), fp);
                        str = aLine; i = 0;
                        while(str[i] == ' ') { i++; }
                        if(str[i] == '#'){
                            if(temp.assign(str+i,3)=="#if") ifcount++;
                            if(temp.assign(str+i,6)=="#endif") ifcount--;
                        }else{
                            flags = 1;
                        }
                        addString(str, &ifContent, fp);
                    }
                    if(flags == 0){
                        header += ifContent;
                    }else{
                        done = true;
                        break;
                    }
                //其他情况，如#include、#define、#undef.
                } else {
                    addString(str, &header, fp);
                }
            } else if(temp.assign(str+i,2) == "//") addString(str, &header, fp);
            else if(temp.assign(str+i,2) == "/ *") { //处理多行注释
                // / *和* /在同一行,其掉/ *...* /,对后面的继续处理.
                j=0;
                int n= 0;
                while(str[n+1] != '\0') n++;
                while(j < n){
                    if(str[j] == '*' && str[j+1] == '/') break;
                    j++;
                }
                if(j < n ){
                    str = str + j + 2;
                    continue;
                }else{ 
                    while(1) {
                        header += aLine;
                        str = aLine;
                        j = 0;
                        while(str[j] != '\0') j++;
                        if(str[j-3] == '*' && str[j-2] == '/') break;
                        memset(aLine, 0, sizeof(aLine));
                        fgets(aLine, sizeof(aLine), fp);
                    }
                }
            } else if(*str == '\n' || *str == '\r') { }
            else{done = true;}

            haveComment = false;
        }

        if(done) break;
    }//end of while
    //llvm::outs() << "头文件:" <<header << "\n";
    llvm::outs() << aLine << "\n";*/
    rewind(fp);    
    char aLine[LINE_SIZE] = { 0 };
    int lineCount = 0;
    bool skip = false;
    headerLineEnd = 0;
    
    bool haveComment;
    
    while(NULL != fgets(aLine, sizeof(aLine), fp)) {
        lineCount++;
        int i = 0;
        int ifcount = 0;
        int j= 0;
        char *str = aLine;
        std::string temp = "";
        std::string ifContent = "";
        haveComment = true;
        while(haveComment){
            while(str[i] == ' ') { i++; }//¿?¿?¿?¿?
            if(false == skip) {
                //¿?¿?¿?#¿?¿?¿?
                if(str[i] == '#') {
                    if(temp.assign(str+i+1,2)=="if") {
                        int flags = 0; //¿?¿?¿?¿?¿?if...¿?¿?¿?¿?¿?¿?¿?¿?¿?.
                        ifcount++;
                        delString(str, &lineCount,fp);
                        while(ifcount > 0) {
                            memset(aLine, 0, sizeof(aLine));
                            fgets(aLine, sizeof(aLine), fp);
                            lineCount++;
                            str = aLine; i = 0;
                            while(str[i] == ' ') { i++; }
                            if(str[i] == '#'){
                                if(temp.assign(str+i,3)=="#if") ifcount++;
                                if(temp.assign(str+i,6)=="#endif") ifcount--;
                            }else{
                                flags = 1;
                            }
                            delString(str, &lineCount, fp);
                        }
                        if(flags == 1) {
                            skip = true;
                        }else{
                            headerLineEnd = lineCount;
                        }

                    //¿?¿?¿?¿?¿?¿?#include¿?#define¿?#undef.
                    } else{ 
                        delString(str, &lineCount, fp);
                        headerLineEnd = lineCount;
                    }
                } else if(temp.assign(str+i,2) == "//") {headerLineEnd++;}
                else if(temp.assign(str+i,2) == "/*") { //¿?¿?¿?¿?
                    j=0;
                    int n= 0;
                    while(str[n+1] != '\0') n++;
                    while(j < n){
                        if(str[j] == '*' && str[j+1] == '/') break;
                        j++;
                    }
                    if(j < n ){
                        str = str + j + 2;
                        continue;
                    }else{ 
                        while(1) {
                            str = aLine;
                            j = 0;
                            while(str[j] != '\0') j++;
                            if(str[j-3] == '*' && str[j-2] == '/') break;
                            memset(aLine, 0, sizeof(aLine));
                            fgets(aLine, sizeof(aLine), fp);
                            lineCount++;
                        }
                    }
                    headerLineEnd = lineCount;
                } else if(*str == '\n' || *str == '\r') {headerLineEnd++;}
                else {skip = true;}
            }

            haveComment = false;
        }
            
       if(skip) break;
    }
    rewind(fp);
    int line = 0;
    int depth = 0;
    std::string tempstr = "";
    std::string tempstr1= "";
    std::string CTempstr="";
    std::string str;
    char *cp;
    bool haveInclude = false;
    while(line < headerLineEnd){
        memset(aLine,0,sizeof(aLine));
        fgets(aLine,sizeof(aLine),fp);
        header += aLine;
        line++;
    }
    //llvm::outs() << aLine << header <<headerLineEnd <<"\n";
    while(fgets(aLine,sizeof(aLine),fp) != NULL){
        line++;
        //去除前面的注释
        cp = aLine;
        skipcomment(cp,fp,line,CTempstr);
        //llvm::outs() << cp << "\n";
        
        if(str.assign(cp,8) == "#include"){
            tempstr = "";
            tempstr= aLine;
            IncludeContent[line] = tempstr;
        }else if(str.assign(cp,3) == "#if"){
            haveInclude = false;
            int lineStart = line;
            //std::vector<std::string> MacroNameVec;
            depth++;
            tempstr = "";
            tempstr1= "";
            tempstr += aLine;
            tempstr1 += aLine;
            //分析#if后面的宏的名称.
            cp = aLine + 3;
            while(isalpha(*cp)) cp++;
            getSymbol(cp,MacroNameVec);
            //llvm::outs() << "sybol1:" << cp << "\n";
            CTempstr = "";
            skipcomment(cp,fp,line,CTempstr);
            //llvm::outs() << "h1    "<<cp << "\n" << CTempstr;
            tempstr += CTempstr;
            tempstr1 += CTempstr;
            while(depth > 0){
                fgets(aLine,sizeof(aLine),fp);
                line++;
                cp = aLine;
                //skipcomment(cp,fp,line,CTempstr);

                while(*cp == ' ') cp++;
                if(str.assign(cp,3)=="#if"){
                    tempstr += aLine;
                    tempstr1 += aLine;
                    depth++;
                    cp += 2;
                    while(isalpha(*cp)) cp++;
                    getSymbol(cp,MacroNameVec);
                    //llvm::outs() << "sybol2:" << cp << "\n";
                    CTempstr = "";
                    skipcomment(cp,fp,line,CTempstr);
                    //llvm::outs() <<"CTempstr:"<< CTempstr << "\n";
                    tempstr += CTempstr;
                    tempstr1 += CTempstr;

                }else if(str.assign(cp,5)=="#else"){
                    tempstr += aLine;
                    tempstr1 += aLine;
                    cp += 4;
                    CTempstr = "";
                    skipcomment(cp,fp,line,CTempstr);
                }else if(str.assign(cp,7)=="#elseif"){
                    tempstr += aLine;
                    tempstr1 += aLine;
                    cp += 6;
                    while(isalpha(*cp)) cp++;
                    getSymbol(cp,MacroNameVec);
                    //llvm::outs() << "sybol3:" << cp << "\n";
                    CTempstr = "";
                    skipcomment(cp,fp,line,CTempstr);
                    tempstr += CTempstr;
                    tempstr1 += CTempstr;
                }else if(str.assign(cp,5)=="#elif"){
                    tempstr += aLine;
                    tempstr1 += aLine;
                    cp += 4;
                    while(isalpha(*cp)) cp++;
                    getSymbol(cp,MacroNameVec);
                    //llvm::outs() << "sybol3:" << cp << "\n";
                    CTempstr = "";
                    skipcomment(cp,fp,line,CTempstr);
                    tempstr += CTempstr;
                    tempstr1 += CTempstr;
                }else if(str.assign(cp,6)=="#endif"){
                    tempstr += aLine;
                    tempstr1 += aLine;
                    depth--;
                    cp += 5;
                    while(isalpha(*cp)) cp++;
                    getSymbol(cp,MacroNameVec);
                    CTempstr = "";
                    skipcomment(cp,fp,line,CTempstr);
                    tempstr += CTempstr;
                    tempstr1 += CTempstr;
                }else if(str.assign(cp,1)=="#"){
                    haveInclude = true;
                    tempstr += aLine; 
                    includeLoc.push_back(line);
                    llvm::outs() << "line:"<<line <<"\n";
                    //cp += 7;
                    while(*cp != '\0' ){
                        if(*cp == '/' && *cp == '*')
                            break;
                        else 
                            cp++;
                    }
                    if(*cp == '\0'){
                        CTempstr = "";
                        skipcomment(cp,fp,line,CTempstr);
                        tempstr += CTempstr;

                    }
                }else if(str.assign(cp,2) == "/*"){
                    skipcomment(cp,fp,line,CTempstr);
                }else{
                    tempstr1 += aLine;
                    while(*cp != '\0' ){
                        if(*cp == '/' && *cp == '*')
                            break;
                        else 
                            cp++;
                    }

                    if(*cp != '\0'){
                        CTempstr = "";
                        skipcomment(cp,fp,line,CTempstr);
                        tempstr1 += CTempstr;
                    }
                }
            }
            if(haveInclude){
                //delNoContentIf(tempstr);
                //delNoContentIf(tempstr1);
                std::pair<std::string,std::string> Content;
                Content.first = tempstr;
                Content.second = tempstr1;
                llvm::outs() << tempstr << tempstr1;
                ConditionCompileLoc[lineStart] = line;
                ConditionCompileContent[lineStart] = Content;
                //ConditionCompileToMacroName[lineStart] = MacroNameVec;
                for(std::vector<std::string>::iterator sN=MacroNameVec.begin(),eN = MacroNameVec.end();sN != eN; sN++){
                    llvm::outs() << *sN << "\n";
                }
                llvm::outs() << lineStart << " "<< line;
            }

        } 

    }

    if(DebugOutput_LoopExtractAction && !header.empty()) llvm::outs() << "头文件:\n" << header << "\n";
    
    
    return header;
}

std::string LoopExtractorConsumer::getForStmt(FILE *fp, ForStmt *FS, ASTContext &context) {
    SourceRange SR = FS->getSourceRange();
    //Stmt *ForBody = FS->getBody(); 
    std::string forStmt = "";
    if(fp == NULL) {
        if(ErrorOutput_LoopExtractAction) llvm::errs() << "传入的文件指针无效.\n";
        return "";
    }
    const LangOptions &Lang = context.getLangOpts();
    
    rewind(fp);
    SourceManager &SM = context.getSourceManager();
    SourceLocation End = SR.getEnd();
   // SourceLocation EndLoc(clang::Lexer::getLocForEndOfToken(End, 1, SM, Lang));


    const std::pair<SourceLocation,SourceLocation> ExpansionLoc(SM.getExpansionRange(End));
    const SourceLocation EEnd = ExpansionLoc.second;
  
    
    SourceLocation EndLoc(clang::Lexer::getLocForEndOfToken(EEnd, 1, SM, Lang));

    //llvm::outs() << "Expansion:"<< SM.getPresumedLoc(EndLoc).getColumn() << " " << SM.getPresumedLoc(EndLoc).getColumn()<< "\n"; 
    bool Invalid = false;
    const char *EEndptr = SM.getCharacterData(EndLoc,&Invalid);
    //llvm::outs() << EEndptr[-1] <<EEndptr[0] << EEndptr[1] << EEndptr[2] <<"\n";
   
    int lineStart = SM.getPresumedLoc(SR.getBegin()).getLine();
    int lineEnd = SM.getPresumedLoc(EndLoc).getLine();
    int colStart = SM.getPresumedLoc(SR.getBegin()).getColumn();
    int colEnd = SM.getPresumedLoc(EndLoc).getColumn();


    if(EEndptr[0] != '}'){
        int k= 0;
        while(EEndptr[k] != ';'){
            k++;
        }
        colEnd += k;
    }


    std::pair<int,std::pair<int,std::pair<int,int> > > Position;
    Position.first = lineStart;
    Position.second.first = colStart;
    Position.second.second.first = lineEnd;
    Position.second.second.second = colEnd;
    LoopPosition.push_back(Position);
    ForPosition[lineStart] = FS;
    if(DebugOutput_LoopExtractAction)
        llvm::outs() << "For:"<< lineStart << " " << colStart << " " << lineEnd << " " << colEnd << "\n";
    int lineCount = 1;
    int colCount = 0;
    char ch = ' ';
    while( (ch = fgetc(fp)) != EOF){
        colCount++;
        if(true == isInTwoLocation(lineStart,colStart,lineEnd,colEnd,lineCount,colCount)){
            forStmt += ch;
        }
        if(ch == '\n') {
            lineCount++;
            colCount = 0;
        }
        ch = ' ';
    }
    forStmt += '\n';
   // llvm::outs() << forStmt << "\n";


    //forStmt = getStrBetweenTwoLine(lineStart,lineEnd,lineCount,fp);
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
            StringBetweenTwoLine +="\n";
        }else{
            StringBetweenTwoLine += aLine;
        }
        memset(aLine, 0, sizeof(aLine));
    }
    return StringBetweenTwoLine;
}

bool LoopExtractorConsumer::isInTwoLocation(int lineStart,int colStart,int lineEnd,int colEnd ,int lineCount,int colCount){
    if(lineCount == lineStart){
        if(lineCount == lineEnd){
            if(colCount >=colStart && colCount <=colEnd)
                return true;
        }else{
            if(colCount >= colStart){
                return true;
            }
        }
    }else if(lineCount == lineEnd && colCount <= colEnd){
        return true;
                                              
    }else if(lineCount > lineStart && lineCount < lineEnd){
            return true;
                                                                        
   }
                                                                              

    return false;
}
bool LoopExtractorConsumer::isInLocationVector(std::vector< std::pair<int,std::pair<int,std::pair<int, int> > > > Position,
                                            int lineCount,int colCount,int &IN){
        for(std::vector<std::pair<int,std::pair<int,std::pair<int, int> > > >::iterator sP = Position.begin(),\
                                                                             eP = Position.end();sP!=eP;++sP){
            std::pair<int,std::pair<int,std::pair<int, int> > > SP = *sP;
            int lineStart = SP.first;
            //llvm::outs() << " atuo x: " << x.first << "\n";
            int colStart = SP.second.first;
            int lineEnd = SP.second.second.first;
            int colEnd = SP.second.second.second; 
            if(lineStart == lineCount && colStart == colCount){
                IN = 1; //表示在入口
                break;
            }else if(lineEnd == lineCount && colEnd == colCount){
                IN = 0; //表示在出口
                break;
            } else {
                IN = 2; //表示不在出口和入口
            }
        }
    
    for(std::vector<std::pair<int,std::pair<int,std::pair<int, int> > > >::iterator sP = Position.begin(),\
                                                                 eP = Position.end();sP!=eP;++sP){
        std::pair<int,std::pair<int,std::pair<int, int> > > SP = *sP;
        int lineStart = SP.first;
        //llvm::outs() << " atuo x: " << x.first << "\n";
        int colStart = SP.second.first;
        int lineEnd = SP.second.second.first;
        int colEnd = SP.second.second.second;
        if(isInTwoLocation(lineStart,colStart,lineEnd,colEnd,lineCount,colCount))
             return true;
        }


    return false;
}

