/*
 * @file SystemParams.h
 * @copyright Copyright (C) 2016-2022 Flexiv Ltd. All Rights Reserved.
 */

#ifndef FLEXIVRDK_SYSTEMPARAMS_H_
#define FLEXIVRDK_SYSTEMPARAMS_H_

// Flexiv header files
#include <flexiv/Exception.hpp>
#include <flexiv/Gripper.hpp>
#include <flexiv/Log.hpp>
#include <flexiv/Mode.hpp>
#include <flexiv/Model.hpp>
#include <flexiv/Robot.hpp>
#include <flexiv/Scheduler.hpp>
#include <flexiv/StatesData.hpp>
#include <flexiv/Visualization.hpp>

// std header files
#include <iostream>
#include <string>
#include <cstring>
#include <mutex>
#include <list>
#include <vector>
#include <array>
#include <algorithm>
#include <thread>
#include <iomanip>
#include <fstream>
#include <future>
#include "math.h"

// third-party lib files
#include <jsoncpp/json/json.h> // sudo apt-get install libjsoncpp-dev
#include <boost/asio.hpp> // sudo apt-get install libboost-dev
#include <boost/thread.hpp>
#include <boost/asio/thread_pool.hpp>
#include "spdlog/spdlog.h" // sudo apt-get install libspdlog-dev

// Communication param
const unsigned short g_COMMPORT = 6060; // The port number of destination host
std::string ROBOTADDRESS = "127.0.0.1";
std::string LOCALADDRESS = "127.0.0.1";
const int g_MSGMAXSIZE=1024;
const std::string g_TOKEN = "kostal";
struct timeval timeo = {10, 0};
const std::string UPLOADADDRESS = "/home/ftp/"; // The file stored location

//spi config
int g_CPHA=1;
int g_CPOL=0;
int g_LSB=0;
int g_SelPol=0;

// The global variant status shows the status of the system, 0 means success, 1-4 means errors in different periods
enum Status{SUCCESS, SOCKET, JSON, ROBOT, SPI, CSV, FTP, SYSTEM};
std::mutex g_kostalDataMutex;

// The status of the flexiv server
enum serverStatus{INIT, IDLE, BUSY, FAULT};

// Testman 's message constants
const char* cobotStatus   = "COBOT_STATUS"; // read cobot current status 
const char* processBias   = "PROCESS_BIAS"; // bias mode 
const char* processNormal = "PROCESS_NORMAL"; // normal mode
const char* dutStatus     = "DUT_STATUS_READY"; // work piece status
const char* variantCode   = "VARIANT_CODE"; // work piece variant number
const char* planCode      = "PLAN_CODE"; // associated plan name
const char* taskStatus    = "TASK_STATUS"; // the status of the whole task

// SPI config key that testman will send
const std::string CPOL   = "CPOL";
const std::string CPHA   = "CPHA";
const std::string LSB    = "LSB";
const std::string SELP   = "SELP";
const std::string TOKEN  = "TOKEN";

// Task msg keythat testman will send
const std::string QUERYSTATUS    = "TM_FLEXIV_QUERY_STATUS"; // TRUE FALSE
const std::string TASKTYPE       = "TM_FLEXIV_TASK_TYPE"; //NORMAL BIAS DUMMY
const std::string TASKNAME       = "TM_FLEXIV_TASK_NAME"; // xxplan

// Key that flexiv system will response
const std::string SYSTEMSTATUS   = "FLEXIV_TM_STATUS"; // IDLE BUSY FAULT

// Whether the node data should be collected or not
std::atomic<bool> g_dataCollectFlag = {false};

// Whether the whole collecting logic should be used or not
std::atomic<bool> g_collectSwitch = {false};

// Connection timeout interval after first handshake with Testman, unit is second
int64_t g_timeoutInterval = 5;

#endif