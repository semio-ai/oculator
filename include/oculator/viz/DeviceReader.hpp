#pragma once

#include <gst/gst.h>
#include <mutex>
#include <functional>

#include <torch/torch.h>

namespace oculator
{

  /* Structure to contain all our information, so we can pass it around */
  typedef struct _DeviceReaderStruct {
    GstElement *gst_play;           /* Our one and only pipeline */
    GstElement *gst_conv;           /* Our one and only pipeline */
    GstElement *gst_sink;           /* Our one and only pipeline */

    GstState state;                 /* Current state of the pipeline */
    uint32_t width, height;         /* Width and height of the video */
    bool should_quit;
  } DeviceReaderStruct;

  class DeviceReader
  {
  public:
    DeviceReader(std::string uri, std::function< void(torch::Tensor) > callback);
    ~DeviceReader();
    
    bool isPlaying();
    bool waitForCompletion();
    void setImageWidth(const uint32_t width);
    void setImageHeight(const uint32_t height);

    uint32_t getImageWidth();
    uint32_t getImageHeight();

    friend GstPadProbeReturn cb_have_data (GstPad          *pad,
                                                  GstPadProbeInfo *info,
                                                  gpointer         user_data);
    void pump(torch::Tensor &tensor);
    void iterate();
  private:
    bool mIsInitialized;
    bool mIsPlaying;
    bool mAreCapsKnown;

    uint32_t mImageWidth, mImageHeight;

    GMainLoop *mGSTLoop;
    GMainContext *mGSTContext;
    GstElement *mPipeline;

    std::mutex mMutex;
    DeviceReaderStruct mInternalGSTStruct;

    bool initializeGST();
    void shutdownGST();

    
    std::function< void(torch::Tensor) > mCallback;
  };
}