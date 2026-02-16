#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "DrawDebugHelpers.h"

// declaration order:
// - Seek
// - Wander
// - Flee
// - Arrive
// - Pursuit
// - Evade
// - Face

SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering.LinearVelocity = Target.Position - Agent.GetPosition();
	Steering.LinearVelocity.Normalize();

	// Add debug rendering for grades
	if (Agent.GetDebugRenderingEnabled())
	{
		// draw player Circle
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), 35, 12, FColor::Yellow, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
		// draw target Circle
		DrawDebugCircle(Agent.GetWorld(), FVector(Target.Position, Agent.GetActorLocation().Z) + FVector(0, 0, -50), 15, 12, FColor::Red, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
		// draw LinearVelocity
		DrawDebugLine(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), FVector(Agent.GetActorLocation().X + Agent.GetLinearVelocity().X, Agent.GetActorLocation().Y + Agent.GetLinearVelocity().Y, Agent.GetActorLocation().Z) + FVector(0, 0, -50), FColor::Green, false, 0, 0, 4.f);
	}
	return Steering;
};

SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	float randAngle = FMath::RandRange(-m_MaxAngleChange, m_MaxAngleChange);
	m_WanderAngle = FMath::UnwindDegrees(m_WanderAngle + randAngle);

	FVector2D circleCenter = FVector2D(Agent.GetPosition().X + Agent.GetActorForwardVector().X * m_OffsetDistance, Agent.GetPosition().Y + Agent.GetActorForwardVector().Y * m_OffsetDistance);
	Target.Position = FVector2D(circleCenter.X + FMath::Cos(FMath::DegreesToRadians(m_WanderAngle)) * m_Radius, circleCenter.Y + FMath::Sin(FMath::DegreesToRadians(m_WanderAngle)) * m_Radius);

	Steering = Seek::CalculateSteering(DeltaT, Agent);

	// Add debug rendering for grades
	if (Agent.GetDebugRenderingEnabled())
	{
		// draw WanderCircle
		DrawDebugCircle(Agent.GetWorld(), FVector(circleCenter,Agent.GetActorLocation().Z) + FVector(0, 0, -50), m_Radius, 24, FColor::Blue, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
	}
	return Steering;
};

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering = Seek::CalculateSteering(DeltaT, Agent);
	Steering.LinearVelocity *= -1;

	return Steering;
};

SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering = Seek::CalculateSteering(DeltaT, Agent);

	FVector2D vecToTarget = Target.Position - Agent.GetPosition();
	if (vecToTarget.Length() < slowRadius)
	{	
		if (vecToTarget.Length() < targetRadius)
		{
			Agent.SetMaxLinearSpeed(0);
		}
		else
		{
			if (AgentMaxLinearSpeed == 0) AgentMaxLinearSpeed = Agent.GetMaxLinearSpeed(); // keep a trace of the agent's max speed

			float percentageFromTargetToSlowCircle = (vecToTarget.Length() - targetRadius) / (slowRadius - targetRadius);
			Agent.SetMaxLinearSpeed(AgentMaxLinearSpeed * percentageFromTargetToSlowCircle);
		}
	}

	// Add debug rendering for grades
	if (Agent.GetDebugRenderingEnabled())
	{
		// draw slow Circle
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), slowRadius, 12, FColor::Red, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
		// draw target Circle
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), targetRadius, 12, FColor::Red, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
	}
	return Steering;
};

SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	FVector2D vecToTarget = Target.Position - Agent.GetPosition();
	float timeToReachTarget = vecToTarget.Length() / Agent.GetMaxLinearSpeed();
	FVector2D targetPredictedPos = Target.Position + Target.LinearVelocity * timeToReachTarget;
	Target.Position = targetPredictedPos;

	Steering = Seek::CalculateSteering(DeltaT, Agent);

	return Steering;
};

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering = Pursuit::CalculateSteering(DeltaT, Agent);
	Steering.LinearVelocity *= -1;

	return Steering;
};

SteeringOutput Face::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	FVector2D vecToTarget = Target.Position - Agent.GetPosition();
	float desiredAngle = FMath::RadiansToDegrees(FMath::Atan2(vecToTarget.Y, vecToTarget.X)); // in degrees
	float deltaAngle = FMath::FindDeltaAngleDegrees(Agent.GetRotation(), desiredAngle);

	Steering.AngularVelocity = (deltaAngle >= 0) ? Agent.GetMaxAngularSpeed(): -Agent.GetMaxAngularSpeed();
	if (abs(deltaAngle) < 5.f)
	{
		Steering.AngularVelocity *= 0.1;
	}

	// Add debug rendering for grades
	if (Agent.GetDebugRenderingEnabled())
	{
		// draw player Circle
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), 35, 12, FColor::Yellow, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
		// draw target Circle
		DrawDebugCircle(Agent.GetWorld(), FVector(Target.Position, Agent.GetActorLocation().Z) + FVector(0, 0, -50), 15, 12, FColor::Red, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
	}
	return Steering;
};


