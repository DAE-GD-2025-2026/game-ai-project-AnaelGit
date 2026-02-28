// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombinedSteeringBehaviors.h"
#include "GameAIProg/Shared/Level_Base.h"
#include "GameAIProg/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "Level_CombinedSteering.generated.h"

UCLASS()
class GAMEAIPROG_API ALevel_CombinedSteering : public ALevel_Base
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevel_CombinedSteering();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

private:
	//Datamembers
	bool UseMouseTarget = false;
	bool CanDebugRender = false;


	ISteeringBehavior* m_pDrunkAgentSeek = new Seek;
	ISteeringBehavior* m_pDrunkAgentWander = new Wander;
	BlendedSteering* m_pDrunkAgentSteering = new BlendedSteering({ { m_pDrunkAgentSeek,0.5f },{ m_pDrunkAgentWander, 0.5f} });

	ISteeringBehavior* m_pEvadingWander = new Wander;
	ISteeringBehavior* m_pEvadingEvade = new Evade;
	PrioritySteering* m_pEvadingSteering = new PrioritySteering({ m_pEvadingWander, m_pEvadingEvade });
	float m_evadingRadiusSquared = 500*500; // I use SquaredLength() in the calculations

	ASteeringAgent* m_pDrunkAgent = nullptr;
	ASteeringAgent* m_pEvadingAgent = nullptr;

};
