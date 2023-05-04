#include <iostream>
#include <thread>
//简单日志宏，打印日志及附加的线程ID
#define  LOG(a) std::cout<<"[T"<<std::this_thread::get_id()<<"]"<<a<<"\n"
 
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/strand.hpp>
 
 
class Printer
{
public:
    Printer(boost::asio::io_context& io)
        :m_strand(io), 
        m_timer1(io, boost::posix_time::seconds(1)),
        m_timer2(io, boost::posix_time::seconds(1)),
        m_timer3(io, boost::posix_time::seconds(1)),
        m_count(0)
    {
        m_timer1.async_wait(m_strand.wrap(boost::bind(&Printer::print1, this)));
        m_timer2.async_wait(m_strand.wrap(boost::bind(&Printer::print2, this)));
        m_timer3.async_wait(m_strand.wrap(boost::bind(&Printer::print3, this)));
    }
    ~Printer()
    {
        std::cout << "Final count is " << m_count << "\n";
    }
    void print1()
    {
        if (m_count< 10)
        {
            std::cout << "Timer 1: " << m_count << "\n";
            ++m_count;
            m_timer1.expires_at(m_timer1.expires_at() + boost::posix_time::seconds(1));
            m_timer1.async_wait(m_strand.wrap(boost::bind(&Printer::print1, this)));

        }
    }
    void print2()
    {
        if (m_count < 10)
        {
            std::cout << "Timer 2: " << m_count << "\n";
            ++m_count;
            m_timer2.expires_at(m_timer2.expires_at() + boost::posix_time::seconds(1));
            m_timer2.async_wait(m_strand.wrap(boost::bind(&Printer::print2, this)));
        }
    }
    void print3()
    {
        if (m_count < 10)
        {
            std::cout << "Timer 3: " << m_count << "\n";
            ++m_count;
            m_timer3.expires_at(m_timer3.expires_at() + boost::posix_time::seconds(1));
            m_timer3.async_wait(m_strand.wrap(boost::bind(&Printer::print3, this)));
        }
    }
private:
    boost::asio::io_context::strand m_strand;
    boost::asio::deadline_timer m_timer1;
    boost::asio::deadline_timer m_timer2;
    boost::asio::deadline_timer m_timer3;
    int m_count = 0;
};
 
 
int main()
{
    boost::asio::io_context io;
    Printer p(io);
    std::cout<<"main thread start1"<<std::endl;
    boost::thread t1(boost::bind(&boost::asio::io_service::run, &io));
    boost::thread t2(boost::bind(&boost::asio::io_service::run, &io));
    std::cout<<"main thread start2"<<std::endl;
    io.run();
    std::cout<<"main thread start3"<<std::endl;
    t1.join();
    t2.join();
    std::cout<<"main thread start4"<<std::endl;

    return 0;
}
