/*
 * @file JsonParser.hpp
 * @copyright Copyright (C) 2016-2022 Flexiv Ltd. All Rights Reserved.
 */

#ifndef FLEXIVRDK_PARSER_HPP_
#define FLEXIVRDK_PARSER_HPP_

// Kostal header files
#include <kostal/KostalLogger.hpp>
#include <kostal/SystemParams.h>

namespace kostal {

    /**
     * @class JSONMessageHandler
     * @brief Base class for message to jsonalize or unjsonalize
     */
    class JSONMessageHandler
    {
    private:
        Json::Reader m_jsonReader;
        Json::Value m_jsonRecvValue;
        Json::Value m_jsonSendValue;
        Json::StyledWriter m_jsonStyledWriter;
        
    public:
        JSONMessageHandler() = default;
        virtual ~JSONMessageHandler() = default;

        /**
         * @brief Parse the received message and take out key value for SPI initialization
         * @param[in] recvMsg the received message that will be parsed
         * @param[out] recvToken the received token parsed from recvMsg
         * @param[in] logPtr kostal's log pointer
         * @return Status code
         */
        Status parseSPI(std::string* recvMsg, std::string* recvToken, kostal::Log* logPtr)
        {
            // if the received message is null
            if (recvMsg == nullptr || recvMsg->size() == 0){
                logPtr->error("The received json message is empty");
                return JSON;
            }
            // if the json reader is failed
            bool result;
            result = m_jsonReader.parse(*recvMsg, m_jsonRecvValue);
            if (!result){
                logPtr->error("The received message is not json format");
                return JSON;
            }
            // Check token first 
            if (m_jsonRecvValue.isMember(TOKEN.c_str())==0){
                logPtr->error("The received json message does not have initialization param: " + TOKEN);
                return JSON;
            }
            // Check whether other json has associated spi params
            if (m_jsonRecvValue.isMember(CPOL.c_str())==0){
                logPtr->error("The received json message does not have SPI param: " + CPOL);
                return JSON;
            }
            if (m_jsonRecvValue.isMember(CPHA.c_str())==0){
                logPtr->error("The received json message does not have SPI param: " + CPHA);
                return JSON;
            }
            if (m_jsonRecvValue.isMember(LSB.c_str())==0){
                logPtr->error("The received json message does not have SPI param: " + LSB);
                return JSON;
            }
            if (m_jsonRecvValue.isMember(SELP.c_str())==0){
                logPtr->error("The received json message does not have SPI param: " + SELP);
                return JSON;
            }
            // Retrieve the key value from init json message
            g_CPOL = std::stoi(m_jsonRecvValue[CPOL].asCString());
            g_CPHA = std::stoi(m_jsonRecvValue[CPHA].asCString());
            g_LSB = std::stoi(m_jsonRecvValue[LSB].asCString());
            g_SelPol = std::stoi(m_jsonRecvValue[SELP].asCString());
            *recvToken = m_jsonRecvValue[TOKEN].asString();

            return SUCCESS;
        }
        
        /**
         * @brief Parse the received message and take out key value
         * @param[in] recvMsg the received message that will be parsed
         * @param[out] queryStatus whether client want to query status or not
         * @param[in] logPtr kostal's log pointer
         * @return Status code
         */
        Status parseJSON(std::string* recvMsg, 
                        std::string* queryStatus, 
                        kostal::Log* logPtr)
        {
            // if the received message is null
            if (recvMsg == nullptr || recvMsg->size() == 0){
                logPtr->error("The received json message is empty");
                return JSON;
            }
            // if the json reader is failed
            bool result;
            result = m_jsonReader.parse(*recvMsg, m_jsonRecvValue);
            if (!result){
                logPtr->error("The received message is not json format");
                return JSON;
            }
            
            // Check whether json has associated members
            if (m_jsonRecvValue.isMember(QUERYSTATUS.c_str())==0){
                logPtr->error("The received json message does not have param: " + QUERYSTATUS);
                return JSON;
            }
            
            // Try to retrieve the key value from json
            *queryStatus = m_jsonRecvValue[QUERYSTATUS].asString();

            logPtr->info("****************************************************");
            logPtr->info("Check Message received!");
            logPtr->info("The received queryStatus is: " + *queryStatus);
            
            return SUCCESS;
        }

