#pragma once
#include <string>

class GameData {
public:
	static GameData& Instance() {
		static GameData instance;
		return instance;
	}
	// コピーや代入を禁止
	GameData(const GameData&) = delete;
	GameData& operator=(const GameData&) = delete;

	// --- ゲーム用データ ---
	static const int windowWidth = 1280;
	static const int windowHeight = 720;

	static int DrawStringWithAnchor(int x, int y, float rateX, float rateY, unsigned int Color, int fontHandle, const char* fmt, ...) {
		// 可変引数処理
		char buffer[1024];
		va_list args;
		va_start(args, fmt);
		vsprintf_s(buffer, fmt, args);
		va_end(args);

		// 文字列のサイズ取得
		int strWidth = GetDrawStringWidthToHandle(buffer, -1, fontHandle);
		int strHeight = GetFontSizeToHandle(fontHandle);

		// アンカー位置を考慮した描画座標
		int drawX = x - static_cast<int>(strWidth * rateX);
		int drawY = y - static_cast<int>(strHeight * rateY);

		// 描画
		return DrawStringToHandle(drawX, drawY, buffer, Color, fontHandle);
	}

	// 可変個引数とアンカー位置に対応した文字列描画関数
	static int DrawStringWithAnchor(int x, int y, float rateX, float rateY, unsigned int Color, const char* FormatString, ...) {
		// 最大256文字のバッファを確保
		char strBuffer[256];

		// 可変個引数リストの開始
		va_list arg;
		va_start(arg, FormatString);

		// vsprintf_s を使ってフォーマット済み文字列をバッファに書き込む
		// バッファサイズを渡すことで、バッファオーバーフローを防止
		vsprintf_s(strBuffer, sizeof(strBuffer), FormatString, arg);

		// 可変個引数リストの終了
		va_end(arg);

		// 文字列の幅と高さを取得
		int stringWidth = GetDrawStringWidth(strBuffer, strlen(strBuffer));
		int stringHeight = GetFontSize();

		// アンカー位置に基づいて描画座標を計算
		int drawX = x - static_cast<int>(stringWidth * rateX);
		int drawY = y - static_cast<int>(stringHeight * rateY);

		// DXライブラリの描画関数を呼び出す
		return DrawString(drawX, drawY, strBuffer, Color);
	}

private:
	GameData() {}  // コンストラクタを private に
};