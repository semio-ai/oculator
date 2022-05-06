#include "oculator/viz/DeviceReader.hpp"

/**
 *   ____  ____  _     _     ____  _____  ____  ____ 
 *  /  _ \/   _\/ \ /\/ \   /  _ \/__ __\/  _ \/  __\
 *  | / \||  /  | | ||| |   | / \|  / \  | / \||  \/|
 *  | \_/||  \_ | \_/|| |_/\| |-||  | |  | \_/||    /
 *  \____/\____/\____/\____/\_/ \|  \_/  \____/\_/\_\
 * 
 * Oculator's main entry point. Really this just triggers the model loading and the video feed. 
 * 
 * @author: ndepalma@alum.mit.edu
 * @license: MIT License
 */ 

#include <gst/gstplugin.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <torch/torch.h>

#include <functional>
#include<iostream>

static bool save_once = true;

using namespace oculator;
using namespace std::placeholders;
static DeviceReader *singleton = NULL;


static GstPadProbeReturn cb_have_data (GstPad          *pad,
                                       GstPadProbeInfo *info,
                                       gpointer         user_data) {
  GstEvent *event = GST_PAD_PROBE_INFO_EVENT(info);
  // DeviceReader *reader = reinterpret_cast<DeviceReader *>(user_data);
  if (GST_EVENT_CAPS == GST_EVENT_TYPE(event)) {
    GstCaps * caps = gst_caps_new_any();
    gst_event_parse_caps(event, &caps);
    int32_t width, height;

    GstStructure *s = gst_caps_get_structure(caps, 0);

    gboolean res;
    res = gst_structure_get_int (s, "width", &width);
    res |= gst_structure_get_int (s, "height", &height);

    if (!res)
      g_print( "no dimensions");

    singleton->setImageWidth(width);    
    singleton->setImageHeight(height);
    g_print(" Width: %d, Height: %d\n", width, height);
    g_print("Structure: %s\n", gst_structure_to_string(s));
    gst_caps_unref(caps);
  } else if(GST_EVENT_TYPE(event) == GST_EVENT_UNKNOWN) {
    gint x, y;
    GstMapInfo map;
    guint16 *ptr, t;
    GstBuffer *buffer;

    buffer = GST_PAD_PROBE_INFO_BUFFER (info);
    if (buffer == NULL )
        return GST_PAD_PROBE_OK;
    // g_print("Have data %s, %d\n", GST_EVENT_TYPE_NAME(event), GST_EVENT_TYPE(event));
    if(GST_IS_BUFFER(buffer)) {
      /* Making a buffer writable can fail (for example if it
      * cannot be copied and is used more than once)
      */
      int total_size = gst_buffer_get_size(buffer);
      char *array = (char*)malloc(total_size);
      gst_buffer_extract(buffer, 0, array, total_size);
      long reduce = 0;
      for(int h = 0;h < total_size;h++) {
        reduce += array[h];
      }
      g_print("Sum first 1000: %d\n", reduce);
      
      /* Mapping a buffer can fail (non-writable) */
      if(save_once) {
        // Create the torch tensor to return and fill it from OpenIL. 
        uint32_t width = singleton->getImageWidth();
        uint32_t height = singleton->getImageHeight();
        // torch::Tensor imageData = torch::zeros({height,width,3}, torch::TensorOptions().dtype(torch::kUInt8));
        torch::Tensor imageData = torch::zeros({3, height,width}, torch::TensorOptions().dtype(torch::kUInt8));
        std::memcpy(imageData.data_ptr(), array, width*height*3);
        g_print("MEMCPY SUCCEED\n");
          
        torch::save(imageData, "img.pt");
        save_once = false;
      }  
      free(array);
    }
  }
  // If you're writing
  // GST_PAD_PROBE_INFO_DATA (info) = buffer;

  return GST_PAD_PROBE_OK;
}

/* This function is called when an error message is posted on the bus */
static void error_cb (GstBus *bus, GstMessage *msg, DeviceReaderStruct *data) {
  GError *err;
  gchar *debug_info;

  /* Print error details on the screen */
  gst_message_parse_error (msg, &err, &debug_info);
  g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
  g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
  g_clear_error (&err);
  g_free (debug_info);

  /* Set the pipeline to READY (which stops playback) */
  gst_element_set_state (data->gst_play, GST_STATE_READY);
}

