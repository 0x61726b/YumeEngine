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

		YumeSceneNode* CreateModel(Vector3 Pos,Quaternion Rot,float scale,const YumeString& model);
		void CreateCube(Vector3 Pos,Quaternion Rot,float size,YumeColor color);
		void CreateSphere(Vector3 Pos,Quaternion Rot,float size,YumeColor color);
		void CreateCylinder(Vector3 Pos,Quaternion Rot,float size,YumeColor color);
		void CreatePyramid(Vector3 Pos,Quaternion Rot,float size,YumeColor color);
		void CreateCone(Vector3 Pos,Quaternion Rot,float size,YumeColor color);

		YumeSceneNode* cubeNode_;
		YumeSceneNode* dirLightNode_;
		Quaternion rot_;

		YumeSceneNode* buddha_;
		YumeSceneNode* dragon_;

		Vector4Vector::type mOffsets;
	};
}

//--------------------------------------------------------------------------------
#endif
