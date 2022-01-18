#include <smtTP/MutexQueue.h>

using namespace std;
template <typename T> 
bool MutexQueue<T>::isEmpty(){
	unique_lock<mutex> lock(this->mutex_q);
	return q.empty();
}
template <typename T>
int MutexQueue<T>::size(){
	unique_lock<mutex> lock(this->mutex_q);
	return q.size();
}
template <typename T>
void MutexQueue<T>::enqueue(T& t){
	unique_lock<mutex> lock(this->mutex_q);
	q.push(t);
}
template <typename T>
bool MutexQueue<T>::dequeue(T& t){
	unique_lock<mutex> lock(this->mutex_q);
	if(q.empty()){
		return false;	
	}
	t = move(q.front());
	q.pop();
	return true;
}

template class MutexQueue<function<void()>>;
