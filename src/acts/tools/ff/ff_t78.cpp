#include <includes.hpp>
#include <deps/oodle.hpp>
#include <tools/fastfile.hpp>
#include <tools/fastfile_handlers.hpp>
#include <games/bo4/pool.hpp>

namespace {
	static deps::oodle::Oodle oodle{};

	class T78FFHandler : public fastfile::FFHandler {
	public:
		T78FFHandler() : fastfile::FFHandler("Black Ops 3/4", 0x3030303066664154, fastfile::MASK64) {}

		void LoadFastFile(fastfile::FFAssetPool& pool, fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, const char* file) {
			if (!reader.CanRead(sizeof(fastfile::TXFileHeader))) {
				throw std::runtime_error("Can't read XFile header");
			}

			fastfile::TXFileHeader* header{ reader.Ptr<fastfile::TXFileHeader>() };

			if (header->encrypted) {
				throw std::runtime_error("Can't read encrypted header");
			}

			size_t fastFileSize;

			bool xhashType{};

			// todo: probably we can scan the data and find the chunks
			switch (header->version) {
			case 0x251: // Black ops 3
				fastFileSize = 0x248;
				break;
			case 0x265: // Black ops 4 Dev
				fastFileSize = 0x638;
				break;
			case 0x27E:
			case 0x27F: // Black Ops 4
				fastFileSize = 0x840;
				xhashType = true;
				break;
			default:
				throw std::runtime_error(std::format("Fast file version not supported: 0x{:x}", header->version));
			}

			if (!reader.CanRead(fastFileSize)) {
				throw std::runtime_error("Can't read XFile header");
			}

			reader.Goto(fastFileSize);

			std::vector<byte>& ffdata{ pool.CreateMemoryBuffer() };

			size_t idx{};
			while (true) {
				size_t loc{ reader.Loc() };

				fastfile::DBStreamHeader* block{ reader.ReadPtr<fastfile::DBStreamHeader>() };
				if (!block->alignedSize) break; // end

				if (block->offset != loc) {
					LOG_ERROR("bad block position: 0x{:x} != 0x{:x}", loc, block->offset);
					break;
				}
				idx++;

				if (!block->uncompressedSize) {
					reader.Align(0x800000);
					continue;
				}

				byte* blockBuff{ reader.ReadPtr<byte>(block->alignedSize) };


				LOG_TRACE("Decompressing {} block 0x{:x} (0x{:x}/0x{:x} -> 0x{:x})", 
					fastfile::GetFastFileCompressionName(header->compression), loc,
					block->compressedSize, block->alignedSize, block->uncompressedSize
				);

				size_t unloc{ utils::Allocate(ffdata, block->uncompressedSize) };
				byte* decompressed{ &ffdata[unloc] };

				switch (header->compression) {
				case fastfile::XFILE_UNCOMPRESSED:
					if (block->uncompressedSize > block->compressedSize) {
						throw std::runtime_error(std::format("Can't decompress block, decompressed size isn't big enough: 0x{:x} != 0x{:x}", block->compressedSize, block->uncompressedSize));
					}
					memcpy(decompressed, blockBuff, block->uncompressedSize);
					break;
				case fastfile::XFILE_ZLIB:
				case fastfile::XFILE_ZLIB_HC: {

					uLongf sizef = (uLongf)block->uncompressedSize;
					uLongf sizef2{ (uLongf)block->compressedSize };
					int ret;
					if (ret = uncompress2(decompressed, &sizef, blockBuff, &sizef2) < 0) {
						throw std::runtime_error(std::format("error when decompressing {}", zError(ret)));
					}
					break;
				}
				case fastfile::XFILE_OODLE_KRAKEN:
				case fastfile::XFILE_OODLE_MERMAID:
				case fastfile::XFILE_OODLE_SELKIE:
				case fastfile::XFILE_OODLE_LZNA: {
					if (!oodle && !oodle.LoadOodle(deps::oodle::OO2CORE_6)) {
						throw std::runtime_error("Oodle is required to read this fastfile, did you put oo2core_6_win64.dll inside the deps directory?");
					}
					int ret{ oodle.Decompress(blockBuff, block->compressedSize, decompressed, block->uncompressedSize, deps::oodle::OODLE_FS_YES) };

					if (ret != block->uncompressedSize) {
						throw std::runtime_error(std::format("Can't decompress block, returned size isn't the expected one: 0x{:x} != 0x{:x}", ret, block->uncompressedSize));
					}
					break;
				}
				default:
					throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)header->compression));
				}
			}

			LOG_TRACE("Decompressed 0x{:x} byte(s) from 0x{:x} block(s)", ffdata.size(), idx);


			core::bytebuffer::ByteBuffer buff{ ffdata };

			if (opt.m_fd) {
				std::filesystem::path fdfile{ file };

				fdfile.replace_extension(".fd");
				const std::string filenameFd{ fdfile.string() };

				std::vector<byte> fileFDBuff{};
				if (opt.ReadFile(filenameFd.data(), fileFDBuff)) {
					core::bytebuffer::ByteBuffer bufffd{ fileFDBuff };

					// todo: handle fd file
				}
			}

			if (opt.dump_decompressed) {
				std::filesystem::path decfile{ file };

				decfile.replace_extension(".ff.dec");

				if (!utils::WriteFile(decfile, ffdata)) {
					LOG_ERROR("Can't dump {}", decfile.string());
				} else{
					LOG_INFO("Dump into {}", decfile.string());
				}
			}

			// read asset data
			struct XAsset_0 {
				byte type;
				uintptr_t header;
			};

			struct AssetList {
				uint32_t numStrings;
				uintptr_t strings;
				uint32_t assetCount;
				uintptr_t assets;
			};

			AssetList* assetList{ buff.ReadPtr<AssetList>() };

			if (!assetList->assetCount) {
				LOG_TRACE("No assets in ff");
				return;
			}

			// skip list
			buff.ReadPtr<void*>(assetList->numStrings);

			LOG_TRACE("{:x}", buff.Loc());

			while (true) {
				if (!buff.CanRead(sizeof(XAsset_0) * assetList->assetCount)) {
					LOG_ERROR("Can't find assets start");
					return;
				}

				XAsset_0* ptr{ buff.Ptr<XAsset_0>() };

				size_t i{};

				size_t end{ std::min<size_t>(assetList->assetCount, 4) };

				while (i < end) {
					if (ptr[i].header != 0xFFFFFFFFFFFFFFFF) {
						break;
					}
					i++;
				}

				if (i != end) {
					buff.Skip(1); // next byte
					continue;
				}
				break;
			}

			XAsset_0* assets{ buff.Ptr<XAsset_0>() };
			for (size_t i = 0; i < assetList->assetCount; i++) {
				LOG_DEBUG("{} {:x}", (int)assets[i].type, assets[i].header);
			}



			// todo: read buffer
		}
	};

	utils::ArrayAdder<T78FFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
}