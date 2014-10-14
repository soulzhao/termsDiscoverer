#ifndef __PATRICIA_TRIE_H
#define __PATRICIA_TRIE_H

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <list>
#include <vector>
#include <string>
#include <bitset>
#include <functional>
#include <algorithm>
#include <windows.h>

using std::list;
using std::vector;
using namespace std;

//将单字节char*转化为宽字节wchar_t*
wchar_t* char2wchar(const char* szStr)
{
	int nLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0);
	if (nLen == 0)
	{
		return NULL;
	}
	wchar_t* pResult = new wchar_t[nLen];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen);
	return pResult;
}

//将宽字节wchar_t*转化为单字节char*
char* wchar2char(const wchar_t* szStr)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
	if (nLen == 0)
	{
		return NULL;
	}
	char* pResult = new char[nLen];
	WideCharToMultiByte(CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL);
	return pResult;
}

//wchar_t* char2wchar(const char* src){
//	size_t len = strlen(src) + 1;
//	size_t converted = 0;
//	wchar_t* dst = (wchar_t*)malloc(len*sizeof(wchar_t));
//	setlocale(LC_ALL, "zh-CN");
//	mbstowcs_s(&converted, dst, len, src, _TRUNCATE);
//	return dst;
//}
//
//char* wchar2char(const wchar_t* src){
//	size_t len = wcslen(src) + 1;
//	size_t converted = 0;
//	char *dst = (char*)malloc(len*sizeof(char) * 2);
//	setlocale(LC_ALL, "zh-CN");
//	wcstombs_s(&converted, dst, len, src, _TRUNCATE);
//	return dst;
//}

template <class S, class T> class PatriciaTrie;
//----------------------------------------------------------------------------
//
// Class:   PatriciaTrieNode
// Purpose: A node in a PATRICIA trie.
//          Each node stores one key, and the data associated with that key.
//
//----------------------------------------------------------------------------
template <class S, class T>
class PatriciaTrieNode {

private:

	friend class			PatriciaTrie<S, T>;
	int						bit_index;					// compare bit
	S						key;						// We can use the the word it self as the key
	vector<T>*				data;						// We can use the position of the word in the sentence as the data
	int						frequency;					//
	PatriciaTrieNode<S, T>*	left;
	PatriciaTrieNode<S, T>*	right;

	string					type;						// use this to record the word's type

public:

	int						visited;					// used to help the travel

	// Constructors & destructor
	PatriciaTrieNode();
	PatriciaTrieNode(S, T, int, PatriciaTrieNode<S, T>*, PatriciaTrieNode<S, T>*);
	virtual					~PatriciaTrieNode();

	bool dataExists(T val){
		bool res = false;
		for (int i = 0; i < data->size(); i++){
			if (data->at(i) == val) {
				res = true;
				break;
			}
		}
		return res;
	}
	// Name:    Initialize
	// Args:    key, data, left, right
	// Return:  void
	// Purpose: Initialize this node with the given data.
	void					Initialize(S, T, int, string, PatriciaTrieNode<S, T>*, PatriciaTrieNode<S, T>*);

	// Name:	GetData/SetData
	// Args:	data : T
	// Return:	T | bool
	// Purpose:	Accessors for the data field.
	vector<T>*              GetData();
	//bool                  SetData(T);
	int						GetFrequency();
	string					GetType();

	PatriciaTrieNode<S, T>* getEmptyNode(S k, T d);

	//static bool CheckDataList(list<T> d1, list<T> d2){
	//	bool res = true;
	//	for (int i = 0; i < d1.size(); i++){
	//		if (d1[i] != d2[i]){
	//			res = false;
	//			break;
	//		}
	//	}
	//	return res;
	//}

	// Name:	GetKey
	// Args:	none
	// Return:	S
	// Purpose:	Getter for the key field.
	S						GetKey();

