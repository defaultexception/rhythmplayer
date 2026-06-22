#include "osuparser.h"

// #include <bits/stdc++.h>
#include <unordered_map>
#include <fstream>
#include <functional>
#include <variant>
#include <string>
#include <iostream>
#include <ranges>

// ParseMapType used for finding parser to use for section
using ParseMapType = std::variant<
    std::string,
    GeneralSection,
    EditorSection,
    MetadataSection,
    DifficultySection,
    std::vector<Event*>,
    std::vector<TimingPoint>,
    std::vector<Colour>,
    std::vector<HitObject*>
>;

// Helper Functions
std::vector<std::string> substring(const std::string& str, const std::string& delim)
{
    std::vector<std::string> res;
    size_t lastPos = 0, nextPos;

    while ((nextPos = str.find(delim, lastPos)) != std::string::npos)
    {
        res.push_back(str.substr(lastPos, nextPos - lastPos));
        lastPos = nextPos + delim.length();
    }
    res.push_back(str.substr(lastPos));

    return res;
}

// Helper Functions
std::vector<std::string> keyValSubstr(const std::string& str, const std::string& delim)
{
    std::vector<std::string> res;
    size_t lastPos = 0, nextPos;

    nextPos = str.find(delim, lastPos);
    if (nextPos != std::string::npos)
    {
        res.push_back(str.substr(lastPos, nextPos - lastPos));
        lastPos = nextPos + delim.length();
    }
    res.push_back(str.substr(lastPos));

    return res;
}

// Get Parser
template<typename T>
auto getParser(const T& parsers, const std::string& name)
{
    auto it = parsers.find(name);
    if (it != parsers.end())
    {
        return it->second;
    }
    throw std::invalid_argument("Parser not found.");
}

// Copied from Stack Overflow:
// Trim from the start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch) && ch != '\r';
    }));
}

// Trim from the end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch) && ch != '\r';
    }).base(), s.end());
}

// Trim from both ends (in place)
inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

/*
    Section Parsers
*/

// Parse FileFormat
std::string parseFileFormat(const std::vector<std::string>& lines)
{
    if (lines.size() > 0)
    {
        std::string fileFormat = lines[0];
        return fileFormat.substr(3);
    }
    return "";
}

// GeneralSection parsers
static const std::unordered_map<std::string, std::function<void(GeneralSection&, const std::string&)>> generalParsers = {
    {"AudioFilename", [](GeneralSection& generalSection, const std::string& val){ generalSection.AudioFilename = val; }},
    {"AudioLeadIn", [](GeneralSection& generalSection, const std::string& val){ generalSection.AudioLeadIn = std::stoi(val); }},
    {"PreviewTime", [](GeneralSection& generalSection, const std::string& val){ generalSection.PreviewTime = std::stoi(val); }},
    {"Countdown", [](GeneralSection& generalSection, const std::string& val){ generalSection.Countdown = std::stoi(val); }},
    {"SampleSet", [](GeneralSection& generalSection, const std::string& val){ generalSection.SampleSet = val; }},
    {"StackLeniency", [](GeneralSection& generalSection, const std::string& val){ generalSection.StackLeniency = std::stod(val); }},
    {"Mode", [](GeneralSection& generalSection, const std::string& val){ generalSection.Mode = std::stoi(val); }},
    {"LetterboxInBreaks", [](GeneralSection& generalSection, const std::string& val){ generalSection.LetterboxInBreaks = std::stoi(val); }},
    {"UseSkinSprites", [](GeneralSection& generalSection, const std::string& val){ generalSection.UseSkinSprites = std::stoi(val); }},
    {"OverlayPosition", [](GeneralSection& generalSection, const std::string& val){ generalSection.OverlayPosition = val; }},
    {"SkinPreference", [](GeneralSection& generalSection, const std::string& val){ generalSection.SkinPreference = val; }},
    {"EpilepsyWarning", [](GeneralSection& generalSection, const std::string& val){ generalSection.EpilepsyWarning = std::stoi(val); }},
    {"CountdownOffset", [](GeneralSection& generalSection, const std::string& val){ generalSection.CountdownOffset = std::stoi(val); }},
    {"SpecialStyle", [](GeneralSection& generalSection, const std::string& val){ generalSection.SpecialStyle = std::stoi(val); }},
    {"WidescreenStoryboard", [](GeneralSection& generalSection, const std::string& val){ generalSection.WidescreenStoryboard = std::stoi(val); }},
    {"SamplesMatchPlaybackRate", [](GeneralSection& generalSection, const std::string& val){ generalSection.SamplesMatchPlaybackRate = std::stoi(val); }},
};

