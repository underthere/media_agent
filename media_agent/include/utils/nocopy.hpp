//
// Created by underthere on 2023/8/14.
//

#ifndef MEDIA_AGENT_NOCOPY_HPP
#define MEDIA_AGENT_NOCOPY_HPP

class nocopy {
    public:
    nocopy() = default;
    nocopy(const nocopy&) = delete;
    nocopy& operator=(const nocopy&) = delete;
};

#endif  // MEDIA_AGENT_NOCOPY_HPP
