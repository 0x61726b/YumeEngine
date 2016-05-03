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

	class PlaygroundDemo : public BaseApplication
	{
	public:
		PlaygroundDemo();
		~PlaygroundDemo();

		virtual void Start();
		virtual void Setup();

		void MoveCamera(float timeStep);

		virtual void HandleUpdate(float timeStep);
		virtual void HandleRenderUpdate(float timeStep);

		void SSAOOffsetVectors();

		void CreateModel(Vector3 Pos,Quaternion Rot);
		void CreateCube(Vector3 Pos,Quaternion Rot,float size,YumeColor color);
		void CreateCube(Vector3 Pos,Quaternion Rot,Vector3 size,YumeColor color);
		void CreateSphere(Vector3 Pos,Quaternion Rot,float size,YumeColor color);
		void CreateCylinder(Vector3 Pos,Quaternion Rot,float size,YumeColor color);
		void CreatePyramid(Vector3 Pos,Quaternion Rot,float size,YumeColor color);
		void CreateCone(Vector3 Pos,Quaternion Rot,float size,YumeColor color);

		void CreateLight(DirectX::XMFLOAT3 pos,YumeColor color,float range);

		YumeSceneNode* cubeNode_;
		YumeSceneNode* dirLightNode_;
		Quaternion rot_;

		Vector4Vector::type mOffsets;
	};
}

//--------------------------------------------------------------------------------
#endif
