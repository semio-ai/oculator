#include "oculator/viz/DeviceReader.hpp"

#include <QtCore/QDebug>

using namespace oculator;

DeviceReader::DeviceReader(ImageView *const target, ImageView *const saliency, QObject *const parent)
  : QObject(parent)
  , target_(target)
  , saliency_(saliency)
{
  //  vlcPlayer = NULL;

  /* Initialize libVLC */
  //vlcInstance = libvlc_new(0, NULL);

  /* Complain in case of broken installation */
  //if (vlcInstance == NULL) {
  //  QMessageBox::critical(this, "Qt libVLC player", "Could not init libVLC");
  //  exit(1);
  //}
}

DeviceReader::~DeviceReader() {
  /* Release libVLC instance on quit */
  //if (vlcInstance)
  //  libvlc_release(vlcInstance);
}

void DeviceReader::update()
{
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

