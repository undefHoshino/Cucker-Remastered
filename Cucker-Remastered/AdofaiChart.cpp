#include "AdofaiChart.h"

bool AdofaiChartParser::JsonValueAccessor::verify = true;

AdofaiChartJson::AdofaiChartJson(const AdofaiChartJson& other) {
    chartDoc.CopyFrom(other.chartDoc, chartDoc.GetAllocator());
}

AdofaiChartJson& AdofaiChartJson::operator=(const AdofaiChartJson& other) {
    if (this != &other) {
        // 通过`CopyFrom`来实现深拷贝
        chartDoc.CopyFrom(other.chartDoc, chartDoc.GetAllocator());
    }
    return *this;
}

void AdofaiChartJson::ReadFileAndParse(const std::string& filePath, LoaderSetting setting) {
    std::string jsonContent = GetJsonContent(filePath, setting);
    ClearDocument();
    SettingApply(jsonContent, setting);

    if (chartDoc.Parse(jsonContent.c_str()).HasParseError()) {
        LogParseError(jsonContent);
        throw std::runtime_error("Json parse failed.");
    }
}

std::string AdofaiChartJson::GetJsonContent(const std::string& filePath, LoaderSetting setting) {
    if (setting.readByUtf16) {
        std::wifstream file(filePath, std::ios::in | std::ios::binary); // 以二进制模式读取，防止文本处理问题
        if (!file) {
            throw std::runtime_error("Failed to open file: " + filePath);
        }
        std::wstringstream buffer;
        buffer << file.rdbuf();
        file.close();

        std::wstring&& _temp = buffer.str();
        if (setting.removeBOM) {
            RemoveBOM<std::wstring>(_temp);
        }
        return Converter::Convert<std::wstring, std::string>(_temp);
    }
    else {
        std::ifstream file(filePath, std::ios::in | std::ios::binary); // 以二进制模式读取，防止文本处理问题
        if (!file) {
            throw std::runtime_error("Failed to open file: " + filePath);
        }
        std::string jsonContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        if (setting.removeBOM) {
            RemoveBOM<std::string>(jsonContent);
        }
        return jsonContent;
    }
}

void AdofaiChartJson::ClearDocument() {
    if (!chartDoc.IsNull()) {
        chartDoc = rapidjson::Document();
    }
}

void AdofaiChartJson::LogParseError(std::string& jsonContent) {
    auto& logger = LogOverlay::GetInstance();

    rapidjson::ParseErrorCode errorCode = chartDoc.GetParseError();
    size_t outputRange = 40;
    size_t errorOffset = chartDoc.GetErrorOffset();

    size_t start = (errorOffset > outputRange) ? errorOffset - outputRange : 0;
    size_t end = (errorOffset + outputRange < jsonContent.length()) ? errorOffset + outputRange : jsonContent.length();

    std::string errorContext = jsonContent.substr(start, end - start);

    errorContext.erase(
        std::remove_if(errorContext.begin(), errorContext.end(), [](char c) {
            return c < 32;
            }),
        errorContext.end());

    std::string strPoint(20 + (errorContext.length() / 2), ' ');
    strPoint += "^^^";

    size_t errorAtColumn = 1;
    size_t errorAtLine = 1;
    GetErrorAtLine(jsonContent, errorOffset, errorAtColumn, errorAtLine);

    logger.error("Error during parse json:");
    logger.error("  Code:", errorCode);
    logger.error("  Offset: ", errorOffset, " (Line: ", errorAtLine, " , Column: ", errorAtColumn, ")");
    logger.error("  Description: ", GetParseError_En(errorCode));
    logger.error("  Error context: ...", errorContext, "...");
    logger.error(strPoint);
}

void AdofaiChartJson::GetErrorAtLine(std::string& jsonContent, size_t errorOffset, size_t& column, size_t& line) {
    column = 1;
    line = 1;
    // 计算行和列
    for (size_t i = 0; i < errorOffset; ++i) {
        if (jsonContent[i] == '\n') {
            line++;
            column = 1;  // 下一行列数重置
        }
        else {
            column++;
        }
    }

}

void AdofaiChartJson::SettingApply(std::string& jsonContent, LoaderSetting setting) {
    auto& log = LogOverlay::GetInstance();

    if (setting.clearControlChar) {
        log.info("Removing Controlchars");
        jsonContent.erase(
            std::remove_if(jsonContent.begin(), jsonContent.end(), [](char c) {
                // 允许 \n 和 \r，过滤其他控制字符
                return (c < 0x20 && c != '\n' && c != '\r');
                }),
            jsonContent.end());

    }
    if (setting.fixTrailingCommas) {
        log.info("Replacing Trailing Commas");
        jsonContent = std::regex_replace(jsonContent, std::regex(",\\s*(?=[}\\]])"), "");
    }
    if (setting.fixTailCommas) {
        log.info("Fixing Tail Commas");

        // 修复如果遇到 "]" 后紧跟着 "" 时，插入一个逗号
        jsonContent = std::regex_replace(jsonContent, std::regex(R"(\]\s*(?=\s*")\s*)"), "] ,");
        log.info("Complete Stage 1 / 1");

    }
}

rapidjson::Document& AdofaiChartJson::GetChartDocument() {
    return chartDoc;
}

void AdofaiChartJson::isEmpty() {
    if (chartDoc.IsObject()) {
        if (chartDoc.ObjectEmpty()) {  // 用对象特定的空判断
            throw std::runtime_error("Parsed document is unexpectedly empty.");
        }
    }
    else if (chartDoc.IsArray()) {
        if (chartDoc.Empty()) {
            throw std::runtime_error("Parsed document is unexpectedly empty.");
        }
    }
    else {
        throw std::runtime_error("Parsed document is neither an object nor an array.");
    }
}