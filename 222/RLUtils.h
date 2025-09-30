#pragma once

#include "raylib.h"
#include "raylibRAII.h"
#include <vector>

Font LoadFont_cn(const char* fileName, int fontSize) {
	// ������Ҫ���ص��ַ���Χ
	std::vector<std::pair<int, int>> ranges = {
		// ����ASCII�ַ� (32-126)
		{32, 126},

		// ���ĳ����ַ�
		{0x4E00, 0x9FFF},   // CJKͳһ���ֻ�����
		{0x3400, 0x4DBF},   // CJK��չA��
		{0x3000, 0x303F},   // ���ı�����

		// ȫ���ַ� (����ȫ����ĸ�����֡����)
		{0xFF01, 0xFF5E},   // ȫ��ASCII����

		// �������ķ���֧��
		{0x2010, 0x2027},   // ���ַ�����Ŀ���ŵ�
		{0x2E80, 0x2EFF},   // CJK���ײ���
		{0x3000, 0x303F}    // �ظ�ȷ������
	};

	// �ռ��������
	std::vector<int> codepoints;

	for (const auto& range : ranges) {
		for (int i = range.first; i <= range.second; i++) {
			codepoints.push_back(i);
		}
	}

	// ��Ӷ��ⳣ���ַ���ȷ�����ǣ�
	const int extraChars[] = {
		0x00B7,  // ����� (��)
		0x2014,  // ���ۺ� (��)
		0x2018,  // ������ (��)
		0x2019,  // �ҵ����� (��)
		0x201C,  // ��˫���� (��)
		0x201D,  // ��˫���� (��)
		0x2026,  // ʡ�Ժ� (��)
		0x3001,  // �ٺ� (��)
		0x3002,  // ��� (��)
		0xFF0D,  // ȫ�����ַ� (��)
		0xFFE5   // ����ҷ��� (��)
	};

	for (int ch : extraChars) {
		codepoints.push_back(ch);
	}

	// ��������
	Font font = LoadFontEx(fileName, fontSize, codepoints.data(), static_cast<int>(codepoints.size()));

	// ���������������Ϊ˫����(�����Ⱦ����)
	SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

	return font;
}

const char* shaderText = R"(
#version 330

in vec2 fragTexCoord;//
in vec4 fragColor; 

out vec4 finalColor0;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

void main() {
	// ��ȡSDF�����alphaͨ�������볡ֵ��
	float sdfValue = texture(texture0, fragTexCoord).a;
	
	// ����SDF�㷨�������볡ת��Ϊƽ����alphaֵ
	float smoothing = 0.02;  // ���Ʊ�Ե������
	float alpha = smoothstep(0.42 - smoothing, 0.42 + smoothing, sdfValue);
	
	// Ӧ����ɫ��͸����
	vec4 texelColor = vec4(fragColor.rgb, fragColor.a * alpha);
	
	// �������
	finalColor0 = texelColor * colDiffuse;
}
)";



Font LoadSDFFontEx(const char* fontInfoFile, const char* textureFile) {
	// �����ļ�����
	int bytesRead = 0;
	unsigned char* data = LoadFileData(fontInfoFile, &bytesRead);

	if (!data || bytesRead < sizeof(int) * 3) {
		TraceLog(LOG_ERROR, "FONT IMPORT: Invalid font data file");
		if (data) UnloadFileData(data);
		return GetFontDefault();
	}

	Font font = { 0 };
	unsigned char* ptr = data;

	// ��ȡͷ��
	font.baseSize = *(int*)ptr; ptr += sizeof(int);
	font.glyphCount = *(int*)ptr; ptr += sizeof(int);
	font.glyphPadding = *(int*)ptr; ptr += sizeof(int);

	// �������������
	int expectedSize = sizeof(int) * 3 + font.glyphCount * (sizeof(int) * 4 + sizeof(float) * 4);
	if (bytesRead != expectedSize) {
		TraceLog(LOG_ERROR, "FONT IMPORT: Data size mismatch (%d vs %d)", bytesRead, expectedSize);
		UnloadFileData(data);
		return GetFontDefault();
	}

	// �����ڴ�
	font.recs = (Rectangle*)RL_MALLOC(font.glyphCount * sizeof(Rectangle));
	font.glyphs = (GlyphInfo*)RL_MALLOC(font.glyphCount * sizeof(GlyphInfo));

	// ��ȡ�ַ�����
	for (int i = 0; i < font.glyphCount; i++) {
		// GlyphInfo ����
		font.glyphs[i].value = *(int*)ptr; ptr += sizeof(int);
		font.glyphs[i].offsetX = *(int*)ptr; ptr += sizeof(int);
		font.glyphs[i].offsetY = *(int*)ptr; ptr += sizeof(int);
		font.glyphs[i].advanceX = *(int*)ptr; ptr += sizeof(int);
		font.glyphs[i].image = Image{ 0 }; // ���ͼ������

		// Rectangle ����
		font.recs[i].x = *(float*)ptr; ptr += sizeof(float);
		font.recs[i].y = *(float*)ptr; ptr += sizeof(float);
		font.recs[i].width = *(float*)ptr; ptr += sizeof(float);
		font.recs[i].height = *(float*)ptr; ptr += sizeof(float);
	}
	// ��������
	font.texture = LoadTexture(textureFile);
	SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR); // SDF����

	UnloadFileData(data);

	TraceLog(LOG_INFO, "FONT IMPORT: Loaded %d glyphs from %s", font.glyphCount, fontInfoFile);
	return font;
}