/* This function is called when an End-Of-Stream message is posted on the bus.
 * We just set the pipeline to READY (which stops playback) */
static void eos_cb (GstBus *bus, GstMessage *msg, DeviceReaderStruct *data) {
  g_print ("End-Of-Stream reached.\n");
  gst_element_set_state (data->gst_play, GST_STATE_READY);
}

static void
on_pad_added (GstElement *element,
              GstPad     *pad,
              gpointer    data)
{
  GstPad *sinkpad;
  GstElement *vid_sink = (GstElement *) data;

  /* We can now link this pad with the vorbis-decoder sink pad */
  g_print ("Dynamic pad created, linking demuxer/sink\n");

  sinkpad = gst_element_get_static_pad (vid_sink, "sink");

  gst_pad_link (pad, sinkpad);

  gst_object_unref (sinkpad);

  //TODO: Catch this probe and free it later.
  // long id = gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_DATA_DOWNSTREAM, (GstPadProbeCallback) cb_have_data, this, NULL);
    // std::bind(cb_have_data, _1, _2, _3, reader);
  long id = gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_DATA_DOWNSTREAM, (GstPadProbeCallback) cb_have_data, NULL, NULL);
}

static void state_changed_cb (GstBus *bus, GstMessage *msg, DeviceReaderStruct *data) {
  GstState old_state, new_state, pending_state;
  gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
  if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data->gst_play)) {
    data->state = new_state;
    // g_print ("State was %s\n", gst_element_state_get_name (old_state));
    g_print ("State set to %s\n", gst_element_state_get_name (new_state));
    // g_print ("State pending %s\n", gst_element_state_get_name (pending_state));
    // if (old_state == GST_STATE_READY && new_state == GST_STATE_PAUSED) {
    //   /* For extra responsiveness, we refresh the GUI as soon as we reach the PAUSED state */
    //   // refresh_ui (data);
    // }
  }
}

DeviceReader::DeviceReader(std::string uri) : mMutex{}
{
  singleton = this;
  mIsInitialized = initializeGST();

  if(mIsInitialized) {
    // Check to see what kind of media we're dealing with and handle it appropriately
    // if (uri.find("http") == 0)
  //     vlc_media = libvlc_media_new_location (m_vlc_instance, uri.c_str());
    // else {
  //     std::cout << "Opening file: " << uri << std::endl;
  //     vlc_media = libvlc_media_new_path (m_vlc_instance, uri.c_str() );
    // }
      // Make it loop
  }
  
}

void DeviceReader::shutdownGST() {
  //TODO: Free everythign in order
  if(mInternalGSTStruct.gst_play != NULL) {
    gst_element_set_state (mInternalGSTStruct.gst_play, GST_STATE_READY);

    gst_object_unref(mPipeline);
    mPipeline = NULL;
    g_main_loop_unref(mGSTLoop);
    mGSTLoop = NULL;
    mInternalGSTStruct.gst_play = NULL;
  }
}

DeviceReader::~DeviceReader() {
    shutdownGST();
}

