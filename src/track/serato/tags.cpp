#include "track/serato/tags.h"

namespace mixxx {

RgbColor::optional_t SeratoTags::storedToDisplayedTrackColor(RgbColor color) {
    // Serato stores Track colors differently from how they are displayed in
    // the library column. Instead of the color from the library view, the
    // value from the color picker is stored instead (which is different).
    // To make sure that the track looks the same in both Mixxx' and Serato's
    // libraries, we need to convert between the two values.
    //
    // See this for details:
    // https://github.com/Holzhaus/serato-tags/blob/master/docs/colors.md#track-colors

    quint32 value = static_cast<quint32>(color);

    if (color == 0xFFFFFF) {
        return RgbColor::nullopt();
    }

    if (color == 0x999999) {
        return RgbColor::optional(0x090909);
    }

    if (color == 0x000000) {
        return RgbColor::optional(0x333333);
    }

    value = (value < 0x666666) ? value + 0x99999A : value - 0x666666;
    return RgbColor::optional(value);
}

RgbColor SeratoTags::displayedToStoredTrackColor(RgbColor::optional_t color) {
    if (!color) {
        return RgbColor(0xFFFFFF);
    }

    quint32 value = static_cast<quint32>(*color);

    if (value == 0x090909) {
        return RgbColor(0x999999);
    }

    if (value == 0x333333) {
        return RgbColor(0x000000);
    }

    // Special case: 0x999999 and 0x99999a are not representable as Serato
    // track color We'll just modify them a little, so that the look the
    // same in Serato.
    if (value == 0x999999) {
        return RgbColor(0x999998);
    }

    if (value == 0x99999a) {
        return RgbColor(0x99999b);
    }

    value = (value < 0x99999A) ? value + 0x666666 : value - 0x99999A;
    return RgbColor(value);
}

RgbColor::optional_t SeratoTags::getTrackColor() const {
    RgbColor::optional_t color = m_seratoMarkers.getTrackColor();

    if (!color) {
        // Markers_ is empty, but we may have a color in Markers2
        color = m_seratoMarkers2.getTrackColor();
    }

    if (color) {
        color = SeratoTags::storedToDisplayedTrackColor(*color);
    }

    return color;
}

bool SeratoTags::isBpmLocked() const {
    return m_seratoMarkers2.isBpmLocked();
}

} // namespace mixxx
