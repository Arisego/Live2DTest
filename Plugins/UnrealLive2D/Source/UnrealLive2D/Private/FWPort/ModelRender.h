#include "Live2DCubismCore.hpp"
#include "Rendering/CubismRenderer.hpp"
#include "Type/csmRectF.hpp"
#include "Type/csmVector.hpp"



/**
 * This head file is not supposed to be include by other head.
 * So we can use Csm directly
 */
using namespace Csm;
using Rendering::CubismRenderer;


class CubismClippingContext;


/**
 * @brief  クリッピングマスクの処理を実行するクラス
 *
 */
class CubismClippingManager_UE
{

    friend struct FCubismSepRender;

public:
    /**
     * @brief    コンストラクタ
     */
    CubismClippingManager_UE();

    /**
     * @brief    デストラクタ
     */
    virtual ~CubismClippingManager_UE();

public:

    /**
     * @brief カラーチャンネル(RGBA)のフラグを取得する
     *
     * @param[in]   channelNo   ->   カラーチャンネル(RGBA)の番号(0:R , 1:G , 2:B, 3:A)
     */
    CubismRenderer::CubismTextureColor* GetChannelFlagAsColor(csmInt32 channelNo);

    /**
     * @brief   マスクされる描画オブジェクト群全体を囲む矩形(モデル座標系)を計算する
     *
     * @param[in]   model            ->  モデルのインスタンス
     * @param[in]   clippingContext  ->  クリッピングマスクのコンテキスト
     */
    void CalcClippedDrawTotalBounds(CubismModel& model, CubismClippingContext* clippingContext);



    /**
     * @brief    マネージャの初期化処理<br>
     *           クリッピングマスクを使う描画オブジェクトの登録を行う
     *
     * @param[in]   model           ->  モデルのインスタンス
     * @param[in]   drawableCount   ->  描画オブジェクトの数
     * @param[in]   drawableMasks   ->  描画オブジェクトをマスクする描画オブジェクトのインデックスのリスト
     * @param[in]   drawableMaskCounts   ->  描画オブジェクトをマスクする描画オブジェクトの数
     */
    void Initialize(CubismModel& model, csmInt32 drawableCount, const csmInt32** drawableMasks, const csmInt32* drawableMaskCounts);

    /**
     * @brief   クリッピングコンテキストを作成する。モデル描画時に実行する。
     *
     * @param[in]   model       ->  モデルのインスタンス
     * @param[in]   renderer    ->  レンダラのインスタンス
     */
    void SetupClippingContext(CubismModel& model, struct FCubismRenderState* renderer);

    /**
     * @brief   既にマスクを作っているかを確認。<br>
     *          作っているようであれば該当するクリッピングマスクのインスタンスを返す。<br>
     *          作っていなければNULLを返す
     *
     * @param[in]   drawableMasks    ->  描画オブジェクトをマスクする描画オブジェクトのリスト
     * @param[in]   drawableMaskCounts ->  描画オブジェクトをマスクする描画オブジェクトの数
     * @return          該当するクリッピングマスクが存在すればインスタンスを返し、なければNULLを返す。
     */
    CubismClippingContext* FindSameClip(const csmInt32* drawableMasks, csmInt32 drawableMaskCounts) const;

    /**
     * @brief   クリッピングコンテキストを配置するレイアウト。<br>
     *           ひとつのレンダーテクスチャを極力いっぱいに使ってマスクをレイアウトする。<br>
     *           マスクグループの数が4以下ならRGBA各チャンネルに１つずつマスクを配置し、5以上6以下ならRGBAを2,2,1,1と配置する。
     *
     * @param[in]   usingClipCount  ->  配置するクリッピングコンテキストの数
     */
    bool SetupLayoutBounds(csmInt32 usingClipCount) const;

    /**
     * @breif   カラーバッファのアドレスを取得する
     *
     * @return  カラーバッファのアドレス
     */
    //CubismOffscreenFrame_UE* GetColorBuffer() const;

