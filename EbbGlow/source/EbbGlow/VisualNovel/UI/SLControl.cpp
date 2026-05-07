#include <iomanip>

#include <EbbGlow/VisualNovel/UI/SLControl.h>
#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/Utils/Math.h>
#include <EbbGlow/Components/MessageCallback.h>

namespace ebbglow::visualnovel
{
	void SLBlockDraw::draw()
	{
		auto& cfg = *sl.cfg;
		float scale = cfg.drawRatio;
		gfx::DrawRect(Rect{ sl.pos, Vec2{cfg.virtualScreenWidth * 0.4f, cfg.virtualScreenHeight / 6.0f} }, 0xddddddff);
		gfx::DrawRect(Rect{ sl.pos, Vec2{cfg.virtualScreenWidth * 0.4f, 21 * scale} }, sl.color);
		gfx::DrawRect(Rect{sl.pos.x, sl.pos.y - 16.0f * scale, 200.0f * scale, 16.0f * scale}, sl.color);
		gfx::DrawTriangle(Vec2{ sl.pos.x + 200.0f * scale, sl.pos.y - 16.0f * scale }, Vec2{ sl.pos.x + 200.0f * scale, sl.pos.y }, Vec2{ sl.pos.x + 216.0f * scale, sl.pos.y }, sl.color);
		gfx::DrawRectLines(Rect{ sl.pos, Vec2{cfg.virtualScreenWidth * 0.4f, cfg.virtualScreenHeight / 6.0f} }, sl.color, 3.0f * scale);
		

		if (sl.image.valid()) gfx::DrawTextureRegionToRegion(sl.image, Rect{ 0.0f, 0.0f, sl.image.width(), sl.image.height() }, Rect{ sl.pos + sl.imgOffset * scale, Vec2{ 256, 144 } *scale }, Vec2{ 0, 0 }, scale);
		else gfx::DrawRect(Rect{ sl.pos + sl.imgOffset * scale , Vec2{ 256, 144 } * scale }, 0xAAAAAAFF);

		gfx::DrawText(sl.font, sl.indexStr, sl.pos + Vec2{ 4, -8 } *scale, 24.0f * scale, 2.0f * scale);

		if (!sl.attText.empty()) gfx::DrawText(sl.font, sl.attText, sl.pos + sl.attTxtOffset * scale, sl.textSize * scale, sl.textSize * scale * 0.1f);
		if (!sl.time.empty()) gfx::DrawText(sl.font, sl.time, sl.pos + sl.timeOffset * scale, sl.textSize * scale, sl.textSize * scale * 0.1f);

		if (sl.textCodepoints.size() > 0)
			gfx::DrawTextCodepoints(sl.font, sl.textCodepoints[0], sl.pos + sl.txtLine0Offset * scale, sl.textSize * scale, sl.textSize * scale * 0.1f);
		if (sl.textCodepoints.size() > 1)
			gfx::DrawTextCodepoints(sl.font, sl.textCodepoints[1], sl.pos + sl.txtLine1Offset * scale, sl.textSize * scale, sl.textSize * scale * 0.1f);

		
	}

	void SLBlockSystem::update()
	{
		coms->active()->forEach([](core::entity id, SLBlock& act)
			{
				act.layer->push_back(std::make_unique<SLBlockDraw>(act));
			});
	}

