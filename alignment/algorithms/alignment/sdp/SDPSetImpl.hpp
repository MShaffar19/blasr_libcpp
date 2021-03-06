#include <cstdint>
#include <set>

#include <pbdata/Types.h>
#include <alignment/algorithms/alignment/sdp/SDPSet.hpp>

template <typename T>
int SDPSet<T>::size()
{
    return tree.size();
}

/*
 * Remove a fragment f if it exists.
 */
template <typename T>
int SDPSet<T>::Delete(T &f)
{
    typename std::set<T>::iterator it = tree.find(f);
    if (it != tree.end() and (*it) == f) {
        tree.erase(f);
        return 1;
    }
    return 0;
}
/*
 * Insert a fresh copy of f into the set.  If a copy
 * already exists, replace it with this one.
 */
template <typename T>
inline VectorIndex SDPSet<T>::Insert(T &f)
{
    typename SDPSet<T>::Tree::iterator it = tree.find(f);
    if (it != tree.end()) tree.erase(it);
    tree.insert(f);
    return tree.size();
}
/*
   Returns true if there is a value such that value == f
   */

template <typename T>
int SDPSet<T>::Member(T &f)
{
    typename SDPSet<T>::Tree::iterator it = tree.find(f);
    if (it != tree.end()) {
        f = *it;
        return 1;
    }
    return 0;
}

template <typename T>
int SDPSet<T>::Min(T &f)
{
    if (tree.size() == 0) {
        return 0;
    } else {
        f = *(tree.begin());
    }
}

/*
 * Given f, set succ to be the first value greater than f.
 * Return 1 if such a value exists, 0 otherwise.
 */
template <typename T>
int SDPSet<T>::Successor(T &f, T &succ)
{

    //
    // Set succ to the first value > f, if such a value exists.
    //
    if (tree.size() < 2) {
        return 0;
    }
    typename Tree::iterator it = tree.upper_bound(f);

    if (it == tree.end()) return 0;

    succ = *it;
    return 1;
}

/*
 * Given f, set pred to the first value less than f.
 * Returns 1 if such a value exists, 0 otherwise.
 */
template <typename T>
int SDPSet<T>::Predecessor(T &f, T &pred)
{
    //
    // Set pred equal to the largest value <= f.
    // Return 1 if such a value exists, 0 otherwise.
    //
    if (tree.size() == 0) return 0;

    typename Tree::iterator it = tree.find(f);

    if (it != tree.end()) {
        pred = *it;
        return 1;
    }

    it = tree.lower_bound(f);
    if (it != tree.begin()) --it;

    if (f < *it) {
        // No elements less than f exist.
        return 0;
    } else {
        pred = *it;
        return 1;
    }
}
