// app.hpp
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
#include <map>
#include <optional>
#include <boost/pool/pool_alloc.hpp>
#include <neolib/async_thread.hpp>
#include <neogfx/app/i_app.hpp>
#include <neogfx/app/i_service_factory.hpp>
#include <neogfx/app/i_basic_services.hpp>
#include <neogfx/hid/i_surface_manager.hpp>
#include <neogfx/hid/i_window_manager.hpp>
#include <neogfx/hid/keyboard.hpp>
#include <neogfx/gfx/i_rendering_engine.hpp>
#include <neogfx/gui/widget/i_widget.hpp>
#include <neogfx/audio/i_audio.hpp>
#include <neogfx/app/style.hpp>
#include <neogfx/app/clipboard.hpp>
#include <neogfx/app/action.hpp>
#include <neogfx/app/i_mnemonic.hpp>
#include <neogfx/app/i_help.hpp>

namespace neogfx
{
	class program_options : public boost::program_options::variables_map
	{
		struct invalid_options : std::runtime_error { invalid_options(const std::string& aReason) : std::runtime_error("Invalid program options: " + aReason) {} };
	public:
		program_options(int argc, char* argv[]);
	public:
		bool debug() const;
		neogfx::renderer renderer() const;
		std::optional<std::pair<uint32_t, uint32_t>> full_screen() const;
		bool double_buffering() const;
		bool nest() const;
	};

