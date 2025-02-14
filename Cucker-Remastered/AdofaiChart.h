#pragma once
#include <string>
#include <regex>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <functional>
#include <prettywriter.h>
#include <document.h>
#include <error/en.h> // 提供错误码的字符串描述
#include "Chroma.h"
#include "LogOverlay.h"


// Version 15
struct AdofaiChart {
public:
    template<class T>
    class StringTable {
    protected:
        static std::unordered_map<std::string, T> table;
        static bool initialized;
    public:
        StringTable() {};
        // static void initialize();

        static void clear() {
            if (initialized) {
                table.clear();
                initialized = false;
            }
        }

        static T Get(const std::string& name) {
            auto it = table.find(name);
            if (it != table.end()) {
                return it->second;
            }
            throw std::runtime_error("Object '" + name + "' not found in table");
        }
    };

    // 定义枚举
#define DEFINE_ENUM(EnumName, ...)           \
    enum struct EnumName {                   \
        __VA_ARGS__                          \
    };

// 定义映射表
#define DEFINE_STRING_TABLE(EnumName, ...)                                  \
    class EnumName##Table : public StringTable<EnumName> {                  \
    public:                                                                 \
        static void initialize() {                                          \
            if (!initialized) {                                             \
                table = {                                                   \
                    __VA_ARGS__                                             \
                };                                                          \
                initialized = true;                                         \
            }                                                               \
        }                                                                   \
    };

    static const int ChartVersion = 15;

    struct Actions {
        struct SetSpeed {
            // 速度型
            enum struct SpeedType {
                Bpm,        // BPM
                Multiplier  // 倍频器
            };
        };
    };

    struct Elements {
        // 打拍声
        DEFINE_ENUM(HitSound,
            None,            // 无
            Hat,             // 高帽钹
            Kick,            // 底鼓
            Shaker,          // 沙槌
            Sizzle,          // 镲片
            Chuck,           // 低音底鼓
            ShakerLoud,      // 沙槌（响）
            Hammer,          // 音锤
            KickChroma,      // 底鼓-铬
            SnareAcoustic2,  // 军鼓原声2
            Sidestick,       // 鼓边
            Stick,           // 鼓棒
            ReverbClack,     // 混响噼啪
            Squareshot,      // 正角快照
            PowerDown,       // 减速音效
            PowerUp,         // 加速音效
            KickHouse,       // 底鼓-浩室
            KickRupture,     // 底鼓-裂解
            HatHouse,        // 镲片-浩室
            SnareHouse,      // 军鼓-浩室
            SnareVapor,      // 军鼓-蒸汽
            ClapHit,         // 拍掌
            ClapHitEcho,     // 回音拍掌
            ReverbClap,      // 混响拍掌
            FireTile,        // 火方块
            IceTile,         // 冰方块
            VehiclePositive, // 更多星球
            VehicleNegative  // 更少星球
        );

        // 定义Ease枚举
        DEFINE_ENUM(Ease,
            Linear,       // 线性
            InSine,       // 正弦内插
            OutSine,      // 正弦外插
            InOutSine,    // 正弦内外插
            InQuad,       // 二次内插
            OutQuad,      // 二次外插
            InOutQuad,    // 二次内外插
            InCubic,      // 三次内插
            OutCubic,     // 三次外插
            InOutCubic,   // 三次内外插
            InQuart,      // 四次内插
            OutQuart,     // 四次外插
            InOutQuart,   // 四次内外插
            InQuint,      // 五次内插
            OutQuint,     // 五次外插
            InOutQuint,   // 五次内外插
            InExpo,       // 指数内插
            OutExpo,      // 指数外插
            InOutExpo,    // 指数内外插
            InCirc,       // 圆形内插
            OutCirc,      // 圆形外插
            InOutCirc,    // 圆形内外插
            InElastic,    // 弹性内插
            OutElastic,   // 弹性外插
            InOutElastic, // 弹性内外插
            InBack,       // 后退内插
            OutBack,      // 后退外插
            InOutBack,    // 后退内外插
            InBounce,     // 弹跳内插
            OutBounce,    // 弹跳外插
            InOutBounce,  // 弹跳内外插
            Flash,        // 闪光
            InFlash,      // 闪光内插
            OutFlash,     // 闪光外插
            InOutFlash    // 闪光内外插
        );

        // 打拍声字符串表
        DEFINE_STRING_TABLE(HitSound,
            { "None", HitSound::None },              // 无
            { "Hat", HitSound::Hat },                // 高帽钹
            { "Kick", HitSound::Kick },              // 底鼓
            { "Shaker", HitSound::Shaker },          // 沙槌
            { "Sizzle", HitSound::Sizzle },          // 镲片
            { "Chuck", HitSound::Chuck },            // 低音底鼓
            { "ShakerLoud", HitSound::ShakerLoud },  // 沙槌（响）
            { "Hammer", HitSound::Hammer },          // 音锤
            { "KickChroma", HitSound::KickChroma },  // 底鼓-铬
            { "SnareAcoustic2", HitSound::SnareAcoustic2 },  // 军鼓原声2
            { "Sidestick", HitSound::Sidestick },    // 鼓边
            { "Stick", HitSound::Stick },            // 鼓棒
            { "ReverbClack", HitSound::ReverbClack },// 混响噼啪
            { "Squareshot", HitSound::Squareshot },  // 正角快照
            { "PowerDown", HitSound::PowerDown },    // 减速音效
            { "PowerUp", HitSound::PowerUp },        // 加速音效
            { "KickHouse", HitSound::KickHouse },    // 底鼓-浩室
            { "KickRupture", HitSound::KickRupture },// 底鼓-裂解
            { "HatHouse", HitSound::HatHouse },      // 镲片-浩室
            { "SnareHouse", HitSound::SnareHouse },  // 军鼓-浩室
            { "SnareVapor", HitSound::SnareVapor },  // 军鼓-蒸汽
            { "ClapHit", HitSound::ClapHit },        // 拍掌
            { "ClapHitEcho", HitSound::ClapHitEcho },// 回音拍掌
            { "ReverbClap", HitSound::ReverbClap },  // 混响拍掌
            { "FireTile", HitSound::FireTile },      // 火方块
            { "IceTile", HitSound::IceTile },        // 冰方块
            { "VehiclePositive", HitSound::VehiclePositive }, // 更多星球
            { "VehicleNegative", HitSound::VehicleNegative }  // 更少星球
        );

        // 定义Ease字符串表
        DEFINE_STRING_TABLE(Ease,
            { "Linear", Ease::Linear },               // 线性
            { "InSine", Ease::InSine },               // 正弦内插
            { "OutSine", Ease::OutSine },             // 正弦外插
            { "InOutSine", Ease::InOutSine },         // 正弦内外插
            { "InQuad", Ease::InQuad },               // 二次内插
            { "OutQuad", Ease::OutQuad },             // 二次外插
            { "InOutQuad", Ease::InOutQuad },         // 二次内外插
            { "InCubic", Ease::InCubic },             // 三次内插
            { "OutCubic", Ease::OutCubic },           // 三次外插
            { "InOutCubic", Ease::InOutCubic },       // 三次内外插
            { "InQuart", Ease::InQuart },             // 四次内插
            { "OutQuart", Ease::OutQuart },           // 四次外插
            { "InOutQuart", Ease::InOutQuart },       // 四次内外插
            { "InQuint", Ease::InQuint },             // 五次内插
            { "OutQuint", Ease::OutQuint },           // 五次外插
            { "InOutQuint", Ease::InOutQuint },       // 五次内外插
            { "InExpo", Ease::InExpo },               // 指数内插
            { "OutExpo", Ease::OutExpo },             // 指数外插
            { "InOutExpo", Ease::InOutExpo },         // 指数内外插
            { "InCirc", Ease::InCirc },               // 圆形内插
            { "OutCirc", Ease::OutCirc },             // 圆形外插
            { "InOutCirc", Ease::InOutCirc },         // 圆形内外插
            { "InElastic", Ease::InElastic },         // 弹性内插
            { "OutElastic", Ease::OutElastic },       // 弹性外插
            { "InOutElastic", Ease::InOutElastic },   // 弹性内外插
            { "InBack", Ease::InBack },               // 后退内插
            { "OutBack", Ease::OutBack },             // 后退外插
            { "InOutBack", Ease::InOutBack },         // 后退内外插
            { "InBounce", Ease::InBounce },           // 弹跳内插
            { "OutBounce", Ease::OutBounce },         // 弹跳外插
            { "InOutBounce", Ease::InOutBounce },     // 弹跳内外插
            { "Flash", Ease::Flash },                 // 闪光
            { "InFlash", Ease::InFlash },             // 闪光内插
            { "OutFlash", Ease::OutFlash },           // 闪光外插
            { "InOutFlash", Ease::InOutFlash }        // 闪光内外插
        );

        struct Track {
            // 轨道颜色类型
            DEFINE_ENUM(ColorType,
                Single,     // 单独
                Stripes,    // 条纹
                Glow,       // 发光
                Blink,      // 闪烁
                Switch,     // 切换
                Rainbow,    // 彩虹
                Volume      // 音量
            );

            // 颜色缓冲类型   
            DEFINE_ENUM(ColorPulse,
                None,       // 无
                Forward,    // 前进
                Backward    // 后退
            );

            // 轨道风格
            DEFINE_ENUM(Style,
                Standard,   // 标准
                Neon,       // 霓虹
                NeonLight,  // 霓虹灯
                Basic,      // 基础
                Gems,       // 宝石
                Minimal     // 极简
            );

            // 轨道出现动画
            DEFINE_ENUM(AppearAnimation,
                None,           // 无
                Assemble,       // 聚合
                Assemble_Far,   // 聚合：远
                Extend,         // 延伸
                Grow,           // 生长
                Grow_Spin,      // 生长+旋转
                Fade,           // 出现
                Drop,           // 下坠
                Rise            // 升起
            );

            // 轨道消失动画
            DEFINE_ENUM(DisappearAnimation,
                None,           // 无
                Scatter,        // 分散
                Scatter_Far,    // 分散：远
                Retract,        // 收缩
                Shrink,         // 黯淡
                Shrink_Spin,    // 黯淡+旋转
                Fade            // 出现
            );

            // 颜色缓冲类型字符串表
            DEFINE_STRING_TABLE(ColorPulse,
                { "None", ColorPulse::None },         // 无
                { "Forward", ColorPulse::Forward },   // 前进
                { "Backward", ColorPulse::Backward }  // 后退
            );

            // 轨道颜色类型字符串表
            DEFINE_STRING_TABLE(ColorType,
                { "Single", ColorType::Single },     // 单独
                { "Stripes", ColorType::Stripes },   // 条纹
                { "Glow", ColorType::Glow },         // 发光
                { "Blink", ColorType::Blink },       // 闪烁
                { "Switch", ColorType::Switch },     // 切换
                { "Rainbow", ColorType::Rainbow },   // 彩虹
                { "Volume", ColorType::Volume }      // 音量
            );

            // 轨道风格字符串表
            DEFINE_STRING_TABLE(Style,
                { "Standard", Style::Standard },     // 标准
                { "Neon", Style::Neon },             // 霓虹
                { "NeonLight", Style::NeonLight },   // 霓虹灯
                { "Basic", Style::Basic },           // 基础
                { "Gems", Style::Gems },             // 宝石
                { "Minimal", Style::Minimal }        // 极简
            );

            // 轨道出现动画字符串表
            DEFINE_STRING_TABLE(AppearAnimation,
                { "None", AppearAnimation::None },             // 无
                { "Assemble", AppearAnimation::Assemble },     // 聚合
                { "Assemble_Far", AppearAnimation::Assemble_Far }, // 聚合：远
                { "Extend", AppearAnimation::Extend },         // 延伸
                { "Grow", AppearAnimation::Grow },             // 生长
                { "Grow_Spin", AppearAnimation::Grow_Spin },   // 生长+旋转
                { "Fade", AppearAnimation::Fade },             // 出现
                { "Drop", AppearAnimation::Drop },             // 下坠
                { "Rise", AppearAnimation::Rise }              // 升起
            );

            // 轨道消失动画字符串表
            DEFINE_STRING_TABLE(DisappearAnimation,
                { "None", DisappearAnimation::None },             // 无
                { "Scatter", DisappearAnimation::Scatter },       // 分散
                { "Scatter_Far", DisappearAnimation::Scatter_Far }, // 分散：远
                { "Retract", DisappearAnimation::Retract },       // 收缩
                { "Shrink", DisappearAnimation::Shrink },         // 黯淡
                { "Shrink_Spin", DisappearAnimation::Shrink_Spin }, // 黯淡+旋转
                { "Fade", DisappearAnimation::Fade }              // 出现
            );
        };
        struct BackGround {
            // 教程背景图形
            DEFINE_ENUM(DefaultShapeType,
                Default,        // 默认
                SingleColor,    // 纯色
                Disabled        // 无
            );

            // 图像显示模式
            DEFINE_ENUM(DisplayMode,
                FitToScreen,    // 拉伸
                Unscaled,       // 未缩放
                Tiled           // 平铺
            );

            // 教程背景图形字符串表
            DEFINE_STRING_TABLE(DefaultShapeType,
                { "Default", DefaultShapeType::Default },        // 默认
                { "SingleColor", DefaultShapeType::SingleColor },// 纯色
                { "Disabled", DefaultShapeType::Disabled }       // 无
            );

            // 图像显示模式字符串表
            DEFINE_STRING_TABLE(DisplayMode,
                { "FitToScreen", DisplayMode::FitToScreen },    // 拉伸
                { "Unscaled", DisplayMode::Unscaled },          // 未缩放
                { "Tiled", DisplayMode::Tiled }                 // 平铺
            );
        };

        struct Camera {
            // 相对于
            DEFINE_ENUM(RelativeTo,
                Player,                 // 玩家
                Tile,                   // 方块
                Global,                 // 全局
                LastPositionNoRotation  // 上次位置（无旋转）
            );

            // 相对于 字符串表
            DEFINE_STRING_TABLE(RelativeTo,
                { "Player", RelativeTo::Player },                 // 玩家
                { "Tile", RelativeTo::Tile },                     // 方块
                { "Global", RelativeTo::Global },                 // 全局
                { "LastPositionNoRotation", RelativeTo::LastPositionNoRotation }  // 上次位置（无旋转）
            );
        };

        struct Parallax {
            double X;
            double Y;
        };

        struct Position {
            double X;
            double Y;
        };
	};

    static void InitTables() {
        ChartSetting::LevelSetting::SpecialArtistTypeTable::initialize();
        ChartSetting::MiscellaneousSettings::PlanetEasePartBehaviorTable::initialize();
        Elements::HitSoundTable::initialize();
        Elements::EaseTable::initialize();
        Elements::BackGround::DefaultShapeTypeTable::initialize();
        Elements::BackGround::DisplayModeTable::initialize();
        Elements::Camera::RelativeToTable::initialize();
        Elements::Track::AppearAnimationTable::initialize();
        Elements::Track::ColorPulseTable::initialize();
        Elements::Track::ColorTypeTable::initialize();
        Elements::Track::DisappearAnimationTable::initialize();
        Elements::Track::StyleTable::initialize();
    }

    static void FreeTables() {
        ChartSetting::LevelSetting::SpecialArtistTypeTable::clear();
        ChartSetting::MiscellaneousSettings::PlanetEasePartBehaviorTable::clear();
        Elements::HitSoundTable::clear();
        Elements::EaseTable::clear();
        Elements::BackGround::DefaultShapeTypeTable::clear();
        Elements::BackGround::DisplayModeTable::clear();
        Elements::Camera::RelativeToTable::clear();
        Elements::Track::AppearAnimationTable::clear();
        Elements::Track::ColorPulseTable::clear();
        Elements::Track::ColorTypeTable::clear();
        Elements::Track::DisappearAnimationTable::clear();
        Elements::Track::StyleTable::clear();
    }

    struct ChartSetting {
        struct LevelVersion {
            int version;
        };

        struct LevelSetting {
            // 转换 SpecialArtistType 枚举
            DEFINE_ENUM(SpecialArtistType,
                None,               // 无
                AuthorIsArtist,     // 我是艺术家本人
                PublicLicense       // 公开版权
            );

            DEFINE_STRING_TABLE(SpecialArtistType,
                { "None", SpecialArtistType::None },                           // 无
                { "AuthorIsArtist", SpecialArtistType::AuthorIsArtist },       // 我是艺术家本人
                { "PublicLicense", SpecialArtistType::PublicLicense }          // 公开版权
            );

            std::string artist;
            SpecialArtistType specialArtistType;
            std::string artistPermission;
            std::string song;
            std::string author;
            bool separateCountdownTime;
            std::string previewImage;
            std::string previewIcon;
            std::string previewIconColor;
            int previewSongStart;
            int previewSongDuration;
            bool seizureWarning;
            std::string levelDesc;
            std::string levelTags;
            std::string artistLinks;
            int speedTrialAim;
            int difficulty;
        };

        struct SongSetting {
            std::string songFilename;
            double bpm;
            int volume;
            int offset;
            int pitch;
            Elements::HitSound hitsound;
            int hitsoundVolume;
            int countdownTicks;
            std::string songURL;
        };

        struct TrackSetting {
            Elements::Track::ColorType trackColorType;
            Color trackColor;
            Color secondaryTrackColor;
            double trackColorAnimDuration;
            Elements::Track::ColorPulse trackColorPulse;
            int trackPulseLength;
            Elements::Track::Style trackStyle;
            std::string trackTexture;
            double trackTextureScale;
            int trackGlowIntensity;
            Elements::Track::AppearAnimation trackAnimation;
            int beatsAhead;
            Elements::Track::DisappearAnimation trackDisappearAnimation;
            int beatsBehind;
        };

        struct DLCMods {
            std::vector<std::string> requiredMods;
        };

        struct BackGroundSetting {
            Color backgroundColor;
            bool showDefaultBGIfNoImage;
            bool showDefaultBGTile;
            Color defaultBGTileColor;
            Elements::BackGround::DefaultShapeType defaultBGShapeType;
            Color defaultBGShapeColor;
            std::string bgImage;
            Color bgImageColor;
            Elements::Parallax parallax;
            Elements::BackGround::DisplayMode bgDisplayMode;
            bool imageSmoothing;
            bool lockRot;
            bool loopBG;
            int scalingRatio;
        };

        struct CameraSetting {
            Elements::Camera::RelativeTo relativeTo;
            Elements::Position position;
            double rotation;
            double zoom;
            bool pulseOnFloor;
        };

        struct MiscellaneousSettings {
            // 转换 PlanetEasePartBehavior 枚举
            DEFINE_ENUM(PlanetEasePartBehavior,
                Mirror, // 镜像
                Repeat  // 重复
            );

            DEFINE_STRING_TABLE(PlanetEasePartBehavior,
                { "Mirror", PlanetEasePartBehavior::Mirror }, // 镜像
                { "Repeat", PlanetEasePartBehavior::Repeat }  // 重复
            );

            std::string bgVideo;
            bool loopVideo;
            int vidOffset;
            bool floorIconOutlines;
            bool stickToFloors;
            Elements::Ease planetEase;
            int planetEaseParts;
            PlanetEasePartBehavior planetEasePartBehavior;
            Color defaultTextColor;
            Color defaultTextShadowColor;
            std::string congratsText;
            std::string perfectText;
        };

        struct Legacy {
            bool legacyFlash;
            bool legacyCamRelativeTo;
            bool legacySpriteTiles;
            bool legacyTween;
            bool disableV15Features;
        };

        LevelVersion levelVersion;
        LevelSetting levelSetting;
        SongSetting songSetting;
        TrackSetting trackSetting;
        BackGroundSetting backgroundSetting;
        CameraSetting cameraSetting;
        MiscellaneousSettings miscellaneousSetting;
        DLCMods dlcMods;
        Legacy legacy;
    };
};

