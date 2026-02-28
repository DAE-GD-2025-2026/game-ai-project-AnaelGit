#include "Level_CombinedSteering.h"

#include "imgui.h"


// Sets default values
ALevel_CombinedSteering::ALevel_CombinedSteering()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALevel_CombinedSteering::BeginPlay()
{
	Super::BeginPlay();

	// DrunkAgent setup
	m_pDrunkAgentSeek->SetTarget(MouseTarget);
	m_pDrunkAgent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, FVector{ 0,0,90 }, FRotator::ZeroRotator);
	m_pDrunkAgentSteering->SetTarget(MouseTarget);
	m_pDrunkAgent->SetSteeringBehavior(m_pDrunkAgentSteering);
	m_pDrunkAgent->SetDebugRenderingEnabled(false);

	// EvdaingAgent setup
	m_pEvadingAgent = GetWorld()->SpawnActor<ASteeringAgent>(SteeringAgentClass, FVector{ 0,0,90 }, FRotator::ZeroRotator);

	ASteeringAgent* const TargetAgent = m_pDrunkAgent;
	FTargetData Target;
	Target.Position = TargetAgent->GetPosition();
	Target.Orientation = TargetAgent->GetRotation();
	Target.LinearVelocity = TargetAgent->GetLinearVelocity();
	Target.AngularVelocity = TargetAgent->GetAngularVelocity();

	m_pEvadingSteering->SetTarget(Target);
	m_pEvadingAgent->SetSteeringBehavior(m_pEvadingSteering);
	m_pEvadingAgent->SetDebugRenderingEnabled(false);
}

void ALevel_CombinedSteering::BeginDestroy()
{
	delete m_pDrunkAgentSeek;
	delete m_pDrunkAgentWander;
	delete m_pDrunkAgentSteering;

	delete m_pEvadingSteering;
	delete m_pEvadingEvade;
	delete m_pEvadingWander;

	Super::BeginDestroy();
}

// Called every frame
void ALevel_CombinedSteering::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#pragma region UI
	//UI
	{
		//Setup
		bool windowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Game AI", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Flocking");
		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Checkbox("Debug Rendering", &CanDebugRender))
		{
			m_pDrunkAgent->SetDebugRenderingEnabled(CanDebugRender);
			m_pEvadingAgent->SetDebugRenderingEnabled(CanDebugRender);
		}
		ImGui::Checkbox("Trim World", &TrimWorld->bShouldTrimWorld);
		if (TrimWorld->bShouldTrimWorld)
		{
			ImGuiHelpers::ImGuiSliderFloatWithSetter("Trim Size",
				TrimWorld->GetTrimWorldSize(), 1000.f, 3000.f,
				[this](float InVal) { TrimWorld->SetTrimWorldSize(InVal); });
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Behavior Weights");
		ImGui::Spacing();

		ImGuiHelpers::ImGuiSliderFloatWithSetter("Seek",
			m_pDrunkAgentSteering->GetWeightedBehaviorsRef()[0].Weight, 0.f, 1.f,
			[this](float InVal) {
				m_pDrunkAgentSteering->GetWeightedBehaviorsRef()[0].Weight = InVal;}, "%.2f");

		ImGuiHelpers::ImGuiSliderFloatWithSetter("Wander",
			m_pDrunkAgentSteering->GetWeightedBehaviorsRef()[1].Weight, 0.f, 1.f,
			[this](float InVal) {
				m_pDrunkAgentSteering->GetWeightedBehaviorsRef()[1].Weight = InVal;}, "%.2f");

		//End
		ImGui::End();
	}
#pragma endregion

	// Combined Steering Update
	m_pDrunkAgentSeek->SetTarget(MouseTarget);

	const float distanceBetweenAgents = (m_pDrunkAgent->GetPosition() - m_pEvadingAgent->GetPosition()).SquaredLength();
	if (distanceBetweenAgents < m_evadingRadiusSquared)
	{
		ASteeringAgent* const TargetAgent = m_pDrunkAgent;
		FTargetData Target;
		Target.Position = TargetAgent->GetPosition();
		Target.Orientation = TargetAgent->GetRotation();
		Target.LinearVelocity = TargetAgent->GetLinearVelocity();
		Target.AngularVelocity = TargetAgent->GetAngularVelocity();

		m_pEvadingEvade->SetTarget(Target);
		m_pEvadingAgent->SetSteeringBehavior(m_pEvadingEvade);
	}
	else
	{
		m_pEvadingAgent->SetSteeringBehavior(m_pEvadingWander);
	}
}