#include <EbbGlow/VisualNovel/ScriptLoader/SceneFunctions.h>
#include <EbbGlow/VisualNovel/VisualNovel/MainTextBox.h>
#include <EbbGlow/VisualNovel/VisualNovel/ColorTween.h>
#include <EbbGlow/VisualNovel/VisualNovel/JumpAttachment.h>
#include <EbbGlow/VisualNovel/UI/BackLog.h>
#include <EbbGlow/Utils/Math.h>
#include <EbbGlow/UI/UI.h>

namespace ebbglow::visualnovel
{
	void Scene_TextScene(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 6) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;
		std::vector<std::string> texts;
		texts.reserve(4);
		for (int i = 0; i < 4; ++i)
		{
			texts.push_back(args[i]);
		}

		core::entity textBoxId = loader->world.getEntityManager()->getId();
		if (loader->sceneType == "TextScene")
		{
			loader->exIdList.push_back(textBoxId);
		}
		else
		{
			loader->idList.push_back(textBoxId);
		}

		auto readIt = cfg.readTextSet.find(loader->sceneName);

		loader->world.createUnit(textBoxId, vn::MainTextBoxCom
		(
			Vec2{ cfg.virtualScreenWidth * 0.1666667f, cfg.virtualScreenHeight * 0.75f },
			cfg.virtualScreenWidth * 0.6666667f,
			GetString(args[cfg.mainLanguage], *loader),
			GetString(args[cfg.secondaryLanguage], *loader),
			cfg.fontData, cfg.textSize, cfg.textSize * 0.1, cfg.textSize * 0.3, cfg.textSpeed,
			&(loader->tmpLayers)[cfg.LayerDefine.textBoxLayer],
			cfg.showReadText && readIt != cfg.readTextSet.end() ? cfg.readTextColor : colors::White
		));

		rsc::SharedTexture2D backGround(reinterpret_cast<const char8_t*>(GetString(args[4], *loader).c_str()));
		float scaleX = static_cast<float>(cfg.virtualScreenWidth) / static_cast<float>(backGround.width());
		float scaleY = static_cast<float>(cfg.virtualScreenHeight) / static_cast<float>(backGround.height());

		float bgScale;
		Vec2 bgPosition;

		if (args[5] == "@Cover")
		{
			bgScale = std::max(scaleX, scaleY);
			if (scaleX < scaleY)
			{
				bgPosition = { (cfg.virtualScreenWidth - backGround.width() * bgScale) / 2.0f, 0.0f };
			}
			else
			{
				bgPosition = { 0.0f, (cfg.virtualScreenHeight - backGround.height() * bgScale) / 2.0f };
			}
		}
		else
		{
			bgScale = std::min(scaleX, scaleY);
			if (scaleX > scaleY)
			{
				bgPosition = { (cfg.virtualScreenWidth - backGround.width() * bgScale) / 2.0f, 0.0f };
			}
			else
			{
				bgPosition = { 0.0f, (cfg.virtualScreenHeight - backGround.height() * bgScale) / 2.0f };
			}
		}

		auto idMgr = world.getEntityManager();
		if (backGround.valid())
		{
			loader->idList.push_back(idMgr->getId());
			world.createUnit(loader->idList.back(), ui::ImageBoxExCom{ backGround, bgPosition, &loader->tmpLayers[cfg.LayerDefine.backGroundLayer], bgScale });
		}
		if (cfg.textBoxBackGround.valid())
		{
			float scale = std::max(float(cfg.virtualScreenWidth) / cfg.textBoxBackGround.width(), float(cfg.virtualScreenHeight * (0.25f + 0.03125f)) / cfg.textBoxBackGround.height());
			loader->idList.push_back(idMgr->getId());
			world.createUnit(loader->idList.back(), ui::ImageBoxExCom{ cfg.textBoxBackGround, Vec2{ (cfg.virtualScreenWidth - cfg.textBoxBackGround.width() * scale) / 2, cfg.virtualScreenHeight * (0.75f - 0.03125f)}, &loader->tmpLayers[cfg.LayerDefine.textBoxBackGroundLayer], scale });
		}