	// Name:	GetLeft/GetRight
	// Args:	none
	// Return:	PatriciaTrieNode*
	// Purpose:	Getters for the left/right fields.
	PatriciaTrieNode<S, T>*	GetLeft();
	PatriciaTrieNode<S, T>*	GetRight();

};

template <class S, class T>
class WordPosNode{
public:
	int pos;
	PatriciaTrieNode<S, T> * node;

	WordPosNode(int p, PatriciaTrieNode<S, T> * pNode){
		pos = p;
		node = pNode;
	}

	bool operator <(const WordPosNode& rhs) const   //升序排序时必须写的函数
	{
		return pos < rhs.pos;
	}
	bool operator >(const WordPosNode& rhs)  const   //降序排序时必须写的函数
	{
		return pos > rhs.pos;
	}
};

//----------------------------------------------------------------------------
//
// Class:   PatriciaTrie
// Purpose: Implements a PATRICIA trie structure with keys of
//          type S and values of type T
//
//----------------------------------------------------------------------------
template <class S, class T>
class PatriciaTrie {
private:

	vector<WordPosNode<S, T>> posList;

	void					recursive_remove(PatriciaTrieNode<S, T>*);
	int						bit_get(S, int);
	int						bit_first_different(S, S);
	bool					key_compare(S, S);
	void					key_copy(PatriciaTrieNode<S, T>*, PatriciaTrieNode<S, T>*);
	PatriciaTrieNode<S, T>*	find_bitindex_node(PatriciaTrieNode<S, T>* root, int bitindex);
	PatriciaTrieNode<S, T>*	head;
	void tree_inner_preOrder_iterate(PatriciaTrieNode<S, T>* root, string &cnt);

	static int compareUnit(WordPosNode<S, T> v1, WordPosNode<S, T> v2);
	void SortPosList();
	int						isPosLstSorted;

	vector<string> GetNWords(string word, int n, bool left);

public:

	// Constructor and destructor
	PatriciaTrie();
	virtual					~PatriciaTrie();

	// Name:	Insert(key, data)
	// Args:	key : S, data : T
	// Return:	PatriciaTrieNode*
	// Purpose:	Insert a new key+data pair in the Patricia structure, and
	//          return the new node.
	virtual	PatriciaTrieNode<S, T>*	Insert(S, T, string);

	// Name:	Exists(key)
	// Args:	key : S
	// Return:	bool
	// Purpose:	Test if a node with the given key exists.
	virtual bool			Exists(S);

	// Name:	Lookup(key)
	// Args:	key : S
	// Return:	T
	// Purpose:	Search for the given key, and return the data associated
	//          with it.
	virtual vector<T>*			Lookup(S);

	// Name:	LookupNode(key)
	// Args:	key : S
	// Return:	T
	// Purpose:	Search for the given key, and return the node that
	//          contains it (or NULL).
	virtual PatriciaTrieNode<S, T>*	LookupNode(S);

	// Name:	Delete(key)
	// Args:	key : S
	// Return:	bool
	// Purpose:	Remove the node containing the given key. Return
	//          true if the operation succeeded, false otherwise.
	virtual bool			Delete(S);

	// Name:	NODE_LIST
	// Purpose	a list of patricia nodes
	typedef typename list<PatriciaTrieNode<S, T>*>	NODE_LIST;
	typedef typename NODE_LIST::iterator			NODE_LIST_ITERATOR;

	// Name:	GetNodeList
	// Args:	PatriciaTrieNode<S,T>*
	// Return:	NODE_LIST
	// Purpose:	Get all the nodes from the tree
	virtual NODE_LIST		GetNodeList(PatriciaTrieNode<S, T>* root = NULL);

	// Name:	LookupGroup
	// Args:	S
	// Return:	PatriciaTrieNode<S,T>*
	// Purpose:	Get the node that has the longest prefix with the given key
	//			this is the potential father node if we would insert the given node
	virtual PatriciaTrieNode<S, T>*	LookupGroup(S);

