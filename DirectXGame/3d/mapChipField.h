
#include <vector>
#include "Vector3.h"
#include <string>
#include "Model.h"
#include "WorldTransform.h"
#include "Viewprojection.h"

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
	kRedBlock,
	kBlueBlock,
	kYellowBlock,
	kGoal,
	kCheck,
	kPlayer,
	kEnemy,
};

//enum MapChipCharIndex { 
//	kChipType = 0, // マップチップタイプ
//	kChipSubID = 1 // タイプごとのサブID
//};

// 1マス分のデータ
//struct MapChipDataUnit {
//	MapChipType type; // マップチップの種別
//	uint8_t subID;    // 種類ごとのサブID
//};

// ステージ全体のマップチップデータ
struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

//struct MapChipData {
//	std::vector<std::vector<MapChipDataUnit>>data;
//};

/// <summary>
/// マップチップ基礎部分
/// </summary>
class MapChipField{

public:

	struct IndexSet
	{
		uint32_t xIndex;
		uint32_t yIndex;
	};

	// 範囲矩形
	struct Rect {
		float left;
		float right;
		float top;
		float bottom;
	};

	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 80;

	MapChipData mapChipData_;

	/// <summary>
	/// CSVファイルの読み込み
	/// </summary>
	/// <param name="filePath">ファイル名</param>
	void LoadMapChipCsv(const std::string& filePath);

	/// <summary>
	/// マップチップの種別取得
	/// </summary>
	/// <param name="xIndex"></param>
	/// <param name="yIndex"></param>
	/// <returns></returns>
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	/// <summary>
	/// マップチップのサブID取得
	/// </summary>
	/// <param name="xIndex"></param>
	/// <param name="yIndex"></param>
	/// <returns></returns>
	uint8_t GetMapChipSubIDByIndex(uint32_t xIndex, uint32_t yIndex);

	/// <summary>
	/// マップチップの位置取得
	/// </summary>
	/// <param name="xIndex"></param>
	/// <param name="yIndex"></param>
	/// <returns></returns>
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	/// <summary>
	/// 縦のブロックの数を取得する
	/// </summary>
	/// <returns></returns>
	uint32_t GetNumBlockVirtical()const;
	/// <summary>
	/// 横のブロックの数を取得する
	/// </summary>
	/// <returns></returns>
	uint32_t GetNumBlockHorizontal()const;

	/// <summary>
	/// 取得した位置にマップチップをセットする
	/// </summary>
	/// <param name="position"></param>
	/// <returns></returns>
	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);

	/// <summary>
	/// 中心とした矩形を取得する
	/// </summary>
	/// <param name="xIndex"></param>
	/// <param name="yIndex"></param>
	/// <returns></returns>
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:
	void ResetMapChipData();


	// ワールド変換データ
	WorldTransform worldTransform_;
	ViewProjection* viewProjection_ = nullptr;
	// モデル
	Model* model_ = nullptr;

	static inline const float noDepth = 0.0f;

	static inline const float half = 2.0f;

	static inline const int next = 1;

	static inline const int usuallyIndex = 0;
};
