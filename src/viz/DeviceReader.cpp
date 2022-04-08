#include "oculator/viz/DeviceReader.hpp"
#include <vlc/vlc.h>
#include <functional>
#include<iostream>
using namespace oculator;


#include <string.h>
#include <stdio.h>
#include <gst/gst.h>
#include <gst/gstplugin.h>
#include <unistd.h>


static GstPadProbeReturn
cb_have_data (GstPad          *pad,
              GstPadProbeInfo *info,
              gpointer         user_data)
{
  GstEvent *event = GST_PAD_PROBE_INFO_EVENT(info);
  if (GST_EVENT_CAPS == GST_EVENT_TYPE(event)) {
      GstCaps * caps = gst_caps_new_any();
      int width, height;
      gst_event_parse_caps(event, &caps);

      GstStructure *s = gst_caps_get_structure(caps, 0);

      gboolean res;
      res = gst_structure_get_int (s, "width", &width);
      res |= gst_structure_get_int (s, "height", &height);
      if (!res) {
          g_print( "no dimensions");
      }
      g_print(" Width: %d, Height: %d\n", width, height);
   } else {
     g_print("Buffer callback\n");
   }
  // gint x, y;
  // GstMapInfo map;
  // guint16 *ptr, t;
  // GstBuffer *buffer;

  // buffer = GST_PAD_PROBE_INFO_BUFFER (info);

  // buffer = gst_buffer_make_writable (buffer);

  /* Making a buffer writable can fail (for example if it
   * cannot be copied and is used more than once)
   */
  // if (buffer == NULL)
  //   return GST_PAD_PROBE_OK;

  /* Mapping a buffer can fail (non-writable) */
  // if (gst_buffer_map (buffer, &map, GST_MAP_WRITE)) {
  //   ptr = (guint16 *) map.data;
  //   /* invert data */
  //   for (y = 0; y < 288; y++) {
  //     for (x = 0; x < 384 / 2; x++) {
  //       t = ptr[384 - 1 - x];
  //       ptr[384 - 1 - x] = ptr[x];
  //       ptr[x] = t;
  //     }
  //     ptr += 384;
  //   }
  //   gst_buffer_unmap (buffer, &map);
  // }

  // GST_PAD_PROBE_INFO_DATA (info) = buffer;

  return GST_PAD_PROBE_OK;
}


/* Structure to contain all our information, so we can pass it around */
typedef struct _CustomData {
  GstElement *playbin;           /* Our one and only pipeline */
  GstElement *my_sink;           /* Our one and only pipeline */


  GstState state;                 /* Current state of the pipeline */
  gint64 duration;                /* Duration of the clip, in nanoseconds */
} CustomData;


/* This function is called when an error message is posted on the bus */
static void error_cb (GstBus *bus, GstMessage *msg, CustomData *data) {
  GError *err;
  gchar *debug_info;

  /* Print error details on the screen */
  gst_message_parse_error (msg, &err, &debug_info);
  g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
  g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
  g_clear_error (&err);
  g_free (debug_info);

  /* Set the pipeline to READY (which stops playback) */
  gst_element_set_state (data->playbin, GST_STATE_READY);
}

/* This function is called when an End-Of-Stream message is posted on the bus.
 * We just set the pipeline to READY (which stops playback) */
static void eos_cb (GstBus *bus, GstMessage *msg, CustomData *data) {
  g_print ("End-Of-Stream reached.\n");
  gst_element_set_state (data->playbin, GST_STATE_READY);
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


// gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_EVENT_BOTH, pad_cb, &customData_, nullptr);
  long id = gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_EVENT_BOTH, (GstPadProbeCallback) cb_have_data, NULL, NULL);
  //  GST_PAD_PROBE_TYPE_BUFFER
}

static void state_changed_cb (GstBus *bus, GstMessage *msg, CustomData *data) {
  GstState old_state, new_state, pending_state;
  gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
  if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data->playbin)) {
    data->state = new_state;
    // g_print ("State was %s\n", gst_element_state_get_name (old_state));
    // g_print ("State set to %s\n", gst_element_state_get_name (new_state));
    // g_print ("State pending %s\n", gst_element_state_get_name (pending_state));
    // if (old_state == GST_STATE_READY && new_state == GST_STATE_PAUSED) {
    //   /* For extra responsiveness, we refresh the GUI as soon as we reach the PAUSED state */
    //   // refresh_ui (data);
    // }
  } else {

  }
}

