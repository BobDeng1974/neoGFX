﻿#include <neogfx/neogfx.hpp>
#include <iomanip>
#include <boost/format.hpp>
#include <neolib/random.hpp>
#include <neolib/singleton.hpp>
#include <neogfx/app/i_app.hpp>
#include <neogfx/gui/layout/i_layout.hpp>
#include <neogfx/gfx/image.hpp>
#include <neogfx/game/chrono.hpp>
#include <neogfx/game/ecs.hpp>
#include <neogfx/game/canvas.hpp>
#include <neogfx/game/standard_archetypes.hpp>
#include <neogfx/game/game_world.hpp>
#include <neogfx/game/time.hpp>
#include <neogfx/game/simple_physics.hpp>
#include <neogfx/game/rigid_body.hpp>
#include <neogfx/game/sprite.hpp>
#include <neogfx/game/text_mesh.hpp>
#include <neogfx/game/collider.hpp>
#include <neogfx/game/rectangle.hpp>
#include <neogfx/game/broadphase_collider.hpp>

namespace ng = neogfx;
using namespace neolib::stdint_suffix;

/*void create_game(ng::i_layout& aLayout)
{
	spritePlane.enable_z_sorting(true);

	auto score = std::make_shared<std::pair<uint32_t, ng::text>>(0, ng::text{ spritePlane, ng::vec3{}, "", ng::font("SnareDrum Two NBP", "Regular", 60.0), ng::text_appearance{ ng::colour::White, ng::text_effect{ ng::text_effect_type::Outline, ng::colour::Black } } });
	score->second.set_value("000000");
	score->second.set_position(ng::vec3{ 0.0, 0.0, 1.0 });
	auto positionScore = [&spritePlane, score]()
	{
		score->second.set_position(ng::vec3{ spritePlane.extents().cx - score->second.extents().x, spritePlane.extents().cy - score->second.extents().y, 1.0 });
	};
	spritePlane.size_changed(positionScore);
	positionScore();
	spritePlane.add_shape(score->second);
	auto shipInfo = std::make_shared<ng::text>(spritePlane, ng::vec3{}, "", ng::font("SnareDrum One NBP", "Regular", 24.0), ng::colour::White);
	shipInfo->set_border(1.0);
	shipInfo->set_margins(ng::margins(2.0));
	shipInfo->set_tag_of(spaceshipSprite, ng::vec3{ 18.0, 18.0, 1.0 });
	spritePlane.add_shape(shipInfo);
#ifdef NDEBUG
	for (int i = 0; i < 50; ++i)
#else
	for (int i = 0; i < 5; ++i)
#endif
	{
		create_target(spritePlane);
	}
	auto debugInfo = std::make_shared<ng::text>(spritePlane, ng::vec3{ 0.0, 132.0, 1.0 }, "", spritePlane.font().with_size(spritePlane.font().size() * 0.5), ng::text_appearance{ ng::colour::Orange.with_lightness(0.9), ng::text_effect{ ng::text_effect_type::Outline, ng::colour::Black } });
	spritePlane.add_shape(debugInfo);
	spritePlane.sprites_painted([&spritePlane](ng::graphics_context& aGraphicsContext)
	{
		aGraphicsContext.draw_text(ng::point{ 0.0, 0.0 }, "Hello, World!", spritePlane.font().with_style(ng::font_info::Underline), ng::colour::White);
		if (ng::service<ng::i_app>().keyboard().is_key_pressed(ng::ScanCode_C))
			spritePlane.collision_tree_2d().visit_aabbs([&aGraphicsContext](const neogfx::aabb_2d& aAabb)
			{
				ng::rect aabb{ ng::point{ aAabb.min }, ng::point{ aAabb.max } };
				aGraphicsContext.draw_rect(aabb, ng::pen{ ng::colour::Blue });
			});
	});

	auto explosion = std::make_shared<ng::texture>(ng::image{ ":/test/resources/explosion.png" });


	~~~~spritePlane.physics_applied([debugInfo, &spritePlane](ng::canvas::step_time_interval)
	{
		debugInfo->set_value(
			"Objects: " + boost::lexical_cast<std::string>(spritePlane.objects().size()) + "\n" +
			"Collision tree (quadtree) size: " + boost::lexical_cast<std::string>(spritePlane.collision_tree_2d().count()) + "\n" +
			"Collision tree (quadtree) depth: " + boost::lexical_cast<std::string>(spritePlane.collision_tree_2d().depth()) + "\n" +
			"Collision tree (quadtree) update type: " + (spritePlane.dynamic_update_enabled() ? "dynamic" : "full") + "\n" +
			"Physics update time: " + boost::str(boost::format("%.6f") % spritePlane.update_time()) + " s");
	});

	spritePlane.mouse_event([&spritePlane, &spaceshipSprite](const neogfx::mouse_event& e)
	{
		if ((e.type() == neogfx::mouse_event_type::ButtonClicked || 
			e.type() == neogfx::mouse_event_type::Moved) && (e.mouse_button() & neogfx::mouse_button::Left) == neogfx::mouse_button::Left)
		{
			auto newPos = ng::point{ e.position() - spritePlane.origin() };
			newPos.y = spritePlane.extents().cy - newPos.y;
			spaceshipSprite.set_position(newPos.to_vec3());
		}
	});
}
*/

