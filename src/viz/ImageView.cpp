#include "oculator/viz/ImageView.hpp"


namespace oculator {
  // Instead of generating this, we're just using what was in matplotlib.
  static const float __HOT_COLORMAP__[][4] = 
      {{0.0416, 0.0, 0.0, 1.0},
      {0.05189484405443485, 0.0, 0.0, 1.0},
      {0.0621896881088697, 0.0, 0.0, 1.0},
      {0.07248453216330454, 0.0, 0.0, 1.0},
      {0.0827793762177394, 0.0, 0.0, 1.0},
      {0.09307422027217424, 0.0, 0.0, 1.0},
      {0.10336906432660908, 0.0, 0.0, 1.0},
      {0.11366390838104393, 0.0, 0.0, 1.0},
      {0.12395875243547878, 0.0, 0.0, 1.0},
      {0.13425359648991364, 0.0, 0.0, 1.0},
      {0.14454844054434846, 0.0, 0.0, 1.0},
      {0.15484328459878333, 0.0, 0.0, 1.0},
      {0.1651381286532182, 0.0, 0.0, 1.0},
      {0.175432972707653, 0.0, 0.0, 1.0},
      {0.18572781676208786, 0.0, 0.0, 1.0},
      {0.1960226608165227, 0.0, 0.0, 1.0},
      {0.20631750487095757, 0.0, 0.0, 1.0},
      {0.2166123489253924, 0.0, 0.0, 1.0},
      {0.22690719297982725, 0.0, 0.0, 1.0},
      {0.2372020370342621, 0.0, 0.0, 1.0},
      {0.24749688108869694, 0.0, 0.0, 1.0},
      {0.2577917251431318, 0.0, 0.0, 1.0},
      {0.2680865691975667, 0.0, 0.0, 1.0},
      {0.2783814132520015, 0.0, 0.0, 1.0},
      {0.28867625730643637, 0.0, 0.0, 1.0},
      {0.2989711013608711, 0.0, 0.0, 1.0},
      {0.30926594541530605, 0.0, 0.0, 1.0},
      {0.31956078946974087, 0.0, 0.0, 1.0},
      {0.3298556335241757, 0.0, 0.0, 1.0},
      {0.3401504775786106, 0.0, 0.0, 1.0},
      {0.3504453216330454, 0.0, 0.0, 1.0},
      {0.36074016568748024, 0.0, 0.0, 1.0},
      {0.37103500974191517, 0.0, 0.0, 1.0},
      {0.38132985379634987, 0.0, 0.0, 1.0},
      {0.3916246978507848, 0.0, 0.0, 1.0},
      {0.4019195419052196, 0.0, 0.0, 1.0},
      {0.41221438595965454, 0.0, 0.0, 1.0},
      {0.42250923001408924, 0.0, 0.0, 1.0},
      {0.43280407406852417, 0.0, 0.0, 1.0},
      {0.4430989181229591, 0.0, 0.0, 1.0},
      {0.4533937621773939, 0.0, 0.0, 1.0},
      {0.4636886062318286, 0.0, 0.0, 1.0},
      {0.47398345028626365, 0.0, 0.0, 1.0},
      {0.48427829434069847, 0.0, 0.0, 1.0},
      {0.4945731383951333, 0.0, 0.0, 1.0},
      {0.5048679824495681, 0.0, 0.0, 1.0},
      {0.5151628265040029, 0.0, 0.0, 1.0},
      {0.5254576705584378, 0.0, 0.0, 1.0},
      {0.5357525146128728, 0.0, 0.0, 1.0},
      {0.5460473586673074, 0.0, 0.0, 1.0},
      {0.5563422027217423, 0.0, 0.0, 1.0},
      {0.5666370467761772, 0.0, 0.0, 1.0},
      {0.576931890830612, 0.0, 0.0, 1.0},
      {0.5872267348850468, 0.0, 0.0, 1.0},
      {0.5975215789394817, 0.0, 0.0, 1.0},
      {0.6078164229939166, 0.0, 0.0, 1.0},
      {0.6181112670483514, 0.0, 0.0, 1.0},
      {0.6284061111027862, 0.0, 0.0, 1.0},
      {0.6387009551572211, 0.0, 0.0, 1.0},
      {0.6489957992116561, 0.0, 0.0, 1.0},
      {0.6592906432660908, 0.0, 0.0, 1.0},
      {0.6695854873205256, 0.0, 0.0, 1.0},
      {0.6798803313749605, 0.0, 0.0, 1.0},
      {0.6901751754293953, 0.0, 0.0, 1.0},
      {0.7004700194838303, 0.0, 0.0, 1.0},
      {0.7107648635382651, 0.0, 0.0, 1.0},
      {0.7210597075926998, 0.0, 0.0, 1.0},
      {0.7313545516471347, 0.0, 0.0, 1.0},
      {0.7416493957015696, 0.0, 0.0, 1.0},
      {0.7519442397560044, 0.0, 0.0, 1.0},
      {0.7622390838104393, 0.0, 0.0, 1.0},
      {0.7725339278648742, 0.0, 0.0, 1.0},
      {0.782828771919309, 0.0, 0.0, 1.0},
      {0.7931236159737438, 0.0, 0.0, 1.0},
      {0.8034184600281785, 0.0, 0.0, 1.0},
      {0.8137133040826136, 0.0, 0.0, 1.0},
      {0.8240081481370484, 0.0, 0.0, 1.0},
      {0.8343029921914833, 0.0, 0.0, 1.0},
      {0.8445978362459181, 0.0, 0.0, 1.0},
      {0.8548926803003528, 0.0, 0.0, 1.0},
      {0.8651875243547877, 0.0, 0.0, 1.0},
      {0.8754823684092226, 0.0, 0.0, 1.0},
      {0.8857772124636573, 0.0, 0.0, 1.0},
      {0.8960720565180923, 0.0, 0.0, 1.0},
      {0.9063669005725272, 0.0, 0.0, 1.0},
      {0.9166617446269619, 0.0, 0.0, 1.0},
      {0.9269565886813969, 0.0, 0.0, 1.0},
      {0.9372514327358317, 0.0, 0.0, 1.0},
      {0.9475462767902666, 0.0, 0.0, 1.0},
      {0.9578411208447014, 0.0, 0.0, 1.0},
      {0.9681359648991361, 0.0, 0.0, 1.0},
      {0.978430808953571, 0.0, 0.0, 1.0},
      {0.9887256530080059, 0.0, 0.0, 1.0},
      {0.9990204970624408, 0.0, 0.0, 1.0},
      {1.0, 0.009314668687192773, 0.0, 1.0},
      {1.0, 0.019608769606337603, 0.0, 1.0},
      {1.0, 0.029902870525482433, 0.0, 1.0},
      {1.0, 0.040196971444627264, 0.0, 1.0},
      {1.0, 0.05049107236377195, 0.0, 1.0},
      {1.0, 0.060785173282916924, 0.0, 1.0},
      {1.0, 0.07107927420206175, 0.0, 1.0},
      {1.0, 0.08137337512120658, 0.0, 1.0},
      {1.0, 0.09166747604035141, 0.0, 1.0},
      {1.0, 0.10196157695949624, 0.0, 1.0},
      {1.0, 0.11225567787864107, 0.0, 1.0},
      {1.0, 0.12254977879778592, 0.0, 1.0},
      {1.0, 0.1328438797169306, 0.0, 1.0},
      {1.0, 0.14313798063607558, 0.0, 1.0},
      {1.0, 0.1534320815552204, 0.0, 1.0},
      {1.0, 0.16372618247436524, 0.0, 1.0},
      {1.0, 0.17402028339351006, 0.0, 1.0},
      {1.0, 0.1843143843126549, 0.0, 1.0},
      {1.0, 0.19460848523179972, 0.0, 1.0},
      {1.0, 0.20490258615094456, 0.0, 1.0},
      {1.0, 0.21519668707008924, 0.0, 1.0},
      {1.0, 0.22549078798923422, 0.0, 1.0},
      {1.0, 0.23578488890837906, 0.0, 1.0},
      {1.0, 0.24607898982752388, 0.0, 1.0},
      {1.0, 0.2563730907466687, 0.0, 1.0},
      {1.0, 0.26666719166581354, 0.0, 1.0},
      {1.0, 0.2769612925849584, 0.0, 1.0},
      {1.0, 0.28725539350410323, 0.0, 1.0},
      {1.0, 0.2975494944232479, 0.0, 1.0},
      {1.0, 0.30784359534239286, 0.0, 1.0},
      {1.0, 0.3181376962615377, 0.0, 1.0},
      {1.0, 0.32843179718068255, 0.0, 1.0},
      {1.0, 0.33872589809982734, 0.0, 1.0},
      {1.0, 0.3490199990189722, 0.0, 1.0},
      {1.0, 0.359314099938117, 0.0, 1.0},
      {1.0, 0.36960820085726187, 0.0, 1.0},
      {1.0, 0.37990230177640666, 0.0, 1.0},
      {1.0, 0.3901964026955515, 0.0, 1.0},
      {1.0, 0.40049050361469607, 0.0, 1.0},
      {1.0, 0.4107846045338412, 0.0, 1.0},
      {1.0, 0.421078705452986, 0.0, 1.0},
      {1.0, 0.4313728063721308, 0.0, 1.0},
      {1.0, 0.44166690729127567, 0.0, 1.0},
      {1.0, 0.4519610082104205, 0.0, 1.0},
      {1.0, 0.46225510912956536, 0.0, 1.0},
      {1.0, 0.47254921004871014, 0.0, 1.0},
      {1.0, 0.482843310967855, 0.0, 1.0},
      {1.0, 0.49313741188699983, 0.0, 1.0},
      {1.0, 0.5034315128061446, 0.0, 1.0},
      {1.0, 0.5137256137252895, 0.0, 1.0},
      {1.0, 0.5240197146444343, 0.0, 1.0},
      {1.0, 0.5343138155635792, 0.0, 1.0},
      {1.0, 0.544607916482724, 0.0, 1.0},
      {1.0, 0.5549020174018688, 0.0, 1.0},
      {1.0, 0.5651961183210134, 0.0, 1.0},
      {1.0, 0.5754902192401584, 0.0, 1.0},
      {1.0, 0.5857843201593033, 0.0, 1.0},
      {1.0, 0.5960784210784481, 0.0, 1.0},
      {1.0, 0.606372521997593, 0.0, 1.0},
      {1.0, 0.6166666229167378, 0.0, 1.0},
      {1.0, 0.6269607238358826, 0.0, 1.0},
      {1.0, 0.6372548247550275, 0.0, 1.0},
      {1.0, 0.6475489256741723, 0.0, 1.0},
      {1.0, 0.6578430265933172, 0.0, 1.0},
      {1.0, 0.6681371275124619, 0.0, 1.0},
      {1.0, 0.6784312284316067, 0.0, 1.0},
      {1.0, 0.6887253293507516, 0.0, 1.0},
      {1.0, 0.6990194302698964, 0.0, 1.0},
      {1.0, 0.7093135311890413, 0.0, 1.0},
      {1.0, 0.7196076321081861, 0.0, 1.0},
      {1.0, 0.7299017330273306, 0.0, 1.0},
      {1.0, 0.7401958339464758, 0.0, 1.0},
      {1.0, 0.7504899348656205, 0.0, 1.0},
      {1.0, 0.7607840357847654, 0.0, 1.0},
      {1.0, 0.7710781367039102, 0.0, 1.0},
      {1.0, 0.7813722376230551, 0.0, 1.0},
      {1.0, 0.7916663385421999, 0.0, 1.0},
      {1.0, 0.8019604394613448, 0.0, 1.0},
      {1.0, 0.8122545403804896, 0.0, 1.0},
      {1.0, 0.8225486412996345, 0.0, 1.0},
      {1.0, 0.8328427422187792, 0.0, 1.0},
      {1.0, 0.843136843137924, 0.0, 1.0},
      {1.0, 0.8534309440570689, 0.0, 1.0},
      {1.0, 0.8637250449762137, 0.0, 1.0},
      {1.0, 0.8740191458953586, 0.0, 1.0},
      {1.0, 0.8843132468145034, 0.0, 1.0},
      {1.0, 0.8946073477336479, 0.0, 1.0},
      {1.0, 0.9049014486527931, 0.0, 1.0},
      {1.0, 0.9151955495719379, 0.0, 1.0},
      {1.0, 0.9254896504910827, 0.0, 1.0},
      {1.0, 0.9357837514102275, 0.0, 1.0},
      {1.0, 0.9460778523293724, 0.0, 1.0},
      {1.0, 0.9563719532485172, 0.0, 1.0},
      {1.0, 0.966666054167662, 0.0, 1.0},
      {1.0, 0.9769601550868069, 0.0, 1.0},
      {1.0, 0.9872542560059517, 0.0, 1.0},
      {1.0, 0.9975483569250966, 0.0, 1.0},
      {1.0, 1.0, 0.011763717646070686, 1.0},
      {1.0, 1.0, 0.02720490955785083, 1.0},
      {1.0, 1.0, 0.04264610146963098, 1.0},
      {1.0, 1.0, 0.05808729338141112, 1.0},
      {1.0, 1.0, 0.07352848529319127, 1.0},
      {1.0, 1.0, 0.08896967720497098, 1.0},
      {1.0, 1.0, 0.10441086911675156, 1.0},
      {1.0, 1.0, 0.1198520610285317, 1.0},
      {1.0, 1.0, 0.13529325294031186, 1.0},
      {1.0, 1.0, 0.150734444852092, 1.0},
      {1.0, 1.0, 0.16617563676387215, 1.0},
      {1.0, 1.0, 0.1816168286756523, 1.0},
      {1.0, 1.0, 0.19705802058743244, 1.0},
      {1.0, 1.0, 0.21249921249921258, 1.0},
      {1.0, 1.0, 0.22794040441099273, 1.0},
      {1.0, 1.0, 0.24338159632277287, 1.0},
      {1.0, 1.0, 0.258822788234553, 1.0},
      {1.0, 1.0, 0.2742639801463332, 1.0},
      {1.0, 1.0, 0.2897051720581133, 1.0},
      {1.0, 1.0, 0.3051463639698935, 1.0},
      {1.0, 1.0, 0.3205875558816736, 1.0},
      {1.0, 1.0, 0.3360287477934533, 1.0},
      {1.0, 1.0, 0.3514699397052339, 1.0},
      {1.0, 1.0, 0.36691113161701405, 1.0},
      {1.0, 1.0, 0.38235232352879417, 1.0},
      {1.0, 1.0, 0.39779351544057434, 1.0},
      {1.0, 1.0, 0.41323470735235446, 1.0},
      {1.0, 1.0, 0.42867589926413463, 1.0},
      {1.0, 1.0, 0.44411709117591475, 1.0},
      {1.0, 1.0, 0.4595582830876949, 1.0},
      {1.0, 1.0, 0.47499947499947504, 1.0},
      {1.0, 1.0, 0.4904406669112552, 1.0},
      {1.0, 1.0, 0.5058818588230354, 1.0},
      {1.0, 1.0, 0.5213230507348154, 1.0},
      {1.0, 1.0, 0.5367642426465956, 1.0},
      {1.0, 1.0, 0.5522054345583758, 1.0},
      {1.0, 1.0, 0.567646626470156, 1.0},
      {1.0, 1.0, 0.5830878183819357, 1.0},
      {1.0, 1.0, 0.5985290102937162, 1.0},
      {1.0, 1.0, 0.6139702022054964, 1.0},
      {1.0, 1.0, 0.6294113941172765, 1.0},
      {1.0, 1.0, 0.6448525860290567, 1.0},
      {1.0, 1.0, 0.6602937779408368, 1.0},
      {1.0, 1.0, 0.6757349698526169, 1.0},
      {1.0, 1.0, 0.6911761617643971, 1.0},
      {1.0, 1.0, 0.7066173536761773, 1.0},
      {1.0, 1.0, 0.7220585455879573, 1.0},
      {1.0, 1.0, 0.7374997374997375, 1.0},
      {1.0, 1.0, 0.7529409294115177, 1.0},
      {1.0, 1.0, 0.7683821213232979, 1.0},
      {1.0, 1.0, 0.7838233132350779, 1.0},
      {1.0, 1.0, 0.7992645051468581, 1.0},
      {1.0, 1.0, 0.8147056970586383, 1.0},
      {1.0, 1.0, 0.830146888970418, 1.0},
      {1.0, 1.0, 0.8455880808821985, 1.0},
      {1.0, 1.0, 0.8610292727939787, 1.0},
      {1.0, 1.0, 0.8764704647057588, 1.0},
      {1.0, 1.0, 0.891911656617539, 1.0},
      {1.0, 1.0, 0.9073528485293191, 1.0},
      {1.0, 1.0, 0.9227940404410993, 1.0},
      {1.0, 1.0, 0.9382352323528794, 1.0},
      {1.0, 1.0, 0.9536764242646596, 1.0},
      {1.0, 1.0, 0.9691176161764397, 1.0},
      {1.0, 1.0, 0.9845588080882198, 1.0},
      {1.0, 1.0, 1.0, 1.0}};

