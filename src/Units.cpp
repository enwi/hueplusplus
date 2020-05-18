#include <cmath>

#include <hueplusplus/Units.h>

namespace hueplusplus
{
namespace
{
float sign(const XY& p0, const XY& p1, const XY& p2)
{
    return (p0.x - p2.x) * (p1.y - p2.y) - (p1.x - p2.x) * (p0.y - p2.y);
}

bool isInTriangle(const XY& xy, const ColorGamut& triangle)
{
    const float d1 = sign(xy, triangle.redCorner, triangle.greenCorner);
    const float d2 = sign(xy, triangle.greenCorner, triangle.blueCorner);
    const float d3 = sign(xy, triangle.blueCorner, triangle.redCorner);

    const bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    const bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);
    return !(hasNeg && hasPos);
}

bool isRightOf(const XY& xy, const XY& p1, const XY& p2)
{
    return sign(xy, p1, p2) < 0;
}

XY projectOntoLine(const XY& xy, const XY& p1, const XY& p2)
{
    // Using dot product to project onto line
    // Vector AB = B - A
    // Vector AX = X - A
    // Projected length l = (AX dot AB) / len(AB)
    // Result: E = A + l*AB/len(AB) = A + AB * (AX dot AB) / (len(AB))^2

    const float abX = p2.x - p1.x;
    const float abY = p2.y - p1.y;
    const float lenABSquared = abX * abX + abY * abY;

    const float dot = (xy.x - p1.x) * abX + (xy.y - p1.y) * abY;
    const float eX = p1.x + abX * dot / lenABSquared;
    const float eY = p1.y + abY * dot / lenABSquared;
    return XY {eX, eY};
}
} // namespace

XY ColorGamut::corrected(const XY& xy) const
{
    // red, green and blue are in counterclockwise orientation
    if (isRightOf(xy, redCorner, greenCorner))
    {
        // Outside of triangle, check whether to use nearest corner or point on line
        if (isRightOf(xy, greenCorner, blueCorner))
        {
            // Point is outside of red-green line, closest to green corner
            return greenCorner;
        }
        else if (isRightOf(xy, blueCorner, redCorner))
        {
            // Point is outside of red-green line, closest to red corner
            return redCorner;
        }
        else
        {
            // Point is closest to line, project onto it
            return projectOntoLine(xy, redCorner, greenCorner);
        }
    }
    else if (isRightOf(xy, greenCorner, blueCorner))
    {
        // Green corner already checked above
        if (isRightOf(xy, blueCorner, redCorner))
        {
            // Point is outside of green-blue line, closest to blue corner
            return blueCorner;
        }
        else
        {
            return projectOntoLine(xy, greenCorner, blueCorner);
        }
    }
    else if (isRightOf(xy, blueCorner, redCorner))
    {
        // All corners already checked
        return projectOntoLine(xy, blueCorner, redCorner);
    }
    return xy;
}

XYBrightness RGB::toXY() const
{
    const float red = r / 255.f;
    const float green = g / 255.f;
    const float blue = b / 255.f;

    const float redCorrected = (red > 0.04045f) ? pow((red + 0.055f) / (1.0f + 0.055f), 2.4f) : (red / 12.92f);
    const float greenCorrected = (green > 0.04045f) ? pow((green + 0.055f) / (1.0f + 0.055f), 2.4f) : (green / 12.92f);
    const float blueCorrected = (blue > 0.04045f) ? pow((blue + 0.055f) / (1.0f + 0.055f), 2.4f) : (blue / 12.92f);

    const float X = redCorrected * 0.664511f + greenCorrected * 0.154324f + blueCorrected * 0.162028f;
    const float Y = redCorrected * 0.283881f + greenCorrected * 0.668433f + blueCorrected * 0.047685f;
    const float Z = redCorrected * 0.000088f + greenCorrected * 0.072310f + blueCorrected * 0.986039f;

    const float x = X / (X + Y + Z);
    const float y = Y / (X + Y + Z);
    return XYBrightness {XY {x, y}, Y};
}

XYBrightness RGB::toXY(const ColorGamut& gamut) const
{
    XYBrightness xy = toXY();
    xy.xy = gamut.corrected(xy.xy);

    return xy;
}

RGB RGB::fromXY(const XYBrightness& xy) {}

} // namespace hueplusplus