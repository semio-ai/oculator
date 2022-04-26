#include "oculator/viz/DeviceReader.hpp"


#include <gst/gstplugin.h>

#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <functional>
#include<iostream>
#include <torch/torch.h>

static bool save_once = true;

using namespace oculator;

static GstPadProbeReturn cb_have_data (GstPad          *pad,
                                       GstPadProbeInfo *info,
                                       gpointer         user_data) {
  GstEvent *event = GST_PAD_PROBE_INFO_EVENT(info);
  // g_print("Have data\n");
  if (GST_EVENT_CAPS == GST_EVENT_TYPE(event)) {
    GstCaps * caps = gst_caps_new_any();
    int width, height;
    gst_event_parse_caps(event, &caps);

    GstStructure *s = gst_caps_get_structure(caps, 0);

    gboolean res;
    res = gst_structure_get_int (s, "width", &width);
    res |= gst_structure_get_int (s, "height", &height);
    if (!res)
      g_print( "no dimensions");
    g_print(" Width: %d, Height: %d\n", width, height);
  } else {
    // g_print("Buffer callback\n");

    // gint x, y;
    // GstMapInfo map;
    // guint16 *ptr, t;
    // GstBuffer *buffer;

    // buffer = GST_PAD_PROBE_INFO_BUFFER (info);

    // // buffer = gst_buffer_make_writable (buffer);

    // /* Making a buffer writable can fail (for example if it
    // * cannot be copied and is used more than once)
    // */
    // if (buffer == NULL)
    //   return GST_PAD_PROBE_OK;

    // /* Mapping a buffer can fail (non-writable) */
    // if (gst_buffer_map (buffer, &map, GST_MAP_READ)) {
    //   ptr = (guint16 *) map.data;

    //   int width = 450;
    //   int height = 360;

    //   if(save_once) {
    //     // Create the torch tensor to return and fill it from OpenIL. 
    //     torch::Tensor imageData = torch::zeros({height,width,3}, torch::TensorOptions().dtype(torch::kUInt8));
    //     std::memcpy(imageData.data_ptr(), ptr, width*height*3);
    //     g_print("MEMCPY SUCCEED\n");
        
    //     torch::save(imageData, "img.pt");
    //     save_once = false;
    //   }

    //   //   /* invert data */
    //   //   for (y = 0; y < 288; y++) {
    //   //     for (x = 0; x < 384 / 2; x++) {
    //   //       t = ptr[384 - 1 - x];
    //   //       ptr[384 - 1 - x] = ptr[x];
    //   //       ptr[x] = t;
    //   //     }
    //   //     ptr += 384;
    //   //   }
    //     gst_buffer_unmap (buffer, &map);

    // }
   
  }
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
  long id = gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_EVENT_BOTH, (GstPadProbeCallback) cb_have_data, NULL, NULL);
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
// VLC_PLUGIN_PATH=/opt/homebrew/opt/libvlc-3.0.16-arm/plugins ./oculator -u /Users/drobotnik/Movies/Project1_BottomLeft.mov
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
  g_object_set (mInternalGSTStruct.gst_play, "uri", filename, NULL);
  
  gst_bin_add_many (GST_BIN (mPipeline), mInternalGSTStruct.gst_play, mInternalGSTStruct.gst_sink, NULL);

  /* Instruct the bus to emit signals for each received message, and connect to the interesting signals */
  GstBus *bus = gst_element_get_bus (mPipeline);
  gst_bus_add_signal_watch (bus);
  g_signal_connect (G_OBJECT (bus), "message::error", (GCallback)error_cb, &mInternalGSTStruct);
  g_signal_connect (G_OBJECT (bus), "message::eos", (GCallback)eos_cb, &mInternalGSTStruct);
  g_signal_connect (G_OBJECT (bus), "message::state-changed", (GCallback)state_changed_cb, &mInternalGSTStruct);
  gst_object_unref (bus);

  /* Finally connect to the pad added event which will let us probe the images and convert to torch tensors */
  g_signal_connect (mInternalGSTStruct.gst_play, "pad-added", G_CALLBACK (on_pad_added), mInternalGSTStruct.gst_sink);

  /* Start playing */
  ret = gst_element_set_state (mInternalGSTStruct.gst_play, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    shutdownGST();
    return false;
  }

  /* Register a function that GLib will call every second */
  // g_timeout_add_seconds (1, (GSourceFunc)refresh_ui, &data);
  mGSTContext = g_main_loop_get_context (mGSTLoop);
  /* Start the GTK main loop. We will not regain control until gtk_main_quit is called. */
  g_print ("Running one iteration...\n");


  if(!g_main_context_iteration (mGSTContext, FALSE)) {
    g_print("BROKEN\n");
  } else
    g_print("Iterated\n");
  ret = gst_element_set_state (mInternalGSTStruct.gst_play, GST_STATE_PLAYING);

  sleep(1);
  g_print("Now playing and main looping\n");

  for(int i =0 ;i < 2;i++) {
    ret = gst_element_set_state (mPipeline, GST_STATE_PLAYING);
    
    if (ret == GST_STATE_CHANGE_FAILURE) {
      g_printerr ("Unable to set the pipeline to the playing state.\n");
      gst_object_unref (mInternalGSTStruct.gst_play);
      return false;
    }
  }

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