template <typename T>
std::unordered_map<std::string, T> AdofaiChart::StringTable<T>::table;

template <typename T>
bool AdofaiChart::StringTable<T>::initialized = false;



#undef DEFINE_ENUM
#undef DEFINE_STRING_TABLE

class AdofaiChartJson {
public:
    struct LoaderSetting {
        bool removeBOM = true;
        bool clearControlChar = false;
        bool fixTrailingCommas = false;
        bool fixTailCommas = false;
        bool readByUtf16 = false;
    };
protected:
    rapidjson::Document chartDoc;
public:
    AdofaiChartJson() = default;
    AdofaiChartJson(const AdofaiChartJson& other);
    AdofaiChartJson& operator=(const AdofaiChartJson& other);

    void ReadFileAndParse(const std::string& filePath, LoaderSetting setting);
    std::string GetJsonContent(const std::string& filePath, LoaderSetting setting);
    void ClearDocument();
    void isEmpty();
    rapidjson::Document& GetChartDocument();
protected:
    void LogParseError(std::string& jsonContent);
    void GetErrorAtLine(std::string& jsonContent, size_t errorOffset, size_t& column, size_t& line);
    void SettingApply(std::string& jsonContent, LoaderSetting setting);

    template<class String>
    void RemoveBOM(String& jsonContent) {
        if (jsonContent.size() >= 3 &&
            static_cast<unsigned char>(jsonContent[0]) == 0xEF &&
            static_cast<unsigned char>(jsonContent[1]) == 0xBB &&
            static_cast<unsigned char>(jsonContent[2]) == 0xBF) {
            jsonContent.erase(0, 3); // 去掉 BOM
        }
    }
};