int main(int argc, char *argv[]) {
  CustomData data;
  GstStateChangeReturn ret;
  GstBus *bus;
  GstElement *pipeline;
  GMainLoop *loop;
  GMainContext *context;

  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  loop = g_main_loop_new (NULL, FALSE);

  // filesrc location=CF1.png ! decodebin ! autovideosink
  pipeline = gst_pipeline_new ("my-pipeline");

  /* Initialize our data structure */
  memset (&data, 0, sizeof (data));
  data.duration = GST_CLOCK_TIME_NONE;

  /* Create the elements */
  data.playbin = gst_element_factory_make ("uridecodebin", "uridecodebin");
  // GstElement *filter   = gst_element_factory_make ("my_sink", "my_sink");
  if (!data.playbin) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }
  // if (!filter) {
  //   g_printerr ("FILTER COULD NOT BE FACTORYIZED.\n");
  //   return -1;
  // }

  char *filename = "file:///Users/drobotnik/Movies/Hedgehog_motion.avi";
  /* Set the URI to play */
  g_print( "playing uri\n");
  // g_object_set (data.playbin, "uri", "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm", NULL);
  
  // data.playbin = gst_element_factory_make ("uridecodebin", "uridecodebin");
  // data.my_sink = gst_element_factory_make ("my_sink", NULL);
  data.my_sink = gst_element_factory_make ("autovideosink", "autovideosink");
  g_object_set (data.playbin, "uri", filename, NULL);
  
  if (!data.playbin || !data.my_sink) {
    g_printerr ("One element could not be created. Exiting.\n");
    return -1;
  }
  
  gst_bin_add_many (GST_BIN (pipeline), data.playbin, data.my_sink, NULL);
  // gst_bin_add_many (GST_BIN (pipeline), data.playbin, NULL);


  /* Connect to interesting signals in playbin */
  // g_signal_connect (G_OBJECT (data.playbin), "video-tags-changed", (GCallback) tags_cb, &data);
  // g_signal_connect (G_OBJECT (data.playbin), "audio-tags-changed", (GCallback) tags_cb, &data);
  // g_signal_connect (G_OBJECT (data.playbin), "text-tags-changed", (GCallback) tags_cb, &data);

  /* Create the GUI */
  // create_ui (&data);

  /* Instruct the bus to emit signals for each received message, and connect to the interesting signals */
  bus = gst_element_get_bus (pipeline);
  gst_bus_add_signal_watch (bus);
  g_signal_connect (G_OBJECT (bus), "message::error", (GCallback)error_cb, &data);
  g_signal_connect (G_OBJECT (bus), "message::eos", (GCallback)eos_cb, &data);
  // g_signal_connect (G_OBJECT (bus), "message::state-changed", (GCallback)state_changed_cb, &data);
  // g_signal_connect (G_OBJECT (bus), "message::application", (GCallback)application_cb, &data);
  gst_object_unref (bus);

  // GstIterator *iterator = gst_element_iterate_pads(data.playbin);
  // GValue value = { 0, };
  // GstPad *pad;
  // gchar *id;
  // int done=0;
  // GstPadDirection dir;
  // while (done == 0) {
  //   switch (gst_iterator_next (iterator, &value)) {
  //     case GST_ITERATOR_OK:
  //       printf("HELLO\n");
  //       pad = g_value_dup_object (&value);
  //       id = gst_pad_get_stream_id(pad);
  //       printf("%s\n", id);
  //       dir = gst_pad_get_direction(pad);
  //       printf("%d\n", dir);
  //       // bcd = g_slice_new0 (BufferCountData);
  //       // g_object_set_data (G_OBJECT (pad), "buffer-count-data", bcd);
  //       // bcd->probe_id = gst_pad_add_probe (pad, GST_PAD_PROBE_TYPE_BUFFER,
  //       //     (GstPadProbeCallback) input_selector_pad_probe, NULL, NULL);
  //       // bcd->pad = pad;
  //       g_value_reset (&value);
  //       break;
  //     case GST_ITERATOR_RESYNC:
  //       printf("resyncing\n");
  //       gst_iterator_resync (iterator);
  //       break;
  //     case GST_ITERATOR_ERROR:
  //       printf("ERROR\n");
  //       done = 0;
  //       break;
  //     case GST_ITERATOR_DONE:
  //       printf("Done\n");
  //       done = 1;
  //       break;
  //     default:
  //       printf("DEFAULT\n");
  //   }
  // }
  // gst_iterator_free (iterator);

  g_signal_connect (data.playbin, "pad-added", G_CALLBACK (on_pad_added), data.my_sink);
  // g_signal_connect (data.playbin, "pad-added", G_CALLBACK (on_pad_added), filter);
  // if (!gst_element_link (data.playbin, data.my_sink)) {
  //   g_warning ("Failed to link elements!");
  // }
  

  /* Start playing */
  ret = gst_element_set_state (data.playbin, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (data.playbin);
    return -1;
  }

  /* Register a function that GLib will call every second */
  // g_timeout_add_seconds (1, (GSourceFunc)refresh_ui, &data);
  context = g_main_loop_get_context (loop);
  /* Start the GTK main loop. We will not regain control until gtk_main_quit is called. */
  g_print ("Running one iteration...\n");


  if(!g_main_context_iteration (context, FALSE)) {
    g_print("BROKEN\n");
  }
   sleep(1);
  g_print("Now playing and main looping\n");

  for(int i =0 ;i < 2;i++) {
    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    
    if (ret == GST_STATE_CHANGE_FAILURE) {
      g_printerr ("Unable to set the pipeline to the playing state.\n");
      gst_object_unref (data.playbin);
      return -1;
    }
  }

  g_main_loop_run (loop);

  ret = gst_element_set_state (data.playbin, GST_STATE_PLAYING);

  /* Free resources */
  gst_element_set_state (data.playbin, GST_STATE_NULL);
  gst_object_unref (data.playbin);
  g_main_loop_unref (loop);
  return 0;
}


