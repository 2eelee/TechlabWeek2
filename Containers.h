#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <Set>
#include <list>


template <typename T>
using TArray = std::vector<T>;

template <typename Keytype, typename Valuetype>
using TMap = std::unordered_map<Keytype, Valuetype>;

template <typename T>
using TQueue = std::queue<T>;

template <typename T>
using TOrderedSet = std::set<T>;

template <typename T>
using TSet = std::unordered_set<T>;

template <typename T>
using TLinkedList = std::list<T>;