// graphics_context.hpp
/*
  neogfx C++ GUI Library
  Copyright (c) 2015 Leigh Johnston.  All Rights Reserved.
  
  This program is free software: you can redistribute it and / or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <neogfx/neogfx.hpp>
#include <memory>
#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable: 4459 ) // declaration of 'name' hides global declaration
#endif
#include <boost/multi_array.hpp>
#ifdef _WIN32
#pragma warning( pop )
#endif
#include <optional>
#include <neogfx/core/primitives.hpp>
#include <neogfx/core/path.hpp>
#include <neogfx/gfx/texture.hpp>
#include <neogfx/gfx/sub_texture.hpp>
#include <neogfx/gfx/pen.hpp>
#include <neogfx/gfx/text/font.hpp>
#include <neogfx/gfx/primitives.hpp>
#include <neogfx/game/ecs_helpers.hpp>
#include <neogfx/game/mesh.hpp>
#include <neogfx/game/material.hpp>

namespace neogfx
{
	class i_surface;
	class i_texture;
	class i_widget;
	class i_native_graphics_context;

	class graphics_context : public i_device_metrics, public i_units_context
	{
		// types
	public:
		enum class type
		{
			Attached,
			Unattached
		};
	private:
		friend class generic_surface;
		class glyph_shapes;
		// exceptions
	public:
		struct unattached : std::logic_error { unattached() : std::logic_error("neogfx::graphics_context::unattached") {} };
		struct password_not_set : std::logic_error { password_not_set() : std::logic_error("neogfx::graphics_context::password_not_set") {} };
		// construction
	public:
		graphics_context(const i_surface& aSurface, type aType = type::Attached);
		graphics_context(const i_surface& aSurface, const font& aDefaultFont, type aType = type::Attached);
		graphics_context(const i_widget& aWidget, type aType = type::Attached);
		graphics_context(const graphics_context& aOther);
		virtual ~graphics_context();
	public:
		const i_surface& surface() const;
		// operations
	public:
		delta to_device_units(const delta& aValue) const;
		size to_device_units(const size& aValue) const;
		point to_device_units(const point& aValue) const;
		vec2 to_device_units(const vec2& aValue) const;
		rect to_device_units(const rect& aValue) const;
		path to_device_units(const path& aValue) const;
		delta from_device_units(const delta& aValue) const;
		size from_device_units(const size& aValue) const;
		point from_device_units(const point& aValue) const;
		rect from_device_units(const rect& aValue) const;
		path from_device_units(const path& aValue) const;
		int32_t layer() const;
		void set_layer(int32_t aLayer);
		neogfx::logical_coordinate_system logical_coordinate_system() const;
		void set_logical_coordinate_system(neogfx::logical_coordinate_system aSystem) const;
		const std::pair<vec2, vec2>& logical_coordinates() const;
		void set_logical_coordinates(const std::pair<vec2, vec2>& aCoordinates) const;
		void set_default_font(const font& aDefaultFont) const;
		void set_extents(const size& aExtents) const;
		void set_origin(const point& aOrigin) const;
		point origin() const;
		void flush() const;
		void scissor_on(const rect& aRect) const;
		void scissor_off() const;
		void clip_to(const rect& aRect) const;
		void clip_to(const path& aPath, dimension aPathOutline = 0) const;
		void reset_clip() const;
		double opacity() const;
		void set_opacity(double aOpacity);
		neogfx::smoothing_mode smoothing_mode() const;
		void set_smoothing_mode(neogfx::smoothing_mode aSmoothingMode) const;
		void push_logical_operation(logical_operation aLogicalOperation) const;
		void pop_logical_operation() const;
		void line_stipple_on(uint32_t aFactor, uint16_t aPattern) const;
		void line_stipple_off() const;
		bool is_subpixel_rendering_on() const;
		void subpixel_rendering_on() const;
		void subpixel_rendering_off() const;
		void clear(const colour& aColour) const;
		void clear_depth_buffer() const;
		void set_pixel(const point& aPoint, const colour& aColour) const;
		void draw_pixel(const point& aPoint, const colour& aColour) const;
		void draw_line(const point& aFrom, const point& aTo, const pen& aPen) const;
		void draw_rect(const rect& aRect, const pen& aPen, const brush& aFill = brush{}) const;
		void draw_rounded_rect(const rect& aRect, dimension aRadius, const pen& aPen, const brush& aFill = brush{}) const;
		void draw_circle(const point& aCentre, dimension aRadius, const pen& aPen, const brush& aFill = brush{}, angle aStartAngle = 0.0) const;
		void draw_arc(const point& aCentre, dimension aRadius, angle aStartAngle, angle aEndAngle, const pen& aPen, const brush& aFill = brush{}) const;
		void draw_path(const path& aPath, const pen& aPen, const brush& aFill = brush{}) const;
		void draw_shape(const game::mesh& aShape, const pen& aPen, const brush& aFill = brush{}) const;
		void draw_entity(const game::i_ecs& aEcs, game::entity_id aEntity) const;
		void draw_focus_rect(const rect& aRect) const;
		void fill_rect(const rect& aRect, const brush& aFill) const;
		void fill_rounded_rect(const rect& aRect, dimension aRadius, const brush& aFill) const;
		void fill_circle(const point& aCentre, dimension aRadius, const brush& aFill) const;
		void fill_arc(const point& aCentre, dimension aRadius, angle aStartAngle, angle aEndAngle, const brush& aFill) const;
		void fill_path(const path& aPath, const brush& aFill) const;
		void fill_shape(const game::mesh& aShape, const brush& aFill) const;
		size text_extent(const string& aText, const font& aFont, const glyph_text_cache_usage& aCacheUsage = DontUseGlyphTextCache) const;
		size text_extent(string::const_iterator aTextBegin, string::const_iterator aTextEnd, const font& aFont, const glyph_text_cache_usage& aCacheUsage = DontUseGlyphTextCache) const;
		size multiline_text_extent(const string& aText, const font& aFont, const glyph_text_cache_usage& aCacheUsage = DontUseGlyphTextCache) const;
		size multiline_text_extent(const string& aText, const font& aFont, dimension aMaxWidth, const glyph_text_cache_usage& aCacheUsage = DontUseGlyphTextCache) const;
		glyph_text to_glyph_text(const string& aText, const font& aFont) const;
		glyph_text to_glyph_text(string::const_iterator aTextBegin, string::const_iterator aTextEnd, const font& aFont) const;
		glyph_text to_glyph_text(string::const_iterator aTextBegin, string::const_iterator aTextEnd, std::function<font(std::string::size_type)> aFontSelector) const;
		glyph_text to_glyph_text(const std::u32string& aText, const font& aFont) const;
		glyph_text to_glyph_text(std::u32string::const_iterator aTextBegin, std::u32string::const_iterator aTextEnd, const font& aFont) const;
		glyph_text to_glyph_text(std::u32string::const_iterator aTextBegin, std::u32string::const_iterator aTextEnd, std::function<font(std::u32string::size_type)> aFontSelector) const;
		bool is_text_left_to_right(const string& aText, const font& aFont, const glyph_text_cache_usage& aCacheUsage = DontUseGlyphTextCache) const;
		bool is_text_right_to_left(const string& aText, const font& aFont, const glyph_text_cache_usage& aCacheUsage = DontUseGlyphTextCache) const;
		void draw_text(const point& aPoint, const string& aText, const font& aFont, const text_appearance& aAppearance, const glyph_text_cache_usage& aCacheUsage = DontUseGlyphTextCache) const;
		void draw_text(const point& aPoint, string::const_iterator aTextBegin, string::const_iterator aTextEnd, const font& aFont, const text_appearance& aAppearance, const glyph_text_cache_usage& aCacheUsage = DontUseGlyphTextCache) const;
		void draw_text(const vec3& aPoint, const string& aText, const font& aFont, const text_appearance& aAppearance, const glyph_text_cache_usage& aCacheUsage = DontUseGlyphTextCache) const;
		void draw_text(const vec3& aPoint, string::const_iterator aTextBegin, string::const_iterator aTextEnd, const font& aFont, const text_appearance& aAppearance, const glyph_text_cache_usage& aCacheUsage = DontUseGlyphTextCache) const;
		void draw_multiline_text(const point& aPoint, const string& aText, const font& aFont, const text_appearance& aAppearance, alignment aAlignment = alignment::Left, const glyph_text_cache_usage& aCacheUsage = DontUseGlyphTextCache) const;
		void draw_multiline_text(const point& aPoint, const string& aText, const font& aFont, dimension aMaxWidth, const text_appearance& aAppearance, alignment aAlignment = alignment::Left, const glyph_text_cache_usage& aCacheUsage = DontUseGlyphTextCache) const;
		void draw_multiline_text(const vec3& aPoint, const string& aText, const font& aFont, const text_appearance& aAppearance, alignment aAlignment = alignment::Left, const glyph_text_cache_usage& aCacheUsage = DontUseGlyphTextCache) const;
		void draw_multiline_text(const vec3& aPoint, const string& aText, const font& aFont, dimension aMaxWidth, const text_appearance& aAppearance, alignment aAlignment = alignment::Left, const glyph_text_cache_usage& aCacheUsage = DontUseGlyphTextCache) const;
		void draw_glyph_text(const point& aPoint, const glyph_text& aText, const text_appearance& aAppearance) const;
		void draw_glyph_text(const point& aPoint, const glyph_text& aText, glyph_text::const_iterator aTextBegin, glyph_text::const_iterator aTextEnd, const text_appearance& aAppearance) const;
		void draw_glyph_text(const vec3& aPoint, const glyph_text& aText, const text_appearance& aAppearance) const;
		void draw_glyph_text(const vec3& aPoint, const glyph_text& aText, glyph_text::const_iterator aTextBegin, glyph_text::const_iterator aTextEnd, const text_appearance& aAppearance) const;
		void draw_glyph(const point& aPoint, const font& aFont, const glyph& aGlyph, const text_appearance& aAppearance) const;
		void draw_glyph(const vec3& aPoint, const font& aFont, const glyph& aGlyph, const text_appearance& aAppearance) const;
		void draw_glyph_underline(const point& aPoint, const font& aFont, const glyph& aGlyph, const text_appearance& aAppearance) const;
		void draw_glyph_underline(const vec3& aPoint, const font& aFont, const glyph& aGlyph, const text_appearance& aAppearance) const;
		void set_glyph_text_cache(glyph_text& aGlyphTextCache) const;
		void reset_glyph_text_cache() const;
		void set_mnemonic(bool aShowMnemonics, char aMnemonicPrefix = '&') const;
		void unset_mnemonic() const;
		bool mnemonics_shown() const;
		bool password() const;
		const std::string& password_mask() const;
		void set_password(bool aPassword, const std::string& aMask = "\xE2\x97\x8F");
		void draw_texture(const point& aPoint, const i_texture& aTexture, const optional_colour& aColour = optional_colour(), shader_effect aShaderEffect = shader_effect::None) const;
		void draw_texture(const rect& aRect, const i_texture& aTexture, const optional_colour& aColour = optional_colour(), shader_effect aShaderEffect = shader_effect::None) const;
		void draw_texture(const point& aPoint, const i_texture& aTexture, const rect& aTextureRect, const optional_colour& aColour = optional_colour(), shader_effect aShaderEffect = shader_effect::None) const;
		void draw_texture(const rect& aRect, const i_texture& aTexture, const rect& aTextureRect, const optional_colour& aColour = optional_colour(), shader_effect aShaderEffect = shader_effect::None) const;
		void draw_texture(const game::mesh& aMesh, const i_texture& aTexture, const optional_colour& aColour = optional_colour(), shader_effect aShaderEffect = shader_effect::None) const;
		void draw_texture(const game::mesh& aMesh, const i_texture& aTexture, const rect& aTextureRect, const optional_colour& aColour = optional_colour(), shader_effect aShaderEffect = shader_effect::None) const;
		// implementation
		// from i_device_metrics
	public:
		bool metrics_available() const override;
		size extents() const override;
		dimension horizontal_dpi() const override;
		dimension vertical_dpi() const override;
		dimension ppi() const override;
		dimension em_size() const override;
		// from i_units_context
	public:
		bool high_dpi() const override;
		dimension dpi_scale_factor() const override;
	public:
		bool device_metrics_available() const override;
		const i_device_metrics& device_metrics() const override;
		neogfx::units units() const override;
		neogfx::units set_units(neogfx::units aUnits) const override;
	protected:
		i_native_graphics_context& native_context() const;
		// helpers
	protected:
		static i_native_font_face& to_native_font_face(const font& aFont);
		// own
	private:
		glyph_text to_glyph_text_impl(string::const_iterator aTextBegin, string::const_iterator aTextEnd, std::function<font(std::string::size_type)> aFontSelector) const;
		glyph_text to_glyph_text_impl(std::u32string::const_iterator aTextBegin, std::u32string::const_iterator aTextEnd, std::function<font(std::u32string::size_type)> aFontSelector) const;
		// attributes
	private:
		const i_surface& iSurface;
		std::unique_ptr<i_native_graphics_context> iNativeGraphicsContext;
		units_context iUnitsContext;
		mutable font iDefaultFont;
		mutable point iOrigin;
		mutable size iExtents;
		mutable neogfx::logical_coordinate_system iLogicalCoordinateSystem;
		mutable std::pair<vec2, vec2> iLogicalCoordinates;
		mutable double iOpacity;
		mutable neogfx::smoothing_mode iSmoothingMode;
		mutable bool iSubpixelRendering;
		mutable std::optional<std::pair<bool, char>> iMnemonic;
		mutable std::optional<std::string> iPassword;
		struct glyph_text_data;
		std::unique_ptr<glyph_text_data> iGlyphTextData;
		mutable glyph_text* iGlyphTextCache;
	};

	template <typename Iter>
	inline void draw_glyph_text(const graphics_context& aGraphicsContext, const vec3& aPoint, const glyph_text& aGlyphText, Iter aTextBegin, Iter aTextEnd, const text_appearance& aAppearance)
	{
		vec3 pos = aPoint;
		for (Iter i = aTextBegin; i != aTextEnd; ++i)
		{
			aGraphicsContext.draw_glyph(pos + i->offset().to_vec3(), i->font(aGlyphText), *i, aAppearance);
			pos.x += i->advance().cx;
		}
	}

	class scoped_mnemonics
	{
	public:
		scoped_mnemonics(graphics_context& aGc, bool aShowMnemonics, char aMnemonicPrefix = '&') :
			iGc(aGc)
		{
			iGc.set_mnemonic(aShowMnemonics, aMnemonicPrefix);
		}
		~scoped_mnemonics()
		{
			iGc.unset_mnemonic();
		}
	private:
		graphics_context& iGc;
	};

	class scoped_coordinate_system
	{
	public:
		scoped_coordinate_system(graphics_context& aGc, const point& aOrigin, const size& aExtents, logical_coordinate_system aCoordinateSystem);
		scoped_coordinate_system(graphics_context& aGc, const point& aOrigin, const size& aExtents, logical_coordinate_system aCoordinateSystem, const std::pair<vec2, vec2>& aCoordinates);
		~scoped_coordinate_system();
	private:
		void apply_origin(const point& aOrigin, const size& aExtents);
	private:
		graphics_context& iGc;
		logical_coordinate_system iPreviousCoordinateSystem;
		std::pair<vec2, vec2> iPreviousCoordinates;
	};

	class scoped_opacity
	{
	public:
		scoped_opacity(graphics_context& aGc, double aOpacity) : 
			iGc{ aGc }, iPreviousOpacity { aGc.opacity() }
		{
			iGc.set_opacity(iGc.opacity() * aOpacity);
		}
		~scoped_opacity()
		{
			iGc.set_opacity(iPreviousOpacity);
		}
	private:
		graphics_context& iGc;
		double iPreviousOpacity;
	};

	class scoped_scissor
	{
	public:
		scoped_scissor(graphics_context& aGc, const rect& aScissorRect) :
			iGc{ aGc }
		{
			iGc.scissor_on(aScissorRect);
		}
		~scoped_scissor()
		{
			iGc.scissor_off();
		}
	private:
		graphics_context & iGc;
	};

	const std::pair<vec2, vec2>& get_logical_coordinates(const size& aSurfaceSize, logical_coordinate_system aSystem, std::pair<vec2, vec2>& aCoordinates);

	template <typename ValueType = double, uint32_t W = 5>
	inline std::array<std::array<ValueType, W>, W> static_gaussian_filter(ValueType aSigma = 1.0)
	{
		const int32_t mean = static_cast<int32_t>(W / 2);
		std::array<std::array<ValueType, W>, W> kernel = {};
		if (aSigma != 0)
		{
			ValueType sum = 0.0;
			for (int32_t x = -mean; x <= mean; ++x)
			{
				for (int32_t y = -mean; y <= mean; ++y)
				{
					kernel[x + mean][y + mean] =
						static_cast<ValueType>((1.0 / (2.0 * boost::math::constants::pi<ValueType>() * aSigma * aSigma)) * std::exp(-((x * x + y * y) / (2.0 * aSigma * aSigma))));
					sum += kernel[x + mean][y + mean];
				}
			}
			for (uint32_t x = 0; x < W; ++x)
				for (uint32_t y = 0; y < W; ++y)
					kernel[x][y] /= sum;
		}
		else
			kernel[mean][mean] = static_cast<ValueType>(1.0);
		return kernel;
	}

	template <typename ValueType = double>
	inline boost::multi_array<ValueType, 2> dynamic_gaussian_filter(uint32_t aWidth = 5, ValueType aSigma = 1.0)
	{
		const int32_t mean = static_cast<int32_t>(aWidth / 2);
		boost::multi_array<ValueType, 2> kernel(boost::extents[aWidth][aWidth]);
		if (aSigma != 0)
		{
			ValueType sum = 0.0;
			for (int32_t x = -mean; x <= mean; ++x)
			{
				for (int32_t y = -mean; y <= mean; ++y)
				{
					kernel[x + mean][y + mean] =
						static_cast<ValueType>((1.0 / (2.0 * boost::math::constants::pi<ValueType>() * aSigma * aSigma)) * std::exp(-((x * x + y * y) / (2.0 * aSigma * aSigma))));
					sum += kernel[x + mean][y + mean];
				}
			}
			for (uint32_t x = 0; x < aWidth; ++x)
				for (uint32_t y = 0; y < aWidth; ++y)
					kernel[x][y] /= sum;
		}
		else
			kernel[mean][mean] = static_cast<ValueType>(1.0);
		return kernel;
	}
}