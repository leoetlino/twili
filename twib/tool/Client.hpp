//
// Twili - Homebrew debug monitor for the Nintendo Switch
// Copyright (C) 2018 misson20000 <xenotoad@xenotoad.net>
//
// This file is part of Twili.
//
// Twili is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Twili is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Twili.  If not, see <http://www.gnu.org/licenses/>.
//

#pragma once

#include<functional>
#include<mutex>
#include<map>

#include "Messages.hpp"
#include "Protocol.hpp"
#include "Buffer.hpp"

namespace twili {
namespace twib {
namespace tool {
namespace client {

class Client {
 public:
	virtual ~Client() = default;
	void SendRequest(Request &&rq, std::function<void(Response)> &&function);
	
	bool deletion_flag = false;
	
 protected:
	virtual void SendRequestImpl(const Request &rq) = 0;
	void PostResponse(protocol::MessageHeader &mh, util::Buffer &payload, util::Buffer &object_ids);
	void FailAllRequests(uint32_t code);
 private:
	std::map<uint32_t, std::function<void(Response r)>> response_map;
	std::mutex response_map_mutex;
	bool failed = false;
	uint32_t fail_code;
};

} // namespace client
} // namespace tool
} // namespace twib
} // namespace twili
