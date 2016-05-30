#ifndef LOCK_FREE_STACK_H__
#define LOCK_FREE_STACK_H__

#include <atomic>
#include <memory>
#include <exception>

namespace lockFree {

template <typename V>
class stack {
public:
	stack() : tail(nullptr), head(nullptr) {
		head.store(&tail);
	}
	~stack()  = default;
	struct stackElem {
		V v;
		stackElem *next;
		stackElem(V value, stackElem *nextElem) : v(value), next(nextElem) { }
		stackElem(stackElem *nextElem) : next(nextElem) { }

	};
	void push(V v) {
		stackElem *elem = new stackElem(v, head.load());

		while (nullptr == elem->next || !head.compare_exchange_weak(elem->next, elem))
			elem->next = head.load();

	}
	V pop() {
		stackElem *elem = head.load();
		while (nullptr == elem || !head.compare_exchange_weak(elem, nullptr)) {
			elem = head.load();
		}
		if (elem == &tail) {
			head.store(elem);
			throw std::runtime_error("empty");
		}

		head.store(elem->next);
		std::unique_ptr<stackElem> toDelete(elem);
		return toDelete->v;
	}
	bool isEmpty() { return head.load() == &tail; }
private:
	stackElem tail;
	std::atomic<stackElem *> head;
};

}
#endif
