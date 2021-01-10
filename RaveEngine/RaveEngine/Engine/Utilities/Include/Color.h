#pragma once

namespace rave
{
	struct Color
	{
		constexpr Color() noexcept : r(0), g(0), b(0), a(0) {}
		constexpr Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) noexcept : r(r), g(g), b(b), a(a) {}
		constexpr Color(const Color& other) noexcept : r(other.r), g(other.g), b(other.b), a(other.a) {}

		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};

	struct FColor
	{
		constexpr FColor() noexcept : r(0), g(0), b(0), a(0) {}
		constexpr FColor(float r, float g, float b, float a = 255) noexcept : r(r), g(g), b(b), a(a) {}
		constexpr FColor(const FColor& other) noexcept : r(other.r), g(other.g), b(other.b), a(other.a) {}

		const float* Get() const noexcept { return &r; }

		float r;
		float g;
		float b;
		float a;
	};

	constexpr Color  ConvertColor(const FColor& color)
	{
		return Color(
			unsigned char(color.r * 255.0f),
			unsigned char(color.g * 255.0f),
			unsigned char(color.b * 255.0f),
			unsigned char(color.a * 255.0f)
		);
	}
	constexpr FColor ConvertColor(const Color& color)
	{
		return FColor(
			(float)color.r / 255.0f,
			(float)color.g / 255.0f,
			(float)color.b / 255.0f,
			(float)color.a / 255.0f
		);
	}

	namespace Colors
	{
		static constexpr Color Transparent	= Color(   0,   0,   0,   0 );
		static constexpr Color White		= Color( 255, 255, 255 );
		static constexpr Color Black		= Color(   0,   0,   0 );
		static constexpr Color Red			= Color( 255,   0,   0 );
		static constexpr Color Green		= Color(   0, 255,   0 );
		static constexpr Color Blue			= Color(   0,   0, 255 );
		static constexpr Color Yellow		= Color( 255, 255,   0 );
		static constexpr Color Cyan			= Color(   0, 255, 255 );
		static constexpr Color Magenta		= Color( 255,   0, 255 );
		static constexpr Color Gray			= Color( 128, 128, 128 );
		static constexpr Color LightGray	= Color( 211, 211, 211 );
		static constexpr Color RaveBlue		= Color(  13,  30,  43 );
		static constexpr Color RaveYellow	= Color( 242, 213,  65 );
	}
	namespace FColors
	{
		static constexpr FColor Transparent	= FColor( 0.0f, 0.0f, 0.0f, 0.0f );
		static constexpr FColor White		= FColor( 1.0f, 1.0f, 1.0f );
		static constexpr FColor Black		= FColor( 0.0f, 0.0f, 0.0f );
		static constexpr FColor Red			= FColor( 1.0f, 0.0f, 0.0f );
		static constexpr FColor Green		= FColor( 0.0f, 1.0f, 0.0f );
		static constexpr FColor Blue		= FColor( 0.0f, 0.0f, 1.0f );
		static constexpr FColor Yellow		= FColor( 1.0f, 1.0f, 0.0f );
		static constexpr FColor Cyan		= FColor( 0.0f, 1.0f, 1.0f );
		static constexpr FColor Magenta		= FColor( 1.0f, 0.0f, 1.0f );
		static constexpr FColor Gray		= ConvertColor( Color( 128, 128, 128 ) );
		static constexpr FColor LightGray	= ConvertColor( Color( 211, 211, 211 ) );
		static constexpr FColor RaveBlue	= FColor( 0.051f, 0.118f, 0.169f );
		static constexpr FColor RaveYellow	= FColor( 0.949f, 0.835f, 0.255f );
	}
}