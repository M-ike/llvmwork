////////////////////////////////////////////////////////////
///// COPYRIGHT NOTICE \n
///// Copyright (c) 2013, 中国科学技术大学(合肥) \n
///// All rights reserved. \n
/////
///// @file main.cpp
///// @brief 本项目的入口函数，同时也是驱动其他分析的入口点
/////
///// 本文件实现了分析前端，可以同时处理多个文件
/////
///// @version 0.1
///// @author xtxwy
/////
///// 版本修订: \n
/////     无
////////////////////////////////////////////////////////////////
#include "LoopExtractAction.h"

#include "clang/CodeGen/CodeGenAction.h"
#include "clang/Driver/Compilation.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Tool.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Frontend/FrontendActions.h"

#include "llvm/IR/Module.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/PassManager.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"

#include <cstring>
#include <stdio.h>

using namespace llvm;
using namespace clang;
using namespace clang::driver;

int DebugOutput_main = 0;
int ErrorOutput_main = 0;
int loop_min_trip_count = 10;

int main(int argc, const char **argv, char * const *envp) {
    IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts = new DiagnosticOptions();
    TextDiagnosticPrinter *DiagClient = new TextDiagnosticPrinter(llvm::errs(), &*DiagOpts);
    IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());
    DiagnosticsEngine Diags(DiagID, &*DiagOpts, DiagClient);
    Driver TheDriver("/usr/local/bin/", llvm::sys::getProcessTriple(), "a.out", Diags);
    TheDriver.setTitle("FrontendDriver by maxs");

    SmallVector<const char *, 16> Args(argv, argv + argc);
    Args.push_back("-fsyntax-only");

    for(SmallVector<const char*, 16>::iterator s = Args.begin(), e = Args.end(); s != e; ++s) {
	    if(0 == strncmp(*s, "-h", 2) || 0 == strncmp(*s, "--help", 6) || 0 == strncmp(*s, "--h", 3)) {
	        llvm::outs() << "LLVM Loop Extractor by xtxwy,zql and zk\n";
            llvm::outs() << "使用方法:loopextractor <源文件1 源文件2 ...> [-loopextract-min-trip-count=循环迭代次数] [-I<路径1> -I<路径2> ...]\n";
            llvm::outs() << "-loopextract-min-trip-count=循环迭代次数      指定循环分割基于的阀值,默认为10\n";
            llvm::outs() << "-I<路径1> ...                                 指定头文件搜索路径\n"; 
            llvm::outs() << "-h或者--help                                  显示帮助菜单\n";
            return 0;
        }
        if(0 == strncmp(*s, "-loopextract-min-trip-count=", 28)) {
            std::string trip_count = "";
            char *pch = strstr(*s, "="); pch++;
            trip_count.append(pch);
            loop_min_trip_count = atoi(trip_count.c_str());
            Args.erase(s);
        }
    }

    OwningPtr<Compilation> C(TheDriver.BuildCompilation(Args));
    if (!C) return 0;
    const driver::JobList &Jobs = C->getJobs();
    for(JobList::const_iterator s = Jobs.begin(), e = Jobs.end(); s != e; ++s) {
  	    const driver::Command *Cmd = cast<driver::Command>(*s);
  	    if (llvm::StringRef(Cmd->getCreator().getName()) != "clang") {
    	    Diags.Report(diag::err_fe_expected_clang_command);
    	    return 1;
  	    }

  	    const driver::ArgStringList &CCArgs = Cmd->getArguments();
  	    OwningPtr<CompilerInvocation> CI(new CompilerInvocation);
  	    CompilerInvocation::CreateFromArgs(*CI, const_cast<const char **>(CCArgs.data()), const_cast<const char **>(CCArgs.data()) + CCArgs.size(), Diags);

  	    CompilerInstance Clang;
  	    Clang.setInvocation(CI.take());
  	    Clang.createDiagnostics();
  	    if (!Clang.hasDiagnostics()) return 1;

  	    OwningPtr<LoopExtractAction> Act(new LoopExtractAction());
  	    Clang.setTarget(TargetInfo::CreateTargetInfo(Clang.getDiagnostics(), &(Clang.getTargetOpts())));
  	    if(!Clang.hasTarget()) return 0;
  	    Clang.getTarget().setForcedLangOptions(Clang.getLangOpts());

  	    for(unsigned i = 0, e = Clang.getFrontendOpts().Inputs.size(); i != e; ++i) {
		    if(Clang.hasSourceManager()) Clang.getSourceManager().clearIDTables();
		    if(Act->BeginSourceFile(Clang, Clang.getFrontendOpts().Inputs[i])) {
			    Act->Execute();
			    Act->EndSourceFile();	
		    }
        }
    }

    llvm::llvm_shutdown();
    return 0;
}
