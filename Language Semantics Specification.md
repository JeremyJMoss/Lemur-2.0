# Lemur Language Semantic Specification

## Introduction

This document outlines the semantic specifications of **Lemur**—a multipurpose, statically typed, imperative, object-oriented programming language inspired by low-level systems languages like C++ and Rust.

Lemur was designed as a platform to explore the inner workings of language tooling, including tokenization, semantic analysis, scope resolution, intermediate representation (IR) generation, and direct assembly output. It intuitive ownership semantics, aiming to strike a balance between control and usability—giving you the low-level power you need, without the boilerplate or baggage.

---

## Values and References

In Lemur, **values** are stored directly on the stack. They are fast to allocate and deallocate and are ideal for primitive types such as `int`, `bool`, `float`, and `char`.

**References**, by contrast, are pointers to data stored on the heap. A reference holds the memory address of a dynamically allocated object, allowing for flexible and persistent access at the cost of heap management overhead.

---

## Semantics

### Ownership & Reference Model

To ensure memory safety and fine-grained control over value semantics, Lemur introduces five core keywords for handling pointers and references. These keywords define both ownership behavior and access permissions.

#### 1. `own` — Exclusive Ownership

* **Semantics**: The variable has full, unique ownership of a heap-allocated value.
* **Reassignment**: The pointer itself cannot be reassigned.
* **Mutation**: Can mutate the value it points to.
* **Use Case**: When the variable is solely responsible for managing the lifetime of the resource.

#### 2. `share` — Shared Ownership

* **Semantics**: The variable shares ownership with others via reference counting.
* **Reassignment**: The pointer cannot be reassigned.
* **Mutation**: Can mutate the value it points to.
* **Use Case**: When multiple components need concurrent access to the same resource.

#### 3. `weak` — Observes Shared Data Without Owning It

* **Semantics**: A non-owning reference to data managed by `share`; does not affect the reference count.
* **Reassignment**: Cannot be reassigned.
* **Mutation**: Cannot mutate the value unless upgraded to `share`.
* **Use Case**: For observers, caches, or cyclic references where ownership would be unsafe.

#### 4. `view` — Immutable Borrow

* **Semantics**: A read-only, non-owning reference.
* **Reassignment**: Not rebindable.
* **Mutation**: Not allowed.
* **Use Case**: Safe read-only access without affecting ownership.

#### 5. `mut` — Mutable Borrow

* **Semantics**: A temporary, exclusive borrow with write access.
* **Reassignment**: Not rebindable.
* **Mutation**: Allowed.
* **Use Case**: Temporary mutations without transferring ownership.

### Summary Table

| Keyword | Ownership | Rebindable | Mutable | Extends Pointer Lifetime |
| ------- | --------- | ---------- | ------- | ------------------------ |
| `own`   | ✅        | ❌         | ✅      | ✅                       |
| `share` | ✅        | ❌         | ✅      | ✅                       |
| `weak`  | ❌        | ❌         | ❌      | ❌                       |
| `view`  | ❌        | ❌         | ❌      | ❌                       |
| `mut`   | ❌        | ❌         | ✅      | ❌                       |

### Ownership Flow Decision Tree

```
Start: What kind of access do you need?

Do you want full ownership of the data?
├── Yes
│   └── Should ownership be shared?
│       ├── Yes
│       │   └── Should this reference extend the lifetime?
│       │       ├── Yes → Use `share`
│       │       └── No  → Use `weak`
│       └── No  → Use `own`
└── No (you want to borrow/reference existing data)
    └── Should the referenced value be mutable?
        ├── Yes → Use `mut`
        └── No  → Use `view`
```

---

## Additional Notes

* References are **non-nullable** on initialization and **non-rebindable**.

* All pointer types (`own`, `share`, `weak`, `view`, `mut`) are initialized to `null` by default unless assigned—preventing accidental garbage access.

