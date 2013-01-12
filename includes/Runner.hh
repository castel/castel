#pragma once

#include <castel/runtime/Box.hh>
#include <llvm/Module.h>

class Runner
{

private:

    static void staticInitializer( void );

public:

    Runner( llvm::Module * module );

public:

    castel::runtime::Box * operator()( castel::runtime::Box * process );

private:

    llvm::Module * mModule;

};
