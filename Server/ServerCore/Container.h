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

//typename Pred = less<KeyType>의 의미는 keytype에 대한 기본적인 대소비교를 통한 정렬predicate를 사용한다는 뜻.
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
using Queue = queue<Type, Container>;//que랑 stack은 커스텀 알로케이터타입을 사용하려면 직접 커스텀알로케이터를
//넣어주는 것이 아니라, 커스텀알로케이터가 할당 되어있는 기본 deque라는 컨테이너를 넣어주어야 한다.

template <typename Type, typename Container = Deque<Type>>
using Stack = stack<Type, Container>;

template <typename Type, typename Container = Vector<Type>,typename Pred = less<typename Container::value_type>>
using PriorityQueue = priority_queue<Type, Container, Pred>;

//using String = basic_string<char, char_traits<char>, StlAllocator<char>>;

using String = basic_string<wchar_t, char_traits<wchar_t>, StlAllocator<wchar_t>>;

