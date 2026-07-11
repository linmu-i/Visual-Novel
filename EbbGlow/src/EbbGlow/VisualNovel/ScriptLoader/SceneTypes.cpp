#include <EbbGlow/VisualNovel/ScriptLoader/SceneTypes.h>
#include <EbbGlow/Utils/Input.h>
#include <EbbGlow/VisualNovel/ScriptLoader/SaveLoad.h>
#include <chrono>

namespace ebbglow::visualnovel
{
	static void GotoBackLogSupport(ScriptLoader& scLoader)
	{
		if (input::MouseWheelDelta() > 0)
		{
			scLoader.retName = scLoader.sceneName;
			scLoader.loadScene(scLoader.backLogSceneName);
		}
	}

	static void GoToSaveSupport(ScriptLoader& scLoader)
	{
		if (input::KeyPressed(input::Keyboard::F5))
		{
			scLoader.retName = scLoader.sceneName;
			scLoader.tmpLayers[0].push_back(std::make_unique<ImageExporter>(&scLoader.tmpLayers, scLoader.cfg));
			ExportArchiveInfo(scLoader);
			scLoader.loadScene(scLoader.saveSceneName);
		}
	}

	static void GotoLoadSupport(ScriptLoader& scLoader)
	{
		if (input::KeyPressed(input::Keyboard::F9))
		{
			scLoader.retName = scLoader.sceneName;
			scLoader.loadScene(scLoader.loadSceneName);
		}
	}

	void TextSceneSystem::update()
	{
		bool processed = false;
		bool thisFrameClicked = false;
		
		if (!clicked)
		{
			if (input::MousePressed(input::MouseButton::Left) ||
				input::KeyDown(input::Keyboard::LeftControl)  ||
				input::KeyDown(input::Keyboard::LeftControl)  ||
				input::KeyPressed(input::Keyboard::Space)     ||
				input::MouseWheelDelta() < 0
				) thisFrameClicked = true;
			for (auto& p : input::PointList())
			{
				if (input::PointPressed(p.id))
				{
					thisFrameClicked = true;
					break;
				}
			}

		}
		if (thisFrameClicked)
		{
			clicked = true;
		}
		else
		{
			clicked = false;
		}

		coms->active()->forEach([this, &processed](core::entity id, TextSceneCom& comActive)
			{
				if (clicked)
				{
					buttonComs->active()->forEach([this, &processed](core::entity id, ui::ButtonExCom& comActive)
						{
							if (!processed)
							{
								auto msgs = world->getMessageManager()->getMessageList(id);
								for (auto& msg : *msgs)
								{
									if (msg->getType() == pressMsgId)
									{
										processed = true;
										break;
									}
								}
							}
						});
					if (!processed)
					{
						if (!scLoader->exIdList.empty())
						{
							if (textBoxComs->active()->get(scLoader->exIdList.back())->activePixels >= textBoxComs->active()->get(scLoader->exIdList.back())->totalPixel)
							{
								auto next = rsc::SharedFile::Iterator(scLoader->scriptData.getSize(), scLoader->scriptData.getData(), scLoader->sceneView.find(scLoader->sceneArgs.front())->second);
								scLoader->loadScene(next);
							}
							else
							{
								world->getSystem<vn::MainTextBoxSystem>()->skip(scLoader->exIdList.back());
							}
						}
						else
						{
							auto next = rsc::SharedFile::Iterator(scLoader->scriptData.getSize(), scLoader->scriptData.getData(), scLoader->sceneView.find(scLoader->sceneArgs.front())->second);
							scLoader->loadScene(next);
						}
					}
					clicked = false;
				}

				GotoBackLogSupport(*scLoader);
				GoToSaveSupport(*scLoader);
				GotoLoadSupport(*scLoader);
			});

		
	}

	void SceneType_TextScene(ScriptLoader* scLoader, std::vector<std::string> args) noexcept
	{
		//if (!scLoader->backLogTmp.empty()) scLoader->addToBackLog(std::move(scLoader->backLogTmp));
		//scLoader->backLogTmp.clear();

		auto id = scLoader->world.getEntityManager()->getId();
		scLoader->world.createUnit(id, TextSceneCom());
		scLoader->idList.push_back(id);
	}


	/*
	void SelectSceneSystem::update()
	{
		coms->active()->forEach([this](core::entity id, SelectSceneCom& comActive)
			{
				for (size_t i = 0; i < scLoader->exIdList.size(); ++i)
				{
					auto msgs = world->getMessageManager()->getMessageList(scLoader->exIdList[i]);
					for (auto& msg : *msgs)
					{
						if (msg->getType() == releaseMsgId)
						{
							auto it = scLoader->sceneView.find(scLoader->sceneArgs[i]);
							if (it == scLoader->sceneView.end()) break;
							auto scIt = rsc::SharedFile::Iterator(scLoader->scriptData.getSize(), scLoader->scriptData.getData(), it->second);
							
							auto selectI18nIt = scLoader->i18nText.find("ExLogText_Select");
							std::string exLogTextSelect;
							if (selectI18nIt != scLoader->i18nText.end())
							{
								exLogTextSelect = selectI18nIt->second[scLoader->cfg.uiLanguage];
							}
							else
							{
								exLogTextSelect = reinterpret_cast<const char*>(u8"选择:");
							}
							
							//if (!scLoader->backLogTmp.empty()) scLoader->addToBackLog(std::move(scLoader->backLogTmp));
							//BackLogView view;
							//view.exText = exLogTextSelect;
							//view.text = world->getDoubleBuffer<ui::ButtonExCom>()->active()->get(msg->getSender())->text;
							//scLoader->backLogTmp = std::move(view);

							scLoader->loadScene(scIt);
							return;
						}
					}
				}

				GotoBackLogSupport(*scLoader);
			});
	}

	void SceneType_SelectScene(ScriptLoader* scLoader, std::vector<std::string> args) noexcept
	{
		auto id = scLoader->world.getEntityManager()->getId();
		scLoader->world.createUnit(id, SelectSceneCom());
		scLoader->idList.push_back(id);
	}

	*/

	void DelaySceneSystem::update()
	{
		coms->active()->forEach([this](core::entity id, DelaySceneCom& activeCom)
			{
				auto& inactiveCom = *coms->inactive()->get(id);
				if (activeCom.timeCount > activeCom.delay)
				{
					auto it = scLoader->sceneView.find(scLoader->sceneArgs[0]);
					if (it == scLoader->sceneView.end()) return;
					auto scIt = rsc::SharedFile::Iterator(scLoader->scriptData.getSize(), scLoader->scriptData.getData(), it->second);
					scLoader->loadScene(scIt);
				}
				inactiveCom = activeCom;
				inactiveCom.timeCount += utils::GetFrameTime() * 1000;//To ms
			});
	}

	void SceneType_DelayScene(ScriptLoader* scLoader, std::vector<std::string> args) noexcept
	{
		float delay = 0.0f;
		try
		{
			delay = std::stof(args[1]);
		}
		catch (...)
		{
			delay = 0.0f;
		}

		auto id = scLoader->world.getEntityManager()->getId();
		scLoader->world.createUnit(id, DelaySceneCom(delay));
		scLoader->idList.push_back(id);
	}

	void SceneType_BlankScene(ScriptLoader* scLoader, std::vector<std::string> args) noexcept {}
}