	class app : public neolib::async_thread, private async_event_queue, public i_app, private i_keyboard_handler
	{
	public:
		class event_processing_context : public i_event_processing_context
		{
		public:
			event_processing_context(app& aParent, const std::string& aName = std::string{});
		public:
			virtual const std::string& name() const;
		private:
			neolib::message_queue::scoped_context iContext;
			std::string iName;
		};
	private:
		class loader
		{
		public:
			loader(const neogfx::program_options& aProgramOptions, app& aApp);
			~loader();
		private:
			app& iApp;
		};
	private:
		typedef std::map<std::string, style> style_list;
		typedef std::multimap<std::string, action, std::less<std::string>, boost::fast_pool_allocator<std::pair<const std::string, action>>> action_list;
		typedef std::vector<i_mnemonic*> mnemonic_list;
	public:
		struct no_instance : std::logic_error { no_instance() : std::logic_error("neogfx::app::no_instance") {} };
		struct no_basic_services : std::logic_error { no_basic_services() : std::logic_error("neogfx::app::no_basic_services") {} };
		struct no_renderer : std::logic_error { no_renderer() : std::logic_error("neogfx::app::no_renderer") {} };
		struct no_surface_manager : std::logic_error { no_surface_manager() : std::logic_error("neogfx::app::no_surface_manager") {} };
		struct no_window_manager : std::logic_error { no_window_manager() : std::logic_error("neogfx::app::no_window_manager") {} };
		struct no_keyboard : std::logic_error { no_keyboard() : std::logic_error("neogfx::app::no_keyboard") {} };
		struct no_clipboard : std::logic_error { no_clipboard() : std::logic_error("neogfx::app::no_clipboard") {} };
		struct no_audio : std::logic_error { no_audio() : std::logic_error("neogfx::app::no_audio") {} };
		struct action_not_found : std::runtime_error { action_not_found() : std::runtime_error("neogfx::app::action_not_found") {} };
		struct style_not_found : std::runtime_error { style_not_found() : std::runtime_error("neogfx::app::style_not_found") {} };
		struct style_exists : std::runtime_error { style_exists() : std::runtime_error("neogfx::app::style_exists") {} };
	public:
		app(const std::string& aName = std::string(), i_service_factory& aServiceFactory = default_service_factory());
		app(int argc, char* argv[], const std::string& aName = std::string(), i_service_factory& aServiceFactory = default_service_factory());
		~app();
	public:
		static app& instance();
		const neogfx::program_options& program_options() const override;
		const std::string& name() const override;
		int exec(bool aQuitWhenLastWindowClosed = true) override;
		bool in_exec() const override;
		void quit(int aResultCode = 0) override;
		i_basic_services& basic_services() const override;
		i_rendering_engine& rendering_engine() const override;
		i_surface_manager& surface_manager() const override;
		i_window_manager& window_manager() const override;
		i_keyboard& keyboard() const override;
		i_clipboard& clipboard() const override;
		i_audio& audio() const override;
	public:
		dimension default_dpi_scale_factor() const override;
	public:
		const i_texture& default_window_icon() const override;
		void set_default_window_icon(const i_texture& aIcon) override;
		void set_default_window_icon(const i_image& aIcon) override;
		const i_style& current_style() const override;
		i_style& current_style() override;
		i_style& change_style(const std::string& aStyleName) override;
		i_style& register_style(const i_style& aStyle) override;
	public:
		const std::string& translate(const std::string& aTranslatableString, const std::string& aContext = std::string{}) const override;
	public:
		i_action& action_file_new() override;
		i_action& action_file_open() override;
		i_action& action_file_close() override;
		i_action& action_file_close_all() override;
		i_action& action_file_save() override;
		i_action& action_file_save_all() override;
		i_action& action_file_exit() override;
		i_action& action_undo() override;
		i_action& action_redo() override;
		i_action& action_cut() override;
		i_action& action_copy() override;
		i_action& action_paste() override;
		i_action& action_delete() override;
		i_action& action_select_all() override;
		i_action& find_action(const std::string& aText) override;
		i_action& add_action(const std::string& aText) override;
		i_action& add_action(const std::string& aText, const std::string& aImageUri, dimension aDpiScaleFactor = 1.0, texture_sampling aSampling = texture_sampling::Normal) override;
		i_action& add_action(const std::string& aText, const i_texture& aImage) override;
		i_action& add_action(const std::string& aText, const i_image& aImage) override;
		void remove_action(i_action& aAction) override;
		void add_mnemonic(i_mnemonic& aMnemonic) override;
		void remove_mnemonic(i_mnemonic& aMnemonic) override;
	public:
		i_menu& add_standard_menu(i_menu& aParentMenu, standard_menu aStandardMenu) override;
	public:
		i_help& help() const override;
	public:
		bool process_events() override;
		bool process_events(i_event_processing_context& aContext) override;
	private:
		bool do_process_events();
	private:
		bool key_pressed(scan_code_e aScanCode, key_code_e aKeyCode, key_modifiers_e aKeyModifiers) override;
		bool key_released(scan_code_e aScanCode, key_code_e aKeyCode, key_modifiers_e aKeyModifiers) override;
		bool text_input(const std::string& aText) override;
		bool sys_text_input(const std::string& aText) override;
	private:
		neolib::async_event_queue iAsyncEventQueue;
		neogfx::program_options iProgramOptions;
		loader iLoader;
		std::string iName;
		bool iQuitWhenLastWindowClosed;
		bool iInExec;
		std::unique_ptr<i_basic_services> iBasicServices;
		std::unique_ptr<i_keyboard> iKeyboard;
		std::unique_ptr<i_clipboard> iClipboard;
		std::unique_ptr<i_rendering_engine> iRenderingEngine;
		std::unique_ptr<i_surface_manager> iSurfaceManager;
		std::unique_ptr<i_window_manager> iWindowManager;
		std::unique_ptr<i_audio> iAudio;
		std::optional<int> iQuitResultCode;
		texture iDefaultWindowIcon;
		style_list iStyles;
		style_list::iterator iCurrentStyle;
		action_list iActions;
		i_action& iActionFileNew;
		i_action& iActionFileOpen;
		i_action& iActionFileClose;
		i_action& iActionFileCloseAll;
		i_action& iActionFileSave;
		i_action& iActionFileSaveAll;
		i_action& iActionFileExit;		
		i_action& iActionUndo;
		i_action& iActionRedo;
		i_action& iActionCut;
		i_action& iActionCopy;
		i_action& iActionPaste;
		i_action& iActionDelete;
		i_action& iActionSelectAll;
		neolib::callback_timer iStandardActionManager;
		mnemonic_list iMnemonics;
		event_processing_context iAppContext;
		event_processing_context iAppMessageQueueContext;
		std::vector<std::pair<key_code_e, key_modifiers_e>> iKeySequence;
		mutable std::unique_ptr<i_help> iHelp;
	};
}