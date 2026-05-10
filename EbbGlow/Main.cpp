#define _CRT_SECURE_NO_WARNINGS

#include <EbbGlow/Graphics/Graphics.h>
#include <EbbGlow/VisualNovel/VisualNovel.h>
#include <EbbGlow/Utils/Control.h>

//const char* FONT_PATH = R"(D:\Downloads\GSdefaultfonts.ttf)";//
//const char* FONT_PATH = R"(resource/font/Noto_Sans_SC/static/NotoSansSC-SemiBold.ttf)";
ebbglow::vn::VisualNovelConfig CFG;

int main()
{
	
	ebbglow::visualnovel::ReadVisualNovelConfig(u8"config/config.ini", CFG);

	if (CFG.win.fullScreen)
	{
		ebbglow::SetConfigFlag(ebbglow::flags::WindowUndecorated);
		ebbglow::Init(0, 0, CFG.win.title);
		ebbglow::SetTargetFPS(120);
	}
	else
	{
		ebbglow::Init(CFG.win.width, CFG.win.height, CFG.win.title);
		ebbglow::SetTargetFPS(120);
	}

	ebbglow::visualnovel::CalculateVirtualScreen(CFG);


	auto& mainLibRsc = ebbglow::visualnovel::GetMainLibRsc();
	mainLibRsc.textBoxBackGround = ebbglow::rsc::SharedTexture2D("resource/img/TextBoxBackground.png");
	mainLibRsc.chrNameBackGround = ebbglow::rsc::SharedTexture2D("resource/img/ChrBoxBackground.png");
	mainLibRsc.chrNameOffsetX = 0.5f;

	ebbglow::core::World2D mainWorld(ebbglow::utils::ScreenSize().x, ebbglow::utils::ScreenSize().y);
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
	ebbglow::rsc::SharedFont asciiFont = ebbglow::utils::DynamicLoadFont(CFG.text.fontData, cdpt, 64);

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
		DrawAvrFPS();
		ebbglow::EndDrawing();
	}
	
	return 0;
}