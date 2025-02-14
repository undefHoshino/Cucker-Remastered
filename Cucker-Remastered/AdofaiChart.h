#pragma once
#include <string>
#include <regex>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <functional>
#include <prettywriter.h>
#include <document.h>
#include <error/en.h> // �ṩ��������ַ�������
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

    // ����ö��
#define DEFINE_ENUM(EnumName, ...)           \
    enum struct EnumName {                   \
        __VA_ARGS__                          \
    };

// ����ӳ���
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
            // �ٶ���
            enum struct SpeedType {
                Bpm,        // BPM
                Multiplier  // ��Ƶ��
            };
        };
    };

    struct Elements {
        // ������
        DEFINE_ENUM(HitSound,
            None,            // ��
            Hat,             // ��ñ��
            Kick,            // �׹�
            Shaker,          // ɳ�
            Sizzle,          // ��Ƭ
            Chuck,           // �����׹�
            ShakerLoud,      // ɳ鳣��죩
            Hammer,          // ����
            KickChroma,      // �׹�-��
            SnareAcoustic2,  // ����ԭ��2
            Sidestick,       // �ı�
            Stick,           // �İ�
            ReverbClack,     // ������ž
            Squareshot,      // ���ǿ���
            PowerDown,       // ������Ч
            PowerUp,         // ������Ч
            KickHouse,       // �׹�-����
            KickRupture,     // �׹�-�ѽ�
            HatHouse,        // ��Ƭ-����
            SnareHouse,      // ����-����
            SnareVapor,      // ����-����
            ClapHit,         // ����
            ClapHitEcho,     // ��������
            ReverbClap,      // ��������
            FireTile,        // �𷽿�
            IceTile,         // ������
            VehiclePositive, // ��������
            VehicleNegative  // ��������
        );

        // ����Easeö��
        DEFINE_ENUM(Ease,
            Linear,       // ����
            InSine,       // �����ڲ�
            OutSine,      // �������
            InOutSine,    // ���������
            InQuad,       // �����ڲ�
            OutQuad,      // �������
            InOutQuad,    // ���������
            InCubic,      // �����ڲ�
            OutCubic,     // �������
            InOutCubic,   // ���������
            InQuart,      // �Ĵ��ڲ�
            OutQuart,     // �Ĵ����
            InOutQuart,   // �Ĵ������
            InQuint,      // ����ڲ�
            OutQuint,     // ������
            InOutQuint,   // ��������
            InExpo,       // ָ���ڲ�
            OutExpo,      // ָ�����
            InOutExpo,    // ָ�������
            InCirc,       // Բ���ڲ�
            OutCirc,      // Բ�����
            InOutCirc,    // Բ�������
            InElastic,    // �����ڲ�
            OutElastic,   // �������
            InOutElastic, // ���������
            InBack,       // �����ڲ�
            OutBack,      // �������
            InOutBack,    // ���������
            InBounce,     // �����ڲ�
            OutBounce,    // �������
            InOutBounce,  // ���������
            Flash,        // ����
            InFlash,      // �����ڲ�
            OutFlash,     // �������
            InOutFlash    // ���������
        );

        // �������ַ�����
        DEFINE_STRING_TABLE(HitSound,
            { "None", HitSound::None },              // ��
            { "Hat", HitSound::Hat },                // ��ñ��
            { "Kick", HitSound::Kick },              // �׹�
            { "Shaker", HitSound::Shaker },          // ɳ�
            { "Sizzle", HitSound::Sizzle },          // ��Ƭ
            { "Chuck", HitSound::Chuck },            // �����׹�
            { "ShakerLoud", HitSound::ShakerLoud },  // ɳ鳣��죩
            { "Hammer", HitSound::Hammer },          // ����
            { "KickChroma", HitSound::KickChroma },  // �׹�-��
            { "SnareAcoustic2", HitSound::SnareAcoustic2 },  // ����ԭ��2
            { "Sidestick", HitSound::Sidestick },    // �ı�
            { "Stick", HitSound::Stick },            // �İ�
            { "ReverbClack", HitSound::ReverbClack },// ������ž
            { "Squareshot", HitSound::Squareshot },  // ���ǿ���
            { "PowerDown", HitSound::PowerDown },    // ������Ч
            { "PowerUp", HitSound::PowerUp },        // ������Ч
            { "KickHouse", HitSound::KickHouse },    // �׹�-����
            { "KickRupture", HitSound::KickRupture },// �׹�-�ѽ�
            { "HatHouse", HitSound::HatHouse },      // ��Ƭ-����
            { "SnareHouse", HitSound::SnareHouse },  // ����-����
            { "SnareVapor", HitSound::SnareVapor },  // ����-����
            { "ClapHit", HitSound::ClapHit },        // ����
            { "ClapHitEcho", HitSound::ClapHitEcho },// ��������
            { "ReverbClap", HitSound::ReverbClap },  // ��������
            { "FireTile", HitSound::FireTile },      // �𷽿�
            { "IceTile", HitSound::IceTile },        // ������
            { "VehiclePositive", HitSound::VehiclePositive }, // ��������
            { "VehicleNegative", HitSound::VehicleNegative }  // ��������
        );

        // ����Ease�ַ�����
        DEFINE_STRING_TABLE(Ease,
            { "Linear", Ease::Linear },               // ����
            { "InSine", Ease::InSine },               // �����ڲ�
            { "OutSine", Ease::OutSine },             // �������
            { "InOutSine", Ease::InOutSine },         // ���������
            { "InQuad", Ease::InQuad },               // �����ڲ�
            { "OutQuad", Ease::OutQuad },             // �������
            { "InOutQuad", Ease::InOutQuad },         // ���������
            { "InCubic", Ease::InCubic },             // �����ڲ�
            { "OutCubic", Ease::OutCubic },           // �������
            { "InOutCubic", Ease::InOutCubic },       // ���������
            { "InQuart", Ease::InQuart },             // �Ĵ��ڲ�
            { "OutQuart", Ease::OutQuart },           // �Ĵ����
            { "InOutQuart", Ease::InOutQuart },       // �Ĵ������
            { "InQuint", Ease::InQuint },             // ����ڲ�
            { "OutQuint", Ease::OutQuint },           // ������
            { "InOutQuint", Ease::InOutQuint },       // ��������
            { "InExpo", Ease::InExpo },               // ָ���ڲ�
            { "OutExpo", Ease::OutExpo },             // ָ�����
            { "InOutExpo", Ease::InOutExpo },         // ָ�������
            { "InCirc", Ease::InCirc },               // Բ���ڲ�
            { "OutCirc", Ease::OutCirc },             // Բ�����
            { "InOutCirc", Ease::InOutCirc },         // Բ�������
            { "InElastic", Ease::InElastic },         // �����ڲ�
            { "OutElastic", Ease::OutElastic },       // �������
            { "InOutElastic", Ease::InOutElastic },   // ���������
            { "InBack", Ease::InBack },               // �����ڲ�
            { "OutBack", Ease::OutBack },             // �������
            { "InOutBack", Ease::InOutBack },         // ���������
            { "InBounce", Ease::InBounce },           // �����ڲ�
            { "OutBounce", Ease::OutBounce },         // �������
            { "InOutBounce", Ease::InOutBounce },     // ���������
            { "Flash", Ease::Flash },                 // ����
            { "InFlash", Ease::InFlash },             // �����ڲ�
            { "OutFlash", Ease::OutFlash },           // �������
            { "InOutFlash", Ease::InOutFlash }        // ���������
        );

        struct Track {
            // �����ɫ����
            DEFINE_ENUM(ColorType,
                Single,     // ����
                Stripes,    // ����
                Glow,       // ����
                Blink,      // ��˸
                Switch,     // �л�
                Rainbow,    // �ʺ�
                Volume      // ����
            );

            // ��ɫ��������   
            DEFINE_ENUM(ColorPulse,
                None,       // ��
                Forward,    // ǰ��
                Backward    // ����
            );

            // ������
            DEFINE_ENUM(Style,
                Standard,   // ��׼
                Neon,       // �޺�
                NeonLight,  // �޺��
                Basic,      // ����
                Gems,       // ��ʯ
                Minimal     // ����
            );

            // ������ֶ���
            DEFINE_ENUM(AppearAnimation,
                None,           // ��
                Assemble,       // �ۺ�
                Assemble_Far,   // �ۺϣ�Զ
                Extend,         // ����
                Grow,           // ����
                Grow_Spin,      // ����+��ת
                Fade,           // ����
                Drop,           // ��׹
                Rise            // ����
            );

            // �����ʧ����
            DEFINE_ENUM(DisappearAnimation,
                None,           // ��
                Scatter,        // ��ɢ
                Scatter_Far,    // ��ɢ��Զ
                Retract,        // ����
                Shrink,         // ����
                Shrink_Spin,    // ����+��ת
                Fade            // ����
            );

            // ��ɫ���������ַ�����
            DEFINE_STRING_TABLE(ColorPulse,
                { "None", ColorPulse::None },         // ��
                { "Forward", ColorPulse::Forward },   // ǰ��
                { "Backward", ColorPulse::Backward }  // ����
            );

            // �����ɫ�����ַ�����
            DEFINE_STRING_TABLE(ColorType,
                { "Single", ColorType::Single },     // ����
                { "Stripes", ColorType::Stripes },   // ����
                { "Glow", ColorType::Glow },         // ����
                { "Blink", ColorType::Blink },       // ��˸
                { "Switch", ColorType::Switch },     // �л�
                { "Rainbow", ColorType::Rainbow },   // �ʺ�
                { "Volume", ColorType::Volume }      // ����
            );

            // �������ַ�����
            DEFINE_STRING_TABLE(Style,
                { "Standard", Style::Standard },     // ��׼
                { "Neon", Style::Neon },             // �޺�
                { "NeonLight", Style::NeonLight },   // �޺��
                { "Basic", Style::Basic },           // ����
                { "Gems", Style::Gems },             // ��ʯ
                { "Minimal", Style::Minimal }        // ����
            );

            // ������ֶ����ַ�����
            DEFINE_STRING_TABLE(AppearAnimation,
                { "None", AppearAnimation::None },             // ��
                { "Assemble", AppearAnimation::Assemble },     // �ۺ�
                { "Assemble_Far", AppearAnimation::Assemble_Far }, // �ۺϣ�Զ
                { "Extend", AppearAnimation::Extend },         // ����
                { "Grow", AppearAnimation::Grow },             // ����
                { "Grow_Spin", AppearAnimation::Grow_Spin },   // ����+��ת
                { "Fade", AppearAnimation::Fade },             // ����
                { "Drop", AppearAnimation::Drop },             // ��׹
                { "Rise", AppearAnimation::Rise }              // ����
            );

            // �����ʧ�����ַ�����
            DEFINE_STRING_TABLE(DisappearAnimation,
                { "None", DisappearAnimation::None },             // ��
                { "Scatter", DisappearAnimation::Scatter },       // ��ɢ
                { "Scatter_Far", DisappearAnimation::Scatter_Far }, // ��ɢ��Զ
                { "Retract", DisappearAnimation::Retract },       // ����
                { "Shrink", DisappearAnimation::Shrink },         // ����
                { "Shrink_Spin", DisappearAnimation::Shrink_Spin }, // ����+��ת
                { "Fade", DisappearAnimation::Fade }              // ����
            );
        };
        struct BackGround {
            // �̳̱���ͼ��
            DEFINE_ENUM(DefaultShapeType,
                Default,        // Ĭ��
                SingleColor,    // ��ɫ
                Disabled        // ��
            );

            // ͼ����ʾģʽ
            DEFINE_ENUM(DisplayMode,
                FitToScreen,    // ����
                Unscaled,       // δ����
                Tiled           // ƽ��
            );

            // �̳̱���ͼ���ַ�����
            DEFINE_STRING_TABLE(DefaultShapeType,
                { "Default", DefaultShapeType::Default },        // Ĭ��
                { "SingleColor", DefaultShapeType::SingleColor },// ��ɫ
                { "Disabled", DefaultShapeType::Disabled }       // ��
            );

            // ͼ����ʾģʽ�ַ�����
            DEFINE_STRING_TABLE(DisplayMode,
                { "FitToScreen", DisplayMode::FitToScreen },    // ����
                { "Unscaled", DisplayMode::Unscaled },          // δ����
                { "Tiled", DisplayMode::Tiled }                 // ƽ��
            );
        };

        struct Camera {
            // �����
            DEFINE_ENUM(RelativeTo,
                Player,                 // ���
                Tile,                   // ����
                Global,                 // ȫ��
                LastPositionNoRotation  // �ϴ�λ�ã�����ת��
            );

            // ����� �ַ�����
            DEFINE_STRING_TABLE(RelativeTo,
                { "Player", RelativeTo::Player },                 // ���
                { "Tile", RelativeTo::Tile },                     // ����
                { "Global", RelativeTo::Global },                 // ȫ��
                { "LastPositionNoRotation", RelativeTo::LastPositionNoRotation }  // �ϴ�λ�ã�����ת��
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
            // ת�� SpecialArtistType ö��
            DEFINE_ENUM(SpecialArtistType,
                None,               // ��
                AuthorIsArtist,     // ���������ұ���
                PublicLicense       // ������Ȩ
            );

            DEFINE_STRING_TABLE(SpecialArtistType,
                { "None", SpecialArtistType::None },                           // ��
                { "AuthorIsArtist", SpecialArtistType::AuthorIsArtist },       // ���������ұ���
                { "PublicLicense", SpecialArtistType::PublicLicense }          // ������Ȩ
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
            // ת�� PlanetEasePartBehavior ö��
            DEFINE_ENUM(PlanetEasePartBehavior,
                Mirror, // ����
                Repeat  // �ظ�
            );

            DEFINE_STRING_TABLE(PlanetEasePartBehavior,
                { "Mirror", PlanetEasePartBehavior::Mirror }, // ����
                { "Repeat", PlanetEasePartBehavior::Repeat }  // �ظ�
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
            jsonContent.erase(0, 3); // ȥ�� BOM
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
                data.requiredMods.push_back(mod);  // ��mod��ӵ� dlcMods ��
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
        // ��ȡUTF-8�ַ����ĳ���
        int wideCharLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
        if (wideCharLen == 0) {
            // �������
            return "";
        }

        // ת��Ϊ���ַ�
        std::wstring wideStr(wideCharLen, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wideStr[0], wideCharLen);

        // �����ַ�ת��ΪANSI
        int ansiLen = WideCharToMultiByte($CSetting.codePageID, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (ansiLen == 0) {
            // �������
            return "";
        }

        // ת��ΪANSI�ַ���
        std::string ansiStr(ansiLen, '\0');
        WideCharToMultiByte(936, 0, wideStr.c_str(), -1, &ansiStr[0], ansiLen, nullptr, nullptr);

        ansiStr.pop_back();
        return ansiStr;
    }

    void OverwriteChart(const size_t index) {
        CheckIndexIsOutofRange(index);
        auto& chartData = charts[index];
        // ʹ�� RapidJSON ��ȡ JSON �ַ���
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        // ʹ�� Writer �� Document ����д���ַ���������
        chartData.chart.GetChartDocument().Accept(writer);

        // ���ļ�������д��
        std::ofstream ofs(chartData.filepath);

        if (!ofs.is_open()) {
            LogOverlay::GetInstance().error("Failed to open file for writing: ", chartData.filepath);
            return;
        }

        // �����ɵ��ַ���д���ļ�
        ofs << buffer.GetString();

        ofs.close();  // �ر��ļ���
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