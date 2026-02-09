#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"

// declaration order:
// - Seek
// - Wander
// - Flee
// - Arrive
// - Evade
// - Pursuit

SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	Steering.LinearVelocity.Normalize();

	// Add debug rendering for grades
	return Steering;
};

SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	Steering.LinearVelocity.Normalize();

	// Add debug rendering for grades
	return Steering;
};

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering.LinearVelocity = Agent.GetPosition() - Target.Position;
	Steering.LinearVelocity.Normalize();

	// Add debug rendering for grades
	return Steering;
};

SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	FVector2D vecToTarget = Target.Position - Agent.GetPosition();
	Steering.LinearVelocity = vecToTarget;
	Steering.LinearVelocity.Normalize();

	if (vecToTarget.Length() < slowRadius)
	{	
		if (vecToTarget.Length() < targetRadius)
		{
			Agent.SetMaxLinearSpeed(0);
		}
		else
		{
			if (AgentMaxLinearSpeed == 0) AgentMaxLinearSpeed = Agent.GetMaxLinearSpeed();

			Agent.SetMaxLinearSpeed(AgentMaxLinearSpeed*(vecToTarget.Length() - targetRadius) / (slowRadius - targetRadius));
		}
	}

	// Add debug rendering for grades
	return Steering;
};

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	Steering.LinearVelocity.Normalize();

	// Add debug rendering for grades
	return Steering;
};

SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	Steering.LinearVelocity.Normalize();

	// Add debug rendering for grades
	return Steering;
};