		loader->backLogTmp.text = GetString(args[cfg.mainLanguage], *loader) + '\n' + GetString(args[cfg.secondaryLanguage], *loader);
	}

	void Scene_Chr(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.empty()) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;

		float offsetX = cfg.virtualScreenWidth * (1.0f / 12.0f);
		float offsetY = cfg.virtualScreenHeight * 0.0625;
		float x = offsetX * 1.5f;
		float y = cfg.virtualScreenHeight * 0.75f - offsetY;
		float textOffsetX = cfg.chrNameOffsetX * offsetX;
		if (cfg.chrNameBackGround.valid())
		{
			loader->idList.push_back(world.getEntityManager()->getId());
			float scale = static_cast<float>(offsetY) / static_cast<float>(cfg.chrNameBackGround.height());
			world.createUnit(loader->idList.back(), ui::ImageBoxExCom{ cfg.chrNameBackGround, Vec2{ static_cast<float>(x), static_cast<float>(y) }, &loader->tmpLayers[cfg.LayerDefine.textBoxLayer], scale});
		}

		loader->idList.push_back(world.getEntityManager()->getId());
		world.createUnit(loader->idList.back(), ui::TextBoxExCom{ cfg.fontData, GetString(args[0], *loader), Vec2{float(x + textOffsetX), float(y)}, float(cfg.textSize), 0.1f * cfg.textSize, colors::White, &loader->tmpLayers[cfg.LayerDefine.textBoxLayer] });

		loader->backLogTmp.exText = GetString(args[0], *loader);
	}

	void Scene_Button(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 16) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;

		float relativeX = static_cast<float>(GetNumber(args[0], '\0', *loader));
		float relativeY = static_cast<float>(GetNumber(args[1], '\0', *loader));
		float ratio = static_cast<float>(GetNumber(args[2], '\0', *loader));
		float width = static_cast<float>(GetNumber(args[3], '\0', *loader));
		rsc::SharedTexture2D normalImg(reinterpret_cast<const char8_t*>(GetString(args[13], *loader).c_str()));
		rsc::SharedTexture2D hoverImg(reinterpret_cast<const char8_t*>(GetString(args[14], *loader).c_str()));
		rsc::SharedTexture2D pressImg(reinterpret_cast<const char8_t*>(GetString(args[15], *loader).c_str()));
		ColorR8G8B8A8 textColor = 
		{ 
			static_cast<uint8_t>(round(GetNumber(args[9], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[10], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[11], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[12], '\0', *loader)))
		};

		float offsetX = cfg.virtualScreenWidth * relativeX, offsetY = cfg.virtualScreenHeight * relativeY;
		float height = width / ratio;
		if (args[8] == "@Center")
		{
			offsetX -= (width * cfg.virtualScreenWidth) / 2.0f;
			offsetY -= (height * cfg.virtualScreenHeight) / 2.0f;
		}

		core::entity buttonId = world.getEntityManager()->getId();
		if (loader->sceneType == "SelectScene")
		{
			loader->exIdList.push_back(buttonId);
		}
		else
		{
			loader->idList.push_back(buttonId);
		}
		auto text = GetString(args[4 + cfg.uiLanguage], *loader);
		world.createUnit(buttonId, ui::ButtonExCom
			{
				Rect{ Vec2{ offsetX, offsetY }, Vec2{ width * cfg.virtualScreenWidth, height * cfg.virtualScreenWidth } },
				&loader->tmpLayers[cfg.LayerDefine.ButtonLayer],
				normalImg,
				hoverImg,
				pressImg,
				(width * cfg.virtualScreenWidth) / normalImg.width(),
				utils::DynamicLoadFont(cfg.fontData, text, cfg.textSize),
				text,
				textColor,
				static_cast<float>(cfg.textSize),
				cfg.textSize * 0.1f
			});
		world.getMessageManager()->subscribe(buttonId);
	}

	void Scene_ImageBox(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 7) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;
		float relativeX = static_cast<float>(GetNumber(args[0], '\0', *loader));
		float relativeY = static_cast<float>(GetNumber(args[1], '\0', *loader));
		float ratio = static_cast<float>(GetNumber(args[2], '\0', *loader));
		float width = static_cast<float>(GetNumber(args[3], '\0', *loader));
		rsc::SharedTexture2D img(reinterpret_cast<const char8_t*>(GetString(args[4], *loader).c_str()));
		float scale;
		if (args[5] == "@Cover")
		{
			scale = std::max(width * cfg.virtualScreenWidth / img.width(), width / ratio * cfg.virtualScreenWidth / img.height());
		}
		else
		{
			scale = std::min(width * cfg.virtualScreenWidth / img.width(), width / ratio * cfg.virtualScreenWidth / img.height());
		}
		float x, y;
		if (args[6] == "@Center")
		{
			x = (relativeX * cfg.virtualScreenWidth - img.width() * scale / 2);
			y = (relativeY * cfg.virtualScreenHeight - img.height() * scale / 2);
		}
		else
		{
			x = relativeX * cfg.virtualScreenWidth;
			y = relativeY * cfg.virtualScreenHeight;
		}
		loader->idList.push_back(world.getEntityManager()->getId());
		world.createUnit(loader->idList.back(), ui::ImageBoxExCom{ img, Vec2{x, y}, &loader->tmpLayers[cfg.LayerDefine.backGroundLayer], scale });
	}

	void Scene_SetStr(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 2) return;
		std::string* var = GetTextVariable(args[0], *loader);
		if (var == nullptr) return;
		*var = GetString(args[1], *loader);
	}

	void Scene_SetNum(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 2) return;
		if (args[0].empty()) return;
		int32_t offset = 0;
		size_t posOfIndex = args[0].find_last_of('[');
		if (args[0].back() == ']')
		{
			std::string offsetText = args[0].substr(posOfIndex + 1, args[0].length() - posOfIndex - 2);
			offset = static_cast<int32_t>(round(GetNumber(offsetText, '\0', *loader)));
		}
		double* var = GetNumberVariable(args[0], *loader);
		if (var == nullptr) return;
		*var = GetNumber(args[1], '\0', * loader);
	}

	struct KeyFrameAnimationBufCom
	{
		core::entity id = 0;
		ui::KeyFramesAnimationCom com;
	};

	static KeyFrameAnimationBufCom& KeyframeAnimBuffer()
	{
		static thread_local KeyFrameAnimationBufCom buf;
		return buf;
	}

	void Scene_BeginKeyFrameAnimation(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 3) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;
		rsc::SharedTexture2D img(reinterpret_cast<const char8_t*>(GetString(args[0], *loader).c_str()));
		float scale = cfg.virtualScreenWidth / 1920.0f;
		uint8_t layerDepth = static_cast<uint8_t>(GetNumber(args[1], '\0', *loader));
		bool isLoop = args[2] == "@Loop";
		KeyframeAnimBuffer().id = world.getEntityManager()->getId();
		KeyframeAnimBuffer().com = ui::KeyFramesAnimationCom{ img, &loader->tmpLayers, scale, layerDepth, isLoop };
	}

	void Scene_AddKeyFrame(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 9) return;
		auto& world = loader->world;
		auto& animationActive = KeyframeAnimBuffer().com;
		ui::KeyFrame keyFrame;
		float relativeX = static_cast<float>(GetNumber(args[0], '\0', *loader));
		float relativeY = static_cast<float>(GetNumber(args[1], '\0', *loader));
		float originX = static_cast<float>(GetNumber(args[2], '\0', *loader));
		float originY = static_cast<float>(GetNumber(args[3], '\0', *loader));
		float scale = static_cast<float>(GetNumber(args[4], '\0', *loader));
		float rotation = static_cast<float>(GetNumber(args[5], '\0', *loader));
		float duration = static_cast<float>(GetNumber(args[6], '\0', *loader));
		uint8_t alpha = static_cast<uint8_t>(round(GetNumber(args[7], '\0', *loader)));
		keyFrame.origin = Vec2{ originX * animationActive.texture.width(), originY * animationActive.texture.height() };
		keyFrame.duration = duration;
		keyFrame.rotation = rotation;
		keyFrame.scale = scale;
		keyFrame.alpha = alpha;
		if (args[8] == "@Center")
		{
			keyFrame.position = Vec2{ relativeX * loader->cfg.virtualScreenWidth - animationActive.texture.width() * animationActive.scale / 2.0f * scale, relativeY * loader->cfg.virtualScreenHeight - animationActive.texture.height() * animationActive.scale / 2.0f * scale };
		}
		else
		{
			keyFrame.position = Vec2{ relativeX * loader->cfg.virtualScreenWidth, relativeY * loader->cfg.virtualScreenHeight };
		}
		KeyframeAnimBuffer().com.keyFrames.push_back(keyFrame);
	}

	void Scene_EndKeyFrameAnimation(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		auto& buf = KeyframeAnimBuffer();
		loader->idList.push_back(buf.id);
		loader->world.createUnit(buf.id, buf.com);
		buf.id = 0;
		buf.com = ui::KeyFramesAnimationCom();
	}

	void Scene_SetBgm(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		std::string path = GetString(args[0], *loader);
		uint16_t volumeIndex = static_cast<uint16_t>(round(GetNumber(args[1], '\0', *loader)));
		float volume = 1.0f;
		if (volumeIndex < loader->cfg.volumes.size())
		{
			volume = loader->cfg.volumes[volumeIndex];
		}
		loader->musicMgr.SetBgm(rsc::SharedMusic(reinterpret_cast<const char8_t*>(path.c_str())), volume, path, volumeIndex);
	}

	void Scene_SetVoice(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		std::string path = GetString(args[0], *loader);
		uint16_t volumeIndex = static_cast<uint16_t>(round(GetNumber(args[1], '\0', *loader)));
		float volume = 1.0f;
		if (volumeIndex < loader->cfg.volumes.size())
		{
			volume = loader->cfg.volumes[volumeIndex];
		}
		loader->musicMgr.SetVoice(rsc::SharedSound(reinterpret_cast<const char8_t*>(path.c_str())), volume, path, volumeIndex);
	}

	void Scene_ColorTween(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		core::entity id = loader->world.getEntityManager()->getId();
		ColorR8G8B8A8 fromColor =
		{
			static_cast<uint8_t>(round(GetNumber(args[0], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[1], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[2], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[3], '\0', *loader)))
		};
		ColorR8G8B8A8 toColor =
		{
			static_cast<uint8_t>(round(GetNumber(args[4], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[5], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[6], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[7], '\0', *loader)))
		};
		float duration = static_cast<float>(GetNumber(args[8], '\0', *loader));
		uint8_t layerDepth = static_cast<uint8_t>(GetNumber(args[9], '\0', *loader));

		loader->idList.push_back(id);
		loader->world.createUnit(id, vn::ColorTweenCom{ fromColor, toColor, duration, &((loader->tmpLayers)[layerDepth])});
	}

	void Scene_BackLogCom(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 1) return;
		core::entity id = loader->world.getEntityManager()->getId();
		loader->idList.push_back(id);
		loader->world.createUnit(id, BackLogCom{ loader->cfg, &(loader->tmpLayers)[10], GetString(args[0], *loader), *loader});
	}

	void Scene_TextBoxCom(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 3) return;
		core::entity id = loader->world.getEntityManager()->getId();
		loader->idList.push_back(id);
		auto& cfg = loader->cfg;
		std::string text = GetString(args[0], *loader);
		float offsetX = static_cast<float>(GetNumber(args[1], '\0', *loader));
		float offsetY = static_cast<float>(GetNumber(args[2], '\0', *loader));
		float textSize = {};
		if (args.size() >= 3) textSize = GetNumber(args[3], '\0', *loader);
		else textSize = cfg.textSize;
		offsetX *= cfg.virtualScreenWidth;
		offsetY *= cfg.virtualScreenHeight;
		auto font = utils::DynamicLoadFont(cfg.fontData, text, textSize);
		Vec2 offset = utils::MeasureTextSize(font, text, textSize, 0.1f * cfg.textSize);
		loader->world.createUnit(id, ui::TextBoxExCom{ cfg.fontData, text, Vec2{ offsetX - offset.x / 2.0f, offsetY - offset.y / 2.0f }, textSize, 0.1f * textSize, colors::White, &loader->tmpLayers[cfg.LayerDefine.textBoxLayer] });
		//loader->world.createUnit(id, ui::TextBoxExCom{ cfg.fontData, GetString(args[0], *loader), Vec2{ offsetX - offset.x / 2.0f, offsetY - offset.y / 2.0f }, static_cast<float>(cfg.textSize), 0.1f * static_cast<float>(cfg.textSize), colors::White, &loader->tmpLayers[cfg.LayerDefine.textBoxLayer] });
		//loader->world.createUnit(id, ui::TextBoxExCom{ cfg.fontData, GetString(args[0], *loader), Vec2{ static_cast<float>(GetNumber(args[1], '\0', *loader)), static_cast<float>(GetNumber(args[2], '\0', *loader)) }, static_cast<float>(GetNumber(args[3], '\0', *loader)), 0.1f * static_cast<float>(GetNumber(args[3], '\0', *loader)), colors::White, &loader->tmpLayers[cfg.LayerDefine.textBoxLayer] });
	}

	void Scene_JumpButton(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 17) return;
		auto& cfg = loader->cfg;
		auto& world = loader->world;

		float relativeX = static_cast<float>(GetNumber(args[0], '\0', *loader));
		float relativeY = static_cast<float>(GetNumber(args[1], '\0', *loader));
		float ratio = static_cast<float>(GetNumber(args[2], '\0', *loader));
		float width = static_cast<float>(GetNumber(args[3], '\0', *loader));
		rsc::SharedTexture2D normalImg(reinterpret_cast<const char8_t*>(GetString(args[13], *loader).c_str()));
		rsc::SharedTexture2D hoverImg(reinterpret_cast<const char8_t*>(GetString(args[14], *loader).c_str()));
		rsc::SharedTexture2D pressImg(reinterpret_cast<const char8_t*>(GetString(args[15], *loader).c_str()));
		ColorR8G8B8A8 textColor =
		{
			static_cast<uint8_t>(round(GetNumber(args[9], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[10], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[11], '\0', *loader))),
			static_cast<uint8_t>(round(GetNumber(args[12], '\0', *loader)))
		};

		float offsetX = cfg.virtualScreenWidth * relativeX, offsetY = cfg.virtualScreenHeight * relativeY;
		float height = width / ratio;
		if (args[8] == "@Center")
		{
			offsetX -= (width * cfg.virtualScreenWidth) / 2.0f;
			offsetY -= (height * cfg.virtualScreenHeight) / 2.0f;
		}

		core::entity buttonId = world.getEntityManager()->getId();
		if (loader->sceneType == "SelectScene")
		{
			loader->exIdList.push_back(buttonId);
		}
		else
		{
			loader->idList.push_back(buttonId);
		}
		auto text = GetString(args[4 + cfg.uiLanguage], *loader);
		auto sceneName = GetString(args[16], *loader);
		world.createUnit(
			buttonId,
			ui::ButtonExCom
			{
				Rect{ Vec2{ offsetX, offsetY }, Vec2{ width * cfg.virtualScreenWidth, height * cfg.virtualScreenWidth } },
				&loader->tmpLayers[cfg.LayerDefine.ButtonLayer],
				normalImg,
				hoverImg,
				pressImg,
				(width * cfg.virtualScreenWidth) / normalImg.width(),
				utils::DynamicLoadFont(cfg.fontData, text, cfg.textSize),
				text,
				textColor,
				static_cast<float>(cfg.textSize),
				cfg.textSize * 0.1f
			},
			JumpAttachmentCom
			{
				sceneName
			});
		world.getMessageManager()->subscribe(buttonId);
	}
}