* These keywords wrap types in pointer-like constructs with defined semantics:

  * `own`: Unique heap ownership.
  * `share`: Shared heap ownership with reference counting.
  * `weak`: Non-owning reference that can be upgraded to `share` (throws a runtime error if the original is deallocated).
  * `view`: Immutable, read-only reference.
  * `mut`: Exclusive, mutable borrow.

* **Mutability requires `mut`**. `view` is strictly read-only and cannot be cast to `mut`.

* **Complex types** (e.g., arrays, strings, structs) default to `own`.

---

## Memory & Lifetime

* **Heap-allocated data** is automatically freed:

  * `own` types are freed when they go out of scope.
  * `share` types are freed when the reference count drops to zero.

* **Primitive types** (e.g., `int`, `bool`, `float`, `char`) are always stack-allocated and copied by default.

  * Attempting to `move` a primitive triggers a compile-time error.

---

## Examples

```lemur
x: int = 5;                        // stack-allocated int
y: own int = 5;                    // heap-allocated int with exclusive ownership
z: share int = 5;                  // shared heap allocation

a: view int = y;                   // immutable borrow
b: mut int = borrow_mut(z);       // mutable borrow (exclusive at runtime)

c: weak int = z;                   // weak reference to z
d: mut int = y;                    // mutable borrow from unique owner

e: atomic int = 6;                 // thread-safe shared integer
```

---

## Class and Inheritance Model (Not Implemented)

* Classes

  * Classes can inherit from one class
  * Classes can implement multiple interfaces

* Abstract Classes

  * Abstract methods can only exist in abstract classes
  * Abstract classes can have a constructor but cannot be directly instantiated
  * Abstract classes cannot contain fields
  * Abstract classes can contain virtual methods
  * Abstract classes can also contain non-virtual methods with full implementations.
  * Abstract classes can declare abstract methods with no implementation, but non-abstract virtuals must have an implementation.
  * Classes inheriting from abstract classes MUST override all abstract methods
  * Classes inheriting from abstract classes CAN optionally override virtual methods

* Final Classes

  * Final classes cannot be inherited from

* Interfaces

  * All methods in interfaces are abstract by default
  * Interfaces cannot contain fields
  * Classes implementing interfaces MUST implement all interface methods
  * Interface implementations MUST be marked as override
  * Interfaces do not declare fields

* Virtual Methods

  * Regular classes can have virtual methods
  * Virtual methods MUST provide default implementations
  * Virtual methods can be public or protected visibility
  * Abstract methods in abstract classes are public by default
  * Interface methods are public by default
  * Virtual methods cannot be defined as final, abstract, static or override

* Abstract Methods
  
  * Abstract methods do not have an implementation
  * Abstract methods can only be in interfaces and abstract classes
  * Abstract methods must be overriden in inheriting or implementing classes
  * Abstract methods cannot be defined as final, virtual, static or override
  * Protected abstract methods are intended to be used only within the class hierarchy and cannot be invoked externally

* Override Methods

  * All method overriding MUST use explicit override keyword
  * Override methods must match the visibility and the function signature of the virtual method being overridden
  * This applies to: interface implementations, abstract method implementations, and virtual method overrides
  * Override methods cannot be defined as virtual or static
  * Override methods may call their base implementation (if one exists) to extend rather than fully replace the inherited behavior.

* Final Methods

  * Final methods can only be applied to methods marked as override
  * Final methods prevent further overriding in the inheritance chain
  * Final methods must provide their own implementation
  * Final methods cannot be defined as virtual, abstract or static

* Static Methods

  * Static methods are NOT allowed in abstract classes or interfaces
  * Static methods ARE allowed in regular classes
  * Static methods cannot be defined as virtual, abstract, final or override
  * Static methods can have visibility modifiers (public, private, protected)
  * Static methods are separate from the inheritance system
  * Static methods in derived classes may hide static methods with the same name in the base class

* Visibility Rules

  * Interface methods: public by default cannot be protected or private
  * Abstract methods: can be public or protected
  * Virtual methods: can be public or protected
  * Override methods: must match visibility of the method being overridden
  * Static methods: can be public, private, or protected

  ---
