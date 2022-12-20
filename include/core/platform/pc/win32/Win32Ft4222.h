//
// Created by tu.jungke on 10.11.2022.
//

#ifndef __WIN32FT4222_H__
#define __WIN32FT4222_H__

#include <windows.h>
#include <stdexcept>
#include "core/Error.h"
#include "ftdiimports/ftd2xx.h"
#include "ftdiimports/LibFT4222.h"
#include "Types.h"

// type declarations for the function pointer into ftd2xx.dll
typedef FT_STATUS (__stdcall FT_CreateDeviceInfoList)(LPDWORD lpdwNumDevs);
typedef FT_STATUS (__stdcall FT_GetDeviceInfoList)(FT_DEVICE_LIST_INFO_NODE *pDest, LPDWORD lpdwNumDevs);
typedef FT_STATUS (__stdcall FT_GetDeviceInfoDetail)(DWORD dwIndex, LPDWORD lpdwFlags, LPDWORD lpdwType,
                                                     LPDWORD lpdwID, LPDWORD lpdwLocId, LPVOID lpSerialNumber,
                                                     LPVOID lpDescription, FT_HANDLE *pftHandle);
typedef FT_STATUS (__stdcall FT_OpenEx)(PVOID pArg1, DWORD Flags, FT_HANDLE *pHandle);
typedef FT_STATUS (__stdcall FT_Close)(FT_HANDLE ftHandle);
typedef FT_STATUS (__stdcall FT_SetTimeouts)(FT_HANDLE ftHandle, ULONG readTimeout, ULONG writeTimeout);
typedef FT_STATUS (__stdcall FT_Read)(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToRead, LPDWORD lpdwBytesReturned);
typedef FT_STATUS (__stdcall FT_Write)(FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToWrite, LPDWORD lpdwBytesWritten);
typedef FT_STATUS (__stdcall FT_GetQueueStatus)(FT_HANDLE ftHandle, LPDWORD lpdwBytesReturned);
typedef FT_STATUS (__stdcall FT_GetStatus)(FT_HANDLE ftHandle, LPDWORD lpdwAmountInRxQueue, LPDWORD lpdwAmountInTxQueue, LPDWORD lpdwEventStatus);

typedef FT_STATUS (__stdcall FT_GetDriverVersion)(FT_HANDLE ftHandle, LPDWORD lpdwVersion);
typedef FT_STATUS (__stdcall FT_GetLibraryVersion)(LPDWORD lpdwVersion);

// type declarations for the function pointer into libFT4222.dll
typedef FT4222_STATUS (__stdcall FT_4222_UnInitialize)(FT_HANDLE pHandle);
typedef FT4222_STATUS (__stdcall FT_4222_SetClock)(FT_HANDLE pHandle, FT4222_ClockRate clk);
typedef FT4222_STATUS (__stdcall FT_4222_GetClock)(FT_HANDLE pHandle, FT4222_ClockRate* pClk);
typedef FT4222_STATUS (__stdcall FT_4222_GetVersion)(FT_HANDLE pHandle, FT4222_Version* pVersion);
typedef FT4222_STATUS (__stdcall FT_4222_GetMaxTransferSize)(FT_HANDLE pHandle, uint16* pMaxSize);
typedef FT4222_STATUS (__stdcall FT_4222_GPIO_Init)(FT_HANDLE ftHandle, GPIO_Dir gpioDir[4]);
typedef FT4222_STATUS (__stdcall FT_4222_GPIO_Read)(FT_HANDLE ftHandle, GPIO_Port portNum, BOOL* pValue);
typedef FT4222_STATUS (__stdcall FT_4222_GPIO_Write)(FT_HANDLE ftHandle, GPIO_Port portNum, BOOL bValue);

typedef FT4222_STATUS (__stdcall FT_4222_SPIMaster_Init)(FT_HANDLE ftHandle, FT4222_SPIMode ioLine,
                                                         FT4222_SPIClock clock_div, FT4222_SPICPOL cpol,
                                                         FT4222_SPICPHA cpha, uint8 ssoMap);
typedef FT4222_STATUS (__stdcall FT_4222_SPIMaster_SingleRead)(FT_HANDLE ftHandle, uint8* buffer,
                                                               uint16 bytesToRead, uint16* sizeOfRead,
                                                               BOOL isEndTransaction);
typedef FT4222_STATUS (__stdcall FT_4222_SPIMaster_SingleWrite)(FT_HANDLE ftHandle, uint8* buffer,
                                                                uint16 bytesToWrite, uint16* sizeTransferred,
                                                                BOOL isEndTransaction);
