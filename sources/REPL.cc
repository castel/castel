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

castel::ast::tools::List< castel::ast::Statement > REPL::parse( char * line )
{
    std::string string = line;
    std::free( line );

    castel::ast::tools::List< castel::ast::Statement > statements =
        castel::toolchain::Source::fromString( string ).parse( );

    this->wrap( statements );
    return statements;
}

template < typename T >
using Hold = castel::ast::tools::Hold< T >;

void REPL::wrap( castel::ast::tools::List< castel::ast::Statement > & statements )
{
    using Dict = castel::ast::expr::literal::Dict;
    using Function = castel::ast::expr::literal::Function;
    using Multary = castel::ast::expr::Multary;
    using Return = castel::ast::stmt::Return;
    using String = castel::ast::expr::literal::String;
    using Variables = castel::ast::stmt::decl::Variables;
    using Variable = castel::ast::expr::Variable;

    REPLVisitor replVisitor;
    replVisitor.run( statements );

    for ( auto & symbol : replVisitor.symbols( ) )
        mSymbols.insert( symbol );

    /* var a = __castel_context["variables"]["a"], ...; */
    Hold< Variables > preExecutionDeclarations( new Variables( ) );

    for ( auto & symbol : mSymbols ) {
        Hold< Multary > variables( new Multary( Multary::Operator::Subscript, { } ) );
        variables->operands( ).push_back( Hold< Variable >( new Variable( "__castel_context" ) ) );
        variables->operands( ).push_back( Hold< String >( new String( "variables" ) ) );
        Hold< Multary > value( new Multary( Multary::Operator::Subscript, { } ) );
        value->operands( ).push_back( std::move( variables ) );
        value->operands( ).push_back( Hold< String >( new String( symbol ) ) );
        Hold< Variables::Item > variable( new Variables::Item( symbol, std::move( value ) ) );
        preExecutionDeclarations->variables( ).push_back( std::move( variable ) );
    }

    /* { "a": a, ... } */
    Hold< Dict > postExecutionDump( new Dict( ) );

    for ( auto & symbol : mSymbols ) {
        Hold< Variable > variable( new Variable( symbol ) );
        Hold< Dict::Item > item( new Dict::Item( symbol, std::move( variable ) ) );
        postExecutionDump->items( ).push_back( std::move( item ) );
    }

    /* { "return": ( function ( ) { [...] } )( ), "variables": variableStorage } */
    Hold< Dict > replData( new Dict( ) );

    if ( true ) {
        Hold< Multary > functionCall( new Multary( Multary::Operator::Call, { } ) );
        functionCall->operands( ).push_back( Hold< Function >( new Function( { }, std::move( statements ) ) ) );
        replData->items( ).push_back( Hold< Dict::Item >( new Dict::Item( "return", std::move( functionCall ) ) ) );
        replData->items( ).push_back( Hold< Dict::Item >( new Dict::Item( "variables", std::move( postExecutionDump ) ) ) );
    }

    /* var [...]; return [...]; */
    statements.push_back( std::move( preExecutionDeclarations ) );
    statements.push_back( Hold< Return >( new Return( std::move( replData ) ) ) );
}

castel::runtime::Box * REPL::execute( castel::ast::tools::List< castel::ast::Statement > const & statements )
{
    llvm::Module * module = mCompiler.build( statements );

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

        castel::ast::tools::List< castel::ast::Statement > statements = this->parse( line );

        castel::runtime::Box * box = this->execute( statements );

        if ( ! dynamic_cast< castel::runtime::boxes::Undefined * >( box ) ) {
            this->print( box );
        }

    }

    std::cout << std::endl;

    return 0;
}
