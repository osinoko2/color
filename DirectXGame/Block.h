class Block {

    enum class BlockColor {
        Red,
        Blue,
        Yellow,
        None  // 色なしの場合のデフォルト値
    };

public:

    BlockColor color;  // ブロックの色を管理する変数

    Block(MapChipType type, WorldTransform* transform)
        : type_(type), transform_(transform), isActive_(true) {}

    MapChipType GetType() const { return type_; }
    WorldTransform* GetTransform() const { return transform_; }
    bool IsActive() const { return isActive_; }
    void SetActive(bool active) { isActive_ = active; }

    AABB GetAABB() const {
        // transform_ の位置やサイズに基づいて AABB を計算して返す
    }

    void Initialize(BlockColor blockColor) {
        color = blockColor;
    }

private:
    MapChipType type_;
    WorldTransform* transform_;
    bool isActive_;
};