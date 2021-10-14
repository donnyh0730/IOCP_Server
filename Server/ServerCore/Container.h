#pragma once
#include "Types.h"
#include "Allocator.h"
#include <array>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

using namespace std;

template<typename Type, uint32 Size>
using Array = array<Type, Size>;

template<typename Type>
using Vector = vector<Type, StlAllocator<Type>>;

template<typename Type>
using List = list<Type, StlAllocator<Type>>;

//typename Pred = less<KeyType>�� �ǹ̴� keytype�� ���� �⺻���� ��Һ񱳸� ���� ����predicate�� ����Ѵٴ� ��.
template<typename KeyType,typename ValueType,typename Pred = less<KeyType>>
using Map = map<KeyType, ValueType, Pred, StlAllocator<pair<const KeyType, ValueType>>>;

template <typename KeyType,typename ValueType, typename Hasher = hash<KeyType>, typename KeyEq = equal_to<KeyType>>
using HashMap = unordered_map<KeyType, ValueType, Hasher, KeyEq, StlAllocator<pair<const KeyType, ValueType>>>;

template<typename KeyType,typename Pred = less<KeyType>>
using Set = set<KeyType, Pred, StlAllocator<KeyType>>;

template <typename KeyType, typename Hasher = hash<KeyType>, typename KeyEq = equal_to<KeyType>>
using HashSet = unordered_set<KeyType, Hasher, KeyEq, StlAllocator<KeyType>>;

template <typename Type>
using Deque = deque<Type, StlAllocator<Type>>;

template <typename Type, typename Container = Deque<Type>>
using Queue = queue<Type, Container>;//que�� stack�� Ŀ���� �˷�������Ÿ���� ����Ϸ��� ���� Ŀ���Ҿ˷������͸�
//�־��ִ� ���� �ƴ϶�, Ŀ���Ҿ˷������Ͱ� �Ҵ� �Ǿ��ִ� �⺻ deque��� �����̳ʸ� �־��־�� �Ѵ�.

template <typename Type, typename Container = Deque<Type>>
using Stack = stack<Type, Container>;

template <typename Type, typename Container = Vector<Type>,typename Pred = less<typename Container::value_type>>
using PriorityQueue = priority_queue<Type, Container, Pred>;

//using String = basic_string<char, char_traits<char>, StlAllocator<char>>;

using String = basic_string<wchar_t, char_traits<wchar_t>, StlAllocator<wchar_t>>;