typedef FT4222_STATUS (__stdcall FT_4222_SPIMaster_SingleReadWrite)(FT_HANDLE ftHandle, uint8* readBuffer,
                                                                    uint8* writeBuffer, uint16 sizeToTransfer,
                                                                    uint16* sizeTransferred, BOOL isEndTransaction);
typedef FT4222_STATUS (__stdcall FT_4222_SPISlave_Init)(FT_HANDLE pHandle);
typedef FT4222_STATUS (__stdcall FT_4222_SPISlave_InitEx)(FT_HANDLE pHandle, SPI_SlaveProtocol protocolOpt);
typedef FT4222_STATUS (__stdcall FT_4222_SPISlave_GetRxStatus)(FT_HANDLE pHandle, uint16_t* pRxSize);
typedef FT4222_STATUS (__stdcall FT_4222_SPISlave_Read)(FT_HANDLE ftHandle, uint8* buffer,
                                                        uint16 bytesToRead, uint16* sizeOfRead);
typedef FT4222_STATUS (__stdcall FT_4222_SPISlave_Write)(FT_HANDLE ftHandle, uint8* buffer,
                                                         uint16 bytesToWrite, uint16* sizeTransferred);
typedef FT4222_STATUS (__stdcall FT_4222_SPI_Reset)(FT_HANDLE ftHandle);
typedef FT4222_STATUS (__stdcall FT_4222_SPI_SetDrivingStrength)(FT_HANDLE ftHandle,
                                                                 SPI_DrivingStrength clkStrength,
                                                                 SPI_DrivingStrength ioStrength,
                                                                 SPI_DrivingStrength ssoStrength);

namespace core { namespace platform { namespace pc {
    constexpr char FT4222_A_DESC[] = "FT4222 A";
    constexpr char FT4222_B_DESC[] = "FT4222 B";

    class Win32Ft4222 {
    public:
        struct Error {
            static constexpr core::Error CouldNotOpenDevice = core::Error(0xF7D1FA11, "Could not open FT4222 device.");
            static constexpr core::Error CouldNotSetDeviceClock = core::Error(0xF7D1FA11, "Could not set FT4222 device clock rate.");
            static constexpr core::Error CouldNotInitializeFunction = core::Error(0xF7D1FA11, "Could not initialize FT4222 device function.");
        };

        static auto load() -> core::ErrorOr<Win32Ft4222> {
            auto ftd2xxHandle = TRY(loadLibrary("ftd2xx.dll"));
            auto libFt4222Handle = TRY(loadLibrary("LibFT4222.dll"));
            return Win32Ft4222(std::move(ftd2xxHandle), std::move(libFt4222Handle));
        }

        Win32Ft4222(Win32Ft4222&& other) = default;


        FT_STATUS createDeviceInfoList (LPDWORD lpdwNumDevs) { return ftd2xx_createDeviceInfoList(lpdwNumDevs); }
        FT_STATUS getDeviceInfoList (FT_DEVICE_LIST_INFO_NODE *pDest, LPDWORD lpdwNumDevs) { return ftd2xx_getDeviceInfoList(pDest, lpdwNumDevs); }
        FT_STATUS getDeviceInfoDetail (DWORD dwIndex, LPDWORD lpdwFlags, LPDWORD lpdwType,
                                       LPDWORD lpdwID, LPDWORD lpdwLocId, LPVOID lpSerialNumber,
                                       LPVOID lpDescription, FT_HANDLE *pftHandle) { return ftd2xx_getDeviceInfoDetail(dwIndex, lpdwFlags, lpdwType, lpdwID, lpdwLocId, lpSerialNumber, lpDescription, pftHandle); }
        FT_STATUS openEx (PVOID pArg1, DWORD Flags, FT_HANDLE *pHandle) { return ftd2xx_openEx(pArg1, Flags, pHandle); }
        FT_STATUS close (FT_HANDLE ftHandle) { return ftd2xx_close(ftHandle); }
        FT_STATUS setTimeouts (FT_HANDLE ftHandle, ULONG readTimeout, ULONG writeTimeout) { return ftd2xx_setTimeouts(ftHandle, readTimeout, writeTimeout); }
        FT_STATUS read (FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToRead, LPDWORD lpdwBytesReturned) { return ftd2xx_read(ftHandle, lpBuffer, dwBytesToRead, lpdwBytesReturned); }
        FT_STATUS write (FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD dwBytesToWrite, LPDWORD lpdwBytesWritten) { return ftd2xx_write(ftHandle, lpBuffer, dwBytesToWrite, lpdwBytesWritten); }
        FT_STATUS getQueueStatus (FT_HANDLE ftHandle, LPDWORD lpdwBytesReturned) { return ftd2xx_getQueueStatus(ftHandle, lpdwBytesReturned); }
        FT_STATUS getStatus (FT_HANDLE ftHandle, LPDWORD lpdwAmountInRxQueue, LPDWORD lpdwAmountInTxQueue, LPDWORD lpdwEventStatus) { return ftd2xx_getStatus(ftHandle, lpdwAmountInRxQueue, lpdwAmountInTxQueue, lpdwEventStatus); }
        FT_STATUS getDriverVersion (FT_HANDLE ftHandle, LPDWORD lpdwVersion) { return ftd2xx_getDriverVersion(ftHandle, lpdwVersion); }
        FT_STATUS getLibraryVersion (LPDWORD lpdwVersion) { return ftd2xx_getLibraryVersion(lpdwVersion); }

