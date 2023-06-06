#pragma once
#include <bit>
#include <cstdint>
#include "Assert.h"


namespace Engine {

	template<size_t kBitCount>
	class Bitset {
		static_assert(kBitCount > 0, "A bitset with 0 bits is not possible.");
	public:
		Bitset() {
			Reset();
		}

		friend std::ostream& operator<<(std::ostream& os, const Bitset& bitset) {
			for (size_t i = kBitCount; i > 0; --i) {
				os << bitset.Test(i - 1) ? '1' : '0';
			}
			return os;
		}
		/// <summary>
		/// 先頭から探す
		/// </summary>
		/// <param name="value">値</param>
		/// <returns></returns>
		size_t FindFirst(bool value = false) {
			for (size_t i = 0; i < kWordCount; ++i) {
				if (words_[i] != (value ? 0 : ~0)) {
					size_t bitIndex = static_cast<size_t>(value ? std::countr_zero(words_[i]) : std::countr_one(words_[i]));
					if (bitIndex < kWordSize) {
						return i * kWordSize + bitIndex;
					}
				}
			}
			return kWordCount * kWordSize;
		}
		/// <summary>
		/// 指定したビットの後ろから探す
		/// </summary>
		/// <param name="index"></param>
		/// <param name="value"></param>
		/// <returns></returns>
		size_t FindNext(size_t index, bool value = false) {

		}
		/// <summary>
		/// すべて反転
		/// </summary>
		/// <returns></returns>
		inline Bitset& Flip() {
			for (auto& word : words_) {
				word = ~word;
			}
			return *this;
		}
		/// <summary>
		/// 指定したビットを反転
		/// </summary>
		/// <param name="bitIndex"></param>
		/// <returns></returns>
		inline Bitset& Flip(size_t bitIndex) {
			auto& word = GetWord(bitIndex);
			word = word ^ static_cast<uint64_t>(1) << (bitIndex & kWordMask);
			return *this;
		}
		/// <summary>
		/// すべてのビットを変更
		/// </summary>
		/// <param name="value"></param>
		/// <returns></returns>
		inline Bitset& Set(bool value = true) {
			memset(words_, value ? ~0 : 0, sizeof(words_));
			return *this;
		}
		/// <summary>
		/// 指定したビットを変更
		/// </summary>
		/// <param name="bitIndex"></param>
		/// <param name="value"></param>
		/// <returns></returns>
		inline Bitset& Set(size_t bitIndex, bool value = true) {
			ASSERT_MSG(bitIndex < kBitCount, "Out of range");
			uint64_t& word = GetWord(bitIndex);
			if (value) {
				word |= static_cast<uint64_t>(1) << (bitIndex & kWordMask);
			}
			else {
				word &= ~(static_cast<uint64_t>(1) << (bitIndex & kWordMask));
			}
			return *this;
		}
		/// <summary>
		/// すべてのビットを0にする
		/// </summary>
		/// <returns></returns>
		inline Bitset& Reset() {
			Set(false);
			return *this;
		}
		/// <summary>
		/// 指定したビットを0にする
		/// </summary>
		/// <param name="bitIndex"></param>
		/// <returns></returns>
		inline Bitset& Reset(size_t bitIndex) {
			Set(bitIndex, false);
			return *this;
		}
		/// <summary>
		/// 指定したビットを判別
		/// </summary>
		/// <param name="bitIndex"></param>
		/// <returns></returns>
		inline bool Test(size_t bitIndex) const {
			ASSERT_MSG(bitIndex < kBitCount, "Out of range");
			return (GetWord(bitIndex) & (static_cast<uint64_t>(1) << (bitIndex & kWordMask))) != 0;
		}
		inline bool All(bool value = true) const {
			for (auto& word : words_) {
				if (word != (value ? ~0 : 0)) { return false; }
			}
			return true;
		}
		/// <summary>
		/// ビット数
		/// </summary>
		/// <returns></returns>
		inline size_t GetBitCount() const { return kBitCount; }

	private:
		inline uint64_t& GetWord(size_t bitIndex) {
			return words_[bitIndex >> kBitIndexToWordIndex];
		}
		inline const uint64_t& GetWord(size_t bitIndex) const {
			return words_[bitIndex >> kBitIndexToWordIndex];
		}

		static constexpr size_t kWordSize = CHAR_BIT * sizeof(uint64_t);
		static constexpr size_t kWordCount = (kBitCount - 1) / kWordSize + 1;
		static constexpr size_t kWordMask = kWordSize - 1;
		static constexpr size_t kBitIndexToWordIndex = 6;

		uint64_t words_[kWordCount];
	};


}
