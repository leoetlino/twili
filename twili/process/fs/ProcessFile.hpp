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

#include<stdlib.h>
#include<stdint.h>

namespace twili {
namespace process {
namespace fs {

class ProcessFile {
 public:
	virtual ~ProcessFile() = default;
	virtual size_t Read(size_t offset, size_t size, uint8_t *out) = 0;
	virtual size_t GetSize() = 0;
};

} // namespace fs
} // namespace process
} // namespace twili