        /**
         * @brief Parse the received message and take out key value
         * @param[in] recvMsg the received message that will be parsed
         * @param[out] queryStatus whether client want to query or not
         * @param[out] taskType the name of the work plan in message: NORMAL BIAS DUMMY
         * @param[out] taskName the name of the task: workplan name
         * @param[in] logPtr kostal's log pointer
         * @return Status code
         */
        Status parseJSON(std::string* recvMsg, 
                         std::string* queryStatus, 
                         std::string* taskType,
                         std::string* taskName, 
                         kostal::Log* logPtr)
        {
            // if the received message is null
            if (recvMsg == nullptr || recvMsg->size() == 0){
                logPtr->error("The received json message is empty");
                return JSON;
            }
            // if the json reader is failed
            bool result;
            result = m_jsonReader.parse(*recvMsg, m_jsonRecvValue);
            if (!result){
                logPtr->error("The received message is not json format");
                return JSON;
            }
            
            // Check whether json has associated members
            if (m_jsonRecvValue.isMember(QUERYSTATUS.c_str())==0){
                logPtr->error("The received json message does not have param: " + QUERYSTATUS);
                return JSON;
            }
            if (m_jsonRecvValue.isMember(TASKTYPE.c_str())==0){
                logPtr->error("The received json message does not have param: " + TASKTYPE);
                return JSON;
            }
            if (m_jsonRecvValue.isMember(TASKNAME.c_str())==0){
                logPtr->error("The received json message does not have param: " + TASKNAME);
                return JSON;
            }
            
            // Try to retrieve the key value from json
            *queryStatus = m_jsonRecvValue[QUERYSTATUS].asString();
            *taskType = m_jsonRecvValue[TASKTYPE].asString();
            *taskName = m_jsonRecvValue[TASKNAME].asString();

            logPtr->info("Task message received!");
            logPtr->info("The received queryStatus is: " + *queryStatus);
            logPtr->info("The received taskType is: " + *taskType);
            logPtr->info("The received taskName is: " + *taskName);
            logPtr->info("****************************************************");
            
            return SUCCESS;
        }

        /**
         * @brief This function read current JSON and print it on the screen
         * @param[in] logPtr kostal's log pointer
         * @return Status code
         */
        Status readSelfJSON(kostal::Log* logPtr)
        {
            std::string recvValue = m_jsonStyledWriter.write(m_jsonRecvValue);
            std::string sendValue = m_jsonStyledWriter.write(m_jsonSendValue);
            // if the the json file is empty, answer is "null"(size is 5)
            if (recvValue.size()==5 || recvValue.size()==0){
                logPtr->warn("The received json is empty");
                return JSON;
            }else if(sendValue.size()==5 || sendValue.size()==0){
                logPtr->error("The sent json is empty");
                return JSON;
            }else{
                logPtr->info("The receive json is: "+recvValue);
                logPtr->info("The sent json is: "+sendValue);
            }

            return SUCCESS;
        }

        /**
         * @brief  JSON message before sending
         * @param[in] systemStatus the flexiv system status: IDLE BUSY FAULT
         * @param[out] sendMessage the message will be sent after jsonalizing
         * @param[in] logPtr kostal's log pointer
         * @return Status code
         */
        Status generateSendJSON(std::string* systemStatus,
                                std::string* sendMessage,
                                kostal::Log* logPtr)
        {
            if (systemStatus==nullptr || systemStatus->size()==0){
                logPtr->warn("The system status is empty");
                return JSON;
            }
            m_jsonSendValue[SYSTEMSTATUS] = *systemStatus;
            *sendMessage = m_jsonStyledWriter.write(m_jsonSendValue);
            logPtr->info("The sending json message is: " + *sendMessage);
            return SUCCESS;
        }
    };

} /* namespace kostal */

#endif /* FLEXIVRDK_PARSER_HPP_ */