#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
using namespace boost::asio;
using namespace boost::posix_time;
io_service g_service;

struct talk_to_client;
typedef boost::shared_ptr<talk_to_client> client_ptr;
typedef std::vector<client_ptr> clientArray;
clientArray clients; // a vector filled with client_ptr

// thread-safe access to clients array
boost::recursive_mutex rm;

void update_clients_changed();

struct talk_to_client {
    talk_to_client() : m_socket(g_service), isStarted(false), alreadyRead(0) {
        last_ping = microsec_clock::local_time();
    }
    
    // ask for username
    std::string username() const { 
        return userName; 
    }

    // reply to client, need 1st read request and 2nd process request
    void answer_to_client() {
        try {
            //std::cout<<"start read request"<<std::endl;
            read_request();
            //std::cout<<"start process request"<<std::endl;
            process_request();
        } catch ( boost::system::system_error&) {
            stop();
        }
        // if the interval is larger than 5000
        if (timed_out()) {
            stop();
            std::cout << "stopping " << userName << " due to timeout" << std::endl;
        }
    }

    // put true to isClientsChanged
    void set_clients_changed() { 
        isClientsChanged = true; 
    }

    // get m_socket 
    ip::tcp::socket & sock() { 
        return m_socket; 
    }
    
    // return whether pass 5 seconds
    bool timed_out() const {
        ptime now = microsec_clock::local_time();
        long long ms = (now - last_ping).total_milliseconds();
        return ms > 5000 ;
    }

    // close client connection
    void stop() {
        boost::system::error_code err;
        m_socket.close(err);
    }

private:
    // read some buffer from m_socket
    void read_request() {
        if (m_socket.available()){
            std::cout<<"alreadyRead is : "<<alreadyRead<<std::endl;
            alreadyRead += m_socket.read_some(buffer(buff + alreadyRead, max_msg - alreadyRead));
            std::cout<<"the buff is : "<<buff<<std::endl;
            std::cout<<"alreadyRead is : "<<alreadyRead<<std::endl;
        } 
    }

    // find enter 
    void process_request() {
        bool found_enter = std::find(buff, buff + alreadyRead, '\n') < buff + alreadyRead;
        
        //does not find the '\n', return false
        if (!found_enter)
            return; // message is not full
        
        // find the '\n'
        last_ping = microsec_clock::local_time();
        size_t pos = std::find(buff, buff + alreadyRead, '\n') - buff;
        std::string msg(buff, pos);
        std::copy(buff + alreadyRead, buff + max_msg, buff);
        alreadyRead -= pos + 1;

        if (msg.find("login ") == 0) 
            on_login(msg);
        else if (msg.find("ping") == 0) 
            on_ping();
        else if (msg.find("ask_clients") == 0) 
            on_clients();
        else 
            std::cerr << "invalid msg " << msg << std::endl;
    }
    
    // incoming msg: "login", respond login ok
    void on_login(const std::string & msg) {
        std::istringstream in(msg);
        in >> userName >> userName;
        std::cout << userName << " logged in" << std::endl;
        write("login ok\n");
        update_clients_changed();
    }

    // incoming msg: "ping", check client list has been changed or not. yes:"list changed", no:"ping ok" 
    void on_ping() 
    {
        write(isClientsChanged ? "ping client_list_changed\n" : "ping ok\n");
        isClientsChanged = false;
    }

    void on_clients() {
        std::string msg;
        { 
            boost::recursive_mutex::scoped_lock lk(rm);
            for(clientArray::const_iterator b = clients.begin(), e = clients.end() ; b != e; ++b)
            msg += (*b)->username() + " ";
        }
        write("clients " + msg + "\n");
    }


    void write(const std::string & msg) {
        m_socket.write_some(buffer(msg));
    }
private:
    ip::tcp::socket m_socket;
    enum { max_msg = 1024 };
    int alreadyRead;
    char buff[max_msg];
    bool isStarted;
    std::string userName;
    bool isClientsChanged;
    ptime last_ping;
};

// update client status
void update_clients_changed() {
    boost::recursive_mutex::scoped_lock lk(rm);
    for( clientArray::iterator b = clients.begin(); b != clients.end(); ++b)
        (*b)->set_clients_changed();
}


void accept_client_thread() {
    ip::tcp::acceptor acceptor(g_service, ip::tcp::endpoint(ip::tcp::v4(), 6060));
    while (true) {
        client_ptr newClientPtr(new talk_to_client);
        acceptor.accept(newClientPtr->sock());
        boost::recursive_mutex::scoped_lock lk(rm);
        clients.push_back(newClientPtr);
    }
}

void handle_clients_thread() 
{
    while (true) 
    {
        boost::this_thread::sleep(millisec(1));
        boost::recursive_mutex::scoped_lock lk(rm);
        for (auto b = clients.begin(); b != clients.end(); ++b) {
            (*b)->answer_to_client();
        }
        // erase clients that timed out
        clients.erase(std::remove_if(clients.begin(), clients.end(), boost::bind(&talk_to_client::timed_out,_1)), clients.end());
    }
}

int main() {
    boost::thread_group threads;
    threads.create_thread(accept_client_thread);
    threads.create_thread(handle_clients_thread);
    threads.join_all();
}