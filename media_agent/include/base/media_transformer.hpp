#ifndef __MEDIA_AGENT_MEDIA_TRANSFORMER_HPP__
#define __MEDIA_AGENT_MEDIA_TRANSFORMER_HPP__

namespace MA {

class MediaTransformer {
 public:
  MediaTransformer() {}
  virtual ~MediaTransformer() {}

  virtual int transform() = 0;

 protected:
    
};

}  // namespace MA

#endif  // __MEDIA_AGENT_MEDIA_TRANSFORMER_HPP__