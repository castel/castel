#pragma once

#include <llvm/LLVMContext.h>

#include "Runner.hh"

class Source;

class Compiler
{

public:

    Compiler( void );

public:

    Runner build( Source * source );

private:

    llvm::LLVMContext mContext;

};

#include "Source.hh"