	// to do!!
	string GetPatTreeAsString();
	//static PatriciaTrie<S, T>* GetPatTreeFromString(string str);

	// Name:	Copy
	// Return:	PatriciaTrie<S,T>*
	// Purpose: Copy the complete trie
	virtual PatriciaTrie<S, T>*		Copy(PatriciaTrieNode<S, T>* root = NULL);

	vector<string> GetLeftWords(string word);
	vector<string> GetLeftNWords(string word, int n);

	vector<string> GetRightWords(string word);
	vector<string> GetRightNWords(string word, int n);
};

//----------------------------------------------------------------------------
template <class S, class T>
PatriciaTrieNode<S, T>::PatriciaTrieNode() {
	Initialize(S(), T(), -1, "", this, this);
}

//----------------------------------------------------------------------------
template <class S, class T>
PatriciaTrieNode<S, T>::PatriciaTrieNode(S k,
	T d,
	int bi,
	PatriciaTrieNode<S, T>* l,
	PatriciaTrieNode<S, T>* r) {
	Initialize(k, d, bi, l, r);
}

//----------------------------------------------------------------------------
template <class S, class T>
void PatriciaTrieNode<S, T>::Initialize(S k,
	T d,
	int bi,
	string t,
	PatriciaTrieNode<S, T>* l,
	PatriciaTrieNode<S, T>* r) {

	key = k;
	data = new vector<T>();
	data->insert(data->begin(), d);
	left = l;
	right = r;
	bit_index = bi;
	frequency = 1;

	type = t;

	visited = 0;
}

//----------------------------------------------------------------------------
template <class S, class T>
PatriciaTrieNode<S, T>::~PatriciaTrieNode() {
}

//----------------------------------------------------------------------------
template <class S, class T>
vector<T>* PatriciaTrieNode<S, T>::GetData() {
	return data;
}

template <class S, class T>
int PatriciaTrieNode<S, T>::GetFrequency() {
	return frequency;
}

//----------------------------------------------------------------------------
//template <class S, class T>
//bool PatriciaTrieNode<S, T>::SetData(T d) {
//	data = d;
//	return true;
//}

//----------------------------------------------------------------------------
template <class S, class T>
S PatriciaTrieNode<S, T>::GetKey() {
	return key;
}

//----------------------------------------------------------------------------
template <class S, class T>
PatriciaTrieNode<S, T>* PatriciaTrieNode<S, T>::GetLeft() {
	return left;
}

//----------------------------------------------------------------------------
template <class S, class T>
PatriciaTrieNode<S, T>* PatriciaTrieNode<S, T>::GetRight() {
	return right;
}

//----------------------------------------------------------------------------
template <class S, class T>
PatriciaTrie<S, T>::PatriciaTrie() {
	// Create the head of the structure. The head is never moved
	// around in the trie (i.e. it always stays at the top of the structure).
	// This prevents further complications having to do with node removal.
	head = new PatriciaTrieNode<S, T>();
	head->key = S();
}

//----------------------------------------------------------------------------
template <class S, class T>
PatriciaTrie<S, T>::~PatriciaTrie() {
	posList.clear();
	recursive_remove(head);
}