namespace archetypes
{
	ng::game::sprite_archetype const spaceship{ "Spaceship" };
	ng::game::sprite_archetype const asteroid{ "Asteroid" };
	ng::game::sprite_archetype const missile{ "Missile" };
}

void create_game(ng::i_layout& aLayout)
{
	// Canvas to render game world on...
	auto& canvas = aLayout.add(std::make_shared<ng::game::canvas>());
	canvas.set_font(ng::font{ canvas.font(), ng::font_style::Bold, 16 });
	canvas.set_background_colour(ng::colour::Black);

	// Get ECS associated with canvas...
	auto& ecs = canvas.ecs();

	// Background...
	neolib::basic_random<ng::scalar> prng;
	for (int i = 0; i < 1000; ++i)
		ng::game::shape::rectangle
		{
			ecs,
			ng::vec3{ prng(800), prng(800), -1.0 + 0.5 * (prng(32) / 32.0) },
			ng::vec2{ prng(64), prng(64) },
			ng::colour{ ng::vec4{ prng(0.25), prng(0.25), prng(0.25), 1.0 } }.lighter(0x40)
		}.detach();

	// Asteroids...
	auto make_asteroid_mesh = [&]()
	{
		ng::game::mesh asteroidMesh;
		asteroidMesh.vertices.push_back(ng::vec3{ 0.0, 0.0, 0.0 });
		auto w = prng(20.0) + 10.0;
		for (ng::scalar angle = 0.0; angle < 360.0; angle += (prng(30.0) + 30.0))
			asteroidMesh.vertices.push_back(ng::rotation_matrix(ng::vec3{ 0.0, 0.0, ng::to_rad(angle) }) * ng::vec3{ w + prng(10.0) - 5.0, 0.0, 0.0 });
		for (uint32_t i = 1; i < asteroidMesh.vertices.size() - 1; ++i)
			asteroidMesh.faces.push_back(ng::game::face{ 0u, i, i + 1u });
		asteroidMesh.faces.push_back(ng::game::face{ 0u, 1u, asteroidMesh.vertices.size() - 1u });
		return asteroidMesh;
	};

	for (int i = 0; i < 75; ++i)
		auto asteroid = ecs.create_entity(
			archetypes::asteroid, 
			ng::game::material{ ng::to_ecs_component(ng::colour::from_hsl(prng(360), 1.0, 0.75)) },
			make_asteroid_mesh(),
			ng::game::rigid_body
			{ 
				ng::vec3{ prng(800), prng(800), 0.0 }, 1.0, 
				ng::rotation_matrix(ng::vec3{ 0.0, 0.0, ng::to_rad(prng(360.0)) }) * ng::vec3{ prng(20.0), 0.0, 0.0 }, 
				{}, 
				{}, 
				ng::vec3{ 0.0, 0.0, ng::to_rad(prng(90.0) + 45.0) * (std::rand() % 2 == 0 ? 1.0 : -1.0) } 
			},
			ng::game::broadphase_collider{ 0x2ull });

	// Spaceship...
	const char* spaceshipImage
	{
		"[9,9]"
		"{0,paper}"
		"{1,ink1}"
		"{2,ink2}"

		"000010000"
		"000121000"
		"000121000"
		"001222100"
		"001222100"
		"011222110"
		"010111010"
		"010000010"
		"010000010"
	};

	auto spaceship = ecs.create_entity(
		archetypes::spaceship,
		ng::to_ecs_component(ng::rect{ ng::size{ 36.0, 36.0} }.with_centred_origin()),
		ng::game::material{ {}, {}, {}, ng::to_ecs_component(
			ng::image
			{
				spaceshipImage,
				{
					{ "paper", ng::colour{} },
					{ "ink1", ng::colour::LightGoldenrod },
					{ "ink2", ng::colour::DeepSkyBlue }
				},
				1.0,
				ng::texture_sampling::Nearest
			})
		},
		ng::game::rigid_body
		{
			ng::vec3{ 400.0, 18.0, 0.1 }, 1.0
		},
		ng::game::broadphase_collider{ 0x1ull });

	ng::font clockFont{ "SnareDrum Two NBP", "Regular", 40.0 };
	// Some information text...
	canvas.entities_rendered([&, clockFont](ng::graphics_context& gc)
	{
		std::ostringstream text;
		auto worldTime = static_cast<uint64_t>(ng::game::from_step_time(ecs.system<ng::game::time>().world_time()) * 1000.0);
		text.fill('0');
		text << std::setw(2) << worldTime / (1000 * 60 * 60) << " : " << std::setw(2) << worldTime / (1000 * 60) % 60 << " : " << std::setw(2) << worldTime / (1000) % 60 << " . " << std::setw(3) << worldTime % 1000;
		gc.draw_text(ng::point{ 0.0, 0.0 }, text.str(), clockFont, ng::text_appearance{ ng::colour::White, ng::text_effect{ ng::text_effect_type::Outline, ng::colour::Black, 2.0 } });
	});

	// Instantiate physics...
	ecs.system<ng::game::simple_physics>();

	~~~~ecs.system<ng::game::game_world>().applying_physics([&ecs, spaceship /*, &spritePlane, score, shipInfo, explosion*/](ng::game::step_time aPhysicsStepTime)
	{
		auto const& keyboard = ng::service<ng::i_keyboard>();
		auto& spaceshipPhysics = ecs.component<ng::game::rigid_body>().entity_record(spaceship);
		spaceshipPhysics.acceleration =
			ng::vec3
		{
			keyboard.is_key_pressed(ng::ScanCode_RIGHT) ? 16.0 : keyboard.is_key_pressed(ng::ScanCode_LEFT) ? -16.0 : 0.0,
			keyboard.is_key_pressed(ng::ScanCode_UP) ? 16.0 : keyboard.is_key_pressed(ng::ScanCode_DOWN) ? -16.0 : 0.0
		};
		if (keyboard.is_key_pressed(ng::ScanCode_X))
			spaceshipPhysics.spin.z = ng::to_rad(-30.0);
		else if (keyboard.is_key_pressed(ng::ScanCode_Z))
			spaceshipPhysics.spin.z = ng::to_rad(30.0);
		else
			spaceshipPhysics.spin.z = 0.0;


		/*
				std::ostringstream oss;
				oss << "VELOCITY:  " << spaceshipSprite.velocity().magnitude() << " m/s" << "\n";
				oss << "ACCELERATION:  " << spaceshipSprite.acceleration().magnitude() << " m/s/s";
				shipInfo->set_value(oss.str()); */
	});

	ecs.system<ng::game::game_world>().physics_applied([&ecs, spaceship /*, &spritePlane, score, shipInfo, explosion*/](ng::game::step_time aPhysicsStepTime)
	{
		auto const& keyboard = ng::service<ng::i_keyboard>();
		auto& spaceshipPhysics = ecs.component<ng::game::rigid_body>().entity_record(spaceship);
		static bool sExtraFire = false;
		if (keyboard.is_key_pressed(ng::ScanCode_SPACE) || sExtraFire)
		{
			auto stepTime_ms = static_cast<decltype(aPhysicsStepTime)>(ng::game::chrono::to_milliseconds(ng::game::chrono::flicks{ aPhysicsStepTime }));
			static auto sLastTime_ms = stepTime_ms;
			auto sinceLastTime_ms = stepTime_ms - sLastTime_ms;
			if (sinceLastTime_ms < 100)
			{
				if (sinceLastTime_ms / 10 % 2 == 0)
				{
					sExtraFire = false;
					auto make_missile = [&](double angle)
					{
						auto tm = ng::to_transformation_matrix(spaceshipPhysics, false);
						auto missile = ecs.create_entity(
							archetypes::missile,
							ng::to_ecs_component(ng::rect{ ng::size{ 3.0, 3.0} }.with_centred_origin()),
							ng::game::material{ ng::to_ecs_component(ng::colour{ rand() % 160 + 96, rand() % 160 + 96, rand() % 160 + 96 }), {}, {}, {} },
							ng::game::rigid_body
							{
								spaceshipPhysics.position + ~(tm * ng::vec4{ 0.0, 18.0, 0.0, 1.0 }).xyz,
								0.016,
								~(tm * ng::affine_rotation_matrix(ng::vec3{0.0, 0.0, ng::to_rad(angle)}) * ng::vec4{ 0.0, 360.0, 0.0, 0.0 }).xyz + spaceshipPhysics.velocity,
								{},
								spaceshipPhysics.angle + ng::vec3{ 0.0, 0.0, ng::to_rad(angle) }
							},
							ng::game::broadphase_collider{ 0x1ull });
						ecs.component<ng::game::mesh_renderer>().entity_record(missile).destroyOnFustrumCull = true;
					};
					for (double angle = -30.0; angle <= 30.0; angle += 10.0)
						make_missile(angle);
				}
			}
			else
				sExtraFire = true;
			if (sinceLastTime_ms > 200)
				sLastTime_ms = stepTime_ms;
		}
	});
	/*
	void collided(i_collidable_object& aOther) override
	{
		auto& other = aOther.as_physical_object();
		iScore.first += 250;
		std::ostringstream oss;
		oss << std::setfill('0') << std::setw(6) << iScore.first;
		iScore.second.set_value(oss.str());
		static boost::fast_pool_allocator<ng::sprite> alloc;
		auto explosion = std::allocate_shared<ng::sprite, boost::fast_pool_allocator<sprite>>(
			alloc, *iExplosion, ng::sprite::animation_info{ { { ng::point{}, ng::size{ 60.0, 60.0 } } }, 12, 0.040, false });
		explosion->set_collision_mask(0x1ull);
		static neolib::basic_random<double> r;
		explosion->set_position(position() + ng::vec3{ r.get(-10.0, 10.0), r.get(-10.0, 10.0), -0.1 });
		explosion->set_angle_degrees(ng::vec3{ 0.0, 0.0, r.get(360.0) });
		explosion->set_extents(ng::vec3{ r.get(40.0, 80.0), r.get(40.0, 80.0) });
		iWorld.add_sprite(explosion);
		kill();
		other.kill();
		if (other.collision_mask() != 0x4ull)
			create_target(iWorld);
	}
*/

	canvas.mouse_event([&canvas, spaceship](const neogfx::mouse_event& e)
	{
		if ((e.type() == neogfx::mouse_event_type::ButtonClicked ||
			e.type() == neogfx::mouse_event_type::Moved) && (e.mouse_button() & neogfx::mouse_button::Left) == neogfx::mouse_button::Left)
		{
			auto newPos = ng::point{ e.position() - canvas.origin() };
			newPos.y = canvas.extents().cy - newPos.y;
			ng::game::component_lock_guard<ng::game::rigid_body> lg{ canvas.ecs() };
			canvas.ecs().component<ng::game::rigid_body>().entity_record(spaceship).position = newPos.to_vec3();
		}
	});

}