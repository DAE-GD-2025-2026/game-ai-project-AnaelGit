#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "DrawDebugHelpers.h"

// declaration order:
// - Seek
// - Wander
// - Flee
// - Arrive
// - Evade
// - Pursuit
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
		// draw LinearVelocity * 100
		DrawDebugLine(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), FVector(Agent.GetActorLocation().X + Steering.LinearVelocity.X * 100, Agent.GetActorLocation().Y + Steering.LinearVelocity.Y * 100, Agent.GetActorLocation().Z) + FVector(0, 0, -50), FColor::Green, false, 0, 0, 4.f);
	}
	return Steering;
};

SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	float randAngle = (std::rand() % (2 * static_cast<int>(m_MaxAngleChange *180/PI))) - m_MaxAngleChange; // generate a value between -m_MaxAngleChange and m_MaxAngleChange
	m_WanderAngle += randAngle * PI / 180;

	FVector2D circleCenter = FVector2D(Agent.GetPosition().X + Agent.GetActorForwardVector().X* m_OffsetDistance*100, Agent.GetPosition().Y + Agent.GetActorForwardVector().Y*m_OffsetDistance * 100);
	FVector2D target = FVector2D(circleCenter.X + cos(m_WanderAngle)*m_Radius*100, circleCenter.Y + sin(m_WanderAngle) * m_Radius * 100);

	Steering.LinearVelocity = target - Agent.GetPosition();
	Steering.LinearVelocity.Normalize();

	// Add debug rendering for grades
	if (Agent.GetDebugRenderingEnabled())
	{
		// draw player Circle
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), 35, 12, FColor::Yellow, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
		// draw target Circle
		DrawDebugCircle(Agent.GetWorld(), FVector(Target.Position, Agent.GetActorLocation().Z) + FVector(0, 0, -50), 15, 12, FColor::Red, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
		// draw LinearVelocity * 100
		DrawDebugLine(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), FVector(Agent.GetActorLocation().X + Steering.LinearVelocity.X * 100, Agent.GetActorLocation().Y + Steering.LinearVelocity.Y * 100, Agent.GetActorLocation().Z) + FVector(0, 0, -50), FColor::Green, false, 0, 0, 4.f);
		// draw WanderCircle
		DrawDebugCircle(Agent.GetWorld(), FVector(circleCenter,Agent.GetActorLocation().Z) + FVector(0, 0, -50), m_Radius*100, 12, FColor::Blue, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
		// draw target
		DrawDebugCircle(Agent.GetWorld(), FVector(target, Agent.GetActorLocation().Z) + FVector(0, 0, -50), 10, 12, FColor::Green, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
	}
	return Steering;
};

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering.LinearVelocity = Agent.GetPosition() - Target.Position;
	Steering.LinearVelocity.Normalize();

	// Add debug rendering for grades
	if (Agent.GetDebugRenderingEnabled())
	{
		// draw player Circle
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), 35, 12, FColor::Yellow, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
		// draw target Circle
		DrawDebugCircle(Agent.GetWorld(), FVector(Target.Position, Agent.GetActorLocation().Z) + FVector(0, 0, -50), 15, 12, FColor::Red, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
		// draw LinearVelocity * 100
		DrawDebugLine(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), FVector(Agent.GetActorLocation().X + Steering.LinearVelocity.X * 100, Agent.GetActorLocation().Y + Steering.LinearVelocity.Y * 100, Agent.GetActorLocation().Z) + FVector(0, 0, -50), FColor::Green, false, 0, 0, 4.f);
	}
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
			if (AgentMaxLinearSpeed == 0) AgentMaxLinearSpeed = Agent.GetMaxLinearSpeed(); // keep a trace of the agent's max speed

			float percentageFromTargetToSlowCircle = (vecToTarget.Length() - targetRadius) / (slowRadius - targetRadius);
			Agent.SetMaxLinearSpeed(AgentMaxLinearSpeed * percentageFromTargetToSlowCircle);
		}
	}

	// Add debug rendering for grades
	if (Agent.GetDebugRenderingEnabled())
	{
		// draw player Circle
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), 35, 12, FColor::Yellow, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
		// draw target Circle
		DrawDebugCircle(Agent.GetWorld(), FVector(Target.Position, Agent.GetActorLocation().Z) + FVector(0, 0, -50), 15, 12, FColor::Red, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
		// draw LinearVelocity * 100
		DrawDebugLine(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), FVector(Agent.GetActorLocation().X + Steering.LinearVelocity.X * 100, Agent.GetActorLocation().Y + Steering.LinearVelocity.Y * 100, Agent.GetActorLocation().Z) + FVector(0, 0, -50), FColor::Green, false, 0, 0, 4.f);
		// draw slow Circle
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), slowRadius, 12, FColor::Red, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
		// draw target Circle
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), targetRadius, 12, FColor::Red, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
	}
	return Steering;
};

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	FVector2D vecToTarget = Target.Position - Agent.GetPosition();
	float timeToReachTarget = vecToTarget.Length() / Agent.GetMaxLinearSpeed();
	FVector2D targetFuturePos = Target.Position + Target.LinearVelocity * timeToReachTarget;
	Steering.LinearVelocity =  Agent.GetPosition() - targetFuturePos;
	Steering.LinearVelocity.Normalize();

	// Add debug rendering for grades
	if (Agent.GetDebugRenderingEnabled())
	{
		// draw player Circle
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), 35, 12, FColor::Yellow, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
		// draw target Circle
		DrawDebugCircle(Agent.GetWorld(), FVector(targetFuturePos, Agent.GetActorLocation().Z) + FVector(0, 0, -50), 15, 12, FColor::Red, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
		// draw LinearVelocity * 100
		DrawDebugLine(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), FVector(Agent.GetActorLocation().X + Steering.LinearVelocity.X * 100, Agent.GetActorLocation().Y + Steering.LinearVelocity.Y * 100, Agent.GetActorLocation().Z) + FVector(0, 0, -50), FColor::Green, false, 0, 0, 4.f);
	}
	return Steering;
};

SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	FVector2D vecToTarget = Target.Position - Agent.GetPosition();
	float timeToReachTarget = vecToTarget.Length() / Agent.GetMaxLinearSpeed();
	FVector2D targetFuturePos = Target.Position + Target.LinearVelocity * timeToReachTarget;
	Steering.LinearVelocity = targetFuturePos - Agent.GetPosition();
	Steering.LinearVelocity.Normalize();

	// Add debug rendering for grades
	if (Agent.GetDebugRenderingEnabled())
	{
		// draw player Circle
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), 35, 12, FColor::Yellow, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
		// draw target Circle
		DrawDebugCircle(Agent.GetWorld(), FVector(targetFuturePos, Agent.GetActorLocation().Z) + FVector(0, 0, -50), 15, 12, FColor::Red, false, 0.f, 0, 4.f, FVector::RightVector, FVector::ForwardVector);
		// draw LinearVelocity * 100
		DrawDebugLine(Agent.GetWorld(), Agent.GetActorLocation() + FVector(0, 0, -50), FVector(Agent.GetActorLocation().X + Steering.LinearVelocity.X * 100, Agent.GetActorLocation().Y + Steering.LinearVelocity.Y * 100, Agent.GetActorLocation().Z) + FVector(0, 0, -50), FColor::Green, false, 0, 0, 4.f);
	}
	return Steering;
};

SteeringOutput Face::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};

	Steering.AngularVelocity = 50;
	//UE_LOG(LogTemp, Warning, TEXT("what I get %f and what I can set %f"), Agent.GetAngularVelocity(), Steering.AngularVelocity);
	
	// Add debug rendering for grades
	return Steering;
};