//----------------------------------------------------------------------------
template <class S, class T>
PatriciaTrieNode<S, T>* PatriciaTrie<S, T>::Insert(S k, T d, string type) {

	PatriciaTrieNode<S, T> *p, *t, *x;

	// Start at the root
	p = head;
	t = (PatriciaTrieNode<S, T>*)(p->right);//?

	// Navigate down the tree and look for the key
	while (p->bit_index < t->bit_index) {
		p = t;
		t = (PatriciaTrieNode<S, T>*)(bit_get(k, t->bit_index) ? t->right : t->left);
	}

	// Is the key already in the tree?
	if (key_compare(k, t->key)){
		t->frequency++;
		if (!t->dataExists(d)){
			t->data->insert(t->data->end(), d);

			// insert into the posList
			WordPosNode<S, T> wPos(d, t);
			posList.push_back(wPos);
			isPosLstSorted = 0;
		}
		return NULL; // Already in the tree!
	}

	// Find the first bit that does not match.
	int i = bit_first_different(k, t->key);

	// Find the appropriate place in the tree where
	// the node has to be inserted
	p = head;
	x = (PatriciaTrieNode<S, T>*)(p->right); // ?
	while ((p->bit_index < x->bit_index) &&
		(x->bit_index < i))			{
		p = x;
		x = (PatriciaTrieNode<S, T>*)(bit_get(k, x->bit_index) ? x->right : x->left);
	}

	// Allocate a new node and initialize it.
	t = new PatriciaTrieNode<S, T>();
	t->Initialize(k, d, i, type,(bit_get(k, i) ? x : t), (bit_get(k, i) ? t : x));
	WordPosNode<S, T> wPos(d, t);
	posList.push_back(wPos);
	isPosLstSorted = 0;

	// Rewire
	if (bit_get(k, p->bit_index))
		p->right = t;
	else
		p->left = t;

	// Return the newly created node
	return t;
}

template <class S, class T>
PatriciaTrieNode<S, T>* getEmptyNode(S k, T d){
	PatriciaTrieNode<S, T> *t = new PatriciaTrieNode<S, T>();
	t->Initialize(k, d, -1, t, t);
	return t;
}

////----------------------------------------------------------------------------
//template <class S, class T>
//PatriciaTrieNode<S, T>* PatriciaTrie<S, T>::Insert(S k, T d) {
//
//	PatriciaTrieNode<S, T> *p, *t, *x;
//
//	// Start at the root
//	p = head;
//	t = (PatriciaTrieNode<S, T>*)(p->right);//?
//
//	// Navigate down the tree and look for the key
//	while (p->bit_index < t->bit_index) {
//		p = t;
//		t = (PatriciaTrieNode<S, T>*)(bit_get(k, t->bit_index) ? t->right : t->left);
//	}
//
//	// Is the key already in the tree?
//	if (key_compare(k, t->key))
//		t->frequency++;
//		if (!t->dataExists(d))
//			t->data->insert(t->data->end(), d);
//		return NULL; // Already in the tree!
//
//	// Find the first bit that does not match.
//	int i = bit_first_different(k, t->key);
//
//	// Find the appropriate place in the tree where
//	// the node has to be inserted
//	p = head;
//	x = (PatriciaTrieNode<S, T>*)(p->right); // ?
//	while ((p->bit_index < x->bit_index) &&
//		(x->bit_index < i))			{
//		p = x;
//		x = (PatriciaTrieNode<S, T>*)(bit_get(k, x->bit_index) ? x->right : x->left);
//	}
//
//	// Allocate a new node and initialize it.
//	t = new PatriciaTrieNode<S, T>();
//	t->Initialize(k, d, i, (bit_get(k, i) ? x : t), (bit_get(k, i) ? t : x));
//
//	// Rewire
//	if (bit_get(k, p->bit_index))
//		p->right = t;
//	else
//		p->left = t;
//
//	// Return the newly created node
//	return t;
//}

//----------------------------------------------------------------------------
template <class S, class T>
bool PatriciaTrie<S, T>::Exists(S k) {

	// Lookup the node
	PatriciaTrieNode<S, T>* node = LookupNode(k);
	return (node != NULL);
}

//----------------------------------------------------------------------------
template <class S, class T>
vector<T>* PatriciaTrie<S, T>::Lookup(S k) {

	// Lookup the node
	PatriciaTrieNode<S, T>* node = LookupNode(k);

	// Failed?
	if (!node){
		return NULL;
	}

	// Return the data stored in this node
	return node->data;
}