        FT4222_STATUS unInitialize (FT_HANDLE pHandle) { return ft4222_UnInitialize(pHandle); }
        FT4222_STATUS setClock (FT_HANDLE pHandle, FT4222_ClockRate clk) { return ft4222_setClock(pHandle, clk); }
        FT4222_STATUS getClock  (FT_HANDLE pHandle, FT4222_ClockRate* pClk) { return ft4222_getClock(pHandle, pClk); }
        FT4222_STATUS getVersion (FT_HANDLE pHandle, FT4222_Version* pVersion) { return ft4222_getVersion(pHandle, pVersion); }
        FT4222_STATUS getMaxTransferSize (FT_HANDLE pHandle, uint16* pMaxSize) { return ft4222_GetMaxTransferSize(pHandle, pMaxSize);}
        FT4222_STATUS gpioInit (FT_HANDLE ftHandle, GPIO_Dir gpioDir[4]) { return ft4222_GPIO_Init(ftHandle, gpioDir); }
        FT4222_STATUS gpioRead (FT_HANDLE ftHandle, GPIO_Port portNum, BOOL* pValue) { return ft4222_GPIO_Read(ftHandle, portNum, pValue); }
        FT4222_STATUS gpioWrite (FT_HANDLE ftHandle, GPIO_Port portNum, BOOL bValue) { return ft4222_GPIO_Write(ftHandle, portNum, bValue); }
        FT4222_STATUS spiMasterInit (FT_HANDLE ftHandle, FT4222_SPIMode ioLine,
                                              FT4222_SPIClock clock_div, FT4222_SPICPOL cpol,
                                              FT4222_SPICPHA cpha, uint8 ssoMap) { return ft4222_SPIMaster_Init(ftHandle, ioLine, clock_div, cpol, cpha, ssoMap); }
        FT4222_STATUS spiMasterSingleRead (FT_HANDLE ftHandle, uint8* buffer,
                                                    uint16 bytesToRead, uint16* sizeOfRead,
                                                    BOOL isEndTransaction) { return ft4222_SPIMaster_SingleRead(ftHandle, buffer, bytesToRead, sizeOfRead, isEndTransaction); }
        FT4222_STATUS spiMasterSingleWrite (FT_HANDLE ftHandle, uint8* buffer,
                                                     uint16 bytesToWrite, uint16* sizeTransferred,
                                                     BOOL isEndTransaction) { return ft4222_SPIMaster_SingleWrite(ftHandle, buffer, bytesToWrite, sizeTransferred, isEndTransaction); }
        FT4222_STATUS spiMasterSingleReadWrite (FT_HANDLE ftHandle, uint8* readBuffer,
                                                         uint8* writeBuffer, uint16 sizeToTransfer,
                                                         uint16* sizeTransferred, BOOL isEndTransaction) { return ft4222_SPIMaster_SingleReadWrite(ftHandle, readBuffer, writeBuffer, sizeToTransfer, sizeTransferred, isEndTransaction); }
        FT4222_STATUS spiSlaveInit (FT_HANDLE pHandle) { return ft4222_SPISlave_Init(pHandle); }
        FT4222_STATUS spiSlaveInitEx (FT_HANDLE pHandle, SPI_SlaveProtocol protocolOpt) { return ft4222_SPISlave_InitEx(pHandle, protocolOpt); }
        FT4222_STATUS spiSlaveGetRxStatus (FT_HANDLE pHandle, uint16_t* pRxSize) { return ft4222_SPISlave_GetRxStatus(pHandle, pRxSize); }
        FT4222_STATUS spiSlaveRead (FT_HANDLE ftHandle, uint8* buffer,
                                             uint16 bytesToRead, uint16* sizeOfRead) { return ft4222_SPISlave_Read(ftHandle, buffer, bytesToRead, sizeOfRead); }
        FT4222_STATUS spiSlaveWrite (FT_HANDLE ftHandle, uint8* buffer,
                                              uint16 bytesToWrite, uint16* sizeTransferred) { return ft4222_SPISlave_Write(ftHandle, buffer, bytesToWrite, sizeTransferred); }
        FT4222_STATUS spiReset (FT_HANDLE ftHandle) { return ft4222_SPI_Reset(ftHandle); }
        FT4222_STATUS spiSetDrivingStrength (FT_HANDLE ftHandle,
                                                                 SPI_DrivingStrength clkStrength,
                                                                 SPI_DrivingStrength ioStrength,
                                                                 SPI_DrivingStrength ssoStrength) { return ft4222_SPI_SetDrivingStrength(ftHandle, clkStrength, ioStrength, ssoStrength); }

