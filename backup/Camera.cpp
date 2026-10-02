#include "Camera.h"
#include "MvCameraControl.h"
#include <iostream>
#include <cstdlib>
Camera::Camera()
{
    m_handle = nullptr;
    m_isGrabbing=false;
}
Camera::~Camera()
{
    stop();
    close();
}
bool Camera::open()
{
    MV_CC_DEVICE_INFO_LIST stDeviceList;
    int nRet = MV_CC_EnumDevices(
        MV_GIGE_DEVICE | MV_USB_DEVICE,
        &stDeviceList
    );
    if(MV_OK != nRet)
    {
        return false;
    }
    std::cout << "Device count;"
              << stDeviceList.nDeviceNum
              << std::endl;
    if(stDeviceList.nDeviceNum == 0)
    {
        std::cout << "No camera found" << std::endl;
        return false;
    }
    int nIndex = 0;
    nRet = MV_CC_CreateHandle(
        &m_handle,
        stDeviceList.pDeviceInfo[nIndex]
    );
    if(MV_OK != nRet)
    {
        std::cout<<"CreateHandle failed!"<< std::endl;
        return false;
    }
    nRet = MV_CC_OpenDevice(m_handle);

    if (MV_OK != nRet)
    {
        std::cout << "OpenDevice failed!" << std::endl;
        MV_CC_DestroyHandle(m_handle);
        m_handle=nullptr;
        return false;
    }
    return true;
}
bool Camera::start()
{
    int nRet=MV_CC_StartGrabbing(m_handle);
    if(MV_OK!=nRet)
    {
        std::cout<<"StartGrabbing failed"<<std::endl;
        return false;
    }
    m_isGrabbing=true;
    return true;
}

bool Camera::getFrame(cv::Mat& image)
{
    int nDataSize = 10 * 1024 * 1024;

    unsigned char* pData =
        (unsigned char*)malloc(nDataSize);

    if (pData == nullptr)
    {
        std::cout << "Buffer allocation failed!" << std::endl;
        return false;
    }


    MV_FRAME_OUT_INFO_EX stImageInfo = {0};


    int nRet = MV_CC_GetOneFrameTimeout(
        m_handle,
        pData,
        nDataSize,
        &stImageInfo,
        1000
    );


    if (MV_OK != nRet)
    {
        std::cout << "GetOneFrame failed!" << std::endl;

        free(pData);

        return false;
    }
    if (stImageInfo.enPixelType == PixelType_Gvsp_Mono8)
    {

        cv::Mat temp(
            stImageInfo.nHeight,
            stImageInfo.nWidth,
            CV_8UC1,
            pData
        );


        image = temp.clone();
    }
    else if (stImageInfo.enPixelType == PixelType_Gvsp_BayerRG8)
    {

        cv::Mat rawImage(
            stImageInfo.nHeight,
            stImageInfo.nWidth,
            CV_8UC1,
            pData
        );


        cv::cvtColor(
            rawImage,
            image,
            cv::COLOR_BayerRG2BGR
        );

    }


    else
    {
        std::cout 
            << "Unsupported Pixel Format!"
            << std::endl;


        free(pData);

        return false;
    }



    free(pData);


    std::cout << "Frame received!" << std::endl;

    std::cout << "Width: "
              << stImageInfo.nWidth
              << std::endl;


    std::cout << "Height: "
              << stImageInfo.nHeight
              << std::endl;


    std::cout << "Pixel Type: "
              << stImageInfo.enPixelType
              << std::endl;


    return true;
}

bool Camera::stop()
{

    if(!m_isGrabbing)
    {
        return true;
    }


    int nRet = MV_CC_StopGrabbing(m_handle);


    if(nRet != MV_OK)
    {
        std::cout
        << "Stop grabbing failed!"
        << std::endl;

        return false;
    }


    m_isGrabbing = false;


    return true;
}

bool Camera::close()
{

    if(m_handle == nullptr)
    {
        return true;
    }


    MV_CC_CloseDevice(m_handle);


    MV_CC_DestroyHandle(m_handle);


    m_handle = nullptr;


    return true;
}