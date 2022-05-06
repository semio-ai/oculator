#pragma once

#include <gst/gst.h>
#include <mutex>
#include <functional>

namespace oculator
{

  /* Structure to contain all our information, so we can pass it around */
  typedef struct _DeviceReaderStruct {
    GstElement *gst_play;           /* Our one and only pipeline */
    GstElement *gst_sink;           /* Our one and only pipeline */

    GstState state;                 /* Current state of the pipeline */
    uint32_t width, height;         /* Width and height of the video */
  } DeviceReaderStruct;

  class DeviceReader
  {
  public:
    DeviceReader(std::string uri);
    ~DeviceReader();
    
    bool isPlaying();
    bool waitForCompletion();
    void setImageWidth(const uint32_t width);
    void setImageHeight(const uint32_t height);

    uint32_t getImageWidth();
    uint32_t getImageHeight();
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
    std::function< void() > mCallback;
    void update(void *data, void *id);
  };
}