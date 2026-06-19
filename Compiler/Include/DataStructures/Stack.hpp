#pragma once

#include <stdexcept>

template <typename U>
struct StackNode 
{
    U data;
    StackNode<U>* next;
    StackNode( const U& d ) : data( d ), next( nullptr ) {}
};

template <typename T>
class Stack 
{
    public:
        Stack() : m_top( nullptr ) {}
        ~Stack() 
        { 
            clear(); 
        }

        void push( const T& value );
        T pop();
        const T& peek() const;

        bool isEmpty() const 
        { 
            return m_top == nullptr; 
        }

        void clear();

    private:
        StackNode<T>* m_top;
};

// ---- DEFINITIONS ----
template <typename T>
void Stack<T>::push( const T& value ) 
{
    StackNode<T>* newNode = new StackNode<T>( value );
    newNode->next = m_top;
    m_top = newNode;
}

template <typename T>
T Stack<T>::pop() 
{
    if ( isEmpty() ) throw std::runtime_error( "Stack underflow" );
    StackNode<T>* temp = m_top;
    T data = temp->data;
    m_top = m_top->next;
    delete temp;
    return data;
}

template <typename T>
const T& Stack<T>::peek() const 
{
    if ( isEmpty() ) throw std::runtime_error( "Stack is empty" );
    return m_top->data;
}

template <typename T>
void Stack<T>::clear() 
{
    while ( m_top ) 
    {
        StackNode<T>* temp = m_top;
        m_top = m_top->next;
        delete temp;
    }
}