class AdofaiChartParser {
public:
    class JsonValueAccessor {
    protected:
        const rapidjson::Value& obj;
        LogOverlay* log = nullptr;
        static bool verify;
    public:
        JsonValueAccessor(const rapidjson::Value& obj) :obj(obj) {
            log = &LogOverlay::GetInstance();
        };
        static void SetVerify(bool flag) {
            verify = flag;
        }

        template <typename T>
        static void GetNumberFromArray(const rapidjson::Value& value, T& field) {
            static_assert(std::is_arithmetic<T>::value, "GetNumberFromArray requires an arithmetic type!");
            if (value.IsNull()) {
                LogOverlay::GetInstance().warn("GetNumberFromArray: Value is null");
                return;
            }
            if (value.IsDouble()) {
                field = static_cast<T>(value.GetDouble());
            }
            else if (value.IsInt()) {
                field = static_cast<T>(value.GetInt());
            }
            else if (value.IsInt64()) {
                field = static_cast<T>(value.GetInt64());
            }
            else if (value.IsUint64()) {
                field = static_cast<T>(value.GetUint64());
            }
            else {
                LogOverlay::GetInstance().error("Array value is not a compatible number type");
            }
        }

        template <typename T>
        void GetNumber(const char* fieldName, T& field) {
            static_assert(std::is_arithmetic<T>::value, "GetNumber requires an arithmetic type!");
            Verify(fieldName, [this, &field, &fieldName](const rapidjson::Value& value) {
                if (value.IsDouble()) {
                    field = static_cast<T>(value.GetDouble());
                }
                else if (value.IsInt()) {
                    field = static_cast<T>(value.GetInt());
                }
                else if (value.IsInt64()) {
                    field = static_cast<T>(value.GetInt64());
                }
                else if (value.IsUint64()) {
                    field = static_cast<T>(value.GetUint64());
                }
                else {
                    log->error("Field '", fieldName, "' is not a compatible number type");
                }
            });
        }
        void GetString(const char* fieldName, std::string& field) {
            Verify(fieldName, [this, &field, &fieldName](const rapidjson::Value& value) {
                if (value.IsString()) {
                    field = value.GetString();
                }
                else {
                    log->error("Value '", fieldName, "' is not a compatible string type");
                }
            });
        }
        void GetBool(const char* fieldName, bool& field) {
            Verify(fieldName, [this, &field, &fieldName](const rapidjson::Value& value) {
                if (value.IsBool()) {
                    field = value.GetBool();
                }
                else {
                    log->error("Value '", fieldName, "' is not a compatible boolean type");
                }
            });
        }
        void GetColor(const char* fieldName, Color& field) {
            Verify(fieldName, [this, &field, fieldName](const rapidjson::Value& value) {
                if (value.IsString()) {
                    field = Color::FromHex(value.GetString());
                }
                else {
                    log->error("Value '", fieldName, "' is not a compatible string type");
                }
            });
        }
        template<typename Enum, typename Table>
        void GetEnum(const char* fieldName, Enum& field) {
            Verify(fieldName, [this, &field, &fieldName](const rapidjson::Value& value) {
                if (value.IsString()) {
                    field = Table::Get(value.GetString());
                }
                else {
                    log->error("Value '", fieldName, "' is not a compatible string type");
                }
            });
        }
        void GetArrayForEach(const char* fieldName, std::function<void(const rapidjson::Value&, size_t)> func) {
            Verify(fieldName, [&func](const rapidjson::Value& value) {
                size_t index = 0;
                for (auto& ptr : value.GetArray()) {
                    func(ptr, index);
                    index++;
                }
            });
        }
        void Verify(const char* fieldName, std::function<void(const rapidjson::Value&)> func) {
            if (!verify) { 
                func(obj[fieldName]);
            }
            if (obj.HasMember(fieldName)) {
                const auto& value = obj[fieldName];
                if (value.IsNull()) {
                    log->warn("Field '", fieldName, "' is null");
                    return;
                }
                func(value);
            }
            else {
                log->warn("Field '", fieldName, "' is missing in the JSON object");
            }
        }
    };

