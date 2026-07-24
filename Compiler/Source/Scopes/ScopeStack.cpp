/* === Main Import === */

#include "Scopes/ScopeStack.hpp"
#include "Errors/Errors.hpp"

/* === Scope Stack Methods === */

void ScopeStack::enter( ScopeId scope )
{
    m_stack.push_back( scope );
}

void ScopeStack::leave()
{
    if ( m_stack.empty() ) {
        throw InternalCompilerError( "Tried to leave Top Level Scope.\nPlease report this bug." );
    }

    m_stack.pop_back();
}

ScopeId ScopeStack::current() const 
{
    if ( m_stack.empty() ) {
        throw InternalCompilerError( "Current scope is invalid.\nPlease report this bug." );
    }

    return m_stack.back();
}
