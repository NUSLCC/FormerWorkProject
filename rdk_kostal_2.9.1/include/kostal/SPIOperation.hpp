/*
 * @file SPIOperation.hpp
 * @copyright Copyright (C) 2016-2022 Flexiv Ltd. All Rights Reserved.
 */

#ifndef FLEXIVRDK_SPIOPERATION_HPP_
#define FLEXIVRDK_SPIOPERATION_HPP_

// Kostal header files
#include <kostal/KostalStates.hpp>
#include <kostal/SystemParams.h>
#include <kostal/ControlSPI.h>

namespace kostal {

    /**
     * @class SPIOperationHandler
     * @brief Base class for SPI device operations like connect and collect 
     */
    class SPIOperationHandler
    {
    public:
        SPIOperationHandler() = default;
        virtual ~SPIOperationHandler() = default;
        
        Status buildSPIConnectionDummy(){
            return SUCCESS;
        }

        /**
         * @brief Build connection between SPI device and system with socket message
         * @return Status code
         */
        Status buildSPIConnectionSocket()
        {
            flexiv::Log log;
            int ret;
            VSI_INIT_CONFIG SPI_Config;
            // Scan connected device 
            ret = VSI_ScanDevice(1);
            if (ret <= 0){
                log.error("The SPI device can not be found, please check the USB interface!");
                return SPI;
            }
            ret = VSI_OpenDevice(VSI_USBSPI, 0, 0);
            if (ret != ERR_SUCCESS){
                log.error("The SPI device can not be open, please check SPI device!");
                return SPI;
            }
            SPI_Config.ControlMode = 0;
            SPI_Config.MasterMode = 0; // Slave Mode
            SPI_Config.CPHA = g_CPHA; // Clock Polarity and Phase must be same as master
            SPI_Config.CPOL = g_CPOL;
            SPI_Config.LSBFirst = g_LSB;
            SPI_Config.TranBits = 8; // Support 8bit mode only
            SPI_Config.SelPolarity = g_SelPol;
            SPI_Config.ClockSpeed = 1395000;
            ret = VSI_InitSPI(VSI_USBSPI, 0, &SPI_Config);
            if (ret != ERR_SUCCESS) {
                log.error("The SPI device can not be initialized, please check SPI device or use sudo!");
                return SPI;
            }
            return SUCCESS;
        }
        
        /**
         * @brief Build the connection to SPI device with default params
         * @return Status code
         */
        Status buildSPIConnection()
        {
            flexiv::Log log;
            int ret;
            VSI_INIT_CONFIG SPI_Config;
            // Scan connected device 
            ret = VSI_ScanDevice(1);
            if (ret <= 0){
                log.error("The SPI device can not be found, please check the USB interface!");
                return SPI;
            }
            ret = VSI_OpenDevice(VSI_USBSPI, 0, 0);
            if (ret != ERR_SUCCESS){
                log.error("The SPI device can not be open, please check SPI device!");
                return SPI;
            }
            SPI_Config.ControlMode = 0;
            SPI_Config.MasterMode = 0; // Slave Mode
            SPI_Config.CPHA = 1; // Clock Polarity and Phase must be same as master
            SPI_Config.CPOL = 0;
            SPI_Config.LSBFirst = 0;
            SPI_Config.TranBits = 8; // Support 8bit mode only
            SPI_Config.SelPolarity = 0;
            SPI_Config.ClockSpeed = 1395000;
            ret = VSI_InitSPI(VSI_USBSPI, 0, &SPI_Config);
            if (ret != ERR_SUCCESS) {
                log.error("The SPI device can not be initialized, please check SPI device!");
                return SPI;
            }
            return SUCCESS;
        }

        /**
         * @brief Read SPI data from the USB-SPI device, put them into the SPI data list
         * @param[in] logPtr robot's log pointer
         * @param[in] spiDataPtr spi data's pointer
         * @return Status code
         */
        Status collectSPIData(SPIData* spiDataPtr, flexiv::Log* logPtr)
        {
            while(g_collectSwitch)
            {   
                uint8_t read_buffer[10240] = {0};
                int32_t read_data_num = 0;
                int ret = VSI_SlaveReadBytes(VSI_USBSPI, 0, read_buffer, &read_data_num, 2);

                if (ret != ERR_SUCCESS){
                    logPtr->error("The SPI device read data error");
                    return SPI;
                }
                if (read_data_num >0) // filter and only keep data with 16 bytes length
                {
                    std::lock_guard<std::mutex> lock(g_kostalDataMutex);
                    uint8_t SPISensorBuffer[16]= {0};
                    for (int i = 0; i < read_data_num; i++){
                        SPISensorBuffer[i]=read_buffer[i];
                    }
                    *spiDataPtr = SPISensorBuffer;
                }
            }

            return SUCCESS;
        }

        /**
         * @brief Fake Reading SPI data from the USB-SPI device, put them into the SPI data list
         * @param[in] logPtr robot's log pointer
         * @param[in] spiDataPtr spi data's pointer
         * @param[in] spiDataListPtr spi data list's pointer
         * @return Status code
         */
        Status collectSPIDataDummy(SPIData* spiDataPtr, flexiv::Log* logPtr)  
        {
            int ret;
            uint8_t read_buffer[10240] = {0};
            int32_t read_data_num = 16;
            if (read_data_num == 16) // collect only when data is 16 bytes length
            { 
                std::lock_guard<std::mutex> lock(g_kostalDataMutex);
                uint8_t SPISensorBuffer[16]= {0}; // put 0 in buffer
                for (int i = 0; i < 16; i++){
                    SPISensorBuffer[i]=read_buffer[i];
                }
                *spiDataPtr = SPISensorBuffer;
                // // use mutex to lock spi data, store spi data to kostal data 
                // {
                //     std::lock_guard<std::mutex> lock(g_kostalDataMutex);
                //     spiDataListPtr->push_back(*spiDataPtr);
                // }   
            }
            
            return SUCCESS;
        }
    };

} /* namespace kostal */

#endif /* FLEXIVRDK_SPIOPERATION_HPP_ */