/*
 * Copyright (C) 2018-2020 Rerrah
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

#include "pattern.hpp"
#include "effect.hpp"
#include "misc.hpp"
#include <algorithm>

Pattern::Pattern(int n, size_t defSize)
	: num_(n), size_(defSize), steps_(defSize), usedCnt_(0)
{
}

Pattern::Pattern(int n, size_t size, std::vector<Step> steps)
	: num_(n), size_(size), steps_(steps), usedCnt_(0)
{
}

Step& Pattern::getStep(int n)
{
	return steps_.at(static_cast<size_t>(n));
}

size_t Pattern::getSize() const
{
	for (size_t i = 0; i < size_; ++i) {
		for (int j = 0; j < Step::N_EFFECT; ++j) {
			if (!steps_[i].hasEffectValue(j)) continue;
			// "SoundSource::FM" is dummy, these effects are not related with sound source
			switch (effect_utils::validateEffectId(SoundSource::FM, steps_[i].getEffectId(j))) {
			case EffectType::PositionJump:
			case EffectType::SongEnd:
			case EffectType::PatternBreak:
				return i + 1;
			default:
				break;
			}
		}
	}
	return size_;
}

void Pattern::changeSize(size_t size)
{
	if (size && size <= MAX_STEPS) {
		size_ = size;
		if (steps_.size() < size) steps_.resize(size);
	}
}

void Pattern::insertStep(int n)
{
	if (n < static_cast<int>(size_))
		steps_.emplace(steps_.begin() + n);
}

void Pattern::deletePreviousStep(int n)
{
	if (!n) return;

	steps_.erase(steps_.begin() + n - 1);
	if (steps_.size() < size_)
		steps_.resize(size_);
}

bool Pattern::existCommand() const
{
	auto endIt = steps_.begin() + static_cast<int>(size_);
	return std::any_of(steps_.begin(), endIt,
					   [](const Step& step) { return step.existCommand(); });
}

std::vector<int> Pattern::getEditedStepIndices() const
{
	std::vector<int> list;
	for (size_t i = 0; i < size_; ++i) {
		if (steps_.at(i).existCommand())
			list.push_back(static_cast<int>(i));
	}
	return list;
}

std::unordered_set<int> Pattern::getRegisteredInstruments() const
{
	std::unordered_set<int> set;
	for (size_t i = 0; i < size_; ++i) {
		const Step& step = steps_.at(i);
		if (step.hasInstrument()) set.insert(step.getInstrumentNumber());
	}
	return set;
}

Pattern Pattern::clone(int asNumber)
{
	return Pattern(asNumber, size_, steps_);
}

void Pattern::transpose(int seminotes, std::vector<int> excludeInsts)
{
	for (size_t i = 0; i < size_; ++i) {
		Step& step = steps_.at(i);
		int note = step.getNoteNumber();
		if (step.hasGeneralNote() && std::none_of(excludeInsts.begin(), excludeInsts.end(),
									  [a = step.getInstrumentNumber()](int b) { return a == b; })) {
			step.setNoteNumber(clamp(note + seminotes, 0, 95));
		}
	}
}

void Pattern::clear()
{
	steps_ = std::vector<Step>(size_);
}