namespace fontUtils
{
	static const char* SDFShaderText = R"(
#version 330

in vec2 fragTexCoord;//
in vec4 fragColor; 

out vec4 finalColor0;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

void main() {
    // ��ȡSDF�����alphaͨ�������볡ֵ��
    float sdfValue = texture(texture0, fragTexCoord).a;
    
    // ����SDF�㷨�������볡ת��Ϊƽ����alphaֵ
    float smoothing = 0.01;  // ���Ʊ�Ե������
    float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, sdfValue);
    
    // Ӧ����ɫ��͸����
    vec4 texelColor = vec4(fragColor.rgb, fragColor.a * alpha);
    
    // �������
    finalColor0 = texelColor * colDiffuse;
}
)";

	
}

void DrawSDFText(Font SDFFont, const char* text, Vector2 position, float fontSize, float spacing, Color tInt)
{
	static rlRAII::ShaderRAII SDFShader{ LoadShaderFromMemory(nullptr, fontUtils::SDFShaderText) };
	BeginShaderMode(SDFShader.get());
	DrawTextEx(SDFFont, text, position, fontSize, spacing, tInt);
	EndShaderMode();
}

std::vector<int> GetUnicodePoints(const char* text) {
	std::vector<int> codePoints;

	while (*text != '\0') {
		uint8_t lead = static_cast<uint8_t>(*text);

		// ���ֽ��ַ� (ASCII)
		if (lead < 0x80) {
			codePoints.push_back(lead);
			text++;
			continue;
		}

		// ���ֽ��ַ�����
		int numBytes = 0;
		int codePoint = 0;

		// ȷ���ֽ����ͳ�ʼ����
		if ((lead & 0xE0) == 0xC0) {  // 2�ֽ�
			numBytes = 2;
			codePoint = lead & 0x1F;
		}
		else if ((lead & 0xF0) == 0xE0) {  // 3�ֽ�
			numBytes = 3;
			codePoint = lead & 0x0F;
		}
		else if ((lead & 0xF8) == 0xF0) {  // 4�ֽ�
			numBytes = 4;
			codePoint = lead & 0x07;
		}
		else {
			// ��Ч�����ֽڣ�ʹ���滻�ַ�������
			codePoints.push_back(0xFFFD);
			text++;
			continue;
		}

		// �������ֽ���Ч��
		bool valid = true;
		for (int i = 1; i < numBytes; ++i) {
			text++;
			if (*text == '\0' || (static_cast<uint8_t>(*text) < 0x80 || (static_cast<uint8_t>(*text) > 0xBF))) {
				valid = false;
				break;
			}
			codePoint = (codePoint << 6) | (static_cast<uint8_t>(*text) & 0x3F);
		}

		// ������Ч����
		if (!valid) {
			// ���˵���������
			text -= (numBytes - 1) - 1;  // �ص������ֽ�λ��
			codePoints.push_back(0xFFFD);
			text++;
			continue;
		}

		// ��֤��㷶Χ
		bool rangeValid = true;
		if (numBytes == 2 && codePoint < 0x80) rangeValid = false;
		else if (numBytes == 3 && codePoint < 0x800) rangeValid = false;
		else if (numBytes == 4 && (codePoint < 0x10000 || codePoint > 0x10FFFF)) rangeValid = false;

		if (rangeValid) {
			codePoints.push_back(codePoint);
		}
		else {
			codePoints.push_back(0xFFFD);
		}

		text++;  // �ƶ�����һ���ַ�
	}

	return codePoints;
}

Font DynamicLoadFont(const char* text, const char* fontPath, float fontSize)
{
	int codepointCount = 0;
	//std::vector<int> codepoints = GetUnicodePoints(text);
	int* codepoints = LoadCodepoints(text, &codepointCount);
	Font font = LoadFontEx(fontPath, fontSize, codepoints, codepointCount);
	UnloadCodepoints(codepoints);
	SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
	return font;
}

