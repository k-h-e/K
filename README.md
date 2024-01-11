Re-usable C++ code.

Contains (among other things):
- unified I/O, including networking and serial connections
- distributed event bus
- run loop concept hiding threading intricacies from client code by dispatching all activity on the thread a given run loop is living in

Uses:
- standard C++ threading primitives
- POSIX I/O
