matrix World;           // ワールド変換行列
matrix View;            // ビュー変換行列
matrix Projection;      // 透視変換行列
matrix SMWorldViewProj; // ワールド×ビュー×透視変換行列(シャドウマップ用)
float4 Diffuse;         // ディフューズ色
float3 Light;           // 光源座標(ビュー座標系)


// テクスチャ
Texture2D Tex2D : register(t0);
Texture2D ShadowMap : register(t1); // シャドウマップ

// テクスチャ・サンプラ
SamplerState smpWrap   : register(s0);
SamplerState smpBorder : register(s1);

// **************************************************
// 3Dオブジェクトの描画
// **************************************************

// 頂点シェーダの入力データ定義
struct VS_INPUT {
    float3 Pos  : POSITION;  // 頂点座標(モデル座標系)
    float3 Norm : NORMAL;    // 法線ベクトル(モデル座標系)
    float2 Tex  : TEXTURE;   // テクスチャ座標
};

// ピクセル シェーダの入力データ定義
struct PS_INPUT {
    float4 Pos  : SV_POSITION; // 頂点座標(透視座標系)
    float3 PosView  : POSVIEW; // 頂点座標(ビュー座標系)
    float3 Norm : NORMAL;      // 法線ベクトル(ビュー座標系)
    float2 Tex  : TEXTURE;     // テクスチャ座標
    float3 PosSM: POSITION_SM; // 頂点座標(シャドウマップの透視座標系)
};

// 頂点シェーダの関数
PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;

    // 頂点座標　モデル座標系→透視座標系
    float4 pos4 = float4(input.Pos, 1.0);
    pos4 = mul(pos4, World);
    pos4 = mul(pos4, View);
    output.PosView = pos4.xyz / pos4.w;
    output.Pos = mul(pos4, Projection);

    // 法線ベクトル　モデル座標系→ビュー座標系
    float3 Norm = mul(input.Norm, (float3x3)World);
    output.Norm = mul(Norm, (float3x3)View);

    // テクスチャ座標
    output.Tex = input.Tex;

    // 頂点座標　モデル座標系→透視座標系(シャドウマップ)
    pos4 = mul(float4(input.Pos, 1.0), SMWorldViewProj);
    pos4.xyz = pos4.xyz / pos4.w;
    output.PosSM.x = (pos4.x + 1.0) / 2.0;
    output.PosSM.y = (-pos4.y + 1.0) / 2.0;
    output.PosSM.z = pos4.z;

    // 出力
    return output;
}

// ライティング計算
float lighting(PS_INPUT input)
{
    // 光源ベクトル
    float3 light = Light - input.PosView;
    // 距離
    float  leng = length(light);
    // 明るさ
    return 30 * dot(normalize(light), normalize(input.Norm)) / pow(leng, 2);
}

// ピクセル シェーダの関数
float4 PS(PS_INPUT input) : SV_TARGET
{
    // ライティング計算
    float bright = lighting(input);

    // テクスチャ取得
    float4 texCol = Tex2D.Sample(smpWrap, input.Tex);         // テクセル読み込み

    // シャドウマップ
    float sm = ShadowMap.Sample(smpBorder, input.PosSM.xy).x;
    float sma = (input.PosSM.z < sm) ? 1.0 : 0.5;

    // 色
    return saturate(bright * texCol * Diffuse * sma);
}

// **************************************************
// シャドウマップを使わない3Dオブジェクトの描画
// **************************************************

// ピクセル シェーダの関数
float4 PS_NOSM(PS_INPUT input) : SV_TARGET
{
    // ライティング計算
    float bright = lighting(input);

    // テクスチャ取得
    float4 texCol = Tex2D.Sample(smpWrap, input.Tex);         // テクセル読み込み

    // 色
    return saturate(bright * texCol * Diffuse);
}

// **************************************************
// シャドウマップの描画
// **************************************************

// 頂点シェーダの関数
float4 VS_SM(VS_INPUT input) : SV_POSITION {
    // 頂点座標　モデル座標系→透視座標系
    float4 pos4 = float4(input.Pos, 1.0);
    pos4 = mul(pos4, World);
    pos4 = mul(pos4, View);
    return mul(pos4, Projection);
}