    private:
        Win32DynamicLibrary _ftd2xxHandle;
        Win32DynamicLibrary _libFt4222Handle;

        Win32Ft4222(Win32DynamicLibrary&& ftd2xxHandle, Win32DynamicLibrary&& libft4222Handle) : _ftd2xxHandle(std::move(ftd2xxHandle)), _libFt4222Handle(std::move(libft4222Handle)) {
            loadApiFunctions();
        }

        auto loadApiFunctions()-> void{
            ftd2xx_createDeviceInfoList = reinterpret_cast<::FT_CreateDeviceInfoList *> (GetProcAddress(_ftd2xxHandle.get(), "FT_CreateDeviceInfoList"));
            ftd2xx_getDeviceInfoList = reinterpret_cast<::FT_GetDeviceInfoList *> (GetProcAddress(_ftd2xxHandle.get(), "FT_GetDeviceInfoList"));
            ftd2xx_getDeviceInfoDetail = reinterpret_cast<::FT_GetDeviceInfoDetail *> (GetProcAddress(_ftd2xxHandle.get(), "FT_GetDeviceInfoDetail"));
            ftd2xx_openEx = reinterpret_cast<::FT_OpenEx *> (GetProcAddress(_ftd2xxHandle.get(), "FT_OpenEx"));
            ftd2xx_close = reinterpret_cast<::FT_Close *> (GetProcAddress(_ftd2xxHandle.get(), "FT_Close"));
            ftd2xx_setTimeouts = reinterpret_cast<::FT_SetTimeouts *> (GetProcAddress(_ftd2xxHandle.get(), "FT_SetTimeouts"));
            ftd2xx_read = reinterpret_cast<::FT_Read *> (GetProcAddress(_ftd2xxHandle.get(), "FT_Read"));
            ftd2xx_write = reinterpret_cast<::FT_Write *> (GetProcAddress(_ftd2xxHandle.get(), "FT_Write"));
            ftd2xx_getQueueStatus = reinterpret_cast<::FT_GetQueueStatus *> (GetProcAddress(_ftd2xxHandle.get(), "FT_GetQueueStatus"));
            ftd2xx_getStatus = reinterpret_cast<::FT_GetStatus *> (GetProcAddress(_ftd2xxHandle.get(), "FT_GetStatus"));
            ftd2xx_getDriverVersion = reinterpret_cast<::FT_GetDriverVersion *> (GetProcAddress(_ftd2xxHandle.get(), "FT_GetDriverVersion"));
            ftd2xx_getLibraryVersion = reinterpret_cast<::FT_GetLibraryVersion *> (GetProcAddress(_ftd2xxHandle.get(), "FT_GetLibraryVersion"));

            ft4222_unInitialize = reinterpret_cast<::FT_4222_UnInitialize *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_UnInitialize"));
            ft4222_setClock = reinterpret_cast<::FT_4222_SetClock *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_SetClock"));
            ft4222_getClock = reinterpret_cast<::FT_4222_GetClock *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_GetClock"));
            ft4222_getVersion = reinterpret_cast<::FT_4222_GetVersion *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_GetVersion"));
            ft4222_GetMaxTransferSize = reinterpret_cast<::FT_4222_GetMaxTransferSize *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_GetMaxTransferSize"));
            ft4222_GPIO_Init = reinterpret_cast<::FT_4222_GPIO_Init *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_GPIO_Init"));
            ft4222_GPIO_Read = reinterpret_cast<::FT_4222_GPIO_Read *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_GPIO_Read"));
            ft4222_GPIO_Write = reinterpret_cast<::FT_4222_GPIO_Write *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_GPIO_Write"));
            ft4222_SPIMaster_Init = reinterpret_cast<::FT_4222_SPIMaster_Init *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_SPIMaster_Init"));
            ft4222_SPIMaster_SingleRead = reinterpret_cast<::FT_4222_SPIMaster_SingleRead *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_SPIMaster_SingleRead"));
            ft4222_SPIMaster_SingleWrite = reinterpret_cast<::FT_4222_SPIMaster_SingleWrite *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_SPIMaster_SingleWrite"));
            ft4222_SPIMaster_SingleReadWrite = reinterpret_cast<::FT_4222_SPIMaster_SingleReadWrite *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_SPIMaster_SingleReadWrite"));
            ft4222_SPISlave_Init = reinterpret_cast<::FT_4222_SPISlave_Init *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_SPISlave_Init"));
            ft4222_SPISlave_InitEx = reinterpret_cast<::FT_4222_SPISlave_InitEx *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_SPISlave_InitEx"));
            ft4222_SPISlave_GetRxStatus = reinterpret_cast<::FT_4222_SPISlave_GetRxStatus *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_SPISlave_GetRxStatus"));
            ft4222_SPISlave_Read = reinterpret_cast<::FT_4222_SPISlave_Read *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_SPISlave_Read"));
            ft4222_SPISlave_Write = reinterpret_cast<::FT_4222_SPISlave_Write *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_SPISlave_Write"));
            ft4222_UnInitialize = reinterpret_cast<::FT_4222_UnInitialize *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_UnInitialize"));
            ft4222_SPI_Reset = reinterpret_cast<::FT_4222_SPI_Reset *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_SPI_Reset"));
            ft4222_SPI_SetDrivingStrength = reinterpret_cast<::FT_4222_SPI_SetDrivingStrength *> (GetProcAddress(_libFt4222Handle.get(), "FT4222_SPI_SetDrivingStrength"));
        }

