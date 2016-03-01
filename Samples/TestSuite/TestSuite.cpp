//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) arkenthera.All rights reserved.
//
//BasicRenderWindow.cpp
//--------------------------------------------------------------------------------

#include "Core/YumeHeaders.h"

#include "TestSuite.h"
#include "Core/YumeVariant.h"

#include "Engine/YumeEngine.h"
#include "Core/YumeEnvironment.h"

#include "Logging/logging.h"

#include "Core/YumeMain.h"

#include <boost/shared_ptr.hpp>


#define BOOST_TEST_MODULE YumeTest
#include <boost/test/included/unit_test.hpp>
#include <boost/test/debug.hpp>

namespace utf = boost::unit_test;



namespace YumeEngine
{
	class YumePerTestSuiteFixture
	{
	public:
		YumePerTestSuiteFixture()
		{
			boost::debug::detect_memory_leaks(false);
		}

		~YumePerTestSuiteFixture()
		{

		}

		void Initialize()
		{
			engine_ = boost::shared_ptr<YumeEngine3D>(new YumeEngine3D);
			engineVariants_["testing"] = true;
			engineVariants_["ResourceTree"] = YumeString("Engine/Assets");
		}


		VariantMap engineVariants_;
		boost::shared_ptr<YumeEngine::YumeEngine3D> engine_;
	};
	BOOST_FIXTURE_TEST_SUITE(YumeTestSuite,YumePerTestSuiteFixture);


	BOOST_AUTO_TEST_CASE(InitializeEngine)
	{
		Initialize();
		//Initialize engine in a software rendering mode
		//Since we dont have software rendering , it should fail


		BOOST_REQUIRE(engine_->Initialize(engineVariants_) == true);

		BOOST_REQUIRE(engine_->GetEnvironment()->GetVariant("testing").Get<bool>() == true);

#if YUME_PLATFORM == YUME_PLATFORM_WIN32
		BOOST_REQUIRE(engine_->GetRendererName() == "YumeNull");
#else
		BOOST_REQUIRE(engine_->GetRendererName() == "libYumeNull");
#endif

		engine_->Run();

		engine_->Exit();

		engine_.reset();
	}

	BOOST_AUTO_TEST_CASE(YumeVariantTest)
	{
		VariantMap vm;

		int anInteger = vm["anInteger"].Get<int>();
		double aDouble = vm["double"].Get<double>();
		float aFloat = vm["float"].Get<float>();
		bool aBool = vm["bool"].Get<bool>();
		YumeString aString = vm["string"].Get<YumeString>();

		BOOST_REQUIRE(anInteger == 0);
		BOOST_REQUIRE(aDouble == 0);
		BOOST_REQUIRE(aFloat == 0);
		BOOST_REQUIRE(aBool == 0);
		BOOST_REQUIRE(aString == YumeString());

		YumeVariant var_(true);

		BOOST_REQUIRE(var_.Get<bool>() == true);
		BOOST_REQUIRE(var_.Get<double>() == 0);
		BOOST_REQUIRE(var_.GetType() == VAR_BOOL);

		YumeVariant varInt_(42);

		BOOST_REQUIRE(varInt_.Get<int>() == 42);
		BOOST_REQUIRE(varInt_.Get<double>() == 0);
		BOOST_REQUIRE(varInt_.GetType() == VAR_INT);

		YumeVariant varStr_("42");

		BOOST_REQUIRE(varStr_.Get<YumeString>() == "42");
		BOOST_REQUIRE(varStr_.Get<double>() == 0);
		BOOST_REQUIRE(varStr_.GetType() == VAR_STRING);
	}

	BOOST_AUTO_TEST_SUITE_END()
}