    class SettingParser {
    public:
        static void Parse(AdofaiChart::ChartSetting& data, rapidjson::Document& doc) {
            if (!doc.HasMember("settings") || !doc["settings"].IsObject()) {
                throw std::runtime_error("\"settings\" not found or is not an object.");
            }
            const auto& settings = doc["settings"];
            checkLevelVersion(data, settings);
            parseLevelSetting(data, settings);
            parseSongSetting(data, settings);
            parseTrackSetting(data, settings);
            parseBackgroundSetting(data, settings);
            parseCameraSetting(data, settings);
            parseMiscellaneousSetting(data, settings);
            parseLegary(data, settings);
            parseDLCMods(data, settings);
        }
        static void parseLevelSetting(AdofaiChart::ChartSetting& _data, const rapidjson::Value& settings) {
            using LevelSetting = AdofaiChart::ChartSetting::LevelSetting;
            auto& data = _data.levelSetting;
            JsonValueAccessor accessor(settings);
            accessor.GetString("artist", data.artist);
            accessor.GetEnum<LevelSetting::SpecialArtistType, LevelSetting::SpecialArtistTypeTable>("specialArtistType", data.specialArtistType);
            accessor.GetString("artistPermission", data.artistPermission);
            accessor.GetString("song", data.song);
            accessor.GetString("author", data.author);
            accessor.GetBool("separateCountdownTime", data.separateCountdownTime);
            accessor.GetString("previewImage", data.previewImage);
            accessor.GetString("previewIcon", data.previewIcon);
            accessor.GetString("previewIconColor", data.previewIconColor);
            accessor.GetNumber("previewSongStart", data.previewSongStart);
            accessor.GetNumber("previewSongDuration", data.previewSongDuration);
            accessor.GetBool("seizureWarning", data.seizureWarning);
            accessor.GetString("levelDesc", data.levelDesc);
            accessor.GetString("levelTags", data.levelTags);
            accessor.GetString("artistLinks", data.artistLinks);
            accessor.GetNumber("speedTrialAim", data.speedTrialAim);
            accessor.GetNumber("difficulty", data.difficulty);
        }
        static void parseSongSetting(AdofaiChart::ChartSetting& _data, const rapidjson::Value& settings) {
            auto& data = _data.songSetting;
            JsonValueAccessor accessor(settings);
            accessor.GetString("songFilename", data.songFilename);
            accessor.GetNumber("bpm", data.bpm);
            accessor.GetNumber("volume", data.volume);
            accessor.GetNumber("offset", data.offset);
            accessor.GetNumber("pitch", data.pitch);
            accessor.GetEnum<AdofaiChart::Elements::HitSound, AdofaiChart::Elements::HitSoundTable>("hitsound", data.hitsound);
            accessor.GetNumber("hitsoundVolume", data.hitsoundVolume);
            accessor.GetNumber("countdownTicks", data.countdownTicks);
            accessor.GetString("songURL", data.songURL);
        }
        static void parseTrackSetting(AdofaiChart::ChartSetting& _data, const rapidjson::Value& settings) {
            using Track = AdofaiChart::Elements::Track;
            auto& data = _data.trackSetting;
            JsonValueAccessor accessor(settings);
            accessor.GetEnum<Track::ColorType, Track::ColorTypeTable>("trackColorType", data.trackColorType);
            accessor.GetColor("trackColor", data.trackColor);
            accessor.GetColor("secondaryTrackColor", data.secondaryTrackColor);
            accessor.GetNumber("trackColorAnimDuration", data.trackColorAnimDuration);
            accessor.GetEnum<Track::ColorPulse, Track::ColorPulseTable>("trackColorPulse", data.trackColorPulse);
            accessor.GetNumber("trackPulseLength", data.trackPulseLength);
            accessor.GetEnum<Track::Style, Track::StyleTable>("trackStyle", data.trackStyle);
            accessor.GetString("trackTexture", data.trackTexture);
            accessor.GetNumber("trackTextureScale", data.trackTextureScale);
            accessor.GetNumber("trackGlowIntensity", data.trackGlowIntensity);
            accessor.GetEnum<Track::AppearAnimation, Track::AppearAnimationTable>("trackAnimation", data.trackAnimation);
            accessor.GetNumber("beatsAhead", data.beatsAhead);
            accessor.GetEnum<Track::DisappearAnimation, Track::DisappearAnimationTable>("trackDisappearAnimation", data.trackDisappearAnimation);
            accessor.GetNumber("beatsBehind", data.beatsBehind);
        }
        static void parseBackgroundSetting(AdofaiChart::ChartSetting& _data, const rapidjson::Value& settings) {
            using BackGround = AdofaiChart::Elements::BackGround;
            auto& data = _data.backgroundSetting;
            JsonValueAccessor accessor(settings);
            accessor.GetColor("backgroundColor", data.backgroundColor);
            accessor.GetBool("showDefaultBGIfNoImage", data.showDefaultBGIfNoImage);
            accessor.GetBool("showDefaultBGTile", data.showDefaultBGTile);
            accessor.GetColor("defaultBGTileColor", data.defaultBGTileColor);
            accessor.GetEnum<BackGround::DefaultShapeType, BackGround::DefaultShapeTypeTable>("defaultBGShapeType", data.defaultBGShapeType);
            accessor.GetColor("defaultBGShapeColor", data.defaultBGShapeColor);
            accessor.GetString("bgImage", data.bgImage);
            accessor.GetColor("bgImageColor", data.bgImageColor);
            accessor.GetArrayForEach("parallax", [&data](const rapidjson::Value& val,size_t index) {
                if (index == 0) JsonValueAccessor::GetNumberFromArray(val, data.parallax.X);
                if (index == 1) JsonValueAccessor::GetNumberFromArray(val, data.parallax.Y);
            });
            accessor.GetEnum<BackGround::DisplayMode, BackGround::DisplayModeTable>("bgDisplayMode", data.bgDisplayMode);
            accessor.GetBool("imageSmoothing", data.imageSmoothing);
            accessor.GetBool("lockRot", data.lockRot);
            accessor.GetBool("loopBG", data.loopBG);
            accessor.GetNumber("scalingRatio", data.scalingRatio);
        }
        static void parseCameraSetting(AdofaiChart::ChartSetting& _data, const rapidjson::Value& settings) {
            using Camera = AdofaiChart::Elements::Camera;
            auto& data = _data.cameraSetting;
            JsonValueAccessor accessor(settings);
            accessor.GetEnum<Camera::RelativeTo, Camera::RelativeToTable>("relativeTo", data.relativeTo);
            accessor.GetArrayForEach("position", [&data](const rapidjson::Value& value, size_t index) {
                if (index == 0) {
                    JsonValueAccessor::GetNumberFromArray(value, data.position.X);
                }
                else if (index == 1) {
                    JsonValueAccessor::GetNumberFromArray(value, data.position.Y);
                }
            });
            accessor.GetNumber("rotation", data.rotation);
            accessor.GetNumber("zoom", data.zoom);
            accessor.GetBool("pulseOnFloor", data.pulseOnFloor);
        }
        static void parseMiscellaneousSetting(AdofaiChart::ChartSetting& _data, const rapidjson::Value& settings) {
            using MiscellaneousSettings = AdofaiChart::ChartSetting::MiscellaneousSettings;
            auto& data = _data.miscellaneousSetting;
            JsonValueAccessor accessor(settings);
            accessor.GetString("bgVideo", data.bgVideo);
            accessor.GetBool("loopVideo", data.loopVideo);
            accessor.GetNumber("vidOffset", data.vidOffset);
            accessor.GetBool("floorIconOutlines", data.floorIconOutlines);
            accessor.GetBool("stickToFloors", data.stickToFloors);
            accessor.GetEnum<AdofaiChart::Elements::Ease, AdofaiChart::Elements::EaseTable>("planetEase", data.planetEase);
            accessor.GetNumber("planetEaseParts", data.planetEaseParts);
            accessor.GetEnum<MiscellaneousSettings::PlanetEasePartBehavior, MiscellaneousSettings::PlanetEasePartBehaviorTable>("planetEasePartBehavior", data.planetEasePartBehavior);
            accessor.GetColor("defaultTextColor", data.defaultTextColor);
            accessor.GetColor("defaultTextShadowColor", data.defaultTextShadowColor);
            accessor.GetString("congratsText", data.congratsText);
            accessor.GetString("perfectText", data.perfectText);
        }
        static void parseLegary(AdofaiChart::ChartSetting& _data, const rapidjson::Value& settings) {
            auto& data = _data.legacy;
            JsonValueAccessor accessor(settings);
            accessor.GetBool("legacyFlash", data.legacyFlash);
            accessor.GetBool("legacyCamRelativeTo", data.legacyCamRelativeTo);
            accessor.GetBool("legacySpriteTiles", data.legacySpriteTiles);
            accessor.GetBool("legacyTween", data.legacyTween);
            accessor.GetBool("disableV15Features", data.disableV15Features);
        }
        // Undefined
        static void parseDLCMods(AdofaiChart::ChartSetting& _data, const rapidjson::Value& settings) {
            auto& data = _data.dlcMods;
            JsonValueAccessor accessor(settings);
            accessor.GetArrayForEach("requiredMods", [&data](const rapidjson::Value& value, size_t index) {
                std::string mod = value.GetString();
                data.requiredMods.push_back(mod);  // 将mod添加到 dlcMods 中
            });
        }
        static void checkLevelVersion(AdofaiChart::ChartSetting& data, const rapidjson::Value& settings) {
            auto& log = LogOverlay::GetInstance();
            auto& version = data.levelVersion.version;
            version = settings["version"].GetInt();
            if (version != AdofaiChart::ChartVersion) {
                if (version > AdofaiChart::ChartVersion) {
                    log.warn("Warning: The chart version (", version,
                        ") is higher than the supported version (", AdofaiChart::ChartVersion, ").");
                }
                else {
                    log.warn("Warning: The chart version (", version,
                        ") is lower than the supported version (", AdofaiChart::ChartVersion, ").");
                }
            }
        }
        
    };
};