// Parse GeneralSection
GeneralSection parseGeneralSection(const std::vector<std::string>& lines)
{
    GeneralSection generalSection;

    for (const auto& line : lines)
    {
        std::vector<std::string> keyValuePair = keyValSubstr(line, ":");
        if (keyValuePair.size() < 2)
            continue;

        trim(keyValuePair[1]);
        auto parser = getParser(generalParsers, keyValuePair[0]);
        parser(generalSection, keyValuePair[1]);
    }

    return generalSection;
}

// EditorSection parsers
static const std::unordered_map<std::string, std::function<void(EditorSection&, const std::string&)>> editorParsers = {
    {"Bookmarks", [](EditorSection& editorSection, const std::string& val){ 
        editorSection.Bookmarks = std::vector<int>();
        for (const auto& i : substring(val, ","))
            editorSection.Bookmarks.push_back(std::stoi(i));
    }},
    {"DistanceSpacing", [](EditorSection& editorSection, const std::string& val){ editorSection.DistanceSpacing = std::stod(val); }},
    {"BeatDivisor", [](EditorSection& editorSection, const std::string& val){ editorSection.BeatDivisor = std::stoi(val); }},
    {"GridSize", [](EditorSection& editorSection, const std::string& val){ editorSection.GridSize = std::stoi(val); }},
    {"TimelineZoom", [](EditorSection& editorSection, const std::string& val){ editorSection.TimelineZoom = std::stod(val); }},
};

EditorSection parseEditorSection(const std::vector<std::string>& lines)
{
    EditorSection editorSection;

    for (const auto& line : lines)
    {
        std::vector<std::string> keyValuePair = keyValSubstr(line, ":");
        if (keyValuePair.size() < 2)
            continue;

        trim(keyValuePair[1]);
        auto parser = getParser(editorParsers, keyValuePair[0]);
        parser(editorSection, keyValuePair[1]);
    }

    return editorSection;
}

// MetadataSection parsers
static const std::unordered_map<std::string, std::function<void(MetadataSection&, const std::string&)>> metadataParsers = {
    {"Title", [](MetadataSection& metadataSection, const std::string& val){ metadataSection.Title = val; }},
    {"TitleUnicode", [](MetadataSection& metadataSection, const std::string& val){ metadataSection.TitleUnicode = val; }},
    {"Artist", [](MetadataSection& metadataSection, const std::string& val){ metadataSection.Artist = val; }},
    {"ArtistUnicode", [](MetadataSection& metadataSection, const std::string& val){ metadataSection.ArtistUnicode = val; }},
    {"Creator", [](MetadataSection& metadataSection, const std::string& val){ metadataSection.Creator = val; }},
    {"Version", [](MetadataSection& metadataSection, const std::string& val){ metadataSection.Version = val; }},
    {"Source", [](MetadataSection& metadataSection, const std::string& val){ metadataSection.Source = val; }},
    {"Tags", [](MetadataSection& metadataSection, const std::string& val){ metadataSection.Tags = val; }},
    {"BeatmapID", [](MetadataSection& metadataSection, const std::string& val){ metadataSection.BeatmapID = std::stoi(val); }},
    {"BeatmapSetID", [](MetadataSection& metadataSection, const std::string& val){ metadataSection.BeatmapSetID = std::stoi(val); }},
};

MetadataSection parseMetadataSection(const std::vector<std::string>& lines)
{
    MetadataSection metadataSection;

    for (const auto& line : lines)
    {
        std::vector<std::string> keyValuePair = keyValSubstr(line, ":");
        if (keyValuePair.size() < 2)
            continue;

        std::string val = keyValuePair[1];
        trim(val); // Lazer adds a space

        auto parser = getParser(metadataParsers, keyValuePair[0]);
        parser(metadataSection, val);
    }

    return metadataSection;
}