//----------------------------------------------------------------------------
template <class S, class T>
PatriciaTrieNode<S, T>* PatriciaTrie<S, T>::LookupNode(S k) {

	PatriciaTrieNode<S, T>* p;
	PatriciaTrieNode<S, T>* x;

	// Start at the root.
	p = head;
	x = (PatriciaTrieNode<S, T>*)(head->right);

	// Go down the Patricia structure until an upward
	// link is encountered.
	while (p->bit_index < x->bit_index) {
		p = x;
		x = (PatriciaTrieNode<S, T>*)(bit_get(k, x->bit_index) ? x->right : x->left);
	}

	// Perform a comparison, and return NULL if
	// the key is not found at this location in the structure.
	if (!key_compare(k, x->key))
		return NULL;

	// Return the node
	return x;
}

//----------------------------------------------------------------------------
template <class S, class T>
bool PatriciaTrie<S, T>::Delete(S k) {

	PatriciaTrieNode<S, T> *p, *t, *x, *pp = NULL, *lp;
	int bp, bl, br;
	S key;

	// Start at the root
	p = head;
	t = (PatriciaTrieNode<S, T>*)(p->right);

	// Navigate down the tree and look for the key
	while (p->bit_index < t->bit_index) {
		pp = p;
		p = t;
		t = (PatriciaTrieNode<S, T>*)(bit_get(k, t->bit_index) ? t->right : t->left);
	}

	// Is the key in the tree? If not, get out!
	if (!key_compare(k, t->key))
		return false; // The key could not be found!

	// Copy p's key to t
	if (t != p)
		key_copy(p, t);

	// Is p a leaf?
	bp = p->bit_index;
	bl = ((PatriciaTrieNode<S, T>*)(p->left))->bit_index;
	br = ((PatriciaTrieNode<S, T>*)(p->right))->bit_index;

	if ((bl > bp) || (br > bp)) {

		// There is at least one downward edge.

		if (p != t) {

			// Look for a new (intermediate) key
			key = S(p->key);

			lp = p;
			x = (PatriciaTrieNode<S, T>*)(bit_get(key, p->bit_index) ? p->right : p->left);

			while (lp->bit_index < x->bit_index) {
				lp = x;
				x = (PatriciaTrieNode<S, T>*)(bit_get(key, x->bit_index) ? x->right : x->left);
			}

			// If the intermediate key was not found, we have a problem..
			if (!key_compare(key, x->key)) {
				return false; // The key could not be found!
			}

			// Rewire the leaf (lp) to point to t
			if (bit_get(key, lp->bit_index))
				lp->right = t;
			else
				lp->left = t;

		}

		// Rewire the parent to point to the real child of p
		if (pp != p) {
			PatriciaTrieNode<S, T>* ch = (PatriciaTrieNode<S, T>*)(bit_get(k, p->bit_index) ? p->left : p->right);
			if (bit_get(k, pp->bit_index))
				pp->right = ch;
			else
				pp->left = ch;

		}

	}
	else {

		// Both edges (left, right) are pointing upwards or to the node (self-edges).

		// Rewire the parent
		if (pp != p) {
			PatriciaTrieNode<S, T>* blx = (PatriciaTrieNode<S, T>*)(p->left);
			PatriciaTrieNode<S, T>* brx = (PatriciaTrieNode<S, T>*)(p->right);

			PatriciaTrieNode<S, T>* item = (((blx == brx) && (blx == p)) ? pp : ((blx == p) ? brx : blx));

			if (bit_get(k, pp->bit_index))
				pp->right = item;
			else
				pp->left = item;

		}

	}

	// Deallocate p (no longer needed)
	delete p;

	// Success!
	return true;
}

//----------------------------------------------------------------------------
template <class S, class T>
void PatriciaTrie<S, T>::recursive_remove(PatriciaTrieNode<S, T>* root) {

	if (root == NULL) return;

	PatriciaTrieNode<S, T>* l = (PatriciaTrieNode<S, T>*)root->left;
	PatriciaTrieNode<S, T>* r = (PatriciaTrieNode<S, T>*)root->right;

	// Remove the left branch
	if ((l->bit_index >= root->bit_index) && (l != root) && (l != head))
		recursive_remove(l);

	// Remove the right branch
	if ((r->bit_index >= root->bit_index) && (r != root) && (r != head))
		recursive_remove(r);

	//remove data array
	root->data->clear();
	delete root->data;
	// Remove the root
	delete root;
}