class AdofaiChartLoader {
public:
    static AdofaiChart::ChartSetting LoadSettings(AdofaiChartJson* chart) {
        Check(chart);
        return ParseSetting(chart);
    }
protected:
    static AdofaiChart::ChartSetting ParseSetting(AdofaiChartJson* chart) {
        AdofaiChart::ChartSetting settingData = {};
        auto& doc = chart->GetChartDocument();
        AdofaiChartParser::SettingParser::Parse(settingData, doc);
        return settingData;
    }
    static void Check(AdofaiChartJson* chart) {
        if (!chart) {
            throw std::runtime_error("Chart instance is null!");
        }
        chart->isEmpty();
    }
};

class AdofaiChartJsonManager {
public:
    struct AdofaiChartData {
        AdofaiChartJson chart;
        std::string filepath;
        std::string chartName;
    };
private:
    std::vector<AdofaiChartData> charts;
    size_t maxSize = -1;
public:
    bool AddChart(const std::string& filePath, AdofaiChartJson::LoaderSetting setting) {
        if (charts.size() >= maxSize) { 
            throw std::overflow_error("Charts container has reached its maximum size: " + std::to_string(maxSize));
        }
        AdofaiChartData chartData;
        try {
            chartData.chart.ReadFileAndParse(filePath, setting);
        }
        catch (std::exception e) {
            LogOverlay::GetInstance().error(e.what());
            return false;
        }

        chartData.filepath = filePath;
        chartData.chartName = GetSong(chartData.chart);

        chartData.chartName = UTF8ToANSI(chartData.chartName);

        charts.push_back(chartData);
        return true;
    }

