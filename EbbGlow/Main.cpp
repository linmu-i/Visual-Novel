#define _CRT_SECURE_NO_WARNINGS

#include <EbbGlow/VisualNovel/VisualNovel.h>
#include <EbbGlow/Utils/Control.h>

const char* FONT_PATH = R"(resource\font\Noto_Sans_SC\static\NotoSansSC-SemiBold.ttf)";
ebbglow::vn::VisualNovelConfig CFG;

int main()
{
	ebbglow::SetConfigFlag(ebbglow::flags::WindowUndecorated);
	ebbglow::Init(0, 0, "");
	ebbglow::SetTargetFPS(120);
	CFG.textSpeed = 1.0f;
	CFG.fontData = ebbglow::rsc::SharedFile(FONT_PATH);
	CFG.textBoxBackGround = ebbglow::rsc::SharedTexture2D("resource\\img\\TextBoxBackground.png");
	CFG.chrNameBackGround = ebbglow::rsc::SharedTexture2D("resource\\img\\ChrBoxBackground.png");
	CFG.textSize = 40;
	CFG.readTextColor = { 170, 230, 255, 255 };
	CFG.chrNameOffsetX = 0.5f;
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
	}
	else
	{
		CFG.virtualScreenWidth = static_cast<int>(ScX);
		CFG.virtualScreenHeight = static_cast<int>(ScX / (16.0f / 9.0f));
		CFG.drawOffset = { 0.0f, (ScY - CFG.virtualScreenHeight) / 2.0f };
	}

	ebbglow::core::World2D mainWorld(ScX, ScY);
	mainWorld.addSystem<ebbglow::vn::MusicManager>(ebbglow::vn::MusicManager(CFG));
	ebbglow::vn::ScriptLoader scLoader(mainWorld, CFG, mainWorld.getSystem<ebbglow::vn::MusicManager>());
	ebbglow::vn::ApplyVisualNovel(mainWorld, CFG, scLoader);
	ebbglow::vn::ImportMainLib(scLoader);

	auto loaded = scLoader.init("Script.txt");

	loaded.get();

	scLoader.start();

	while (!ebbglow::WindowShouldClose())
	{
		mainWorld.update();
		ebbglow::BeginDrawing();
		ebbglow::gfx::ClearBackground(ebbglow::colors::Black);
		mainWorld.draw();
		ebbglow::EndDrawing();
	}
	
	return 0;
}