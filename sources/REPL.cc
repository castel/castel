#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>

#include <castel/ast/expr/literal/Dict.hh>
#include <castel/ast/expr/literal/Function.hh>
#include <castel/ast/expr/literal/String.hh>
#include <castel/ast/expr/Multary.hh>
#include <castel/ast/expr/Variable.hh>
#include <castel/ast/stmt/decl/Variables.hh>
#include <castel/ast/stmt/Return.hh>
#include <castel/ast/Statement.hh>
#include <castel/lex/Exception.hh>
#include <castel/parse/Exception.hh>
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
#include <castel/runtime/helper/create.hh>
#include <castel/runtime/Box.hh>
#include <castel/toolchain/Source.hh>
#include <readline/readline.h>

#include "REPLVisitor.hh"
#include "REPL.hh"

REPL::REPL( void )
{
    castel::runtime::boxes::Dict * context = castel::runtime::helper::create< castel::runtime::boxes::Dict >( );
    context->item( { castel::runtime::helper::create< castel::runtime::boxes::String >( "variables" ) }, castel::runtime::helper::create< castel::runtime::boxes::Dict >( ) );
    context->item( { castel::runtime::helper::create< castel::runtime::boxes::String >( "return" ) }, castel::runtime::helper::create< castel::runtime::boxes::Undefined >( ) );
    mLastContext = context;

    mCompiler.globals( ).push_back( "__castel_context" );
    mRunner.globals( )[ "__castel_context" ] = std::function< castel::runtime::Box * ( void ) >( [ this ] ( ) {
        return mLastContext;
    } );
}

castel::ast::Statement * REPL::parse( char * line )
{
    std::string string = line;
    std::free( line );

    castel::ast::Statement * statements;

    try {
        statements = castel::toolchain::Source::fromString( string ).parse( );
    }

    catch ( castel::lex::Exception & exception ) {
        std::cerr << exception.what( ) << std::endl;
        return nullptr;
    }

    catch ( castel::parse::Exception & exception ) {
        std::cerr << exception.what( ) << std::endl;
        return nullptr;
    }

    if ( statements == nullptr )
        return nullptr;

    return this->wrap( statements );
}

castel::ast::Statement * REPL::wrap( castel::ast::Statement * statements )
{
    REPLVisitor replVisitor;
    for ( auto & statement : statements )
        replVisitor.run( statement );

    for ( auto & symbol : replVisitor.symbols( ) )
        mSymbols.insert( symbol );

    /* a = __castel_context["variables"]["a"], ...; */
    castel::ast::stmt::decl::Variables::Variable * variableDeclarations = nullptr;
    for ( auto & symbol : mSymbols )
        variableDeclarations = & ( ( new castel::ast::stmt::decl::Variables::Variable(
            symbol, ( new castel::ast::expr::Multary( castel::ast::expr::Multary::Operator::Subscript, & (
                ( new castel::ast::expr::Multary( castel::ast::expr::Multary::Operator::Subscript, & (
                    ( new castel::ast::expr::Variable( "__castel_context" ) )
                    ->next( ( new castel::ast::expr::literal::String( "variables" ) ) ) ) ) )
                ->next( ( new castel::ast::expr::literal::String( symbol ) ) ) ) ) ) ) )
        ->next( variableDeclarations ) );

    /* "a": a, ... */
    castel::ast::expr::literal::Dict::Item * variableStorage = nullptr;
    for ( auto & symbol : mSymbols )
        variableStorage = & ( ( new castel::ast::expr::literal::Dict::Item(
            symbol, ( new castel::ast::expr::Variable( symbol ) )
        ) )->next( variableStorage ) );

    /* { "variables": variableStorage, "return": ( function ( ) { [...] } )( ) } */
    castel::ast::expr::literal::Dict::Item * returnedDict = & (
        ( new castel::ast::expr::literal::Dict::Item( "return",
            ( new castel::ast::expr::Multary( castel::ast::expr::Multary::Operator::Call,
                ( new castel::ast::expr::literal::Function( nullptr,
                    ( statements ) ) ) ) ) ) )
        ->next( ( new castel::ast::expr::literal::Dict::Item( "variables",
            ( new castel::ast::expr::literal::Dict( variableStorage ) ) ) ) ) );

    /* var [...]; return [...]; */
    castel::ast::Statement * wrappedStatements = & (
        ( new castel::ast::stmt::decl::Variables( variableDeclarations ) )
        ->next( ( new castel::ast::stmt::Return(
            ( new castel::ast::expr::literal::Dict( returnedDict ) ) ) ) ) );

    return wrappedStatements;
}

castel::runtime::Box * REPL::execute( castel::ast::Statement * statement )
{
    llvm::Module * module = mCompiler.build( statement );

    mLastContext = mRunner.run( module );

    castel::runtime::boxes::Dict * context = static_cast< castel::runtime::boxes::Dict * >( mLastContext );
    return context->item( { castel::runtime::helper::create< castel::runtime::boxes::String >( "return" ) } );
}

std::string REPL::format( castel::runtime::Box * box )
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

void REPL::print( castel::runtime::Box * box )
{
    std::cout << this->format( box ) << std::endl;
}

int REPL::run( void )
{
    for ( char * line; ( line = readline( "> " ) ) != nullptr; ) {

        castel::ast::Statement * statements = this->parse( line );
        if ( statements == nullptr ) continue ;

        castel::runtime::Box * box = this->execute( statements );

        if ( dynamic_cast< castel::runtime::boxes::Undefined * >( box ) == nullptr ) {
            this->print( box );
        }

    }

    std::cout << std::endl;

    return 0;
}