    std::string UTF8ToANSI(const std::string& utf8Str) {
        // 获取UTF-8字符串的长度
        int wideCharLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
        if (wideCharLen == 0) {
            // 处理错误
            return "";
        }

        // 转换为宽字符
        std::wstring wideStr(wideCharLen, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wideStr[0], wideCharLen);

        // 将宽字符转换为ANSI
        int ansiLen = WideCharToMultiByte($CSetting.codePageID, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (ansiLen == 0) {
            // 处理错误
            return "";
        }

        // 转换为ANSI字符串
        std::string ansiStr(ansiLen, '\0');
        WideCharToMultiByte(936, 0, wideStr.c_str(), -1, &ansiStr[0], ansiLen, nullptr, nullptr);

        ansiStr.pop_back();
        return ansiStr;
    }

    void OverwriteChart(const size_t index) {
        CheckIndexIsOutofRange(index);
        auto& chartData = charts[index];
        // 使用 RapidJSON 获取 JSON 字符串
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        // 使用 Writer 将 Document 数据写入字符串缓冲区
        chartData.chart.GetChartDocument().Accept(writer);

        // 打开文件流进行写入
        std::ofstream ofs(chartData.filepath);

        if (!ofs.is_open()) {
            LogOverlay::GetInstance().error("Failed to open file for writing: ", chartData.filepath);
            return;
        }

        // 将生成的字符串写入文件
        ofs << buffer.GetString();

        ofs.close();  // 关闭文件流
    }

    void ReloadChart(const size_t index, AdofaiChartJson::LoaderSetting setting) {
        CheckIndexIsOutofRange(index);
        AdofaiChartJson reloadChart;
        reloadChart.ReadFileAndParse(charts[index].filepath, setting);
        charts[index].chart = reloadChart;
    }

    void EraseChart(const size_t index) {
        CheckIndexIsOutofRange(index);
        charts.erase(charts.begin() + index);
    }

    AdofaiChartData& GetChart(const size_t index) {
        CheckIndexIsOutofRange(index);
        return charts[index];
    }

    void CheckIndexIsOutofRange(const size_t index) {
        if (index >= charts.size()) throw std::out_of_range("Chart Index out of range: " + std::to_string(index));
    }

    void ForEach(std::function<void(AdofaiChartData&)> func) {
        for (auto& chart : charts) {
            func(chart);
        }
    }

    std::string GetSong(AdofaiChartJson chartJson) {
        auto& chartDoc = chartJson.GetChartDocument();
        if (chartDoc.HasMember("settings") && chartDoc["settings"].IsObject()) {
            const rapidjson::Value& settings = chartDoc["settings"];

            if (settings.HasMember("song") && settings["song"].IsString()) {
                return settings["song"].GetString();
            }
            else {
                LogOverlay::GetInstance().error("song key is missing or not a string");
            }
        }
        return "";
    }

    void SetMaxSize(size_t maxSize) {
        this->maxSize = maxSize;
    }

    size_t size() const {
        return charts.size();
    }

    std::vector<AdofaiChartData>& data() {
        return charts;
    }
};

class AdofaiChartManager {
private:
    AdofaiChartJsonManager jsonManager;
    AdofaiChartJsonManager::AdofaiChartData* currentChart = nullptr;
public:
    void AddChart(const std::string& filePath, AdofaiChartJson::LoaderSetting setting) {
        if (jsonManager.AddChart(filePath, setting)) {
            currentChart = &jsonManager.GetChart(jsonManager.size() - 1);
        }
    }
    void SetCurrentChart(size_t index) {
        currentChart = &jsonManager.GetChart(index);
    }
    AdofaiChartJsonManager::AdofaiChartData& getChart(size_t index, bool current = false) {
        if (current) return *currentChart;
        return jsonManager.GetChart(index);
    }
};