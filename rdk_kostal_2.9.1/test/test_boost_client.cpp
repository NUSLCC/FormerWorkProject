#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <kostal/SystemParams.h>

using boost::asio::ip::tcp;

enum { max_length = 1024 };

int main(int argc, char* argv[])
{
  try
  {
    //Init
    boost::asio::io_context io_context;
    tcp::socket s1(io_context);
    tcp::resolver resolver(io_context);
    boost::asio::connect(s1, resolver.resolve("127.0.0.1", "6060"));
    std::string input;
    input = "";
    
    // Write
    std::string temp;
    Json::Value config;
    config["CPOL"] = "0";
    config["CPHA"] = "1";
    config["LSB"] = "0";
    config["SELP"] = "0";
    config["TOKEN"] = "kostal";
    temp = Json::FastWriter().write(config);
    std::cout<<"The client's init msg is: "<<temp<<std::endl;
    input = temp;
    const char* request = input.c_str();
    boost::asio::write(s1, boost::asio::buffer(request, std::strlen(request)));

    // Read
    char reply[max_length];
    boost::system::error_code ec;
    size_t read_length = s1.read_some(boost::asio::buffer(reply, max_length), ec);
    std::string m_recvMsg="";
    for (int i=0; i<read_length;i++){
        m_recvMsg+=reply[i];
    }

    std::cout << "The server's reply is: "<<m_recvMsg<<std::endl;
    std::cout << std::endl;
    //std::this_thread::sleep_for(std::chrono::seconds(2));
    // Write
    std::string stringtemp="";
    Json::Value config2;
    config2["TM_FLEXIV_QUERY_STATUS"] = "yes";
    config2["TM_FLEXIV_TASK_TYPE"] = "NORMAL";
    config2["TM_FLEXIV_TASK_NAME"] = "Kostal-MainPlan";
    stringtemp = Json::FastWriter().write(config2);
    std::cout<<"The client task msg is: "<<stringtemp<<std::endl;
    input = stringtemp;
    request = input.c_str();
    int i = 20;
    while (i>0)
    {
      // Write
      boost::asio::write(s1, boost::asio::buffer(request, std::strlen(request)));
      
      // Read
      read_length = s1.read_some(boost::asio::buffer(reply, max_length), ec);
      m_recvMsg="";
      for (int i=0; i<read_length;i++){
          m_recvMsg+=reply[i];
      }
      
      std::cout << "The server's reply is: "<<m_recvMsg<<std::endl;
      std::cout << std::endl;

      int64_t waitTime;
      std::cout<<"Please input wait time:";
      std::cin>>waitTime;
      // End
      std::this_thread::sleep_for(std::chrono::seconds(waitTime));
      i--;
      config2["TM_FLEXIV_QUERY_STATUS"] = "true";
      stringtemp = Json::FastWriter().write(config2);
      input = stringtemp;
      request = input.c_str();
    }

    s1.close();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}