Font DynamicLoadFontFromMemory(const char* text, const char* fileName, unsigned char* fontData, int dataSize, float fontSize)
{
	int codepointCount = 0;
	//std::vector<int> codepoints = GetUnicodePoints(text);
	int* codepoints = LoadCodepoints(text, &codepointCount);
	const char* p = fileName;
	while (*p != '\0') 
	{
		++p;
	}
	do
	{
		--p;
	} while (*p != '.' && p != fileName);
	Font font = LoadFontFromMemory(p, fontData, dataSize, fontSize, codepoints, codepointCount);
	UnloadCodepoints(codepoints);
	SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
	return font;
}



std::string GenerateGaussianBlurShaderCode(int radius)
{
	// �����˹��Ȩ��
	std::vector<float> weights;
	float sum = 0.0f;

	for (int i = -radius; i <= radius; i++)
	{
		float weight = std::exp(-(i * i) / (2.0f * radius * radius));
		weights.push_back(weight);
		sum += weight;
	}

	// ��һ��Ȩ��
	for (auto& weight : weights)
	{
		weight /= sum;
	}

	// ������ɫ������
	std::string shaderCode = R"(
#version 330

// ���� uniforms
uniform sampler2D inputTexture;
uniform vec2 direction;
uniform vec2 textureSize;

// ���붥������
in vec2 fragTexCoord;
out vec4 finalColor;

// ��˹��Ȩ��
const float weights[)";

	shaderCode += std::to_string(weights.size());
	shaderCode += "] = float[";
	shaderCode += std::to_string(weights.size());
	shaderCode += "](";

	for (size_t i = 0; i < weights.size(); i++)
	{
		shaderCode += std::to_string(weights[i]);
		if (i < weights.size() - 1)
			shaderCode += ", ";
	}

	shaderCode += R"();

void main()
{
    vec2 texOffset = 1.0 / textureSize;
    vec4 result = vec4(0.0);
    
    for (int i = -)";

	shaderCode += std::to_string(radius);
	shaderCode += "; i <= ";
	shaderCode += std::to_string(radius);
	shaderCode += R"(; ++i)
    {
        vec2 offset = texOffset * float(i) * direction;
        result += texture(inputTexture, fragTexCoord + offset) * weights[i + )";
	shaderCode += std::to_string(radius);
	shaderCode += R"(];
    }
    
    finalColor = result;
}
)";

	return shaderCode;
}

// ��˹ģ������ - ֧�ֿɵ��뾶
rlRAII::Texture2DRAII TextureBlurGaussian(const Texture2D& inputTexture, int radius)
{

	// ��֤�뾶ֵ
	if (radius < 1) radius = 1;
	if (radius > 20) radius = 20; // �������޷�ֹ��������

	// ��������RenderTexture�������д��ͻ
	RenderTexture2D horizontalBlurRT = LoadRenderTexture(inputTexture.width, inputTexture.height);
	RenderTexture2D verticalBlurRT = LoadRenderTexture(inputTexture.width, inputTexture.height);


	// ����������ģʽΪCLAMP����ֹ��Ե��������
	SetTextureWrap(horizontalBlurRT.texture, TEXTURE_WRAP_CLAMP);
	SetTextureWrap(verticalBlurRT.texture, TEXTURE_WRAP_CLAMP);

	// ������ɫ������
	std::string shaderCode = GenerateGaussianBlurShaderCode(radius);

	// ���ڴ������ɫ��
	Shader blurShader = LoadShaderFromMemory(nullptr, shaderCode.c_str());

	// ��ȡ��ɫ��uniformλ��
	int directionLoc = GetShaderLocation(blurShader, "direction");
	int textureSizeLoc = GetShaderLocation(blurShader, "textureSize");
	int inputTextureLoc = GetShaderLocation(blurShader, "inputTexture");


	// ��������ߴ�uniform
	float textureSize[2] = { (float)inputTexture.width, (float)inputTexture.height };
	SetShaderValue(blurShader, textureSizeLoc, textureSize, SHADER_UNIFORM_VEC2);

	// ��һ��: ˮƽģ��
	float horizontal[2] = { 1.0f, 0.0f };
	SetShaderValue(blurShader, directionLoc, horizontal, SHADER_UNIFORM_VEC2);

	// ��Ⱦ��ˮƽģ��RenderTexture
	BeginTextureMode(horizontalBlurRT);
	ClearBackground(BLANK);
	BeginShaderMode(blurShader);

	// ������������
	SetShaderValueTexture(blurShader, inputTextureLoc, inputTexture);

	// ����ȫ������Ӧ����ɫ��
	DrawTexture(inputTexture, 0, 0, WHITE);

	EndShaderMode();
	EndTextureMode();

	// �ڶ���: ��ֱģ��
	float vertical[2] = { 0.0f, 1.0f };
	SetShaderValue(blurShader, directionLoc, vertical, SHADER_UNIFORM_VEC2);

	// ��Ⱦ����ֱģ��RenderTexture��ʹ��ˮƽģ���Ľ����Ϊ���룩
	BeginTextureMode(verticalBlurRT);
	ClearBackground(BLANK);
	BeginShaderMode(blurShader);

	// ʹ��ˮƽģ���Ľ����Ϊ����
	SetShaderValueTexture(blurShader, inputTextureLoc, horizontalBlurRT.texture);

	// ����ȫ������Ӧ����ɫ��
	DrawTexture(horizontalBlurRT.texture, 0, 0, WHITE);

	EndShaderMode();
	EndTextureMode();

	// �ؼ�����: ��ȡ�����ÿ�RenderTexture����������
	Texture2D blurredTexture = verticalBlurRT.texture;
	verticalBlurRT.texture = {}; // �ÿգ���ֹUnloadRenderTexture�ͷ�����

	// ж�ز�����Ҫ����Դ
	UnloadShader(blurShader);
	UnloadRenderTexture(horizontalBlurRT);
	UnloadRenderTexture(verticalBlurRT); // ����ͷ�֡���������������ͷ�����

	// ʹ����ȡ��������RAII����
	return rlRAII::Texture2DRAII(blurredTexture);
}