// DifficultySection parsers
static const std::unordered_map<std::string, std::function<void(DifficultySection&, const std::string&)>> difficultyParsers = {
    {"HPDrainRate", [](DifficultySection& difficultySection, const std::string& val){ difficultySection.HPDrainRate = std::stod(val); }},
    {"CircleSize", [](DifficultySection& difficultySection, const std::string& val){ difficultySection.CircleSize = std::stod(val); }},
    {"OverallDifficulty", [](DifficultySection& difficultySection, const std::string& val){ difficultySection.OverallDifficulty = std::stod(val); }},
    {"ApproachRate", [](DifficultySection& difficultySection, const std::string& val){ difficultySection.ApproachRate = std::stod(val); }},
    {"SliderMultiplier", [](DifficultySection& difficultySection, const std::string& val){ difficultySection.SliderMultiplier = std::stod(val); }},
    {"SliderTickRate", [](DifficultySection& difficultySection, const std::string& val){ difficultySection.SliderTickRate = std::stod(val); }},
};

DifficultySection parseDifficultySection(const std::vector<std::string>& lines)
{
    DifficultySection difficultySection;

    for (const auto& line : lines)
    {
        std::vector<std::string> keyValuePair = keyValSubstr(line, ":");
        if (keyValuePair.size() < 2)
            continue;

        trim(keyValuePair[1]);
        auto parser = getParser(difficultyParsers, keyValuePair[0]);
        parser(difficultySection, keyValuePair[1]);
    }

    return difficultySection;
}

std::vector<Event*> parseEventsSection(const std::vector<std::string>& lines)
{
    std::vector<Event*> events;

    for (const auto& line : lines)
    {
        std::vector<std::string> elements = substring(line, ",");

        if (elements.size() < 3)
            continue;

        int currentEventType = EUnusedEvent;
        std::vector<std::variant<int, std::string>> currentEventParams;

        const std::string& eventType = elements[0];
        if (eventType.length() > 1) // String
        {
            if (eventType == "Video")
                currentEventType = EEventType::EVideoEvent;
            if (eventType == "Break")
                currentEventType = EEventType::EBreakEvent;
            else
                currentEventType = EEventType::EUnusedEvent;
        }
        else // Integer
            currentEventType = std::stoi(eventType);

        // Create Event and push to events
        Event* currentEvent = nullptr;
        if (currentEventType == EEventType::EVideoEvent)
            currentEvent = new VideoEvent();
        else if (currentEventType == EEventType::EBreakEvent)
            currentEvent = new BreakEvent();
        else
            currentEvent = new BackgroundEvent();

        // Base Elements of Event
        currentEvent->EventType = currentEventType;
        currentEvent->StartTime = std::stoi(elements[1]);
        
        // EventParams
        if (currentEventType == EEventType::EBackgroundEvent ||
            currentEventType == EEventType::EVideoEvent)
        {
            currentEventParams.push_back(elements[2]); // fileName
            currentEventParams.push_back(std::stoi(elements[3])); // xOffset
            currentEventParams.push_back(std::stoi(elements[4])); // yOffset
        }
        else if (currentEventType == EEventType::EBreakEvent)
        {
            currentEventParams.push_back(std::stoi(elements[2])); // endTime
        }
        currentEvent->EventParams = currentEventParams;

        events.push_back(currentEvent);
    }

    return events;
}

std::vector<TimingPoint> parseTimingPointsSection(const std::vector<std::string>& lines)
{
    std::vector<TimingPoint> timingPoints;
    TimingPoint currentTimingPoint;

    for (const auto& line : lines)
    {
        std::vector<std::string> elements = substring(line, ",");

        if (elements.size() <= 1)
            continue;

        currentTimingPoint = TimingPoint();

        currentTimingPoint.Time = std::stod(elements[0]);
        currentTimingPoint.BeatLength = std::stod(elements[1]);
        currentTimingPoint.Meter = std::stoi(elements[2]);
        currentTimingPoint.SampleSet = std::stoi(elements[3]);
        currentTimingPoint.SampleIndex = std::stoi(elements[4]);
        currentTimingPoint.Volume = std::stoi(elements[5]);
        currentTimingPoint.Uninherited = std::stoi(elements[6]);
        currentTimingPoint.Effects = std::stoi(elements[7]);

        timingPoints.push_back(currentTimingPoint);
    }

    return timingPoints;
}

