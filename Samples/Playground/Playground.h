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
	class StaticModel;
	class Light;

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

		StaticModel* CreateModel(const YumeString& name,DirectX::XMFLOAT3 Pos = DirectX::XMFLOAT3(0,0,0),DirectX::XMFLOAT4 rot = DirectX::XMFLOAT4(0,0,0,0),DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1,1,1));
		void CreateCube(Vector3 Pos,Quaternion Rot,float size,YumeColor color);
		void CreateCube(Vector3 Pos,Quaternion Rot,Vector3 size,YumeColor color);
		void CreateSphere(Vector3 Pos,Quaternion Rot,float size,YumeColor color);
		void CreateCylinder(Vector3 Pos,Quaternion Rot,float size,YumeColor color);
		void CreatePyramid(Vector3 Pos,Quaternion Rot,float size,YumeColor color);
		void CreateCone(Vector3 Pos,Quaternion Rot,float size,YumeColor color);

		void CreateLight(DirectX::XMFLOAT3 pos,YumeColor color,float range);
		void CreateCircleOfPointLights(DirectX::XMFLOAT3 origin,float numberOfLights,float rad);

		YumeSceneNode* cubeNode_;
		YumeSceneNode* dirLightNode_;
		Quaternion rot_;

		Vector4Vector::type mOffsets;

		YumeVector<Light*>::type lights_;
		float rotationAngle_;
	};
}

//--------------------------------------------------------------------------------
#endif
