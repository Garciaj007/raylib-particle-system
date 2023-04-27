#pragma once

template<typename T>
class AInterpolator
{
public:
	struct KeyValue
	{
		float key;
		T value;

		KeyValue() = delete;
		KeyValue(float key, T value) : key(key), value(value) {}
	};

protected:
	std::vector<KeyValue> keyValues;

	virtual T Default() const = 0;
	virtual T Interpolate(T index1, T index2, float t) const = 0;

public:
	AInterpolator(const std::initializer_list<KeyValue>& keyValues) : keyValues(keyValues) {}
	virtual ~AInterpolator() = default;
	
	void Add(const KeyValue keyValue) { keyValues.push_back(keyValue); }
	const T Evaluate(float t) const
	{
		if (keyValues.size() < 1) return Default();
		if (keyValues.size() < 2) return keyValues [0].value;

		int index1 = 0;
		int index2 = 0;
		for (int i = 0; i < keyValues.size() - 1; i++)
		{
			if (t >= keyValues [i].key && t < keyValues [i + 1].key)
			{
				index1 = i;
				index2 = i + 1;
				break;
			}
		}

		const auto& keyValue1 = keyValues [index1];
		const auto& keyValue2 = keyValues [index2];

		t = (t - keyValue1.key) / (keyValue2.key - keyValue1.key);

		return Interpolate(keyValue1.value, keyValue2.value, t);
	}
};

namespace advanced
{
	class FloatInterpolator : public AInterpolator<float>
	{
	public:
		FloatInterpolator(const std::initializer_list<KeyValue>& keyvalues) : AInterpolator(keyvalues) {}
	protected:
		float Default() const { return 0.0f; }
		float Interpolate(float index1, float index2, float t) const { return Lerp(index1, index2, t); }
	};

	class Vector2Interpolator : public AInterpolator<Vector2>
	{
	public:
		Vector2Interpolator(const std::initializer_list<KeyValue>& keyvalues) : AInterpolator(keyvalues) {}
	protected:
		Vector2 Default() const { return Zero; }
		Vector2 Interpolate(Vector2 index1, Vector2 index2, float t) const { return Vector2Lerp(index1, index2, t); }
	};
}