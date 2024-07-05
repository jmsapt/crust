# Crust
C implementation of useful Rust/C++ datastructures.

TODO list:
[ ] Make buffers lazily allocated (rn they are allocated upon creation)
[x] Vector
[x] Dequeue
[x] HashMap (C++ std::unordered\_map)
[ ] BTreeMap (C++ std::map)

## Vector
- Ammortized `O(1)` to insert, with a mean insertion time of ~6.6 nanoseconds to both generate and insert
a random integer into the vector
