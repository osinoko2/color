
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include "mapChipField.h"

namespace {

	/*std::map<char, MapChipType> mapChipTypeTable = {
		{'B', MapChipType::kBlock},
		{'E', MapChipType::kEnemy},
	};*/

	std::map<std::string, MapChipType> mapChipTable = {
        {"0",MapChipType::kBlank},
        {"1",MapChipType::kBlock},
        {"2",MapChipType::kRedBlock},
        {"3",MapChipType::kBlueBlock},
        {"4",MapChipType::kYellowBlock},
        {"5",MapChipType::kGoal},
    };

}


MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const Vector3& position)
{
	IndexSet indexSet = {};
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / half) / kBlockWidth);
	indexSet.yIndex = kNumBlockVirtical - next - static_cast<uint32_t>(position.y + kBlockHeight / half / kBlockHeight);
	return indexSet;
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex)
{
	// 矩形の中心を設定する
	Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

	// 中心とした点から矩形を設定する
	Rect rect;
	rect.left = center.x - kBlockWidth / half;
	rect.right = center.x + kBlockWidth / half;
	rect.top = center.y + kBlockHeight / half;
	rect.bottom = center.y - kBlockHeight / half;

	return rect;
}

void MapChipField::ResetMapChipData()
{
	// マップチップデータをリセット
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	/*for (std::vector<MapChipDataUnit>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}*/

	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}



void MapChipField::LoadMapChipCsv(const std::string& filePath)
{
	// マップチップデータをリセット
	ResetMapChipData();

	//ファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	// マップチップCSV
	std::stringstream mapChipCsv;
	// ファイルの内容を文字列ストリームにコピー
	mapChipCsv << file.rdbuf();
	// ファイルを閉じる
	file.close();

	// CSVからマップチップデータを読み込む
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		std::string line;
		getline(mapChipCsv, line);

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			std::string word;
			getline(line_stream, word, ',');

			if (mapChipTable.contains(word)) {
				mapChipData_.data[i][j] = mapChipTable[word];
			}

			//// 空白の場合はスキップ
			//if (word.empty()) {
			//	continue;
			//}

			//// 先頭文字がいずれかのマップチップ種別に該当するか確認
			//if (!mapChipTypeTable.contains(word[kChipType])) {
			//	continue;
			//}

			//// 先頭文字でマップチップのタイプを判別
			//mapChipData_.data[i][j].type = mapChipTypeTable[word[kChipType]];

			//// サブIDを含まない場合はスキップ(0番で確定)
			//if (word.size() <= kChipSubID) {
			//	continue;
			//}

			//// マップチップのサブIDを設定
			//mapChipData_.data[i][j].subID = static_cast<uint8_t>(word[kChipSubID] - '0');
		}
	}
}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex)
{
	if (xIndex < usuallyIndex || kNumBlockHorizontal - next < xIndex) {
		return MapChipType::kBlank;
	}
	if (yIndex < usuallyIndex || kNumBlockVirtical - next < yIndex) {
		return MapChipType::kBlank;
	}

	/*return mapChipData_.data[yIndex][xIndex].type;*/
	
	return mapChipData_.data[yIndex][xIndex];
}

//uint8_t MapChipField::GetMapChipSubIDByIndex(uint32_t xIndex, uint32_t yIndex) { 
//	return mapChipData_.data[yIndex][xIndex].subID; 
//}

Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex)
{

	return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - next - yIndex), noDepth);
}

uint32_t MapChipField::GetNumBlockVirtical() const
{
	return kNumBlockVirtical;
}

uint32_t MapChipField::GetNumBlockHorizontal() const
{
	return kNumBlockHorizontal;
}
