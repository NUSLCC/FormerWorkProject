/*
 * @file SyncServer.hpp
 * @copyright Copyright (C) 2016-2022 Flexiv Ltd. All Rights Reserved.
 */
#ifndef FLEXIVRDK_SYNCSERVER_HPP_
#define FLEXIVRDK_SYNCSERVER_HPP_

// Kostal header files
#include <kostal/SystemParams.h>
#include <kostal/KostalLogger.hpp>
#include <kostal/JsonParser.hpp>

namespace kostal {

    typedef boost::asio::ip::tcp::acceptor acc;
    typedef boost::asio::ip::tcp::endpoint end;
    typedef boost::asio::ip::tcp::socket soc;

    class Server{
        protected:
            std::unique_ptr<soc> m_socketPtr;
            std::unique_ptr<acc> m_acceptorPtr;

        private:
            kostal::Log m_log;
            kostal::JSONMessageHandler m_parser;
            boost::asio::io_context m_ioContext;
            end m_endpoint;
            unsigned short m_portNumber=g_COMMPORT;
            const std::string m_token = g_TOKEN;
            const int msgLength = g_MSGMAXSIZE;
            std::string m_recvMsg;
            std::string m_replyMsg;
            boost::system::error_code m_ec;
            std::atomic<bool> m_clientConnected = {false};

        public:
            Server() = default;
            virtual ~Server() = default;

            /**
             * @brief Initialize the socket and get the spi config from the first message of client
             * @return Status code
             */
            Status init()
            {
                Status result;
                try
                {
                    // create an endpoint
                    m_endpoint = std::move(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), m_portNumber));
                    m_socketPtr = std::make_unique<boost::asio::ip::tcp::socket>(m_ioContext);
                    m_acceptorPtr.reset(new boost::asio::ip::tcp::acceptor(m_ioContext, m_endpoint));
                    m_log.info("Waiting for client to send token...");
                    m_acceptorPtr->accept(*m_socketPtr);
                    
                    //receive buffer
                    char m_recvBuffer[msgLength];
                    