std::string CodepointToUtf8(int codepoint) {
	std::string result;

	if (codepoint <= 0x7F) {
		// 1 �ֽ� UTF-8
		result += static_cast<char>(codepoint);
	}
	else if (codepoint <= 0x7FF) {
		// 2 �ֽ� UTF-8
		result += static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F));
		result += static_cast<char>(0x80 | (codepoint & 0x3F));
	}
	else if (codepoint <= 0xFFFF) {
		// 3 �ֽ� UTF-8
		result += static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F));
		result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
		result += static_cast<char>(0x80 | (codepoint & 0x3F));
	}
	else if (codepoint <= 0x10FFFF) {
		// 4 �ֽ� UTF-8
		result += static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07));
		result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
		result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
		result += static_cast<char>(0x80 | (codepoint & 0x3F));
	}
	else {
		// ��Ч�� Unicode ���
		result += '?';
	}

	return result;
}

// ���������ת��Ϊ UTF-8 �ַ��� (std::string �汾)
std::string CodepointsToString(int* codepoints, int codepointsCount) {
	std::string result;

	for (int i = 0; i < codepointsCount; i++) {
		result += CodepointToUtf8(codepoints[i]);
	}

	return result;
}

std::vector<std::vector<int>> TextLineCaculateWithWordWrap(std::string text, float fontSize, float spacing, const Font& font, float maxLength)
{
	int codepointsCount;
	int* codepoints = LoadCodepoints(text.c_str(), &codepointsCount);

	std::vector<std::vector<int>> result;
	int begin = 0;

	while (begin < codepointsCount)
	{
		int tmpCount = codepointsCount - begin;
		float lineLengthTmp;
		int lastSpace = -1; // ��¼���һ���ո��λ��

		// ���ȳ����ҵ����һ���ո�λ��
		for (int i = 0; i < tmpCount; i++)
		{
			if (codepoints[begin + i] == ' ' || codepoints[begin + i] == '\n')
			{
				lastSpace = i;
			}
		}

		// ������ǰ�еĳ���
		do
		{
			std::string lineText = CodepointsToString(codepoints + begin, tmpCount);
			lineLengthTmp = MeasureTextEx(font, lineText.c_str(), fontSize, spacing).x;

			if (lineLengthTmp > maxLength)
			{
				// ����пո��ҿ����ڿո񴦶Ͽ�
				if (lastSpace != -1 && lastSpace < tmpCount)
				{
					tmpCount = lastSpace + 1; // �����ո�
					break;
				}
				else if (tmpCount > 1)
				{
					tmpCount--;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		} while (true);

		// �����������
		if (tmpCount == 0 && begin < codepointsCount)
		{
			tmpCount = 1;
		}

		// �������׿ո�
		while (tmpCount > 0 && codepoints[begin] == ' ')
		{
			begin++;
			tmpCount--;
		}

		// ������β�ո�
		while (tmpCount > 0 && codepoints[begin + tmpCount - 1] == ' ')
		{
			tmpCount--;
		}

		if (tmpCount > 0)
		{
			std::vector<int> line(codepoints + begin, codepoints + begin + tmpCount);
			result.push_back(line);
		}

		begin += tmpCount;

		// ������һ�еĿ�ͷ�ո�
		while (begin < codepointsCount && codepoints[begin] == ' ')
		{
			begin++;
		}
	}

	UnloadCodepoints(codepoints);
	return result;
}