    /**
     * @brief   画面描画に使用するクリッピングマスクのリストを取得する
     *
     * @return  画面描画に使用するクリッピングマスクのリスト
     */
    csmVector<CubismClippingContext*>* GetClippingContextListForDraw();

    /**
     *@brief  クリッピングマスクバッファのサイズを設定する
     *
     *@param  size -> クリッピングマスクバッファのサイズ
     *
     */
    void SetClippingMaskBufferSize(csmInt32 size);

    /**
     *@brief  クリッピングマスクバッファのサイズを取得する
     *
     *@return クリッピングマスクバッファのサイズ
     *
     */
    csmInt32 GetClippingMaskBufferSize() const;

    //CubismOffscreenFrame_UE*  _colorBuffer;   ///< マスク用カラーバッファーのアドレス
    csmInt32    _currentFrameNo;         ///< マスクテクスチャに与えるフレーム番号

    csmVector<CubismRenderer::CubismTextureColor*>  _channelColors;

    /**
     * 这下面的两个数组是指向同样的数据
     * 只不过Mask是单独渲染的
     * Draw是跟着Order一起渲染的
     */
    csmVector<CubismClippingContext*>               _clippingContextListForMask;   ///< マスク用クリッピングコンテキストのリスト
    csmVector<CubismClippingContext*>               _clippingContextListForDraw;   ///< 描画用クリッピングコンテキストのリスト
    csmInt32                                        _clippingMaskBufferSize; ///< クリッピングマスクのバッファサイズ（初期値:256）

    CubismMatrix44  _tmpMatrix;              ///< マスク計算用の行列
    CubismMatrix44  _tmpMatrixForMask;       ///< マスク計算用の行列
    CubismMatrix44  _tmpMatrixForDraw;       ///< マスク計算用の行列
    csmRectF        _tmpBoundsOnModel;       ///< マスク配置計算用の矩形

};

/**
 * @brief   クリッピングマスクのコンテキスト
 */
class CubismClippingContext
{
    friend class CubismClippingManager_UE;

public:
    /**
     * @brief   引数付きコンストラクタ
     *
     */
    CubismClippingContext(CubismClippingManager_UE* manager, const csmInt32* clippingDrawableIndices, csmInt32 clipCount);

    /**
     * @brief   デストラクタ
     */
    virtual ~CubismClippingContext();

    /**
     * @brief   このマスクにクリップされる描画オブジェクトを追加する
     *
     * @param[in]   drawableIndex   ->  クリッピング対象に追加する描画オブジェクトのインデックス
     */
    void AddClippedDrawable(csmInt32 drawableIndex);

    /**
     * @brief   このマスクを管理するマネージャのインスタンスを取得する。
     *
     * @return  クリッピングマネージャのインスタンス
     */
    CubismClippingManager_UE* GetClippingManager();

    csmBool _isUsing;                                ///< 現在の描画状態でマスクの準備が必要ならtrue
    const csmInt32* _clippingIdList;                 ///< クリッピングマスクのIDリスト
    csmInt32 _clippingIdCount;                       ///< クリッピングマスクの数
    csmInt32 _layoutChannelNo;                       ///< RGBAのいずれのチャンネルにこのクリップを配置するか(0:R , 1:G , 2:B , 3:A)
    csmRectF* _layoutBounds;                         ///< マスク用チャンネルのどの領域にマスクを入れるか(View座標-1..1, UVは0..1に直す)
    csmRectF* _allClippedDrawRect;                   ///< このクリッピングで、クリッピングされる全ての描画オブジェクトの囲み矩形（毎回更新）
    CubismMatrix44 _matrixForMask;                   ///< マスクの位置計算結果を保持する行列
    CubismMatrix44 _matrixForDraw;                   ///< 描画オブジェクトの位置計算結果を保持する行列
    csmVector<csmInt32>* _clippedDrawableIndexList;  ///< このマスクにクリップされる描画オブジェクトのリスト

    CubismClippingManager_UE* _owner;        ///< このマスクを管理しているマネージャのインスタンス
};