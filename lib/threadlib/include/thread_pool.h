#pragma once
#include <functional>
#include <vector>
#include <thread>
#include <tuple>
#include <iostream>

#include "thread_safe_queue.h"

class ThreadPool {
public:
	class ICallback {
	public:
		virtual void operator()() const = 0;
	};
	using ICallbackPtr = std::unique_ptr<ICallback>;

	ThreadPool() {
		startThreads(std::thread::hardware_concurrency() - 1);
	}

	ThreadPool(const int nThreads) {
		startThreads(nThreads);
	}

	~ThreadPool() {
		for (int i = 0; i < threads.size(); i++) {
			threads[i].flag.store(true, std::memory_order_release);
			push([] {});
		}
		for (int i = 0; i < threads.size(); i++) {
			if (threads[i].thread.joinable()) {
				threads[i].thread.join();
			}
		}
	}

	template <typename F, typename... Args>
	void push(F&& f, Args&&... args) {
		ICallbackPtr callback = std::make_unique<Callback<F, Args...>>(std::forward<F>(f), std::forward<Args>(args)...);
		queue.push(std::move(callback));
	}

private:
	template <typename F, typename... Args>
	class Callback : public ICallback {
	public:
		static_assert(std::is_invocable_v<F, Args...>);
		Callback(F&& f, Args&&... args) :
			callback(std::move(f)),
			params(std::make_tuple(args...)) {}
		void operator()() const final override {
			std::apply(callback, params);
		}
	private:
		F callback;
		std::tuple<Args...> params;
	};
	struct ThreadWithFlag {
		std::thread thread;
		std::atomic_bool flag;
	};

	void startThreads(const int nThreads) {
		threads = std::vector<ThreadWithFlag>(nThreads);
		for (int i = 0; i < threads.size(); i++) {
			threads[i].thread = std::thread([&, index = i] {
				while (!threads[index].flag.load(std::memory_order_acquire)) {
					auto callback = queue.waitForObject();
					(*callback)();
				}
			});
		}
	}

	ThreadSafeQueue<std::unique_ptr<ICallback>> queue;
	std::vector<ThreadWithFlag> threads;
};