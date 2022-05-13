#pragma once
/**
 *   ____  ____  _     _     ____  _____  ____  ____ 
 *  /  _ \/   _\/ \ /\/ \   /  _ \/__ __\/  _ \/  __\
 *  | / \||  /  | | ||| |   | / \|  / \  | / \||  \/|
 *  | \_/||  \_ | \_/|| |_/\| |-||  | |  | \_/||    /
 *  \____/\____/\____/\____/\_/ \|  \_/  \____/\_/\_\
 * 
 * Uses GStreamer to read streaming media like webcams, online video, and movie files.
 * Streams to a torch tensor.
 * 
 * @author: ndepalma@alum.mit.edu
 * @license: MIT License
 */ 

#include <gst/gst.h>
#include <mutex>
#include <functional>

#include <torch/torch.h>

namespace oculator
{
  typedef enum {
    FROM_VIDEO_FILE,
    FROM_DEVICE,
    FROM_RTSP
  } SourceType;
  
  class DeviceReader
  {
    friend GstPadProbeReturn cb_have_data (GstPad          *pad,
                                           GstPadProbeInfo *info,
                                           DeviceReader         *user_data);
    friend void error_cb (GstBus *bus, GstMessage *msg, DeviceReader *data);
    friend void state_changed_cb (GstBus *bus, GstMessage *msg, DeviceReader *data);
    friend void eos_cb (GstBus *bus, GstMessage *msg, DeviceReader *data);

  public:
    DeviceReader(std::string uri, std::function< void(torch::Tensor) > callback);
    ~DeviceReader();
    
    uint32_t mImageWidth, mImageHeight;
    bool isPlaying();

  protected:
    bool mIsInitialized;
    bool mIsPlaying;
    bool mAreCapsKnown;

    void pump(torch::Tensor &tensor);

    GMainLoop *mGSTLoop;
    GMainContext *mGSTContext;

    GstElement *mPipeline;
    GstElement *mGstPlay;
    GstElement *mGstConv; 
    GstElement *mGstSink;

    GstState mState;
    bool should_quit;

    bool initializeGST(const std::string uri, const SourceType type);
    void shutdownGST();

    const std::function< void(torch::Tensor) > mCallback;
  };
}