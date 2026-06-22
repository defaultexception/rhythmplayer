#include "hitobjects.h"

#include <cmath>

/*
    HitObject Time:
    Get starting time of HitObject.
    
    getEndTime is used for Spinners and HoldNotes.
*/
int HitObject::getStartTime() const
{
    return Time;
}

/*
    HitObject Type Bitmask:
    0   -> hitcircle
    1   -> slider
    2   -> newcombo
    3   -> spinner
    4-6 -> # of colors to skin on newcombo
    7   -> holdnote
*/
bool HitObject::isHitCircle() const
{
    return Type & 1 == 1; // bit 0 on = 1
}

bool HitObject::isSlider() const
{
    return Type & 2 == 2; // bit 1 on = 2
}

bool HitObject::isNewCombo() const
{
    return Type & 4 == 4; // bit 2 on = 4
}

bool HitObject::isSpinner() const
{
    return Type & 8 == 8; // bit 3 on = 8
}

bool HitObject::isHoldNote() const
{
    return Type & 128 == 128; // bit 7 on = 128
}

// Static methods for determining struct type
bool HitObject::isHitCircle(const int& type)
{
    return type & 1 == 1; // bit 0 on = 1
}

bool HitObject::isSlider(const int& type)
{
    return type & 2 == 2; // bit 1 on = 2
}

bool HitObject::isNewCombo(const int& type)
{
    return type & 4 == 4; // bit 2 on = 4
}

bool HitObject::isSpinner(const int& type)
{
    return type & 8 == 8; // bit 3 on = 8
}

bool HitObject::isHoldNote(const int& type)
{
    return type & 128 == 128; // bit 7 on = 128
}

/*
    HitObject HitSound Bitmask:
    0 -> normal   (1)
    1 -> whistle  (2)
    2 -> finish   (4)
    3 -> clap     (8)
*/
bool HitObject::hasNormalHitsound() const
{
    return HitSound & 1 == 1;
}

bool HitObject::hasWhistleHitsound() const
{
    return HitSound & 2 == 2;
}

bool HitObject::hasFinishHitsound() const
{
    return HitSound & 4 == 4;
}

bool HitObject::hasClapHitsound() const
{
    return HitSound & 8 == 8;
}

/*
    HitObject ObjectParams
*/

/*
    Slider ObjectParams:
    0 -> CurveType
    1 -> CurvePoints
    2 -> Slides
    3 -> Length
    4 -> EdgeSounds
    5 -> EdgeSets
*/
SliderObjectParams HitObject::getSliderObjectParams() const
{
    return SliderObjectParams{
        _getTypeInVector<std::vector<ObjectParamsType>, char>(ObjectParams, 0),
        _getTypeInVector<std::vector<ObjectParamsType>, std::vector<std::string>>(ObjectParams, 1),
        _getTypeInVector<std::vector<ObjectParamsType>, int>(ObjectParams, 2),
        _getTypeInVector<std::vector<ObjectParamsType>, double>(ObjectParams, 3),
        _getTypeInVector<std::vector<ObjectParamsType>, std::vector<int>>(ObjectParams, 4),
        _getTypeInVector<std::vector<ObjectParamsType>, std::vector<std::string>>(ObjectParams, 5),
    };
}

/*
    Spinner ObjectParams:
    0 -> EndTime
*/
SpinnerObjectParams HitObject::getSpinnerObjectParams() const
{
    return SpinnerObjectParams{
        _getTypeInVector<std::vector<ObjectParamsType>, int>(ObjectParams, 0),
    };
}

/*
    HoldNote ObjectParams:
    0 -> EndTime
*/
HoldNoteObjectParams HitObject::getHoldNoteObjectParams() const
{
    return HoldNoteObjectParams{
        _getTypeInVector<std::vector<ObjectParamsType>, int>(ObjectParams, 0),
    };
}

