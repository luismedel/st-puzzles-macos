/*
 * tree234.h: header defining functions in tree234.c.
 * 
 * This file is copyright 1999-2001 Simon Tatham.
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL SIMON TATHAM BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef TREE234_H
#define TREE234_H

#include <stdbool.h>

/*
 * This typedef is typically opaque outside tree234.c itself. But you
 * can define TREE234_INTERNALS to get a definition of it and its
 * subsidiary node structure, as long as you're prepared to commit to
 * responding to changes in the internals (which probably means you're
 * tree234.c itself or tree234-test.c).
 */
typedef struct tree234_Tag tree234;

typedef int (*cmpfn234)(void *, void *);

typedef void *(*copyfn234)(void *state, void *element);

#ifdef TREE234_INTERNALS
typedef struct node234_Tag node234;

struct tree234_Tag {
    node234 *root;
    cmpfn234 cmp;
};

struct node234_Tag {
    node234 *parent;
    node234 *kids[4];
    int counts[4];
    void *elems[3];
};

int height234(tree234 *t);
#endif

/*
 * Create a 2-3-4 tree. If `cmp' is NULL, the tree is unsorted, and
 * lookups by key will fail: you can only look things up by numeric
 * index, and you have to use addpos234() and delpos234().
 */
tree234 *newtree234(cmpfn234 cmp);

/*
 * Free a 2-3-4 tree (not including freeing the elements).
 */
void freetree234(tree234 *t);

/*
 * Add an element e to a sorted 2-3-4 tree t. Returns e on success,
 * or if an existing element compares equal, returns that.
 */
void *add234(tree234 *t, void *e);

/*
 * Add an element e to an unsorted 2-3-4 tree t. Returns e on
 * success, NULL on failure. (Failure should only occur if the
 * index is out of range or the tree is sorted.)
 * 
 * Index range can be from 0 to the tree's current element count,
 * inclusive.
 */
void *addpos234(tree234 *t, void *e, int index);

/*
 * Look up the element at a given numeric index in a 2-3-4 tree.
 * Returns NULL if the index is out of range.
 * 
 * One obvious use for this function is in iterating over the whole
 * of a tree (sorted or unsorted):
 * 
 *   for (i = 0; (p = index234(tree, i)) != NULL; i++) consume(p);
 * 
 * or
 * 
 *   int maxcount = count234(tree);
 *   for (i = 0; i < maxcount; i++) {
 *       p = index234(tree, i);
 *       assert(p != NULL);
 *       consume(p);
 *   }
 */
void *index234(tree234 *t, int index);

/*
 * Find an element e in a sorted 2-3-4 tree t. Returns NULL if not
 * found. e is always passed as the first argument to cmp, so cmp
 * can be an asymmetric function if desired. cmp can also be passed
 * as NULL, in which case the compare function from the tree proper
 * will be used.
 * 
 * Three of these functions are special cases of findrelpos234. The
 * non-`pos' variants lack the `index' parameter: if the parameter
 * is present and non-NULL, it must point to an integer variable
 * which will be filled with the numeric index of the returned
 * element.
 * 
 * The non-`rel' variants lack the `relation' parameter. This
 * parameter allows you to specify what relation the element you
 * provide has to the element you're looking for. This parameter
 * can be:
 * 
 *   REL234_EQ     - find only an element that compares equal to e
 *   REL234_LT     - find the greatest element that compares < e
 *   REL234_LE     - find the greatest element that compares <= e
 *   REL234_GT     - find the smallest element that compares > e
 *   REL234_GE     - find the smallest element that compares >= e
 * 
 * Non-`rel' variants assume REL234_EQ.
 * 
 * If `rel' is REL234_GT or REL234_LT, the `e' parameter may be
 * NULL. In this case, REL234_GT will return the smallest element
 * in the tree, and REL234_LT will return the greatest. This gives
 * an alternative means of iterating over a sorted tree, instead of
 * using index234:
 * 
 *   // to loop forwards
 *   for (p = NULL; (p = findrel234(tree, p, NULL, REL234_GT)) != NULL ;)
 *       consume(p);
 * 
 *   // to loop backwards
 *   for (p = NULL; (p = findrel234(tree, p, NULL, REL234_LT)) != NULL ;)
 *       consume(p);
 */
enum {
    REL234_EQ, REL234_LT, REL234_LE, REL234_GT, REL234_GE
};
void *find234(tree234 *t, void *e, cmpfn234 cmp);
void *findrel234(tree234 *t, void *e, cmpfn234 cmp, int relation);
void *findpos234(tree234 *t, void *e, cmpfn234 cmp, int *index);
void *findrelpos234(tree234 *t, void *e, cmpfn234 cmp, int relation,
		    int *index);

/*
 * Delete an element e in a 2-3-4 tree. Does not free the element,
 * merely removes all links to it from the tree nodes.
 * 
 * delpos234 deletes the element at a particular tree index: it
 * works on both sorted and unsorted trees.
 * 
 * del234 deletes the element passed to it, so it only works on
 * sorted trees. (It's equivalent to using findpos234 to determine
 * the index of an element, and then passing that index to
 * delpos234.)
 * 
 * Both functions return a pointer to the element they delete, for
 * the user to free or pass on elsewhere or whatever. If the index
 * is out of range (delpos234) or the element is already not in the
 * tree (del234) then they return NULL.
 */
void *del234(tree234 *t, void *e);
void *delpos234(tree234 *t, int index);

/*
 * Return the total element count of a tree234.
 */
int count234(tree234 *t);

/*
 * Split a tree234 into two valid tree234s.
 * 
 * splitpos234 splits at a given index. If `before' is true, the
 * items at and after that index are left in t and the ones before
 * are returned; if `before' is false, the items before that index
 * are left in t and the rest are returned.
 * 
 * split234 splits at a given key. You can pass any of the
 * relations used with findrel234, except for REL234_EQ. The items
 * in the tree that satisfy the relation are returned; the
 * remainder are left.
 */
tree234 *splitpos234(tree234 *t, int index, bool before);
tree234 *split234(tree234 *t, void *e, cmpfn234 cmp, int rel);

/*
 * Join two tree234s together into a single one.
 * 
 * All the elements in t1 are placed to the left of all the
 * elements in t2. If the trees are sorted, there will be a test to
 * ensure that this satisfies the ordering criterion, and NULL will
 * be returned otherwise. If the trees are unsorted, there is no
 * restriction on the use of join234.
 * 
 * The tree returned is t1 (join234) or t2 (join234r), if the
 * operation is successful.
 */
tree234 *join234(tree234 *t1, tree234 *t2);
tree234 *join234r(tree234 *t1, tree234 *t2);

/*
 * Make a complete copy of a tree234. Element pointers will be
 * reused unless copyfn is non-NULL, in which case it will be used
 * to copy each element. (copyfn takes two `void *' parameters; the
 * first is private state and the second is the element. A simple
 * copy routine probably won't need private state.)
 */
tree234 *copytree234(tree234 *t, copyfn234 copyfn, void *copyfnstate);

#endif /* TREE234_H */
