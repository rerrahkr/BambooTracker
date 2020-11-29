/*
 * Copyright (C) 2019-2020 Rerrah
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include "instruments_manager.hpp"
#include "binary_container.hpp"
#include "file_io.hpp"

namespace io
{
	class AbstractInstrumentIO
	{
	public:
		AbstractInstrumentIO(std::string ext, std::string desc, bool loadable, bool savable)
			: ext_(ext), desc_(desc), loadable_(loadable), savable_(savable) {}
		virtual ~AbstractInstrumentIO() = default;
		virtual AbstractInstrument* load(const BinaryContainer& ctr, const std::string& fileName,
										 std::weak_ptr<InstrumentsManager> instMan,
										 int instNum) const;
		virtual void save(BinaryContainer& ctr,
						  const std::weak_ptr<InstrumentsManager> instMan, int instNum) const;
		inline std::string getExtension() const { return ext_; }
		inline std::string getFilterText() const { return desc_ + "(*." + ext_ + ")"; }
		inline bool isLoadable() const { return loadable_; }
		inline bool isSavable() const { return savable_; }

	private:
		std::string ext_, desc_;
		bool loadable_, savable_;
	};

	class InstrumentIO
	{
	public:
		static InstrumentIO& getInstance();

		void saveInstrument(BinaryContainer& ctr,
							const std::weak_ptr<InstrumentsManager> instMan, int instNum);
		AbstractInstrument* loadInstrument(const BinaryContainer& ctr,
										   const std::string& path,
										   std::weak_ptr<InstrumentsManager> instMan,
										   int instNum);

		inline bool testLoadableFormat(const std::string& ext) const
		{
			return handler_.testLoadableExtension(ext);
		}

		inline bool testSavableFormat(const std::string& ext) const
		{
			return handler_.testSavableExtension(ext);
		}

		inline std::vector<std::string> getLoadFilter() const
		{
			return handler_.getLoadFilterList();
		}

		inline std::vector<std::string> getSaveFilter() const
		{
			return handler_.getSaveFilterList();
		}

	private:
		InstrumentIO();

		static std::unique_ptr<InstrumentIO> instance_;
		FileIOManagerMap<AbstractInstrumentIO> handler_;

	public:
		static inline int convertDTInTFIVGIDMP(int dt)
		{
			switch (dt) {
			case 0:		return 7;
			case 1:		return 6;
			case 2:		return 5;
			case 3:		return 0;
			case 4:		return 1;
			case 5:		return 2;
			case 6:		return 3;
			case 7:		return 3;
			default:	throw std::out_of_range("Out of range dt");
			}
		}
	};
}
