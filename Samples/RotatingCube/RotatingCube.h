//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) arkenthera.All rights reserved.
//
//BasicRenderWindow.h
//--------------------------------------------------------------------------------
#ifndef __RotatingCube_h__
#define __RotatingCube_h__
//--------------------------------------------------------------------------------
#include "Common/Common.h"
#include "Core/YumeEventHub.h"
namespace YumeEngine
{
	class YumeScene;
	class YumeSceneNode;

	class GodRays : public BaseApplication
	{
	public:
		GodRays();
		~GodRays();

		virtual void Start();
		virtual void Setup();

		void MoveCamera(float timeStep);

		virtual void HandleUpdate(float timeStep);
		virtual void HandleRenderUpdate(float timeStep);


		StaticModel* boxBlue;
		StaticModel* boxRed;
		StaticModel* boxPink;

		float angle1_;

		float updown1_;
		float leftRight1_;
	};
}

//--------------------------------------------------------------------------------
#endif