                    while(true)
                    {
                        size_t read_length = m_socketPtr->read_some(boost::asio::buffer(m_recvBuffer, msgLength), m_ec);
                        if (m_ec == boost::asio::error::eof){
                            m_log.error("===================================================");
                            m_log.error("The connection is closed cleanly by client or timeout");
                            return SOCKET;
                        }else if (m_ec){
                            m_log.error("Socket reads message error: " + m_ec.message());
                            return SOCKET;
                        }         

                        clearMsg();
                        for (int i=0; i<read_length; i++){
                            m_recvMsg += m_recvBuffer[i];
                        }
                        std::string recvToken="";
                        // parse the spi config
                        result = m_parser.parseSPI(&m_recvMsg, &recvToken, &m_log);
                        // Judge whether the received message is matching the token
                        if (recvToken != m_token){
                            m_log.warn("The client is sending an unkown token: " + recvToken);
                            const char wrongreply[6] = "wrong";
                            boost::asio::write(*m_socketPtr, boost::asio::buffer(wrongreply, std::strlen(wrongreply)), m_ec);
                            m_log.info("Waiting for client to resend token...");
                            continue;
                        }
                        m_log.info("The socket initialization is completed");
                        
                        std::string reply = "received";
                        boost::asio::write(*m_socketPtr, boost::asio::buffer(reply, reply.size()), m_ec);

                        if (m_ec == boost::asio::error::eof){
                            m_log.error("===================================================");
                            m_log.error("The connection is closed cleanly by client or timeout");
                            return SOCKET;
                        }else if (m_ec){
                            m_log.error("Socket writes message error: " + m_ec.message());
                            return SOCKET;
                        }
                        
                        m_clientConnected = true;
                        return SUCCESS;
                    }
                }
                catch(std::exception& e){
                    m_log.error(e.what());
                    return SOCKET;
                }
            }

            /**
             * @brief Receive buffer from the established talking session
             * @return Status code
             */
            Status recv()
            {   
                try
                {                
                    // receive buffer
                    char m_recvBuffer[msgLength];
                    size_t read_length = m_socketPtr->read_some(boost::asio::buffer(m_recvBuffer, msgLength), m_ec);
                    if (m_ec == boost::asio::error::eof){
                        m_log.error("===================================================");
                        m_log.error("The connection is closed cleanly by client or timeout");
                        return SOCKET;
                    }else if (m_ec){
                        m_log.error("Socket reads message error: " + m_ec.message());
                        return SOCKET;
                    }
                    
                    clearMsg();
                    for (int i=0; i<read_length;i++){
                        m_recvMsg += m_recvBuffer[i];
                    }
                    //std::cout<<"The recv msg is: "<<m_recvMsg<<std::endl;

                    boost::asio::write(*m_socketPtr, boost::asio::buffer(m_replyMsg, m_replyMsg.size()), m_ec);   
                    if (m_ec == boost::asio::error::eof){
                        m_log.error("===================================================");
                        m_log.error("The connection is closed cleanly by client or timeout");
                        return SOCKET;
                    }
                    else if (m_ec){
                        m_log.error("Socket writes message error: " + m_ec.message());
                        return SOCKET;
                    }
                    
                    return SUCCESS;
                }
                catch (std::exception& e)
                {
                    m_log.error(e.what());
                    return SOCKET;
                }
            }

            /**
             * @brief Check whether the server can not listen to any request from client for 
             * specific seconds, which means timeout interval
             * @return Status code
             */
            Status monitor()
            {
                auto result = std::async(&Server::recv, this);
                std::future_status status = result.wait_for(std::chrono::seconds(g_timeoutInterval));
                if (status == std::future_status::timeout)
                {
                    m_log.error("===================================================");
                    m_log.error("The connection with client is timeout...");
                    this->disconnect();
                    return SOCKET;
                }
                else if (status == std::future_status::ready) 
                {
                    if (result.get()!=SUCCESS){
                        m_log.error("The receiving of client's message fails...");
                        this->disconnect();
                        return SOCKET;
                    }else{
                        return SUCCESS;
                    }
                }
                else{
                    m_log.error("The future status is deferred...");
                    this->disconnect();
                    return SOCKET;
                }
            }

            /**
             * @brief Set the port number of the socket
             */
            void setPortNumber(unsigned short port){
                m_portNumber = port;
            }

            /**
             * @brief Get the port number of the socket
             * @return recv message content
             */
            unsigned short getPortNumber(){
                return m_portNumber;
            }

            /**
             * @brief Get the content of the recv message
             * @return recv message content
             */
            std::string getRecvMsg(){
                if (m_recvMsg.empty()){
                    m_log.warn("The received message is empty");
                }
                return m_recvMsg;
            }

            /**
             * @brief Get the content of the reply message
             * @return reply message content
             */
            std::string getReplyMsg(){
                if (m_replyMsg.empty()){
                    m_log.warn("The reply message is empty");
                }
                return m_replyMsg;
            }

            /**
             * @brief Clear the received message buffer
             */
            void clearMsg(){
                m_recvMsg="";
            }
        
            /**
             * @brief Set the content of the replying message with param reply
             * @param[in] reply 
             */
            void setReplyMsg(std::string reply)
            {
                if (reply.empty()){
                    m_log.error("The server is sending empty response");
                    m_replyMsg="FAULT";
                }
                m_replyMsg = reply;
            }
            
            /**
             * @brief Disconnect the current socket and reset socket and acceptor in asio
             */
            void disconnect()
            {
                // disconnect socket
                if (m_socketPtr->is_open()){
                    m_socketPtr->shutdown(soc::shutdown_both, m_ec);
                }    
                m_socketPtr->close(m_ec);
                if (m_ec){
                    m_log.error(m_ec.message());
                }
                m_socketPtr.reset();
                m_acceptorPtr.reset();
                m_clientConnected = false;
                m_log.error("Flexiv system server closed this connection");
            }
    };

} /* namespace kostal */

#endif /* FLEXIVRDK_SYNCSERVER_HPP_ */