static void update_fn(void *data, void *id)
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

static void *lock_output_fn(void *data, void **p_pixels) {
  return NULL;
}
static void unlock_output_fn(void *data, void *id, void *const *p_pixels) {

}



DeviceReader::DeviceReader(std::string uri) :
                              // m_callback(callback),
                              m_vlc_instance(NULL),
                              m_vlc_player(NULL),
                              mutex{}
{
  m_is_initialized = initializeVLC();

  if(m_is_initialized) {
    libvlc_media_t *vlc_media = NULL;
    if (uri.find("http") == 0)
      vlc_media = libvlc_media_new_location (m_vlc_instance, uri.c_str());
    else {
      std::cout << "Opening file: " << uri << std::endl;
      vlc_media = libvlc_media_new_path (m_vlc_instance, uri.c_str() );
      if(vlc_media == NULL) {
        std::cout << "Failed to open file: " << uri << std::endl;
      } else {
        std::cout << "File opened with handle: " << uri << std::endl;
      }
      // Make it loop
      // libvlc_media_add_option(vlc_media, "input-repeat=-1");
    }
    // Check to see what kind of media we're dealing with and handle it appropriately
  
    // Make sure the media has a handle and continue
    if(vlc_media) {
      std::cout<< "Now let's create a player\n";
      // Create a player and free the media
      m_vlc_player = libvlc_media_player_new_from_media (vlc_media);
      std::cout<< "Releasing media\n";
      // libvlc_media_release (vlc_media);
      auto leng = libvlc_media_player_get_length(m_vlc_player);
      std::cout << "Length : " << leng << std::endl;
      if(m_vlc_player == NULL) {
        std::cout << "Error creating a player\n";
      }
      // std::cout<< "Setting callback\n";
      // Set the callbacks and format
      // libvlc_video_set_callbacks(m_vlc_player, 
      //                     &lock_output_fn, 
      //                     &unlock_output_fn,
      //                     &update_fn,
      //                     NULL);
      // libvlc_video_set_format(m_vlc_player, "RV16", 640, 480, 640*2);
      // play
      std::cout<< "Playing\n";
      int ret_code = libvlc_media_player_play(m_vlc_player);
      std::cout << "With return code: " << ret_code << std::endl;
    }
  }
}

DeviceReader::~DeviceReader() {
  /* Release libVLC instance on quit */
  if (m_vlc_player) {
    libvlc_media_player_stop(m_vlc_player);
    libvlc_media_player_release(m_vlc_player);
  }
  if (m_vlc_instance)
    libvlc_release(m_vlc_instance);
}
// VLC_PLUGIN_PATH=/opt/homebrew/opt/libvlc-3.0.16-arm/plugins ./oculator -u /Users/drobotnik/Movies/Project1_BottomLeft.mov
bool DeviceReader::initializeVLC() {
  char const *vlc_argv[] = {
        "--no-audio", /* skip any audio track */
        "--no-xlib", /* tell VLC to not use Xlib */
        // "--reset-plugins-cache",
        "-v",
        "2"  /* tell VLC to be verbose */
    };
  int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);
  std::cout << "Starting vlc\n";
  /* Initialize libVLC */
  m_vlc_instance = libvlc_new(vlc_argc, vlc_argv);
  // m_vlc_instance = libvlc_new(0, NULL);
  
  if(m_vlc_instance == NULL) {
    const char *err_msg = libvlc_errmsg ();
    if(err_msg != NULL) {
      std::cerr << err_msg << std::endl;
    } else {
      std::cerr << "Critical failure: could not find libVLC!\n";
    }
    return false;
  }
  return true;
}

void DeviceReader::update_(void *data, void *id)
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

void DeviceReader::lock_output_(void *data, void **p_pixels) {
  std::cout << "lock called\n";
}
void DeviceReader::unlock_output_(void *data, void *id, void *const *p_pixels) {}

bool DeviceReader::is_playing() {
  if(m_is_initialized)
    return libvlc_media_player_is_playing(m_vlc_player);
  return false;
}