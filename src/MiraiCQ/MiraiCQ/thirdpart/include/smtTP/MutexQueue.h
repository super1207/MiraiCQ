#pragma once
#ifndef _MUTEX_QUEUE_H
#define _MUTEX_QUEUE_H

#include <mutex>
#include <queue>
#include <iostream>
#include <functional>

using namespace std;

template <typename T>
class MutexQueue{
private:
	queue<T> q;
	mutex mutex_q;

public:
	MutexQueue(){}
	MutexQueue(MutexQueue& q){}
	~MutexQueue(){}
	bool isEmpty();
	int size();
	void enqueue(T& t);
	bool dequeue(T& t);
};

#endif