	static void CreateSLBlocks(ScriptLoader& scLoader, const std::vector<core::entity>& ids, std::vector<std::optional<ArchiveInfo>>& infoList,
		SOrL sOrL, uint32_t index, core::Layer* layer, Color blockColor)
	{
		if (ids.size() < 8) return;
		auto& world = scLoader.world;
		auto& cfg = scLoader.cfg;
		auto& idMgr = *world.getEntityManager();
		auto& mainLibRsc = GetMainLibRsc();

		float scale = cfg.drawRatio;
		Vec2 blockBeginVec = Vec2{ 173, 240 } * scale;
		float xOffset = 786 * scale;
		float yOffset = 212 * scale;

		for (size_t i = 0; i < 4; ++i)
		{
			for (size_t j = 0; j < 2; ++j)
			{
				size_t indexOffset = i * 2 + j;
				Vec2 pos = Vec2{ blockBeginVec.x + j * xOffset, blockBeginVec.y + i * yOffset };
				const ArchiveInfo* info = nullptr;
				size_t infoIndex = indexOffset + index;
				if (infoIndex < infoList.size())
				{
					if (infoList[infoIndex].has_value())
					{
						info = &infoList[infoIndex].value();
					}
				}

				std::ostringstream oss;
				oss << std::setw(3) << std::setfill('0') << infoIndex;
				std::string indexStr = oss.str();

				if (info)
				{
					std::string text = GetString(info->exText, scLoader) + GetString(info->text, scLoader);
					tm timeInfo = {};
#ifdef _WIN32
					localtime_s(&timeInfo, &info->time);
#else
					localtime_r(&info->time, &timeInfo);
#endif
					oss.str("");
					oss.clear();
					oss << std::put_time(&timeInfo, "%Y/%m/%d %H:%M:%S");
					std::string timeStr = oss.str();

					auto font = utils::DynamicLoadFont(cfg.fontData, text + info->attachmentText + "0123456789/:", SLBlock::textSize * scale);
					auto cdpts = utils::TextLineCalculateWithWordWrap(
						text, SLBlock::textSize * scale, SLBlock::textSize * scale * 0.1f, font,
						SLBlock::txtMaxLength * scale);

					world.createUnit(ids[indexOffset],
						SLBlock
						{
							pos, rsc::SharedTexture(info->screenShoot), blockColor,
							GetString(info->attachmentText, scLoader), cdpts, timeStr, font, &cfg, layer, indexStr	
						});
				}
				else
				{
					world.createUnit(ids[indexOffset],
						SLBlock
						{
							pos, {}, blockColor,
							"", {}, "", utils::DynamicLoadFont(cfg.fontData, "0123456789", SLBlock::textSize * scale),& cfg, layer, indexStr
						});
				}

				if (sOrL == SOrL::Save)
				{
					world.createUnit(ids[indexOffset],
						SaveAttachment
						{
							&infoList, cfg.getSavePath(infoIndex), indexOffset
						},
						ui::ButtonExCom
						{
							Rect{ pos + cfg.drawOffset, Vec2{cfg.virtualScreenWidth * 0.4f, cfg.virtualScreenHeight / 6.0f} *scale },
							Rect{ pos, Vec2{cfg.virtualScreenWidth * 0.4f, cfg.virtualScreenHeight / 6.0f} *scale },
							layer, {}, {}, {}, {}, "", colors::Blank, 0.0f, 0.0f
						});
					world.getMessageManager()->subscribe(ids[indexOffset]);
				}
				else if (sOrL == SOrL::Load)
				{
					world.createUnit(ids[indexOffset],
						LoadAttachment
						{
							&infoList, cfg.getSavePath(infoIndex), indexOffset
						},
						ui::ButtonExCom
						{
							Rect{ pos + cfg.drawOffset, Vec2{cfg.virtualScreenWidth * 0.4f, cfg.virtualScreenHeight / 6.0f} *scale },
							Rect{ pos, Vec2{cfg.virtualScreenWidth * 0.4f, cfg.virtualScreenHeight / 6.0f} *scale },
							layer, {}, {}, {}, {}, "", colors::Blank, 0.0f, 0.0f
						});
					world.getMessageManager()->subscribe(ids[indexOffset]);
				}
			}
		}
	}

	void SLControlSystem::update()
	{
		auto& mainLibRsc = GetMainLibRsc();
		auto& world = scLoader->world;
		if (updateInfoListFlag)
		{
			updateInfoListFlag = false;
			for (size_t i = 0; i < scLoader->cfg.maxSaveCount; ++i)
			{
				std::filesystem::path path = scLoader->cfg.getSavePath(i);
				if (std::filesystem::exists(path))
				{
					if (i >= infoList.size())
					{
						infoList.resize(i + 1);
					}
					ArchiveInfo info{};
					if (!ReadArchiveInfo(path, info))
					{
						infoList[i] = std::nullopt;
					}
					else
					{
						infoList[i] = std::move(info);
					}
				}
				else
				{
					if (i < infoList.size())
					{
						infoList[i] = std::nullopt;
					}
				}
			}
		}

		if (saveFlag || loadFlag)
		{
			SOrL sOrL = saveFlag ? SOrL::Save : SOrL::Load;
			Color blockColor = 0x66ccffff;//saveFlag ? 0x66ccffff : 0x66ccff99;
			saveFlag = false;
			loadFlag = false;
			if (rebuildFlag)
			{
				rebuildFlag = false;
				for (auto id : idList)
				{
					world.deleteUnit(id);
				}
				CreateSLBlocks(*scLoader, idList, infoList, sOrL, beginIndex, &scLoader->tmpLayers[MainLibRsc::LayerDefine.SLBlockLayer], blockColor);
			}
			else
			{
				std::vector<core::entity> ids;
				for (int i = 0; i < 8; ++i)
				{
					auto id = scLoader->world.getEntityManager()->getId();
					ids.push_back(id);
					scLoader->idList.push_back(id);
				}
				CreateSLBlocks(*scLoader, ids, infoList, sOrL, beginIndex, &scLoader->tmpLayers[MainLibRsc::LayerDefine.SLBlockLayer], blockColor);

				idList = std::move(ids);
			}
		}
	}

	void SaveAttachmentSystem::update()
	{
		coms->active()->forEach([this](core::entity id, SaveAttachment& act)
			{
				auto& ina = *coms->inactive()->get(id);
				auto* msgList = scLoader->world.getMessageManager()->getMessageList(id);
				if (!msgList) return;
				for (auto& msg : *msgList)
				{
					if (msg->getType() == btReleaseId)
					{
						ArchiveInfo info = GetArchiveInfoBuffer();
						info.time = time(nullptr);
						SaveToFile(act.filePath, *scLoader, info);
						slCtrlSys->registerUpdateInfoList();
						slCtrlSys->registerBuildSave();
						slCtrlSys->registerRebuild();
					}
				}
			});
	}

	void LoadAttachmentSystem::update()
	{
		coms->active()->forEach([this](core::entity id, LoadAttachment& act)
			{
				auto& ina = *coms->inactive()->get(id);
				auto* msgList = scLoader->world.getMessageManager()->getMessageList(id);
				if (!msgList) return;
				for (auto& msg : *msgList)
				{
					if (msg->getType() == btReleaseId)
					{
						ArchiveInfo info;
						LoadFromFile(act.filePath, *scLoader, info);
					}
				}
			});
	}
}