//----------------------------------------------------------------------------
template <class S, class T>
int PatriciaTrie<S, T>::bit_get(S bit_stream, int n) {

	if (n < 0) return 2; // "pseudo-bit" with a value of 2.

	// get the value of the n-th bit from the bit_stream
	// return 0 or 1

	unsigned int	byteno = n / 8;
	unsigned int	bitno = 8 - 1 - (n % 8);
	unsigned char	bitmask = 0x01 << bitno;
	unsigned char*	pnt = (unsigned char*)&bit_stream;

	pnt += byteno;
	int ret = ((*pnt) & bitmask) >> bitno; // protection%power

	assert(ret == 0 || ret == 1);
	return ret;
}

//--------------	--------------------------------------------------------------
template <class S, class T>
bool PatriciaTrie<S, T>::key_compare(S k1, S k2) {
	return (k1 == k2);
}

//----------------------------------------------------------------------------
template <class S, class T>
int PatriciaTrie<S, T>::bit_first_different(S k1, S k2) {
	int d = 0;
	while (bit_get(k1, d) == bit_get(k2, d))
		d++;
	return d;
}

//template <class S, class T>
//int PatriciaTrie<S, T>::bit_first_different(S k1, S k2) {
//
//	int n = 0;
//	int d = 0;
//
//	unsigned char* pnt_k1 = (unsigned char*)&k1;
//	unsigned char* pnt_k2 = (unsigned char*)&k2;
//
//	while (*pnt_k1 == *pnt_k2 && n < sizeof(S)){
//		pnt_k1++;
//		pnt_k2++;
//		n++;
//	}
//
//	S* sobj1 = (S*)pnt_k1;
//	S* sobj2 = (S*)pnt_k2;
//
//	while (bit_get(*sobj1, d) == bit_get(*sobj2, d))
//		d++;
//
//	return ((n << 3) + d);
//}

//----------------------------------------------------------------------------
template <class S, class T>
void PatriciaTrie<S, T>::key_copy(PatriciaTrieNode<S, T>* src, PatriciaTrieNode<S, T>* dest) {
	dest->key = src->key;
	dest->data->clear();
	dest->data->insert(dest->data->begin(), src->data->begin(), src->data->end());
}

//----------------------------------------------------------------------------
template <class S, class T>
typename PatriciaTrie<S, T>::NODE_LIST PatriciaTrie<S, T>::GetNodeList(PatriciaTrieNode<S, T>* root) {

	NODE_LIST				retNodes;
	NODE_LIST				newItemsLeft, newItemsRight;
	PatriciaTrieNode<S, T>*	startNode = (root == NULL ? head : root);

	//
	// insert the current node if it is not the root node
	//

	if (startNode->bit_index >= 0)
		retNodes.insert(retNodes.end(), startNode);

	//
	// walk down left wing
	//

	if (startNode->left != NULL && startNode->left->bit_index > startNode->bit_index)
		newItemsLeft = GetNodeList(startNode->left);

	retNodes.insert(retNodes.end(), newItemsLeft.begin(), newItemsLeft.end());

	//
	// walk down right wing
	//

	if (startNode->right != NULL && startNode->right->bit_index > startNode->bit_index)
		newItemsRight = GetNodeList(startNode->right);

	retNodes.insert(retNodes.end(), newItemsRight.begin(), newItemsRight.end());

	return retNodes;
}

//----------------------------------------------------------------------------
template <class S, class T>
PatriciaTrieNode<S, T>* PatriciaTrie<S, T>::LookupGroup(S k) {

	PatriciaTrieNode<S, T> *p, *t;

	p = head;
	t = (PatriciaTrieNode<S, T>*)(p->right);

	while (p->bit_index < t->bit_index) {
		p = t;
		t = (PatriciaTrieNode<S, T>*)(bit_get(k, t->bit_index) ? t->right : t->left);
	}

	return t;
}

