//
// Created by gp147 on 7/25/2024.
//

#ifndef FALCONEYEALONE_SINGLETON_H
#define FALCONEYEALONE_SINGLETON_H

#include <cstdlib>
#include <memory>

template < typename T >
class Singleton
{
public:
	static T &GetInstance()
	{
		Init();
		return *instance_;
	}

private:
	static void Init()
	{
		if (instance_ == 0)
		{

			pthread_mutex_lock(&g_mutex);
			if (instance_ == 0)
			{
				instance_ = new T;
				atexit(Destroy);    //程序结束时调用注册的函数
			}
			pthread_mutex_unlock(&g_mutex);
		}
	}

	static void Destroy()
	{
		delete instance_;
	}
public:
	Singleton(const Singleton &other) = delete;
	Singleton &operator=(const Singleton &other) = delete;
	Singleton() = delete;
	~Singleton() = default;

	static T * volatile instance_;
	static pthread_mutex_t g_mutex;
};
template < typename T >
T * volatile Singleton < T >::instance_ = 0;

template < typename T >
pthread_mutex_t Singleton<T> ::g_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif //FALCONEYEALONE_SINGLETON_H
