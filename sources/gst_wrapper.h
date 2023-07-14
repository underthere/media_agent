//
// Created by underthere on 2023/6/21.
//

#ifndef MEDIA_AGENT_GST_WRAPPER_H
#define MEDIA_AGENT_GST_WRAPPER_H

#include <gst/gst.h>

#include <memory>

#include "expected.h"
#include "media_common.h"

namespace MA {
struct GstContextWrapper {
  GstElement *pipeline;
  GstElement *source;
  GstElement *sink;
  GstElement *transform;
};

auto gst_context_create() -> tl::expected<std::shared_ptr<GstContextWrapper>, Error>;

auto gst_context_add_source(std::shared_ptr<GstContextWrapper> context, const MediaDescription &description)
    -> tl::expected<std::shared_ptr<GstContextWrapper>, Error>;

auto gst_context_add_transform(std::shared_ptr<GstContextWrapper> context, const MediaDescription &description)
    -> tl::expected<std::shared_ptr<GstContextWrapper>, Error>;

}  // namespace MA

#endif  // MEDIA_AGENT_GST_WRAPPER_H