std::vector<Colour> parseColoursSection(const std::vector<std::string>& lines)
{
    std::vector<Colour> colours;
    Colour currentColour;

    for (const auto& line : lines)
    {
        std::vector<std::string> elements = substring(line, ":");

        if (elements.size() <= 1)
            continue;

        for (auto& elem : elements)
            trim(elem);

        currentColour = Colour();

        std::string colourKey = elements[0];

        // Differentiate colour types
        currentColour.ComboNum = -1; // Default value
        if (colourKey.contains("Combo"))
        {
            currentColour.ColourType = EColourType::Combo;
            
            // Get combo num and add to colour
            if (colourKey.length() > 5) // "Combo" length
            {
                std::string comboNumStr = colourKey.substr(5, colourKey.length());
                currentColour.ComboNum = std::stoi(comboNumStr);
            }
        }
        else if (colourKey.contains("SliderTrackOverride"))
            currentColour.ColourType = EColourType::SliderTrackOveride;
        else if (colourKey.contains("SliderBorder"))
            currentColour.ColourType = EColourType::SliderBorder;

        // Add RGBA
        std::vector<std::string> rgba = substring(elements[1], ",");
        currentColour.Red = std::stoi(rgba[0]);
        currentColour.Green = std::stoi(rgba[1]);
        currentColour.Blue = std::stoi(rgba[2]);
        if (rgba.size() >= 4) // Optional Alpha
            currentColour.Alpha = std::stoi(rgba[3]);

        colours.push_back(currentColour);
    }

    return colours;
}

std::vector<HitObject*> parseHitObjectsSection(const std::vector<std::string>& lines)
{
    std::vector<HitObject*> hitObjects;
    HitObject* currentHitObject = nullptr;

    for (const auto& line : lines)
    {
        std::vector<std::string> elements = substring(line, ",");

        // Get Basic Values (x, y, time, type, hitsound)
        int currentX        = std::stoi(elements[0]);
        int currentY        = std::stoi(elements[1]);
        int currentTime     = std::stoi(elements[2]);
        int currentType     = std::stoi(elements[3]);
        int currentHitsound = std::stoi(elements[4]);

        // Create based on type
        if (HitObject::isHitCircle(currentType))
            currentHitObject = new HitObject();
        else if (HitObject::isSlider(currentType))
            currentHitObject = new SliderHitObject();
        else if (HitObject::isSpinner(currentType))
            currentHitObject = new SpinnerHitObject();
        else if (HitObject::isHoldNote(currentType))
            currentHitObject = new HoldNoteHitObject();

        // Add base values to HitObject
        currentHitObject->X = currentX;
        currentHitObject->Y = currentY;
        currentHitObject->Time = currentTime;
        currentHitObject->Type = currentType;
        currentHitObject->HitSound = currentHitsound;

        // Get ObjectParams
        const int OBJ_PARAMS_START = 5;
        for (int i = OBJ_PARAMS_START; i < elements.size(); i++)
        {
            const std::string& currentStr = elements[i];

            if (currentHitObject->isHitCircle())
                break;
            else if (currentHitObject->isSlider())
            {
                if (i == OBJ_PARAMS_START)
                {
                    // curveType
                    currentHitObject->ObjectParams.push_back(currentStr[0]);

                    // curvePoints
                    std::vector<std::string> curvePointsStrs = substring(
                        currentStr.substr(2), "|"
                    );
                    currentHitObject->ObjectParams.push_back(curvePointsStrs);
                }
                else if (i == OBJ_PARAMS_START+1) // slides
                    currentHitObject->ObjectParams.push_back(std::stoi(currentStr));
                else if (i == OBJ_PARAMS_START+2) // length
                    currentHitObject->ObjectParams.push_back(std::stod(currentStr));
                else if (i == OBJ_PARAMS_START+3)
                {
                    // edgeSounds
                    std::vector<std::string> edgeSoundStrs = substring(
                        currentStr, "|"
                    );
                    std::vector<int> edgeSoundInts;
                    for (const auto& str : edgeSoundStrs)
                        edgeSoundInts.push_back(std::stoi(str));

                    currentHitObject->ObjectParams.push_back(edgeSoundInts);
                }
                else if (i == OBJ_PARAMS_START+4)
                {
                    // edgeSets
                    std::vector<std::string> edgeSetsStrs = substring(
                        currentStr, "|"
                    );
                    currentHitObject->ObjectParams.push_back(edgeSetsStrs);
                    break;
                }
            }
            else if (currentHitObject->isSpinner())
            {
                if (i == OBJ_PARAMS_START) // endTime
                {
                    currentHitObject->ObjectParams.push_back(std::stoi(currentStr));
                    break;
                }
            }
            else if (currentHitObject->isHoldNote())
            {
                if (i == OBJ_PARAMS_START) // endTime
                {
                    std::vector<std::string> edgeSetsStrs = substring(
                        currentStr, ":"
                    );
                    currentHitObject->ObjectParams.push_back(std::stoi(edgeSetsStrs[0]));
                    break;
                }
            }
            else {}
        }

        // Get HitSample
        std::vector<std::string> hitSampleStrings = substring(
            elements[elements.size()-1], ":"
        );
        // Add Integers
        for (int i = 0; i < 4; i++)
            currentHitObject->HitSample.push_back(std::stoi(hitSampleStrings[i]));
        // Add Filename String (optional)
        if (hitSampleStrings.size() > 4)
            currentHitObject->HitSample.push_back(hitSampleStrings[4]);

        hitObjects.push_back(currentHitObject);
    }

    return hitObjects;
}

