#include <iostream>
#include <gst/gst.h>

int main() {
  gst_init(nullptr, nullptr);

  const gchar* mediaFormat = "video/h264";  // Specify the media format you want to decode

  GstRegistry* registry = gst_registry_get();
  GstPluginFeature* feature = gst_registry_find_feature(registry, mediaFormat, GST_ELEMENT_FACTORY_TYPE_VIDEO_ENCODER);

  if (feature) {
    GstElementFactory* factory = GST_ELEMENT_FACTORY(feature);
    const gchar* elementName = gst_element_factory_get_metadata(factory, "name");
    std::cout << "Best decoder element for " << mediaFormat << ": " << elementName << std::endl;
    gst_object_unref(feature);
  } else {
    std::cout << "No decoder element found for " << mediaFormat << std::endl;
  }

  gst_deinit();

  return 0;
}
