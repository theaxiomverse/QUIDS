#include "network/UPnPClient.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <array>
#include <regex>
#include <spdlog/spdlog.h>

namespace quids::network {

// Use the Impl class defined in the header
UPnPClient::UPnPClient() : impl_(std::make_unique<Impl>()) {}
UPnPClient::~UPnPClient() = default;

std::string UPnPClient::get_external_ip() {
    return impl_->discover_gateway();
}

} // namespace quids::network