#pragma once

#include <vector>
#include <raylib.h>
#include <raymath.h>

#include "interpolator.hpp"

namespace naive
{
	class Gradient
	{
	public:
		struct KeyColor
		{
			float key;
			Color color;

			KeyColor() = delete;
			KeyColor(float key, Color color) : key(key), color(color) {}
		};

	private:
		std::vector<KeyColor> colors;

	public:
		Gradient() = default;
		Gradient(const std::initializer_list<KeyColor>& colors) : colors(colors) {}
		void Add(const KeyColor keyColor) { colors.push_back(keyColor); }
		Color Evaluate(float t)
		{
			if (colors.size() < 1) return PINK;
			if (colors.size() < 2) return colors [0].color;

			int index1 = 0;
			int index2 = 0;
			for (int i = 0; i < colors.size() - 1; i++)
			{
				if (t >= colors [i].key && t < colors [i + 1].key)
				{
					index1 = i;
					index2 = i + 1;
					break;
				}
			}

			Vector3 hsv1 = ColorToHSV(colors [index1].color);
			Vector3 hsv2 = ColorToHSV(colors [index2].color);
			float a1 = colors [index1].color.a;
			float a2 = colors [index2].color.a;
			t = (t - colors [index1].key) / (colors [index2].key - colors [index1].key);
			Vector3 hsv = Vector3Lerp(hsv1, hsv2, t);
			return ColorAlpha(ColorFromHSV(hsv.x, hsv.y, hsv.z), Lerp(a1 / 255, a2 / 255, t));
		}
	};
}

namespace advanced
{
	class Gradient : public AInterpolator<Color>
	{
	public:
		Gradient(const std::initializer_list<KeyValue>& keyvalues) : AInterpolator(keyvalues) {}
	protected:
		Color Default() const { return PINK; }
		Color Interpolate(Color index1, Color index2, float t) const
		{
			Vector3 hsv1 = ColorToHSV(index1);
			Vector3 hsv2 = ColorToHSV(index2);
			Vector3 hsv = Vector3Lerp(hsv1, hsv2, t);
			return ColorAlpha(ColorFromHSV(hsv.x, hsv.y, hsv.z), Lerp(index1.a / 255.0f, index2.a / 255.0f, t));
		}
	};
}