template <class S, class T>
void PatriciaTrie<S, T>::tree_inner_preOrder_iterate(PatriciaTrieNode<S, T>* root, string &cnt){
	if (root == NULL || root->bit_index == -1 || root->visited){
		//cnt.append("#");
		return;
	}

	string t;
	char* buf = (char*) malloc(sizeof(char) * 10);
	sprintf(buf, "%d", root->bit_index);
	t.append("(");
	t.append(buf);
	t.append(",");
	t.append(root->key);
	t.append(",");
	t.append(root->type);
	t.append(",");
	sprintf(buf, "%d", root->frequency);
	t.append(buf);
	t.append(",");
	t.append("[");
	int size = root->data->size();
	for (size_t i = 0; i < size; i++)
	{
		sprintf(buf, "%d", root->data->at(i));
		t.append(buf);
		if (i != size - 1)
			t.append(",");
	}
	t.append("]");
	t.append(")\n");

	cnt.append(t);
	delete[] buf;

	root->visited = 1;

	if (root != root->left)
		tree_inner_preOrder_iterate(root->left, cnt);
	if (root != root->right)
		tree_inner_preOrder_iterate(root->right, cnt);
}

template <class S, class T>
string PatriciaTrie<S, T>::GetPatTreeAsString(){
	string cnt;
	tree_inner_preOrder_iterate(head->right, cnt);
	return cnt;
}

template <class S, class T>
int PatriciaTrie<S, T>::compareUnit(WordPosNode<S, T> v1, WordPosNode<S, T> v2)//注意：本函数的参数的类型一定要与vector中元素的类型一致
{
	return v1.pos - v2.pos;
}

template <class S, class T>
void PatriciaTrie<S, T>::SortPosList(){
	if (!isPosLstSorted){
		sort(posList.begin(), posList.end(), less<WordPosNode<S, T>>());
		isPosLstSorted = 1;
	}
}

template <class S, class T>
vector<string> PatriciaTrie<S, T>::GetLeftWords(string word){
	return GetLeftNWords(word, 1);
}

template <class S, class T>
struct wNodeFindByPos : public std::binary_function <WordPosNode<S, T>, int, bool> {
	bool operator()(const WordPosNode<S, T>& wn1, int pos) const
	{
		return (wn1.pos == pos);
	}
};

template <class S, class T>
vector<string> PatriciaTrie<S, T>::GetNWords(string word, int c, bool left){
	vector<string> wLst;
	PatriciaTrieNode<S, T>* node = LookupGroup(word);
	SortPosList();
	if (node->bit_index != -1){
		for (int i = 0; i < node->data->size(); i++){
			int pos = node->data->at(i);
			vector<WordPosNode<S, T>>::iterator iter = find_if(posList.begin(), posList.end(), bind2nd(wNodeFindByPos<S, T>(), pos));
			if (iter == posList.end()) continue; // not found
			int n = c;
			while (n){
				if (left){
					if (n <= iter - posList.begin()){
						vector<WordPosNode<S, T>>::iterator nPIter = iter - n;
						wLst.push_back(nPIter->node->key);
					}
				}
				else{
					if (n <= posList.end() - iter){
						vector<WordPosNode<S, T>>::iterator nPIter = iter + n;
						wLst.push_back(nPIter->node->key);
					}
				}
				n--;
			}
		}
	}
	return wLst;
}

template <class S, class T>
vector<string> PatriciaTrie<S, T>::GetLeftNWords(string word, int n){
	return GetNWords(word, n, TRUE);
}

template <class S, class T>
vector<string> PatriciaTrie<S, T>::GetRightWords(string word){
	return GetRightNWords(word, 1);
}