bool DeviceReader::initializeGST() {
  GstStateChangeReturn ret;

  /* Initialize GStreamer */
  gst_init (NULL, NULL);

  mGSTLoop = g_main_loop_new (NULL, FALSE);

  // filesrc location=CF1.png ! decodebin ! autovideosink
  mPipeline = gst_pipeline_new ("oculator-gst-pipeline");

  /* Initialize our data structure */
  memset (&mInternalGSTStruct, 0, sizeof (mInternalGSTStruct));

  /* Create the elements */
  mInternalGSTStruct.gst_play = gst_element_factory_make ("uridecodebin", "uridecodebin");
  mInternalGSTStruct.gst_sink = gst_element_factory_make ("autovideosink", "autovideosink");
  // mInternalGSTStruct.gst_sink = gst_element_factory_make ("fakesink", "fakesink");

  if (!mInternalGSTStruct.gst_play || !mInternalGSTStruct.gst_sink) {
    g_printerr ("Not all elements could be created.\n");
    shutdownGST();
    return false;
  }

  char *filename = "file:///Users/drobotnik/Movies/Hedgehog_motion.avi";  
  
  /* Set the URI to play */
  
  
  gst_bin_add_many (GST_BIN (mPipeline), mInternalGSTStruct.gst_play, mInternalGSTStruct.gst_sink, NULL);
  gst_element_link_many (mInternalGSTStruct.gst_play, mInternalGSTStruct.gst_sink, NULL);

  g_object_set (mInternalGSTStruct.gst_play, "uri", filename, NULL);

  /* Instruct the bus to emit signals for each received message, and connect to the interesting signals */
  GstBus *bus = gst_element_get_bus (mPipeline);
  gst_bus_add_signal_watch (bus);
  g_signal_connect (G_OBJECT (bus), "message::error", (GCallback)error_cb, &mInternalGSTStruct);
  g_signal_connect (G_OBJECT (bus), "message::eos", (GCallback)eos_cb, &mInternalGSTStruct);
  g_signal_connect (G_OBJECT (bus), "message::state-changed", (GCallback)state_changed_cb, &mInternalGSTStruct);
  gst_object_unref (bus);

  /* Finally connect to the pad added event which will let us probe the images and convert to torch tensors */
  g_signal_connect (mInternalGSTStruct.gst_play, "pad-added", G_CALLBACK (on_pad_added), mInternalGSTStruct.gst_sink);
  // GstPad *pad;
  // pad = gst_element_get_static_pad (mInternalGSTStruct.gst_play, "src");
  // long id = gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_BUFFER, (GstPadProbeCallback) cb_have_data, NULL, NULL);
// GST_PAD_PROBE_TYPE_EVENT_BOTH
  // gst_object_unref (pad);
  /* Start playing */
  ret = gst_element_set_state (mPipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    shutdownGST();
    return false;
  }

  /* Register a function that GLib will call every second */
  // g_timeout_add_seconds (1, (GSourceFunc)refresh_ui, &data);
  // mGSTContext = g_main_loop_get_context (mGSTLoop);
  // /* Start the GTK main loop. We will not regain control until gtk_main_quit is called. */
  // g_print ("Running one iteration...\n");


  // if(!g_main_context_iteration (mGSTContext, FALSE)) {
  //   g_print("BROKEN\n");
  // } else
  //   g_print("Iterated\n");
  // ret = gst_element_set_state (mInternalGSTStruct.gst_play, GST_STATE_PLAYING);

  // sleep(1);
  g_print("Now playing and main looping\n");

  // for(int i =0 ;i < 2;i++) {
  //   ret = gst_element_set_state (mPipeline, GST_STATE_PLAYING);
    
  //   if (ret == GST_STATE_CHANGE_FAILURE) {
  //     g_printerr ("Unable to set the pipeline to the playing state.\n");
  //     gst_object_unref (mInternalGSTStruct.gst_play);
  //     return false;
  //   }
  // }

  g_main_loop_run (mGSTLoop);
  // bool quit = false;
  // while(!quit) {
  //   if(!g_main_context_iteration (mGSTContext, FALSE)) {
  //     g_print("BROKEN\n");
  //     quit = true;
  //   } else {
  //     g_print("Iterated\n");
  //   }
  // }
  g_print("Shutting down\n");
  /* Free resources */
  shutdownGST();

  return true;
}

void DeviceReader::update(void *data, void *id)
{
  std::cout<< "Update called\n";
  /*if (!capture_.grab())
  {
    qWarning() << "Failed to grab frame";
    return;
  }

  cv::Mat frame;
  if (!capture_.retrieve(frame))
  {
    qWarning() << "Failed to retrieve frame";
    return;
  }

  target_->setMat(frame);

  saliency_->setMat(RunMR_AIM(frame, basis_, *mk_aim_, 0));
  */
}

bool DeviceReader::isPlaying() {
  return mIsPlaying;
}

bool DeviceReader::waitForCompletion() {
  // if(m_is_initialized)
  //   return libvlc_media_player_is_playing(m_vlc_player);
  return false;
}

void DeviceReader::setImageWidth(const uint32_t width) {
  mImageWidth = width;
}

void DeviceReader::setImageHeight(const uint32_t height) {
  mImageHeight = height;
}

uint32_t DeviceReader::getImageWidth() {
  return mImageWidth;
}

uint32_t DeviceReader::getImageHeight() {
  return mImageHeight;
}