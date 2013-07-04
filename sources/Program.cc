#include <iostream>
#include <sstream>
#include <string>

#include <castel/ast/tools/List.hh>
#include <castel/ast/Statement.hh>
#include <castel/runtime/boxes/Boolean.hh>
#include <castel/runtime/boxes/Class.hh>
#include <castel/runtime/boxes/Dict.hh>
#include <castel/runtime/boxes/Function.hh>
#include <castel/runtime/boxes/List.hh>
#include <castel/runtime/boxes/Null.hh>
#include <castel/runtime/boxes/Number.hh>
#include <castel/runtime/boxes/Object.hh>
#include <castel/runtime/boxes/String.hh>
#include <castel/runtime/boxes/Undefined.hh>
#include <castel/runtime/Box.hh>
#include <castel/toolchain/Source.hh>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Module.h>
#include <llvm/PassManager.h>

#include "Evaluator.hh"
#include "Program.hh"
#include "REPL.hh"

castel::toolchain::Source Program::makeSource( std::string const & from ) const
{
    if ( from == "-" ) {
        return castel::toolchain::Source::fromStream( std::cin );
    } else {
        return castel::toolchain::Source::fromFile( from );
    }
}

std::string Program::formatBox( castel::runtime::Box * box ) const
{
    std::ostringstream ss;

    if ( dynamic_cast< castel::runtime::boxes::Class * >( box ) )
        ss << "<Class>";

    if ( dynamic_cast< castel::runtime::boxes::Function * >( box ) )
        ss << "<Function>";

    if ( dynamic_cast< castel::runtime::boxes::Number * >( box ) )
        ss << "<Number : " << static_cast< castel::runtime::boxes::Number * >( box )->value( ) << ">";

    if ( dynamic_cast< castel::runtime::boxes::Boolean * >( box ) )
        ss << "<Boolean : " << static_cast< castel::runtime::boxes::Boolean * >( box )->value( ) << ">";

    if ( dynamic_cast< castel::runtime::boxes::String * >( box ) )
        ss << "<String : " << static_cast< castel::runtime::boxes::String * >( box )->value( ) << ">";

    if ( dynamic_cast< castel::runtime::boxes::Null * >( box ) )
        ss << "<Null>";

    if ( dynamic_cast< castel::runtime::boxes::Undefined * >( box ) )
        ss << "<Undefined>";

    if ( dynamic_cast< castel::runtime::boxes::Dict * >( box ) )
        ss << "<Dict>";

    if ( dynamic_cast< castel::runtime::boxes::List * >( box ) )
        ss << "<List>";

    if ( dynamic_cast< castel::runtime::boxes::Object * >( box ) )
        ss << "<Instance>";

    return ss.str( );
}

int Program::help( void ) const
{
    std::cerr << mOptionBag.usageString( );

    return 1;
}

int Program::checkSyntax( void ) const
{
    std::cerr << "Not yet implemented." << std::endl;

    return 1;
}

int Program::emitLLVM( void ) const
{
    Evaluator evaluator;

    castel::toolchain::Source source = this->makeSource( mOptionBag.inputFile( ) );
    llvm::Module * module = evaluator.compile( source );

    llvm::PassManager passManager;
    passManager.add( llvm::createPrintModulePass( & llvm::outs( ) ) );
    passManager.run( * module );

    return 0;
}

int Program::interpret( void ) const
{
    Evaluator evaluator;

    castel::toolchain::Source source = this->makeSource( mOptionBag.inputFile( ) );
    castel::runtime::Box * box = evaluator.run( source );

    if ( dynamic_cast< castel::runtime::boxes::Undefined * >( box ) == nullptr )
        std::cout << "Program returned " << this->formatBox( box ) << std::endl;

    return 0;
}

int Program::run( void ) const
{
    if ( mOptionBag.help( ) ) {
        return this->help( );

    } else if ( mOptionBag.checkSyntax( ) ) {
        return this->checkSyntax( );

    } else if ( mOptionBag.emitLLVM( ) ) {
        return this->emitLLVM( );

    } else if ( mOptionBag.inputFile( ) != "" ) {
        return this->interpret( );

    } else {
        return REPL( ).run( );

    }
}
