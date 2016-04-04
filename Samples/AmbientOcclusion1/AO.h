//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) arkenthera.All rights reserved.
//
//BasicRenderWindow.h
//--------------------------------------------------------------------------------
#ifndef __BasicRenderWindow_h__
#define __BasicRenderWindow_h__
//--------------------------------------------------------------------------------
#include "Common/Common.h"
#include "Core/YumeEventHub.h"
namespace YumeEngine
{
	class YumeScene;
	class YumeSceneNode;

	class AODemo : public BaseApplication
	{
	public:
		AODemo();
		~AODemo();

		virtual void Start();
		virtual void Setup();

		void MoveCamera(float timeStep);

		virtual void HandleUpdate(float timeStep);
		virtual void HandleRenderUpdate(float timeStep);


		void CreateModel(Vector3 Pos,Quaternion Rot);

		YumeSceneNode* cubeNode_;
		Quaternion rot_;
	};
}

//--------------------------------------------------------------------------------
#endif
