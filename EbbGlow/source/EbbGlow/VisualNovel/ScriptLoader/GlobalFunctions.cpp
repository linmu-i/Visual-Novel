#include <EbbGlow/VisualNovel/ScriptLoader/GlobalFunctions.h>

namespace ebbglow::visualnovel
{
	void Global_String(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.empty()) return;
		size_t length = 1;
		if (args.size() > 1)
		{
			length = static_cast<size_t>(round(GetNumber(args[1], '\0', *loader)));
		}
		loader->textView.emplace(args[0], VariableView{ loader->textStorage.size(), length });
		loader->textStorage.resize(loader->textStorage.size() + length);
	}
	void Global_Number(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.empty()) return;
		size_t length = 1;
		if (args.size() > 1)
		{
			length = static_cast<size_t>(round(GetNumber(args[1], '\0', *loader)));
		}
		loader->numberView.emplace(args[0], VariableView{ loader->textStorage.size(), length });
		loader->numberStorage.resize(loader->numberStorage.size() + length);
	}
	void Global_Include(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.empty()) return;
		ScriptData tmp(GetString(args.front(), *loader));
		size_t idx = loader->scIt.position();
		loader->scriptData += tmp;
		loader->scIt = rsc::SharedFile::Iterator(loader->scriptData.getSize(), loader->scriptData.getData(), idx);
	}
	void Global_BeginScene(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.empty()) return;
		loader->beginScene = GetString(args.front(), *loader);
	}
	void Global_I18nText(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 2) return;
		std::vector<std::string> text;
		text.reserve(args.size() - 1);
		for (size_t i = 1; i < args.size(); ++i)
		{
			text.push_back(GetString(args[i], *loader));
		}
		loader->i18nText.emplace(args[0], std::move(text));
	}
	void Global_BackLogScene(ScriptLoader* loader, const std::vector<std::string>& args) noexcept
	{
		if (args.size() < 1) return;
		loader->backLogScene = GetString(args[0], *loader);
	}
}