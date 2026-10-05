#include "CameraBackend.h"

#include <MvCameraControl.h>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class MVSBackend final : public CameraBackend
{
public:
    ~MVSBackend() override
    {
        close();
    }

    bool open(int deviceId, std::string& error) override
    {
        close();

        MV_CC_DEVICE_INFO_LIST deviceList{};
        int ret = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &deviceList);
        if (ret != MV_OK) {
            error = sdkError("MV_CC_EnumDevices", ret);
            return false;
        }

        if (deviceList.nDeviceNum == 0) {
            error = "No Hikvision GigE/USB camera was found.";
            return false;
        }

        if (deviceId < 0 || static_cast<unsigned int>(deviceId) >= deviceList.nDeviceNum) {
            error = "Requested device index is out of range. Found "
                  + std::to_string(deviceList.nDeviceNum) + " device(s).";
            return false;
        }

        MV_CC_DEVICE_INFO* selected = deviceList.pDeviceInfo[deviceId];
        if (selected == nullptr) {
            error = "Selected device information is null.";
            return false;
        }

        ret = MV_CC_CreateHandle(&handle_, selected);
        if (ret != MV_OK) {
            handle_ = nullptr;
            error = sdkError("MV_CC_CreateHandle", ret);
            return false;
        }

        ret = MV_CC_OpenDevice(handle_);
        if (ret != MV_OK) {
            error = sdkError("MV_CC_OpenDevice", ret);
            close();
            return false;
        }
        deviceOpened_ = true;

        // Disable trigger mode so the camera can acquire continuously.
        ret = MV_CC_SetEnumValue(handle_, "TriggerMode", MV_TRIGGER_MODE_OFF);
        if (ret != MV_OK) {
            error = sdkError("MV_CC_SetEnumValue(TriggerMode)", ret);
            close();
            return false;
        }

        ret = MV_CC_StartGrabbing(handle_);
        if (ret != MV_OK) {
            error = sdkError("MV_CC_StartGrabbing", ret);
            close();
            return false;
        }
        grabbing_ = true;
        return true;
    }

    bool getImage(cv::Mat& image, std::string& error) override
    {
        if (handle_ == nullptr || !deviceOpened_ || !grabbing_) {
            error = "MVS camera is not grabbing.";
            return false;
        }

        MV_FRAME_OUT frame{};
        int ret = MV_CC_GetImageBuffer(handle_, &frame, 1000);
        if (ret != MV_OK) {
            error = sdkError("MV_CC_GetImageBuffer", ret);
            return false;
        }

        bool converted = false;
        do {
            if (frame.pBufAddr == nullptr || frame.stFrameInfo.nWidth == 0
                || frame.stFrameInfo.nHeight == 0) {
                error = "MVS returned an invalid image buffer.";
                break;
            }

            const unsigned int width = frame.stFrameInfo.nWidth;
            const unsigned int height = frame.stFrameInfo.nHeight;
            const std::uint64_t dstSize64 =
                static_cast<std::uint64_t>(width) * height * 3U;

            if (dstSize64 > static_cast<std::uint64_t>(SIZE_MAX)) {
                error = "Image is too large to allocate.";
                break;
            }

            std::vector<unsigned char> bgr(static_cast<std::size_t>(dstSize64));

            MV_CC_PIXEL_CONVERT_PARAM convert{};
            convert.nWidth = width;
            convert.nHeight = height;
            convert.pSrcData = frame.pBufAddr;
            convert.nSrcDataLen = frame.stFrameInfo.nFrameLen;
            convert.enSrcPixelType = frame.stFrameInfo.enPixelType;
            convert.enDstPixelType = PixelType_Gvsp_BGR8_Packed;
            convert.pDstBuffer = bgr.data();
            convert.nDstBufferSize = static_cast<unsigned int>(bgr.size());

            ret = MV_CC_ConvertPixelType(handle_, &convert);
            if (ret != MV_OK) {
                error = sdkError("MV_CC_ConvertPixelType", ret);
                break;
            }

            cv::Mat view(static_cast<int>(height), static_cast<int>(width), CV_8UC3, bgr.data());
            image = view.clone();
            converted = !image.empty();
            if (!converted) {
                error = "Failed to create OpenCV image.";
            }
        } while (false);

        const int freeRet = MV_CC_FreeImageBuffer(handle_, &frame);
        if (freeRet != MV_OK && converted) {
            error = sdkError("MV_CC_FreeImageBuffer", freeRet);
            return false;
        }

        return converted;
    }

    void close() override
    {
        if (handle_ != nullptr) {
            if (grabbing_) {
                MV_CC_StopGrabbing(handle_);
                grabbing_ = false;
            }
            if (deviceOpened_) {
                MV_CC_CloseDevice(handle_);
                deviceOpened_ = false;
            }
            MV_CC_DestroyHandle(handle_);
            handle_ = nullptr;
        }
    }

    bool isOpened() const override
    {
        return handle_ != nullptr && deviceOpened_ && grabbing_;
    }

private:
    static std::string sdkError(const char* operation, int code)
    {
        return std::string(operation) + " failed, SDK error code: " + std::to_string(code);
    }

    void* handle_ = nullptr;
    bool deviceOpened_ = false;
    bool grabbing_ = false;
};

std::unique_ptr<CameraBackend> createMVSBackend()
{
    return std::make_unique<MVSBackend>();
}
