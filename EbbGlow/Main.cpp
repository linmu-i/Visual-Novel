#define _CRT_SECURE_NO_WARNINGS

#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/VisualNovel/VisualNovel.h>
#include <EbbGlow/Utils/Control.h>

//const char* FONT_PATH = R"(D:\Downloads\GSdefaultfonts.ttf)";//
const char* FONT_PATH = R"(resource\font\Noto_Sans_SC\static\NotoSansSC-SemiBold.ttf)";
ebbglow::vn::VisualNovelConfig CFG;

int main()
{
	ebbglow::SetConfigFlag(ebbglow::flags::WindowUndecorated);
	ebbglow::Init(0, 0, "");
	ebbglow::SetTargetFPS(120);
	CFG.textSpeed = 1.0f;
	CFG.fontData = ebbglow::rsc::SharedFile(FONT_PATH);
	CFG.textSize = 32;
	CFG.readTextColor = { 170, 230, 255, 255 };
	CFG.mainLanguage = 0;
	CFG.secondaryLanguage = 2;
	CFG.volumes.push_back(0.8f);
	CFG.secondLanguageShow = true;

	auto ScX = ebbglow::utils::ScreenSize().x;
	auto ScY = ebbglow::utils::ScreenSize().y;

	if (ScX / ScY > 16.0f / 9.0f)
	{
		CFG.virtualScreenHeight = static_cast<int>(ScY);
		CFG.virtualScreenWidth = static_cast<int>(ScY * (16.0f / 9.0f));
		CFG.drawOffset = { (ScX - CFG.virtualScreenWidth) / 2.0f, 0.0f };
		CFG.drawRatio = static_cast<float>(CFG.virtualScreenHeight) / 1080.0f;
	}
	else
	{
		CFG.virtualScreenWidth = static_cast<int>(ScX);
		CFG.virtualScreenHeight = static_cast<int>(ScX / (16.0f / 9.0f));
		CFG.drawOffset = { 0.0f, (ScY - CFG.virtualScreenHeight) / 2.0f };
		CFG.drawRatio = static_cast<float>(CFG.virtualScreenWidth) / 1920.0f;
	}

	auto& mainLibRsc = ebbglow::visualnovel::GetMainLibRsc();
	mainLibRsc.textBoxBackGround = ebbglow::rsc::SharedTexture2D("resource\\img\\TextBoxBackground.png");
	mainLibRsc.chrNameBackGround = ebbglow::rsc::SharedTexture2D("resource\\img\\ChrBoxBackground.png");
	mainLibRsc.chrNameOffsetX = 0.5f;

	ebbglow::core::World2D mainWorld(ScX, ScY);
	mainWorld.addSystem<ebbglow::vn::MusicManager>(ebbglow::vn::MusicManager(CFG));
	ebbglow::vn::ScriptLoader scLoader(mainWorld, CFG, mainWorld.getSystem<ebbglow::vn::MusicManager>());
	ebbglow::vn::ApplyVisualNovel(mainWorld, CFG, scLoader);
	ebbglow::vn::ImportMainLib(scLoader);

	std::vector<int> cdpt;
	cdpt.resize(128);
	for (int i = 0; i < 128; ++i)
	{
		cdpt[i] = i;
	}
	ebbglow::rsc::SharedFont asciiFont = ebbglow::utils::DynamicLoadFont(CFG.fontData, cdpt, 64);

	auto loaded = scLoader.init("Script.txt");

	loaded.get();

	scLoader.start();

	auto DrawAvrFPS = [&asciiFont]()
		{
			static float FPSArr[100] = {0};
			static int idx = 0;
			FPSArr[idx] = 1 / ebbglow::GetFrameTime();
			++idx;
			idx %= 100;
			float avrFPS = 0.0f;
			for (int i = 0; i < 100; ++i)
			{
				avrFPS += FPSArr[i];
			}
			ebbglow::gfx::DrawText(asciiFont, std::to_string(static_cast<int>(avrFPS / 100.0f)), { 5.0f, 40.0f }, 32, 3);
		};

	while (!ebbglow::WindowShouldClose())
	{
		mainWorld.update();
		ebbglow::BeginDrawing();
		ebbglow::gfx::ClearBackground(ebbglow::colors::Black);
		mainWorld.draw();
		//ebbglow::gfx::DrawText(asciiFont, std::to_string(static_cast<int>(1 / ebbglow::GetFrameTime())), { 5.0f, 5.0f }, 32, 3);
		DrawAvrFPS();
		ebbglow::EndDrawing();
	}
	
	return 0;
}