# Crust
C implementation of useful Rust/C++ datastructures. 

All implementations are for generic datatypes, however this means that inserting and query the structures should 
be done with care to ensure that only the correct types are being passed to and from the respective functions. This
is because the structures and implemented using sized (but void pointer) buffers, so can't provide type checking.

Data strucutures:
- [x] [Vector](#vector) (stack)
- [x] [Dequeue](#dequeue) (queue)
- [ ] [PriorityQueue](#priorityqueue) (heap)
- [ ] [LinkedList](#linkedlist)
- [ ] [Channels](#channels) (Send & Recv - Rust only)
- [x] [HashMap](#hashmap) (C++ std::unordered\_map)
- [ ] [OrderedMap](#orderedmap) (Rust BTreeMap, C++ std::map)
- [ ] [HashSet](#hashset) (Rust BTreeSet, C++ std::unordered\_set)
- [ ] [OrderedSet](#orderedset) (Rust BTreeSet, C++ std::set)
- [ ] [TriesMap](#triesmap) (Potentially useful)
- [ ] [TriesSet](#triesset) (Potentially useful)

Misc. Tasks:
- [x] Make buffers lazily allocated (rn they are allocated upon creation)
- [x] Hashmap potentially bugged for strings

## Vector
- Ammortized `O(1)` to insert, with a mean insertion time of ~6.6 nanoseconds to both generate and insert
a random integer into the vector

## Dequeue
Todo

## PriorityQueue
Todo

## LinkedList
Todo

## Channels
Todo

## HashMap
Todo

## OrderedMap
Todo

## HashSet
Todo

## OrderedSet
Todo

## TriesMap
Todo

## TriesSet
Todo
