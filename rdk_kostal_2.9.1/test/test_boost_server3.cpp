
#include <iostream>
#include <thread>
#include <future>
#include <chrono>

int myThread1()
{
	std::cout << "myThread()  START..." << "   thread_id:" << std::this_thread::get_id() << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	std::cout << "myThread()  END..." << "   thread_id:" << std::this_thread::get_id() << std::endl;
	return 5;
}

int main()
{
	std::cout << "main thread id: " << std::this_thread::get_id() << std::endl;
	std::future<int>result = std::async(myThread1);
	std::cout << "continue..." << std::endl;

	//枚举类型
	std::future_status status = result.wait_for(std::chrono::seconds(1)); //设置等待3s,根据设置等待时间与子线程执行消耗时间得到返回值。决定程序在当前行阻塞时间。
	if (status == std::future_status::timeout) //子线程还没执行完
	{
		std::cout << "timeout..." << std::endl;
		std::cout << result.get() <<std::endl;
    std::cout << "----" << std::endl;
	}
	else if (status == std::future_status::ready) //子线程已执行完
	{
		std::cout << "ready..." << std::endl;
		std::cout << result.get() << std::endl;
    std::cout << "****" << std::endl;
	}
	
	return 0;
}