        // function pointer into ftd2xx.dll
        ::FT_CreateDeviceInfoList *ftd2xx_createDeviceInfoList;
        ::FT_GetDeviceInfoList *ftd2xx_getDeviceInfoList;
        ::FT_GetDeviceInfoDetail *ftd2xx_getDeviceInfoDetail;
        ::FT_OpenEx *ftd2xx_openEx;
        ::FT_Close *ftd2xx_close;
        ::FT_SetTimeouts *ftd2xx_setTimeouts;
        ::FT_Read *ftd2xx_read;
        ::FT_Write *ftd2xx_write;
        ::FT_GetQueueStatus *ftd2xx_getQueueStatus;
        ::FT_GetStatus *ftd2xx_getStatus;
        ::FT_GetDriverVersion *ftd2xx_getDriverVersion;
        ::FT_GetLibraryVersion *ftd2xx_getLibraryVersion;

        // function pointer into LibFT4222.dll
        ::FT_4222_UnInitialize *ft4222_unInitialize;
        ::FT_4222_SetClock *ft4222_setClock;
        ::FT_4222_GetClock *ft4222_getClock;
        ::FT_4222_GetVersion *ft4222_getVersion;
        ::FT_4222_GetMaxTransferSize *ft4222_GetMaxTransferSize;
        ::FT_4222_GPIO_Init *ft4222_GPIO_Init;
        ::FT_4222_GPIO_Read *ft4222_GPIO_Read;
        ::FT_4222_GPIO_Write *ft4222_GPIO_Write;
        ::FT_4222_SPIMaster_Init *ft4222_SPIMaster_Init;
        ::FT_4222_SPISlave_InitEx *ft4222_SPISlave_InitEx;
        ::FT_4222_SPIMaster_SingleRead *ft4222_SPIMaster_SingleRead;
        ::FT_4222_SPIMaster_SingleWrite *ft4222_SPIMaster_SingleWrite;
        ::FT_4222_SPIMaster_SingleReadWrite *ft4222_SPIMaster_SingleReadWrite;
        ::FT_4222_SPISlave_Init *ft4222_SPISlave_Init;
        ::FT_4222_SPISlave_GetRxStatus *ft4222_SPISlave_GetRxStatus;
        ::FT_4222_SPISlave_Read *ft4222_SPISlave_Read;
        ::FT_4222_SPISlave_Write *ft4222_SPISlave_Write;
        ::FT_4222_UnInitialize *ft4222_UnInitialize;
        ::FT_4222_SPI_Reset *ft4222_SPI_Reset;
        ::FT_4222_SPI_SetDrivingStrength *ft4222_SPI_SetDrivingStrength;
    };
}}}
#endif //__WIN32FT4222_H__
