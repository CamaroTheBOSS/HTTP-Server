#pragma once
#include <mutex>
#include <memory>
#include <vector>
#include <optional>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue {
public:
	ThreadSafeQueue() :
		head(std::make_unique<Node>(T{}, nullptr)) {
		tail = head.get();
	}

	void push(T&& entry) {
		auto newNode = std::make_unique<Node>(T{}, nullptr);
		{
			std::lock_guard lock{ tailLock };
			tail->data = std::move(entry);
			Node* newTail = newNode.get();
			tail->next = std::move(newNode);
			tail = newTail;
		}
		conditionVariable.notify_one();
	}

	T waitForObject() {
		std::unique_lock lock{ conditionLock };
		conditionVariable.wait(lock, [&] { return head.get() != getTail(); });
		return popHead();
	}

	std::optional<T> tryPop() {
		std::lock_guard lock{ headLock };
		if (head.get() == getTail()) {
			return {};
		}
		return std::make_optional(popHead());
	}

private:
	struct Node {
		T data;
		std::unique_ptr<Node> next;
	};

	Node* getTail() {
		std::lock_guard lock{ tailLock };
		return tail;
	}

	T popHead() {
		std::unique_ptr<Node> oldHead = std::move(head);
		head = std::move(oldHead->next);
		return std::move(oldHead->data);
	}

	Node* tail;
	std::unique_ptr<Node> head;
	std::mutex tailLock, headLock, conditionLock;
	std::condition_variable conditionVariable;
};