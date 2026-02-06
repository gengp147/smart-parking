//
// Created by gp147 on 10/15/2024.
//

#ifndef FALCONEYEALONE_CIRCULARARRAY_H
#define FALCONEYEALONE_CIRCULARARRAY_H
#include <iostream>
#include <vector>
#include <stdexcept>

template<typename T>
class CircularArray {
public:
	explicit CircularArray(int size) : front(-1), rear(-1), count(0), maxSize(size), baseIndex(0) {
		arr.resize(size);
	}

	void enqueue(const T& value) {
		if (isFull()) {
			dequeue(); // 删除最开始的元素
		}
		rear = (rear + 1) % maxSize;
		arr[rear] = value;
		if (front == -1) {
			front = rear;
		}
		if (count < maxSize) {
			count++;
		} else {
			baseIndex = (baseIndex + 1) % maxSize; // 调整baseIndex
		}
	}

	T dequeue() {
		if (isEmpty()) {
			throw std::out_of_range("Circular array is empty");
		}
		T value = arr[front];
		front = (front + 1) % maxSize;
		count--;
		if (count == 0) {
			front = -1;
			rear = -1;
			baseIndex = 0;
		} else {
			baseIndex = (baseIndex + 1) % maxSize; // 调整baseIndex
		}
		return value;
	}

	T& operator[](int index) {
		if (index < 0 || index >= count) {
			throw std::out_of_range("Index out of range");
		}
		int actualIndex = (baseIndex + index) % maxSize;
		return arr[actualIndex];
	}

	const T& operator[](int index) const {
		if (index < 0 || index >= count) {
			throw std::out_of_range("Index out of range");
		}
		int actualIndex = (baseIndex + index) % maxSize;
		return arr[actualIndex];
	}

	bool isEmpty() const {
		return count == 0;
	}

	bool isFull() const {
		return count == maxSize;
	}

	int size() const {
		return count;
	}

	std::vector<T> readFirstNElements(int N) const {
		if(N < 0) { N = count; }
		else { N = std::min(count, N); }
		std::vector<T> result;
		for (int i = 0; i < N; ++i) {
			result.push_back((*this)[i]);
		}
		return result;
	}

	std::vector<T> readLastNElements(int N) const {
		if (N <= 0) {
			return readFirstNElements(N);
		}

		int numElementsToRead = std::min(count, N);
		std::vector<T> result;
		result.reserve(numElementsToRead);

		int startIndex = (baseIndex + count - numElementsToRead) % maxSize;

		for (int i = 0; i < numElementsToRead; ++i) {
			int actualIndex = (startIndex + i) % maxSize;
			result.push_back(arr[actualIndex]);
		}

		return result;
	}


	void print() const {
		if (isEmpty()) {
			std::cout << "Circular array is empty\n";
			return;
		}
		int tempBaseIndex = baseIndex;
		for (int i = 0; i < count; i++) {
			std::cout << arr[tempBaseIndex] << ",";
			tempBaseIndex = (tempBaseIndex + 1) % maxSize;
		}
		std::cout << std::endl;
	}

private:
	std::vector<T> arr{};
	int front{};
	int rear{};
	int count{};
	int maxSize{};
	int baseIndex{}; // 基础索引，用于跟踪下标0的位置
};

#endif //FALCONEYEALONE_CIRCULARARRAY_H