// All parsers in map, lookup by section string
static const std::unordered_map<std::string, 
    std::function<ParseMapType(const std::vector<std::string>&)>> sectionParsers = {
        {"", parseFileFormat},
        {"[General]", parseGeneralSection},
        {"[Editor]", parseEditorSection},
        {"[Metadata]", parseMetadataSection},
        {"[Difficulty]", parseDifficultySection},
        {"[Events]", parseEventsSection},
        {"[TimingPoints]", parseTimingPointsSection},
        {"[Colours]", parseColoursSection},
        {"[HitObjects]", parseHitObjectsSection},
};

// Parse osu mania/taiko map
OsuMap* OsuParser::parseMap(const std::string& filePath)
{
    std::ifstream osuFile(filePath);
    OsuMap* newMap = new OsuMap();

    if (!osuFile.is_open()) // Check open
    {
        std::cerr << "Could not open osu file: " << filePath << std::endl;
        return newMap;
    }

    std::string currentSection = "";
    std::vector<std::string> sectionLines;
    std::string line;
    std::vector<ParseMapType> parseResults;
    while (std::getline(osuFile, line))
    {
        trim(line);

        // If line is the start of a new section, parse then start new
        if (line.size() > 0 && line[0] == '[')
        {
            // Run section parser
            auto it = sectionParsers.find(currentSection);
            if (it != sectionParsers.end())
            {
                auto res = it->second(sectionLines);
                parseResults.push_back(res);
            }

            // Restart for next section
            currentSection = line;
            sectionLines.clear();
        }
        // Otherwise, add to section lines
        else
        {
            sectionLines.push_back(line);
        }
    }

    // Parse last section
    if (sectionLines.size() > 0)
    {
        auto it = sectionParsers.find(currentSection);
        if (it != sectionParsers.end())
        {
            ParseMapType res = it->second(sectionLines);
            parseResults.push_back(res);
        }
    }

    // Add parse results to map
    for (auto res : parseResults)
    {
        if      (std::holds_alternative<std::string>(res))
            newMap->fileFormat =         std::get<std::string>(res);
        else if (std::holds_alternative<GeneralSection>(res))
            newMap->generalSection =     std::get<GeneralSection>(res);
        else if (std::holds_alternative<EditorSection>(res))
            newMap->editorSection =      std::get<EditorSection>(res);
        else if (std::holds_alternative<MetadataSection>(res))
            newMap->metadataSection =    std::get<MetadataSection>(res);
        else if (std::holds_alternative<DifficultySection>(res))
            newMap->difficultySection =  std::get<DifficultySection>(res);
        else if (std::holds_alternative<std::vector<Event*>>(res))
            newMap->events =             std::get<std::vector<Event*>>(res);
        else if (std::holds_alternative<std::vector<TimingPoint>>(res))
            newMap->timingPoints =       std::get<std::vector<TimingPoint>>(res);
        else if (std::holds_alternative<std::vector<Colour>>(res))
            newMap->colours =            std::get<std::vector<Colour>>(res);
        else if (std::holds_alternative<std::vector<HitObject*>>(res))
            newMap->hitObjects =         std::get<std::vector<HitObject*>>(res);
    }

    osuFile.close();

    return newMap;
}