/*
    HitObject HitSample:
    0th -> NormalSet
    1st -> AdditionSet
    2nd -> Index
    3rd -> Volume
    4th -> FileName

    x:x:x:x:x
*/
int HitObject::getNormalSet() const
{
    return _getTypeInVector<std::vector<HitSampleType>, int>(HitSample, 0);
}

int HitObject::getAdditionSet() const
{
    return _getTypeInVector<std::vector<HitSampleType>, int>(HitSample, 1);
}

int HitObject::getIndex() const
{
    return _getTypeInVector<std::vector<HitSampleType>, int>(HitSample, 2);
}

int HitObject::getVolume() const
{
    return _getTypeInVector<std::vector<HitSampleType>, int>(HitSample, 3);
}

std::string HitObject::getFileName() const
{
    return _getTypeInVector<std::vector<HitSampleType>, std::string>(HitSample, 4);
}

/*
    SliderHitObject
*/
char SliderHitObject::getCurveType() const
{
    return _sliderObjectParams.CurveType;
}

std::vector<std::string> SliderHitObject::getCurvePoints() const
{
    return _sliderObjectParams.CurvePoints;
}

int SliderHitObject::getSlides() const
{
    return _sliderObjectParams.Slides;
}

double SliderHitObject::getLength() const
{
    return _sliderObjectParams.Length;
}

std::vector<int> SliderHitObject::getEdgeSounds() const
{
    return _sliderObjectParams.EdgeSounds;
}

std::vector<std::string> SliderHitObject::getEdgeSets() const
{
    return _sliderObjectParams.EdgeSets;
}

/*
    SpinnerHitObject
*/
int SpinnerHitObject::getEndTime() const
{
    return _spinnerObjectParams.EndTime;
}

/*
    HoldNoteHitObject
*/
int HoldNoteHitObject::getEndTime() const
{
    return _holdNoteObjectParams.EndTime;
}

/*
    Specific Gamemode HitObjects
*/

/*
    Taiko
*/

/*
    TaikoHitObject
*/
int TaikoHitObject::getStartTime() const
{
    return Time;
}

int TaikoHitObject::isKat() const
{
    // HitObject is kat if has whistle or clap in hitsound
    return hasWhistleHitsound() || hasClapHitsound();
}

/*
    TaikoSliderHitObject
*/
int TaikoSliderHitObject::getStartTime() const
{
    return Time;
}

int TaikoSliderHitObject::getSliderLength() const
{
    // SliderLength is in SliderObjectParams
    // In taiko, this specifies the length of the drumroll
    return _sliderObjectParams.Length;
}

int TaikoSliderHitObject::getEndTime() const
{
    // End of slider is the StartTime + SliderLength
    return getStartTime() + getSliderLength();
}

/*
    TaikoSpinnerHitObject
*/
int TaikoSpinnerHitObject::getStartTime() const
{
    return Time;
}

int TaikoSpinnerHitObject::getSpinnerLength() const
{
    // SpinnerLength is EndTime - StartTime
    return getEndTime() - getStartTime();
}

int TaikoSpinnerHitObject::getEndTime() const
{
    return _spinnerObjectParams.EndTime;
}

/*
    Mania
*/

/*
    ManiaHitObject
*/
int ManiaHitObject::getStartTime() const
{
    return Time;
}

/*
    ManiaHoldNoteHitObject
*/
int ManiaHoldNoteHitObject::getStartTime() const
{
    return Time;
}

int ManiaHoldNoteHitObject::getEndTime() const
{
    return _holdNoteObjectParams.EndTime;
}

/*
    ManiaHitObject X:
    Calculate column number with key count
*/
int _getColumnNum(int x, int keyCount)
{
    int columnNum = std::floor(x * keyCount / 512);
    
    if (columnNum < 0)
        return 0;
    if (columnNum >= keyCount)
        return keyCount;
    return columnNum;
}

int ManiaHitObject::getColumnNum()
{
    return _getColumnNum(X, KeyCount);
}

int ManiaHoldNoteHitObject::getColumnNum()
{
    return _getColumnNum(X, KeyCount);
}