  ImageView::ImageView(QWidget *parent) : QLabel(parent) {
    setAlignment(Qt::AlignCenter);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  }

  ImageView::~ImageView() {
  }
  /**
   * Gets the hot colormap to be used for visualizing grayscale images.
   */
  static QVector<QRgb> getHotColorMap() {
      QVector<QRgb> colorTable(256);
      for (int i = 0; i < 255; i++) {
          const float *color = __HOT_COLORMAP__[i];
          colorTable[i] = qRgb(255*color[0],
                              255*color[1], 
                              255*color[2]);
      }
      return colorTable;
  }
  void ImageView::setTensor(const torch::Tensor &tensor, VizMode mode) {
    int width, height;
    unsigned char* img_data;

    switch(mode) {
      case VIZ_RGB:
        // Get the data from the torch tensor
        width = tensor.size(0);
        height = tensor.size(1);
        img_data = tensor.data_ptr<unsigned char>();
  #ifdef _DEBUG
        std::cout << "Width: " << width << "; Height: " << height << "; data: " << reinterpret_cast<std::uintptr_t>(img_data) << std::endl;
  #endif
        {
          QImage image(img_data, width, height, sizeof(unsigned char)*3*width, QImage::Format_RGB888);
          setPixmap(QPixmap::fromImage(image).scaled(size(), Qt::KeepAspectRatio));
#ifdef _DEBUG
          image.save("/Users/drobotnik/blah.jpg");
#endif
        }
        break;

      case VIZ_HEATMAP:
        width = tensor.size(0);
        height = tensor.size(1);
        img_data = tensor.data_ptr<unsigned char>();
#ifdef _DEBUG
        std::cout << "Width: " << width << "; Height: " << height << "; data: " << reinterpret_cast<std::uintptr_t>(src_data) << std::endl;
#endif
        {   
          // Extract it to a Qt data structure
          QImage greyscale(img_data, width, height, sizeof(unsigned char)*width, QImage::Format_Grayscale8);

          // Rotate it to fit
          QTransform myTransform;
          myTransform.rotate(90);
          greyscale = greyscale.transformed(myTransform).mirrored(true, false);

#ifdef _DEBUG
          rotated.save("/Users/drobotnik/blah2.jpg");
#endif
          // apply the heatmap
          QImage indexed_image = greyscale.convertToFormat(QImage::Format_Indexed8);
          indexed_image.setColorTable(getHotColorMap());
          indexed_image = indexed_image.convertToFormat(QImage::Format_RGB30);

          // Finally display it.
          setPixmap(QPixmap::fromImage(indexed_image).scaled(size(), Qt::KeepAspectRatio));
          
        }
        break;
    }
  }
}