template <class S, class T>
vector<string> PatriciaTrie<S, T>::GetRightNWords(string word, int n){
	return GetNWords(word, n, FALSE);
}


template <class S, class T>
PatriciaTrie<S, T>* PatriciaTrie<S, T>::Copy(PatriciaTrieNode<S, T>* root) {

	PatriciaTrieNode<S, T>* newNode = new PatriciaTrieNode<S, T>();
	bool recursiveHead = (root == NULL);
	if (root == NULL) root = head;

	newNode->bit_index = root->bit_index;
	newNode->data->clear();
	newNode->data->insert(newNode->data->begin(), root->data->begin(), root->data->end());
	newNode->key = root->key;

	//
	// create the subtrees and wire my children to it
	// or (if there are no subtrees) wire the children to myself
	// later we will wire the children correctly with some backedges
	//

	if (root->left != NULL && root->left->bit_index > root->bit_index) {
		PatriciaTrie<S, T>* leftTrie = Copy(root->left);
		newNode->left = leftTrie->head;
		leftTrie->head = NULL;
		delete leftTrie;
	}
	else {
		newNode->left = newNode;
	}

	if (root->right != NULL && root->right->bit_index > root->bit_index) {
		PatriciaTrie<S, T>* rightTrie = Copy(root->right);
		newNode->right = rightTrie->head;
		rightTrie->head = NULL;
		delete rightTrie;
	}
	else {
		newNode->right = newNode;
	}

	PatriciaTrie<S, T>* newTrie = new PatriciaTrie<S, T>();
	newTrie->head = newNode;

	//
	// need to adapt the back edges
	//

	if (recursiveHead) {

		NODE_LIST origNodes = GetNodeList();
		NODE_LIST newNodes = newTrie->GetNodeList();

		assert(origNodes.size() == newNodes.size());

		NODE_LIST_ITERATOR iorigBegin = origNodes.begin();
		NODE_LIST_ITERATOR iorigEnd = origNodes.end();
		NODE_LIST_ITERATOR inewBegin = newNodes.begin();
		NODE_LIST_ITERATOR inewEnd = newNodes.end();

		for (; iorigBegin != iorigEnd && inewBegin != inewEnd; iorigBegin++, inewBegin++) {

			PatriciaTrieNode<S, T>* origNode = *iorigBegin;
			PatriciaTrieNode<S, T>* newNode = *inewBegin;

			//
			// make some checks that everything went fine
			//
			assert(origNode != newNode				&&
				origNode->key == newNode->key			&&
				origNode->bit_index == newNode->bit_index
				);

			//
			// if the original node has backedges:
			// find the correct backedge node in the new trie
			// and wire the children correctly
			//

			if (origNode->left->bit_index < origNode->bit_index)
				newNode->left = newTrie->find_bitindex_node(newTrie->head, origNode->left->bit_index);

			if (origNode->right->bit_index < origNode->bit_index)
				newNode->right = newTrie->find_bitindex_node(newTrie->head, origNode->right->bit_index);

		} // for ( ; iorigBegin != iorigEnd && inewBegin != inewEnd; iorigBegin++, inewBegin++)	

	} // if (recursiveHead)

	return newTrie;
}

template <class S, class T>
PatriciaTrieNode<S, T>* PatriciaTrie<S, T>::find_bitindex_node(PatriciaTrieNode<S, T>* root, int bitindex) {

	//
	// is the head-pseudo-node requested?
	//

	if (bitindex == -1)
		return head;

	//
	// check all the normal nodes
	//

	NODE_LIST nodes = GetNodeList(root);

	NODE_LIST_ITERATOR i = nodes.begin();
	NODE_LIST_ITERATOR iend = nodes.end();

	for (; i != iend; i++) {
		PatriciaTrieNode<S, T>* node = *i;
		if (node->bit_index == bitindex) return node;
	}

	return NULL;
}

//----------------------------------------------------------------------------
#endif // __PATRICIA_TRIE_H
