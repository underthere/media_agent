//
// Created by underthere on 2023/6/21.
//
#include "gst_wrapper.h"

#include <ranges>
#include <unordered_map>

using gst_ctx_creator_t = std::function<tl::expected<GstElement *, MA::Error>(const MA::MediaDescription &)>;

const static std::unordered_map<MA::MediaProtocol, gst_ctx_creator_t> CTX_CREATION_IMPL_MAP = {
    std::pair{MA::MediaProtocol::RTMP, [](const MA::MediaDescription &desc) { return nullptr; }},
    std::pair{MA::MediaProtocol::RTSP, [](const MA::MediaDescription &desc) { return nullptr; }}};

struct element_desc_t {
  std::string factoryname;
  std::string name;
  std::unordered_map<std::string, std::string> properties;
};

auto create_gst_bin(const std::string &name, const std::vector<element_desc_t> &element_descs) -> tl::expected<GstBin *, MA::Error>;

using gst_source_bin_creator_t = std::function<tl::expected<GstBin *, MA::Error>(const MA::MediaDescription &)>;
const static std::unordered_map<MA::MediaProtocol, gst_source_bin_creator_t> SOURCE_BIN_CREATION_IMPL_MAP = {
    std::pair{MA::MediaProtocol::RTMP, [](const MA::MediaDescription &desc) { return nullptr; }},
    std::pair{MA::MediaProtocol::RTSP, [](const MA::MediaDescription &desc) { return nullptr; }}};

static auto gst_init() -> tl::expected<void, MA::Error> {
  gst_init(nullptr, nullptr);
  return {};
}

auto MA::gst_context_create() -> tl::expected<std::shared_ptr<GstContextWrapper>, MA::Error> { return std::shared_ptr<GstContextWrapper>(); }

auto MA::gst_context_add_source(std::shared_ptr<GstContextWrapper> context, const MA::MediaDescription &description)
    -> tl::expected<std::shared_ptr<GstContextWrapper>, MA::Error> {
  if (CTX_CREATION_IMPL_MAP.find(description.protocol) == CTX_CREATION_IMPL_MAP.end()) {
    return tl::unexpected(MA::Error{1, "unknown protocol"});
  }
  if (context->pipeline == nullptr) {
    context->pipeline = gst_pipeline_new(nullptr);
  }
  return CTX_CREATION_IMPL_MAP.at(description.protocol)(description).and_then([&context](GstElement *srcbin) {
    gst_bin_add(GST_BIN(context->pipeline), srcbin);
    context->source = srcbin;
    return tl::expected<std::shared_ptr<GstContextWrapper>, MA::Error>(context);
  });
}

auto MA::gst_context_add_transform(std::shared_ptr<GstContextWrapper> context, const MA::MediaDescription &description)
    -> tl::expected<std::shared_ptr<GstContextWrapper>, MA::Error> {
  return {};
}

auto create_gst_bin(const std::string &name, const std::vector<element_desc_t> &element_descs) -> tl::expected<GstBin *, MA::Error> {
  GstBin *bin = (GstBin *)gst_bin_new(name.c_str());
  std::vector<tl::expected<GstElement *, MA::Error>> elements{element_descs.size()};
  std::transform(element_descs.begin(), element_descs.end(), elements.begin(),
                 [](const element_desc_t &desc) -> tl::expected<GstElement *, MA::Error> {
                   auto elem = gst_element_factory_make(desc.factoryname.c_str(), desc.name.c_str());
                   if (elem == nullptr) {
                     return tl::make_unexpected(MA::Error{1, std::string{"Failed to create element: "} + desc.factoryname});
                   }
                   return elem;
                 });
  decltype(elements) failed_elements;
  std::copy_if(elements.begin(), elements.end(), std::back_inserter(failed_elements),
               [](const tl::expected<GstElement *, MA::Error> &elem) { return !elem.has_value(); });
  if (!failed_elements.empty()) {
    return tl::make_unexpected(MA::Error{1, std::string{"Failed to create bin: "} + name});
  }
  for (auto [desc, elem] : std::views::zip(element_descs, elements)) {
    for (auto [key, value] : desc.properties) {
      GstElementClass *klass = GST_ELEMENT_GET_CLASS(elem.value());
      GParamSpec *pspec = g_object_class_find_property(reinterpret_cast<GObjectClass *>(klass), key.c_str());
      if (pspec == nullptr) {
        return tl::make_unexpected(MA::Error{1, std::string{"Failed to set property: "} + key});
      }
      g_object_set((GObject *)(elem.value()), key.c_str(), value.c_str());
    }
  }
  std::for_each(elements.begin(), elements.end(), [bin](const tl::expected<GstElement *, MA::Error> &elem) { gst_bin_add(bin, elem.value()); });
  if (elements.size() > 1) {
    for (int i = 0; i < elements.size() - 1; ++i) {
      if (gst_element_link(elements[i].value(), elements[i + 1].value()) != TRUE) {
        return tl::make_unexpected(
            MA::Error{1, std::string{"Failed to link elements: "} + element_descs[i].name + " and " + element_descs[i + 1].name});
      }
    }
  }
  return bin;
}
